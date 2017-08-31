
/**
 * @file /check/magma/core/core_check.h
 *
 * @brief The entry point for the core module test suite.
 */

#ifndef CORE_CHECK_H
#define CORE_CHECK_H

/*
#include "core/core.h"
#include "core/strings/strings.h"
#include "core/indexes/indexes.h"
*/

#include "core.h"
#include "strings/strings.h"
#include "indexes/indexes.h"

#include <ctype.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <check.h>
#include <valgrind/valgrind.h>

extern int case_timeout;

//#define log_unit(...) log_internal (__FILE__, __FUNCTION__, __LINE__, M_LOG_LINE_FEED_DISABLE | M_LOG_TIME_DISABLE | M_LOG_FILE_DISABLE | M_LOG_LINE_DISABLE | M_LOG_FUNCTION_DISABLE | M_LOG_STACK_TRACE_DISABLE, __VA_ARGS__)
//#define testcase(s, tc, name, func) tcase_add_test((tc = tcase_create(name)), func); tcase_set_timeout(tc, case_timeout); suite_add_tcase(s, tc)

Suite * suite_check_sample(void);
void log_test(chr_t *test, stringer_t *error);
void suite_check_testcase(Suite *s, const char *tags, const char *name, TFun func);

#endif

//! Quick Test
#if 1
#define RUN_TEST_CASE_TIMEOUT 100
#define PROFILE_TEST_CASE_TIMEOUT 1000

#define INX_CHECK_MTHREADS 2
#define INX_CHECK_OBJECTS 1024

#define IP_CHECK_ROUNDS 10

#define TREE_INSERTS_CHECK 128
#define TREE_CURSORS_CHECK 128
#define LINKED_INSERTS_CHECK 128
#define LINKED_CURSORS_CHECK 128
#define HASHED_INSERTS_CHECK 128
#define HASHED_CURSORS_CHECK 128

#define QP_CHECK_SIZE 1024
#define URL_CHECK_SIZE 1024
#define HEX_CHECK_SIZE 1024
#define BASE64_CHECK_SIZE 1024
#define ZBASE32_CHECK_SIZE 1024
#define CHECKSUM_CHECK_SIZE 1024

#define QP_CHECK_ITERATIONS 16
#define URL_CHECK_ITERATIONS 16
#define HEX_CHECK_ITERATIONS 16
#define BASE64_CHECK_ITERATIONS 16
#define ZBASE32_CHECK_ITERATIONS 16
#define CHECKSUM_CHECK_ITERATIONS 16

#define TANK_CHECK_DATA_HNUM 1L
#define TANK_CHECK_DATA_UNUM 1L
#define TANK_CHECK_DATA_MTHREADS 2 // Disabled
#define TANK_CHECK_DATA_CLEANUP true

#define DSPAM_CHECK_SIZE_MIN 1024
#define DSPAM_CHECK_SIZE_MAX (2 * 1024)
#define DSPAM_CHECK_DATA_UNUM 1L
#define DSPAM_CHECK_ITERATIONS 128

// Controls the size of the compression test block.
#define COMPRESS_CHECK_SIZE_MIN 1024 // 1 kilobyte
#define COMPRESS_CHECK_SIZE_MAX (2 * 1024) // 2 kilobytes
#define COMPRESS_CHECK_MTHREADS 2 // Disabled
#define COMPRESS_CHECK_ITERATIONS 16

#define RAND_CHECK_SIZE_MIN 64
#define RAND_CHECK_SIZE_MAX 128
#define RAND_CHECK_ITERATIONS 128
#define RAND_CHECK_MTHREADS 2

#define SCRAMBLE_CHECK_SIZE_MIN (1024) // 1 kilobyte
#define SCRAMBLE_CHECK_SIZE_MAX (2 * 1024) // 2 kilobytes
#define SCRAMBLE_CHECK_ITERATIONS 16

#define DIGEST_CHECK_SIZE 1024
#define DIGEST_CHECK_ITERATIONS 16

#define SYMMETRIC_CHECK_SIZE_MIN 64
#define SYMMETRIC_CHECK_SIZE_MAX (2 * 1024) // 2 kilobytes
#define SYMMETRIC_CHECK_ITERATIONS 16

#define ECIES_CHECK_SIZE_MIN 64
#define ECIES_CHECK_SIZE_MAX (2 * 1024) // 2 kilobytes
#define ECIES_CHECK_ITERATIONS 16

#define PRIME_CHECK_SIZE_MIN 64
#define PRIME_CHECK_SIZE_MAX (2 * 1024) // 2 kilobytes
#define PRIME_CHECK_ITERATIONS 16
#define PRIME_CHECK_SPANNING_CHUNK_SIZE (1024 * 1024 * 20) // 20 megabytes

