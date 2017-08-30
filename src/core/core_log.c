/**
 * @file /magma/src/core/core_log.c
 *
 * @brief	Internal logging functions. This function should be accessed using the appropriate macro.
 *
 * @Author	Anatoly Supakov
 */

#include "core.h"

magma_core_logger_t magma_core_loging;

void magma_core_set_logger(magma_core_logger_t *info)
{
	if(!info)
	{
		//TODO: Error or default printf implementation?
		memset(&magma_core_loging, 0, sizeof(magma_core_loging));
	}
	else
	{
		memcpy(&magma_core_loging, info, sizeof(magma_core_loging));
	}
}

__attribute__((format (printf, 5, 6)))
void mclog_internal(const char *file, const char *function, const int line, M_LOG_OPTIONS options, const char *format, ...)
{
	va_list args;
	va_start(args, format);

	if(magma_core_loging.log_internal)
	{
		magma_core_loging.log_internal(file, function, line, options, format, args);
	}
	va_end(args);
}
