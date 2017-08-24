
/**
 * @file /magma/core/host/host.h
 *
 * @brief	Provide access to system interfaces.
 */

#ifndef MAGMA_CORE_SYSTEM_H
#define MAGMA_CORE_SYSTEM_H


#define MAGMA_PROC_PATH "/proc"

/**
 * @typedef octet_t
 */
typedef int16_t octet_t;

/**
 * @typedef segment_t
 */
typedef int32_t segment_t;

/**
 * @typedef ip_t
 */
typedef struct {
	sa_family_t family;
	union {
		struct in_addr ip4;
		struct in6_addr ip6;
		void *ip;
	};
} ip_t;

/**
 * @typedef subnet_t
 */
typedef struct {
	uint32_t mask;
	ip_t address;
} subnet_t;

// The spool_start function uses a for loop to validate the spool directory tree. If additional
// spool locations are enumerated, make sure that function is updated.
enum {
	MAGMA_SPOOL_BASE = 0,
	MAGMA_SPOOL_DATA = 1,
	MAGMA_SPOOL_SCAN = 2
};


/// files.c
stringer_t *  file_load(char *name);
int_t         file_read(char *name, stringer_t *output);
int_t         get_temp_file_handle(chr_t *pdir, stringer_t **tmpname);
bool_t        file_accessible(const chr_t *path);
bool_t        file_readwritable(const chr_t *path);
bool_t        file_world_accessible(const chr_t *path);

/// tcp.c
ip_t *        tcp_addr_ip(int sockd, ip_t *output);
stringer_t *  tcp_addr_st(int sockd, stringer_t *output);
int           tcp_continue(int sockd, int result, int syserror);
int_t         tcp_error(int error);
int           tcp_read(int sockd, void *buffer, int length, bool_t block);
int_t         tcp_status(int sockd);
int           tcp_wait(int sockd);
int           tcp_write(int sockd, const void *buffer, int length, bool_t block);

/// host.c
stringer_t *  host_platform(stringer_t *output);
stringer_t *  host_version(stringer_t *output);

/// errors.c
chr_t *  errno_name(int error);

/// signals.c
chr_t *  signal_name(int signal, char *buffer, size_t length);

/// folder.c
int_t   folder_count(stringer_t *path, bool_t recursive, bool_t strict);
int_t   folder_exists(stringer_t *path, bool_t create);

/// process.c
pid_t   process_find_pid(stringer_t *name);
int_t   process_kill(stringer_t *name, int_t signum, int_t wait);
pid_t   process_my_pid(void);

/// spool.c
int_t         spool_check(stringer_t *path);
int_t         spool_check_file(const char *file, const struct stat *info, int type);
int_t         spool_cleanup(void);
uint64_t      spool_error_stats(void);
int_t         spool_mktemp(int_t spool, chr_t *prefix);
stringer_t *  spool_path(int_t spool);
bool_t        spool_start(void);
void          spool_stop(void);
typedef uint64_t (*rand_get_uint64_function)(void) ;
void          spool_set_rand_provider(rand_get_uint64_function generator);

/// ip.c
bool_t        ip_addr_eq(ip_t *ip1, ip_t *ip2);
ip_t *        ip_copy(ip_t *dst, ip_t *src);
int_t         ip_family(ip_t *address);
bool_t        ip_localhost(ip_t *address);
bool_t        ip_matches_subnet(subnet_t *subnet, ip_t *addr);
octet_t       ip_octet(ip_t *address, int_t position);
stringer_t *  ip_presentation(ip_t *address, stringer_t *output);
bool_t        ip_private(ip_t *address);
stringer_t *  ip_reversed(ip_t *address, stringer_t *output);
segment_t     ip_segment(ip_t *address, int_t position);
stringer_t *  ip_standard(ip_t *address, stringer_t *output);
bool_t        ip_addr_st(chr_t *ipstr, ip_t *out);
bool_t        ip_subnet_st(chr_t *substr, subnet_t *out);
stringer_t *  ip_subnet(ip_t *address, stringer_t *output);
int8_t        ip_type(ip_t *address);
uint32_t      ip_word(ip_t *address, int_t position);

#endif

