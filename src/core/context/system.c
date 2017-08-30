
 /**
 * @file /magma/engine/context/system.c
 *
 * @brief	Functions used to interface with and configure the operating system.
 */

#include "../core.h"

/**
 * @brief	Get the hard system limit for a specified resource.
 * @note	This function will never return a value greater than UINT64_MAX.
 * @see		getrlimit64()
 * @param	resource	the system rlimit resource identifier to be queried.
 * @return	-1 on failure, or the system hard limit of the specified resource identifier on success.
 */
uint64_t system_ulimit_max(int_t resource) {

	int_t ret;
	struct rlimit64 limits = { 0, 0 };

	if ((ret = getrlimit64(resource, &limits))) {
		mclog_info("Unable to retrieve the resource limit. { resource = %i / return = %i / error = %s }", resource, ret, strerror_r(errno, bufptr, buflen));
		return -1;
	}

	if (limits.rlim_max > UINT64_MAX) {
		mclog_pedantic("The requested resource has a maximum value that exceeds the range of possible of return values. Returning the maximum possible value instead. "
			"{ resource = %i / actual = %lu / returning = %lu }", resource, limits.rlim_max, UINT64_MAX);
		return UINT64_MAX;
	}

	return (uint64_t)limits.rlim_max;
}

/**
 * @brief	Get the soft system limit for a specified resource.
 * @note	This function will never return a value greater than UINT64_MAX.
 * @see		getrlimit64()
 * @param	resource	the system rlimit resource identifier to be queried.
 * @return	-1 on failure, or the system soft limit of the specified resource identifier on success.
 */
uint64_t system_ulimit_cur(int_t resource) {

	int_t ret;
	struct rlimit64 limits = { 0, 0 };

	if ((ret = getrlimit64(resource, &limits))) {
		mclog_info("Unable to retrieve the resource limit. { resource = %i / return = %i / error = %s }", resource, ret, strerror_r(errno, bufptr, buflen));
		return -1;
	}

	if (limits.rlim_cur > UINT64_MAX) {
		mclog_pedantic("The requested resource is currently set to a value that exceeds the range of possible of return values. Returning the maximum possible value instead. "
			"{ resource = %i / actual = %lu / returning = %lu }", resource, limits.rlim_cur, UINT64_MAX);
		return UINT64_MAX;
	}

	return (uint64_t)limits.rlim_cur;
}

/**
 * @brief	Perform a chroot() on the directory specified in the config option magma_core.system.root_directory, if it is set.
 * @return	true on success or false on failure.
 */
bool_t system_change_root_directory(void) {

	if (magma_core.system.root_directory && chroot(magma_core.system.root_directory)) {
		mclog_info("Could not jail the process inside %s. { root = %s }", magma_core.system.root_directory,
				strerror_r(errno, bufptr, buflen));
		return false;
	}

	return true;
}

/**
 * @brief	Daemonize into the background, if the magma_core.system.daemonize config option is set.
 * @return	true inside the child process, or false inside the parent process or if an error occurs.
 */
bool_t system_fork_daemon(void) {

	pid_t pid;

	// If requested, fork into different processes and release the console session.
	if (magma_core.system.daemonize) {

		if ((pid = fork()) == -1) {
			mclog_info("Could not fork a background daemon process.");
			return false;
		}

		// We are the parent process.
		if (pid != 0) {
			exit(0);
		}

		// Make this the session group leader.
		if (setsid() < 0) {
			mclog_info("Could not become the session group leader.");
			return false;
		}

		// If file based logging is not enabled we'll need to close stdout/stderr so we don't retain a link to the console session.
		if (freopen64("/dev/null", "a+", stdout) == NULL) {
			return false;
		}
		if (freopen64("/dev/null", "a+", stderr) == NULL) {
			fclose(stdout);
			return false;
		}
		fclose(stdin);

		// Block debuggers from attacing to the process daemon and stealing sensitive data.
		if (prctl(PR_SET_DUMPABLE, 0) != 0) {
			mclog_critical("Failed to block debuggers from attaching to the process.");
			return false;
		}

		// Since the process description may have changed we need to refresh it.
		status_process();
	}

	return true;
}

/**
 * @brief	Set the process umask for new file/dir creation to O_RDWR.
 * @return	true if the umask was set successfully, or false on failure.
 */
bool_t system_init_umask(void) {

	// Attempt to set the process mask.
	umask(O_RDWR);

	// Verify that the mask was set by calling the function again.
	if ((O_RDWR) != umask(O_RDWR)) {
		mclog_info("Could not set the process umask { error = %s }", strerror_r(errno, bufptr, buflen));
		return false;
	}

	return true;
}

