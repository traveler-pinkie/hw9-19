#include <sys/stat.h>

#include "test_common.h"

char *test_output_dir;
char *test_outfile;
char *test_errfile;
char *alt_outfile;

char *ref_dir;
char *ref_infile;
char *ref_outfile;
char *ref_errfile;

/*
 * Sets up to run a test.
 * Initialize various filenames, using the name of the test as a base,
 * and then initialize and run a command to remove old output from this test
 * and to make sure that the test output directory exists.
 */
int setup_test(char *name)
{
        FILE *f;
        size_t s;
        char *cmd = NULL;

	// Output directory in which to write test results.
        NEWSTREAM(f, s, test_output_dir);
        fprintf(f, "%s/%s", TEST_OUTPUT_DIR, name);
        fclose(f);

	// File in which to capture normal output from the test.
        NEWSTREAM(f, s, test_outfile);
        fprintf(f, "%s/test.out", test_output_dir);
        fclose(f);

	// File in which to capture error output from the test.
        NEWSTREAM(f, s, test_errfile);
        fprintf(f, "%s/test.err", test_output_dir);
        fclose(f);

	// File in which to alternate output from the test.
        NEWSTREAM(f, s, alt_outfile);
        fprintf(f, "%s/test.alt", test_output_dir);
        fclose(f);

	// Directory containing reference files.
        NEWSTREAM(f, s, ref_dir);
        fprintf(f, "%s/%s", TEST_RSRC_DIR, name);
        fclose(f);

	// Read-only reference input file for the test.
        NEWSTREAM(f, s, ref_infile);
        fprintf(f, "%s/ref.in", ref_dir);
        fclose(f);

	// Read-only reference normal output file for the test.
        NEWSTREAM(f, s, ref_outfile);
        fprintf(f, "%s/ref.out", ref_dir);
        fclose(f);

	// Read-only reference error output file for the test.
        NEWSTREAM(f, s, ref_errfile);
        fprintf(f, "%s/ref.err", ref_dir);
        fclose(f);

	// Run command to set up for test.
	// Right now this just deletes any existing test output directory
	// and creates a new one.
        NEWSTREAM(f, s, cmd);
        fprintf(f, "chmod -fR u+w %s; rm -fr %s; mkdir -p %s",
		test_output_dir, test_output_dir, test_output_dir);
        fclose(f);

        //fprintf(stderr, "setup(%s)\n", cmd);
        int ret = system(cmd);
        free(cmd);
        return ret;
}

/*
 * Run the program as a "black box" using system().
 * A shell command is constructed and run that first performs test setup,
 * then runs the program to be tested with input redirected from a test input
 * file and standard and error output redirected to separate output files.
 *
 * It is assumed that setup_test() has previously been called.
 */
int run_using_system(char *program_path, char *pre_cmd, char *valgrind_cmd,
                     char *program_options, char *limits)
{
        FILE *f;
        size_t s;
        char *cmd = NULL;
        struct stat sbuf;

        int staterr = stat(ref_infile, &sbuf);
        NEWSTREAM(f, s, cmd);
        fprintf(f, "%s%s%s %s %s < %s > %s 2> %s",
                limits, pre_cmd, valgrind_cmd,
                program_path, program_options,
                staterr ? "/dev/null" : ref_infile,
                test_outfile, test_errfile);
        fclose(f);
        fprintf(stderr, "run(%s)\n", cmd);
        int ret = system(cmd);
        free(cmd);
        return ret;
}

/*
 * The Wxxx macros have not worked properly with the return value from system(),
 * in spite of what the Linux man pages say about it.  So there is some fragile
 * hard-coding here to work around it.
 */

void assert_normal_exit(int status)
{
        cr_assert(/*!WIFSIGNALED(status)*/ !(status & 0x8000),
                  "The program terminated with an unexpected signal (%d).\n",
                  /* WTERMSIG(status) */ (status & 0x7f00) >> 8);
}

void assert_expected_status(int expected, int status)
{
        cr_assert(/*!WIFSIGNALED(status)*/ !(status & 0x8000),
                  "The program terminated with an unexpected signal (%d).\n",
                  /* WTERMSIG(status) */ (status & 0x7f00) >> 8);
        cr_assert_eq(
            WEXITSTATUS(status), expected,
            "The program did not exit with the expected status "
            "(expected 0x%x, was 0x%x).\n",
            expected, WEXITSTATUS(status));
}

void assert_signaled(int sig, int status)
{
        cr_assert(/*WIFSIGNALED(status)*/ status & 0x8000,
                  "The program did not terminate with a signal.\n");
        cr_assert(/*WTERMSIG(status)*/ (status & 0x7f00) >> 8 == sig,
                  "The program did not terminate with the expected signal "
                  "(expected %d, was %d).\n",
                  sig, WTERMSIG(status));
}

void assert_no_valgrind_errors(int status)
{
        cr_assert_neq(WEXITSTATUS(status), 37,
                      "Valgrind reported errors -- see %s.err",
                      test_outfile);
}

/*
 * Compare two files, after first possibly using "grep" to remove lines that
 * match a filter pattern.
 */
void assert_files_match(char *ref, char *test, char *filter)
{
        FILE *f;
        size_t s;
        char *cmd = NULL;
        
        NEWSTREAM(f, s, cmd);
        if (filter) {
                fprintf(f,
                        "grep -v '%s' %s > %s.flt && "
                        "grep -v '%s' %s > %s.flt && "
                        "diff --ignore-tab-expansion --ignore-trailing-space "
                        "--ignore-space-change --ignore-blank-lines "
                        "%s.flt %s.flt",
			filter, test, test,
			filter, ref, ref,
			ref, test);
        } else {
                fprintf(f,
                        "diff --ignore-tab-expansion --ignore-trailing-space "
                        "--ignore-space-change --ignore-blank-lines "
                        "%s %s",
			ref, test);
        }
        fclose(f);
        fprintf(stderr, "run(%s)\n", cmd);
        int err = system(cmd);
        free(cmd);
        cr_assert_eq(err, 0,
                     "The output was not what was expected (diff exited with "
                     "status %d).\n",
                     WEXITSTATUS(err));
}

/*
 * Compare two binary files.
 */
void assert_binaries_matches(char *ref, char *test)
{
        FILE *f;
        size_t s;
        char *cmd = NULL;

        NEWSTREAM(f, s, cmd);
        fprintf(f, "cmp %s %s", ref, test);
        fclose(f);
        int err = system(cmd);
        free(cmd);
        cr_assert_eq(err, 0,
                     "The output was not what was expected (cmp exited with "
                     "status %d).\n",
                     WEXITSTATUS(err));
}

/*
 * Compare the contents of two directories.
 */
void assert_dirs_match(char *ref, char *test)
{
        FILE *f;
        size_t s;
        char *cmd = NULL;
        
        NEWSTREAM(f, s, cmd);
	fprintf(f,
		"diff --recursive --ignore-tab-expansion --ignore-trailing-space "
		"--ignore-space-change --ignore-blank-lines "
		"%s %s",
		ref, test);
        fclose(f);
        fprintf(stderr, "run(%s)\n", cmd);
        int err = system(cmd);
        free(cmd);
        cr_assert_eq(err, 0,
                     "The output was not what was expected (diff exited with "
                     "status %d).\n",
                     WEXITSTATUS(err));
}