#define OBJECT_CHECK_ITERATIONS 16

#define MAIL_CHECK_LOAD_MAX 64 // Maximum number of messages loaded per user.

#define REGRESSION_CHECK_FILE_DESCRIPTORS_LEAK_MTHREADS 8

//! Exhaustive Test
#else

// Maximum number of seconds for a given test case.
#define RUN_TEST_CASE_TIMEOUT 86400
#define PROFILE_TEST_CASE_TIMEOUT 864000

#define INX_CHECK_MTHREADS 8
#define INX_CHECK_OBJECTS 8192

#define TREE_INSERTS_CHECK 8192
#define TREE_CURSORS_CHECK 8192
#define LINKED_INSERTS_CHECK 8192
#define LINKED_CURSORS_CHECK 8192
#define HASHED_INSERTS_CHECK 8192
#define HASHED_CURSORS_CHECK 8192

#define QP_CHECK_SIZE 8192
#define URL_CHECK_SIZE 8192
#define HEX_CHECK_SIZE 8192
#define BASE64_CHECK_SIZE 8192
#define ZBASE32_CHECK_SIZE 8192
#define CHECKSUM_CHECK_SIZE 8192

#define QP_CHECK_ITERATIONS 8192
#define URL_CHECK_ITERATIONS 8192
#define HEX_CHECK_ITERATIONS 8192
#define BASE64_CHECK_ITERATIONS 8192
#define ZBASE32_CHECK_ITERATIONS 8192
#define CHECKSUM_CHECK_ITERATIONS 8192

#define TANK_CHECK_DATA_HNUM 1L
#define TANK_CHECK_DATA_UNUM 1L
#define TANK_CHECK_DATA_MTHREADS 8
#define TANK_CHECK_DATA_CLEANUP true

#define DSPAM_CHECK_DATA_UNUM 1L
#define DSPAM_CHECK_ITERATIONS 8192
#define DSPAM_CHECK_SIZE_MIN 1024
#define DSPAM_CHECK_SIZE_MAX (16 * 1024)
//#define DSPAM_CHECK_SIZE_MAX (1 * 1024 * 1024) // 1 megabyte

#define COMPRESS_CHECK_MTHREADS 8
#define COMPRESS_CHECK_ITERATIONS 256
#define COMPRESS_CHECK_SIZE_MIN 1024 // 1 kilobyte
#define COMPRESS_CHECK_SIZE_MAX (16 * 1024)
//#define COMPRESS_CHECK_SIZE_MAX (1 * 1024 * 1024) // 1 megabyte

#define RAND_CHECK_MTHREADS 8
#define RAND_CHECK_ITERATIONS 256
#define RAND_CHECK_SIZE_MIN 1024 // 1 kilobyte
#define RAND_CHECK_SIZE_MAX (16 * 1024)
//#define RAND_CHECK_SIZE_MAX (1 * 1024 * 1024) // 1 megabyte

#define ECIES_CHECK_ITERATIONS 256
#define ECIES_CHECK_SIZE_MIN 1024 // 1 kilobyte
#define ECIES_CHECK_SIZE_MAX (16 * 1024)
//#define ECIES_CHECK_SIZE_MAX (1 * 1024 * 1024) // 1 megabyte

#define PRIME_CHECK_ITERATIONS 256
#define PRIME_CHECK_SIZE_MIN (1024) // 1 kilobyte
#define PRIME_CHECK_SIZE_MAX (1 * 1024 * 1024) // 1 megabyte
#define PRIME_CHECK_SPANNING_CHUNK_SIZE (1024 * 1024 * 256) // 256 megabytes

#define SCRAMBLE_CHECK_ITERATIONS 256
#define SCRAMBLE_CHECK_SIZE_MIN 1024 // 1 kilobyte
#define SCRAMBLE_CHECK_SIZE_MAX (16 * 1024)
//#define SCRAMBLE_CHECK_SIZE_MAX (1 * 1024 * 1024) // 1 megabyte

#define DIGEST_CHECK_ITERATIONS 256
#define DIGEST_CHECK_SIZE (16 * 1024)
//#define DIGEST_CHECK_SIZE (1 * 1024 * 1024) // 1 megabyte

#define SYMMETRIC_CHECK_ITERATIONS 256
#define SYMMETRIC_CHECK_SIZE_MIN 1024 // 1 kilobyte
#define SYMMETRIC_CHECK_SIZE_MAX (16 * 1024)
//#define SYMMETRIC_CHECK_SIZE_MAX (1 * 1024 * 1024) // 1 megabyte

#define OBJECT_CHECK_ITERATIONS 256