/**
 * @brief	Set the magma core dump size rlimit, if magma_core.system.enable_core_dumps was enabled.
 * @return	true if core dumps were successfully enabled or false on failure.
 */
bool_t system_init_core_dumps(void) {

	struct rlimit64 limits = {
		.rlim_cur = magma_core.system.core_dump_size_limit,
		.rlim_max = magma_core.system.core_dump_size_limit
	};

	if (magma_core.system.enable_core_dumps && setrlimit64(RLIMIT_CORE, &limits)) {
		mclog_info("The system does not allow core dumps. { error = %s}", strerror_r(errno, bufptr, buflen));
		return false;
	}

	return true;
}

/**
 * @brief	Set process privileges to run as a specified user if magma_core.system.impersonate_user is set.
 * @note	This function will set the user id and group id to the specified user, and chdir() to their home directory.
 * @return	true on success or if magma_core.system.impersonate_user is not set; false on failure to change privileges.
 */
bool_t system_init_impersonation(void) {

	int err;
	char *pwnam;
	size_t pwnam_len;
	struct passwd user, *result;

	if (magma_core.system.impersonate_user) {

		mm_wipe(&user, sizeof(struct passwd));

		if ((pwnam_len = sysconf(_SC_GETPW_R_SIZE_MAX)) == -1) {
			mclog_info("Unable to determine the required buffer size for the getpwnam_r function. { error = %s }", strerror_r(errno, bufptr, buflen));
			return false;
		} else if (!(pwnam = mm_alloc(pwnam_len))) {
			mclog_info("Unable to allocate the buffer required for the getpwnam_r function.");
			return false;
		}
		// Pull the user information.
		else if ((err = getpwnam_r(magma_core.system.impersonate_user, &user, pwnam, pwnam_len, &result)) || !result) {

			if (!result) {
				mclog_info("The user account %s does not exist.", magma_core.system.impersonate_user);
			} else {
				mclog_info("Unable to retrieve information for the user account %s. { error = %s }", magma_core.system.impersonate_user,
					strerror_r(errno, bufptr, buflen));
			}

			mm_free(pwnam);
			return false;
		}
		// Change into the user's home directory.
		else if (chdir(user.pw_dir)) {
			mclog_info("Unable to change into the %s directory which is the home for the user %s. { error = %s }", user.pw_dir, magma_core.system.impersonate_user,
				strerror_r(errno, bufptr, buflen));
			mm_free(pwnam);
			return false;
		}
		// Assume the proper group permissions.
		else if (getgid() != user.pw_gid && setgid(user.pw_gid)) {
			mclog_info("Unable to assume the group id %i. { error = %s }", user.pw_gid,
				strerror_r(errno, bufptr, buflen));
			mm_free(pwnam);
			return false;
		}
		// Begin impersonating the user.
		else if (getuid() != user.pw_uid && setuid(user.pw_uid)) {
			mclog_info("Unable to begin impersonating the user %s. { error = %s }", magma_core.system.impersonate_user,
				strerror_r(errno, bufptr, buflen));
			mm_free(pwnam);
			return false;
		}

		mm_free(pwnam);

		// Block debuggers from attacing to the process daemon and stealing sensitive data.
		if (prctl(PR_SET_DUMPABLE, 0) != 0) {
			mclog_critical("Failed to block debuggers from attaching to the process.");
			return false;
		}

		// Since the process description may have changed we need to refresh it.
		status_process();
	}

	return true;
}

/**
 * @brief	Increase process resource limits, if magma_core.system.increase_resource_limits is set.
 * @note	Resource limits will be maximized for magma's virtual address space, data and stack segments, available file descriptors and
 * 			sub-processes, and allowed file sizes.
 * 			If output_resource_limits is enabled, the state of the process resource limits will be dumped to the log afterwards.
 * @return	This function always returns true (errors are logged).
 */
