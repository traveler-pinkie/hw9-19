#include <criterion/criterion.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#define TEST_TIMEOUT 15

#define TEST_RSRC_DIR "tests/rsrc"
#define TEST_OUTPUT_DIR "test_output"
#define REF_BIN_DIR TEST_RSRC_DIR"/bin"
#define STANDARD_LIMITS "ulimit -t 10; ulimit -f 2000;"

#define QUOTE1(x) #x
#define QUOTE(x) QUOTE1(x)

#define NEWSTREAM(f, s, v) \
do { \
    if((v) != NULL) \
        free(v); \
    (f) = open_memstream(&(v), &(s)); \
} while(0)

#define ASSERT_RETURN(exp_ret) do { \
    cr_assert_eq(ret, exp_ret, \
		 "Wrong return value from function: expected %d, was %d\n", \
                 exp_ret, ret); \
  } while(0)

#define REDIRECT_STDOUT do { \
    FILE *ret = freopen(test_outfile, "w", stdout); \
    cr_assert(ret != NULL, "Failed to redirect stdout to '%s'", test_outfile); \
  } while(0)

#define REDIRECT_STDERR do { \
    FILE *ret = freopen(test_errfile, "w", stderr); \
    cr_assert(ret != NULL, "Failed to redirect stderr to '%s'", test_errfile); \
  } while(0)

#define REDIRECT_STDIN do { \
    FILE *ret = freopen(ref_infile, "r", stdin); \
    cr_assert(ret != NULL, "Failed to redirect stdin from '%s'", ref_infile); \
  } while(0)

#define CREATE_DIRECTORY(name) do { \
    int ret = mkdir(name, 0777); \
    if(ret == -1) \
	cr_assert_fail("Unable to create directory: '%s'", name); \
  } while(0)

extern int errors, warnings;

extern char *test_output_dir;   // Directory to which to write test output
extern char *test_outfile;	// Normal output from test
extern char *test_errfile;	// Error output from test
extern char *alt_outfile;	// Alternate output from test

extern char *ref_dir;           // Directory containing reference data
extern char *ref_infile;	// Read-only reference input
extern char *ref_outfile;	// Read-only reference normal output
extern char *ref_errfile;	// Read-only reference error output

int setup_test(char *name);
int run_using_system(char *program_path, char *pre_cmd, char *valgrind_cmd,
                     char *program_options, char *limits);

void assert_normal_exit(int status);
void assert_error_exit(int status);
void assert_expected_status(int expected, int status);
void assert_signaled(int sig, int status);
void assert_no_valgrind_errors(int status);

void assert_files_match(char *ref, char *test, char *filter);
void assert_binaries_match(char *ref, char *test);
void assert_dirs_match(char *ref, char *test);