#define MAIL_CHECK_LOAD_MAX UINT64_MAX // Maximum number of messages loaded per user.

#define REGRESSION_CHECK_FILE_DESCRIPTORS_LEAK_MTHREADS 32

#endif


typedef struct {
	inx_t *inx;
	uint64_t type;
} check_inx_opt_t;

struct check_mi_t {
	int nr;
	char *name;
};

extern stringer_t *string_check_constant;

/// clamp_check.c
chr_t * check_clamp_max(void);
chr_t * check_clamp_min(void);
chr_t * check_clamp_randomizer(void);
chr_t * check_clamp_min_max_equal(void);
chr_t * check_clamp_min_max_invalid(void);

/// string_check.c
bool_t   check_string_alloc(uint32_t check);
bool_t   check_string_dupe(uint32_t check);
bool_t   check_string_import(void);
bool_t   check_string_merge(void);
bool_t   check_string_print(void);
bool_t   check_string_write(void);
bool_t   check_string_realloc(uint32_t check);

/// qp_check.c
bool_t   check_encoding_qp(void);

/// inx_check.c
bool_t    check_inx_cursor_mthread(check_inx_opt_t *opts);
void		  check_inx_cursor_mthread_cnv(check_inx_opt_t *opts);
bool_t    check_inx_cursor_sthread(check_inx_opt_t *opts);
bool_t    check_inx_mthread(check_inx_opt_t *opts);
void		  check_inx_mthread_cnv(check_inx_opt_t *opts);
bool_t    check_inx_sthread(check_inx_opt_t *opts);
bool_t	  check_inx_append_helper(inx_t *);
void	  check_inx_append_test(inx_t *);
bool_t 	  check_inx_append_sthread(MAGMA_INDEX, stringer_t*);
bool_t 	  check_inx_append_mthread(MAGMA_INDEX, stringer_t*);

/// ip_check.c
bool_t check_uint16_to_hex_st(uint16_t val, stringer_t *buff);
bool_t check_ip_private_sthread(stringer_t *errmsg);
bool_t check_ip_localhost_sthread(stringer_t *errmsg);

/// linked_check.c
bool_t   check_indexes_linked_cursor(char **errmsg);
bool_t   check_indexes_linked_cursor_compare(uint64_t values[], inx_cursor_t *cursor);
bool_t   check_indexes_linked_simple(char **errmsg);

/// hex_check.c
bool_t   check_encoding_hex(void);

/// url_check.c
bool_t   check_encoding_url(void);

/// core_check.c
Suite *                    suite_check_core(void);

/// base64_check.c
bool_t   check_encoding_base64(bool_t secure_on);
bool_t   check_encoding_base64_mod(bool_t secure_on);

/// hashed_check.c
bool_t   check_indexes_hashed_cursor(char **errmsg);
bool_t   check_indexes_hashed_cursor_compare(uint64_t values[], inx_cursor_t *cursor);
bool_t   check_indexes_hashed_simple(char **errmsg);

/// system_check.c
bool_t   check_system_errnonames(void);
bool_t   check_system_signames(void);

/// tree_check.c
bool_t   check_indexes_tree_cursor(char **errmsg);
bool_t   check_indexes_tree_cursor_compare(uint64_t values[], inx_cursor_t *cursor);
bool_t   check_indexes_tree_simple(char **errmsg);

/// qsort_check.c
int   check_bsearch_compare(const void *m1, const void *m2);
int   check_bsearch_months(int num, char *name);

/// zbase32_check.c
bool_t   check_encoding_zbase32(void);

/// nbo_check.c
bool_t   check_nbo_simple(void);
bool_t   check_nbo_parameters(void);

/// bitwise_check.c
bool_t   check_bitwise_parameters(void);
bool_t   check_bitwise_determinism(void);
bool_t   check_bitwise_simple(void);

/// checksum_check.c
bool_t check_checksum_fuzz_sthread(void);
bool_t check_checksum_fixed_sthread(void);
bool_t check_checksum_loop_sthread(void);

/// address_check.c
void check_address_octet_s (int _i CK_ATTRIBUTE_UNUSED);
void check_address_presentation_s (int _i CK_ATTRIBUTE_UNUSED);
void check_address_reversed_s (int _i CK_ATTRIBUTE_UNUSED);
void check_address_segment_s (int _i CK_ATTRIBUTE_UNUSED);
void check_address_standard_s (int _i CK_ATTRIBUTE_UNUSED);
void check_address_subnet_s (int _i CK_ATTRIBUTE_UNUSED);
bool_t check_ip_private_scheck(stringer_t *errmsg);
bool_t check_ip_localhost_scheck(stringer_t *errmsg);

