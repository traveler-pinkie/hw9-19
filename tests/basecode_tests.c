#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "test_common.h"

#define PROGRAM_PATH "bin/dosiero"

#define TEST_SUITE basecode_suite

/* "BLACKBOX" tests -- these run your program using 'system()' and check the results. */

/**
 * empty_args
 * @brief PROGRAM_PATH
 */

#define TEST_NAME empty_args
Test(TEST_SUITE, TEST_NAME, .timeout=TEST_TIMEOUT)
{
    setup_test(QUOTE(TEST_NAME));
    FILE *f; size_t s = 0; char *args = NULL; NEWSTREAM(f, s, args);
    fprintf(f, "%s", ""); fclose(f);
    int status = run_using_system(PROGRAM_PATH, "", "", args, STANDARD_LIMITS);
    // Error: Disk file must be specified with -f
    assert_expected_status(EXIT_FAILURE, status);
    // outfile must be empty
    assert_files_match(ref_outfile, test_outfile, NULL);
    // Contents of errfile are unspecified.
}
#undef TEST_NAME

/**
 * help_only
 * @brief PROGRAM_PATH -h
 */

#define TEST_NAME help_only
Test(TEST_SUITE, TEST_NAME, .timeout=TEST_TIMEOUT)
{
    setup_test(QUOTE(TEST_NAME));
    FILE *f; size_t s = 0; char *args = NULL; NEWSTREAM(f, s, args);
    fprintf(f, "-h"); fclose(f);
    int status = run_using_system(PROGRAM_PATH, "", "", args, STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, status);
    // outfile must be empty
    assert_files_match(ref_outfile, test_outfile, NULL);
    // errfile to contain Usage message in unspecified format
}
#undef TEST_NAME

/**
 * Extract /etc/passwd
 * @brief PROGRAM_PATH -f rsrc/unix-v5-boot.img -x /etc/passwd -n
 */

#define TEST_NAME extract_etc_passwd
Test(TEST_SUITE, TEST_NAME, .timeout=TEST_TIMEOUT)
{
    setup_test(QUOTE(TEST_NAME));
    FILE *f; size_t s = 0; char *args = NULL; NEWSTREAM(f, s, args);
    fprintf(f, "-f rsrc/unix-v5-boot.img -x /etc/passwd -n"); fclose(f);
    int status = run_using_system(PROGRAM_PATH, "", "", args, STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, status);
    // outfile should contain contents of /etc/passwd
    assert_files_match(ref_outfile, test_outfile, NULL);
    // errfile should be empty
    assert_files_match(ref_errfile, test_errfile, NULL);
}
#undef TEST_NAME

/**
 * List /usr/sys
 * @brief PROGRAM_PATH -f rsrc/unix-v5-boot.img -l /usr/sys -n
 */

#define TEST_NAME list_usr_sys
Test(TEST_SUITE, TEST_NAME, .timeout=TEST_TIMEOUT)
{
    setup_test(QUOTE(TEST_NAME));
    FILE *f; size_t s = 0; char *args = NULL; NEWSTREAM(f, s, args);
    fprintf(f, "-f rsrc/unix-v5-boot.img -l /usr/sys -n"); fclose(f);
    int status = run_using_system(PROGRAM_PATH, "", "", args, STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, status);
    // outfile should contain listing of /usr/sys
    assert_files_match(ref_outfile, test_outfile, NULL);
    // errfile should be empty
    assert_files_match(ref_errfile, test_errfile, NULL);
}
#undef TEST_NAME

/**
 * Resolve /usr/sys/ken/ to i-number
 * @brief PROGRAM_PATH -f rsrc/unix-v5-boot.img -r /usr/sys/ken/ -n
 */

#define TEST_NAME resolve_usr_sys_ken
Test(TEST_SUITE, TEST_NAME, .timeout=TEST_TIMEOUT)
{
    setup_test(QUOTE(TEST_NAME));
    FILE *f; size_t s = 0; char *args = NULL; NEWSTREAM(f, s, args);
    fprintf(f, "-f rsrc/unix-v5-boot.img -r /usr/sys/ken/"); fclose(f);
    int status = run_using_system(PROGRAM_PATH, "", "", args, STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, status);
    // outfile should contain i-number
    assert_files_match(ref_outfile, test_outfile, NULL);
    // errfile should be empty
    assert_files_match(ref_errfile, test_errfile, NULL);
}
#undef TEST_NAME

/**
 * Map i-number 465 to a canonical path
 * @brief PROGRAM_PATH -f rsrc/unix-v5-boot.img -p 465
 */

#define TEST_NAME canonical_path_465
Test(TEST_SUITE, TEST_NAME, .timeout=TEST_TIMEOUT)
{
    setup_test(QUOTE(TEST_NAME));
    FILE *f; size_t s = 0; char *args = NULL; NEWSTREAM(f, s, args);
    fprintf(f, "-f rsrc/unix-v5-boot.img -p 465"); fclose(f);
    int status = run_using_system(PROGRAM_PATH, "", "", args, STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, status);
    // outfile should contain pathname
    assert_files_match(ref_outfile, test_outfile, NULL);
    // errfile should be empty
    assert_files_match(ref_errfile, test_errfile, NULL);
}
#undef TEST_NAME