bool_t system_init_resource_limits(void) {

	struct rlimit64 limits = {
		.rlim_cur = RLIM64_INFINITY,
		.rlim_max = RLIM64_INFINITY
	};
	chr_t *errbuf = MEMORYBUF(1024);

	if (magma_core.system.increase_resource_limits) {

		// Address Space
		if (setrlimit64(RLIMIT_AS, &limits) && magma_core.config.output_resource_limits) {
			mclog_info("Unable to increase the address space limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}

		// Data Segment
		if (setrlimit64(RLIMIT_DATA, &limits) && magma_core.config.output_resource_limits) {
			mclog_info("Unable to increase the data segment limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}

		// Stack Size
		if (setrlimit64(RLIMIT_STACK, &limits) && magma_core.config.output_resource_limits) {
			mclog_info("Unable to increase the stack size limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}

		// File Size
		if (setrlimit64(RLIMIT_FSIZE, &limits) && magma_core.config.output_resource_limits) {
			mclog_info("Unable to increase the file size limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}

		// Number of Threads/Processes
		if (setrlimit64(RLIMIT_NPROC, &limits) && magma_core.config.output_resource_limits) {
			mclog_info("Unable to increase the thread limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}

		// Locked Memory
		if (setrlimit64(RLIMIT_MEMLOCK, &limits) && magma_core.config.output_resource_limits) {
			mclog_info("Unable to increase the locked memory limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}

		// File Descriptors
		limits.rlim_cur = limits.rlim_max = 1048576;

		if (setrlimit64(RLIMIT_NOFILE, &limits) && magma_core.config.output_resource_limits) {
			mclog_info("Unable to increase the file descriptor limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}

	}

	if (magma_core.config.output_resource_limits) {

		mclog_info("---------------------------- RESOURCES LIMITS ----------------------------");

		// Core Dumps
		if (getrlimit64(RLIMIT_CORE, &limits)) {
			mclog_info("Unable to retrieve the core dump size limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}
		else if (limits.rlim_cur == RLIM64_INFINITY) {
			mclog_info("RLIMIT_CORE = RLIM64_INFINITY");
		}
		else {
			mclog_info("RLIMIT_CORE = %ld", limits.rlim_cur);
		}

		// Address Space
		if (getrlimit64(RLIMIT_AS, &limits)) {
			mclog_info("Unable to retrieve the address space limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}
		else if (limits.rlim_cur == RLIM64_INFINITY) {
			mclog_info("RLIMIT_AS = RLIM64_INFINITY");
		}
		else {
			mclog_info("RLIMIT_AS = %ld", limits.rlim_cur);
		}

		// Data Segment
		if (getrlimit64(RLIMIT_DATA, &limits)) {
			mclog_info("Unable to retrieve the data segment limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}
		else if (limits.rlim_cur == RLIM64_INFINITY) {
			mclog_info("RLIMIT_DATA = RLIM64_INFINITY");
		}
		else {
			mclog_info("RLIMIT_DATA = %ld", limits.rlim_cur);
		}

		// Stack Size
		if (getrlimit64(RLIMIT_STACK, &limits)) {
			mclog_info("Unable to retrieve the stack size limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}
		else if (limits.rlim_cur == RLIM64_INFINITY) {
			mclog_info("RLIMIT_STACK = RLIM64_INFINITY");
		}
		else {
			mclog_info("RLIMIT_STACK = %ld", limits.rlim_cur);
		}

		// File Size
		if (getrlimit64(RLIMIT_FSIZE, &limits)) {
			mclog_info("Unable to retrieve the file size limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}
		else if (limits.rlim_cur == RLIM64_INFINITY) {
			mclog_info("RLIMIT_FSIZE = RLIM64_INFINITY");
		}
		else {
			mclog_info("RLIMIT_FSIZE = %ld", limits.rlim_cur);
		}

		// Number of Threads/Processes
		if (getrlimit64(RLIMIT_NPROC, &limits)) {
			mclog_info("Unable to retrieve the thread limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}
		else if (limits.rlim_cur == RLIM64_INFINITY) {
			mclog_info("RLIMIT_NPROC = RLIM64_INFINITY");
		}
		else {
			mclog_info("RLIMIT_NPROC = %ld", limits.rlim_cur);
		}

		// Locked Memory
		if (getrlimit64(RLIMIT_MEMLOCK, &limits)) {
			mclog_info("Unable to retrieve the locked memory limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}
		else if (limits.rlim_cur == RLIM64_INFINITY) {
			mclog_info("RLIMIT_MEMLOCK = RLIM64_INFINITY");
		}
		else {
			mclog_info("RLIMIT_MEMLOCK = %ld", limits.rlim_cur);
		}

		// File Descriptors
		if (getrlimit64(RLIMIT_NOFILE, &limits)) {
			mclog_info("Unable to retrieve the file descriptor limit. { error = %s }", strerror_r(errno, errbuf, 1024));
		}
		else if (limits.rlim_cur == RLIM64_INFINITY) {
			mclog_info("RLIMIT_NOFILE = RLIM64_INFINITY");
		}
		else {
			mclog_info("RLIMIT_NOFILE = %ld", limits.rlim_cur);
		}

		mclog_info(" ");
	}

	return true;
}
