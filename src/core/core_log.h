
/**
 * @file /magma/src/core/core_log.h
 *
 * @brief	The function declarations and macros needed to access external error log subsystem in core.
 *
 * @Author	Anatoly Supakov
 */

#ifndef MAGMA_CORE_LOGGING_H
#define MAGMA_CORE_LOGGING_H

// Options used to control the behavior of the log subsystem.
typedef enum {
	M_LOG_PEDANTIC = 1,
	M_LOG_INFO,
	M_LOG_ERROR,
	M_LOG_CRITICAL,
	M_LOG_TIME,
	M_LOG_FILE,
	M_LOG_LINE,
	M_LOG_FUNCTION,
	M_LOG_STACK_TRACE,
	M_LOG_PEDANTIC_DISABLE,
	M_LOG_INFO_DISABLE,
	M_LOG_ERROR_DISABLE,
	M_LOG_CRITICAL_DISABLE,
	M_LOG_LINE_FEED_DISABLE,
	M_LOG_TIME_DISABLE,
	M_LOG_FILE_DISABLE,
	M_LOG_LINE_DISABLE,
	M_LOG_FUNCTION_DISABLE,
	M_LOG_STACK_TRACE_DISABLE
} M_LOG_OPTIONS;

// All of the different log levels.
#define MAGMA_LOG_LEVELS (M_LOG_PEDANTIC | M_LOG_INFO | M_LOG_ERROR | M_LOG_CRITICAL)

void mclog_internal(const char *file, const char *function, const int line, M_LOG_OPTIONS options, const char *format, ...) __attribute__((format (printf, 5, 6)));
typedef void (*magma_core_log_internal_callback)(const char *file, const char *function, const int line, M_LOG_OPTIONS options, const char *format, va_list args);

typedef struct {
	magma_core_log_internal_callback log_internal;
	//TODO: other functions
} magma_core_logger_t;

void magma_core_set_logger(magma_core_logger_t *info);

extern magma_core_logger_t magma_core_loging;

#undef mclog_pedantic
#undef mclog_check
#undef mclog_info
#undef mclog_error
#undef mclog_critical
#undef mclog_options

#ifndef MAGMA_PEDANTIC
	#define mclog_pedantic(...) do {} while (0)
	#define mclog_check(expr) do {} while (0)
#endif

#ifndef MAGMA_LOG_CONSOLE

#ifdef MAGMA_PEDANTIC
	// Macro used record debug information during development.
	#define mclog_pedantic(...) mclog_internal (__FILE__, __FUNCTION__, __LINE__, M_LOG_PEDANTIC, __VA_ARGS__)
	// Log an error message if the specified conditional evaluates to true.
	#define mclog_check(expr) do { if (expr) mclog_internal (__FILE__, __FUNCTION__, __LINE__, M_LOG_PEDANTIC, __STRING (expr)); } while (0)
#else
	#define mclog_pedantic(...) do {} while (0)
	#define mclog_check(expr) do {} while (0)
#endif

#ifndef MAGMA_LOG_CONSOLE

	// Used to record information related to daemon performance.
	#define mclog_info(...) mclog_internal (__FILE__, __FUNCTION__, __LINE__, M_LOG_INFO, __VA_ARGS__)
	// Used to log errors that may indicate a problem requiring user intervention to solve.
	#define mclog_error(...) mclog_internal (__FILE__, __FUNCTION__, __LINE__, M_LOG_ERROR, __VA_ARGS__)
	// Used to record errors that could cause system instability.
	#define mclog_critical(...) mclog_internal (__FILE__, __FUNCTION__, __LINE__, M_LOG_CRITICAL, __VA_ARGS__)
	// Used to override the globally configured log options for a specific entry.
	#define mclog_options(options, ...) mclog_internal (__FILE__, __FUNCTION__, __LINE__, options, __VA_ARGS__)
#else// MAGMA_LOG_CONSOLE

#ifdef MAGMA_PEDANTIC
	#define mclog_pedantic(...) printf(__VA_ARGS__)
	#define mclog_check(expr) do {} while (0)
#endif

	#define mclog_info(...) printf(__VA_ARGS__)
	#define mclog_error(...) printf(__VA_ARGS__)
	#define mclog_critical(...) printf(__VA_ARGS__)
	#define mclog_options(options, ...) printf(__VA_ARGS__)
#endif// MAGMA_LOG_CONSOLE

#endif // MAGMA_CORE_LOGGING_H

