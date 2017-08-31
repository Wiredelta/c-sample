#ifndef __RANDOM_WRITE_CHECK__H__
#define __RANDOM_WRITE_CHECK__H__

#include "core.h"
//#include "strings/strings.h"

size_t random_write_check(stringer_t *s);

int8_t rand_get_int8_check(void);
int16_t rand_get_int16_check(void);
int32_t rand_get_int32_check(void);
int64_t rand_get_int64_check(void);

uint8_t rand_get_uint8_check(void);
uint16_t rand_get_uint16_check(void);
uint32_t rand_get_uint32_check(void);
uint64_t rand_get_uint64_check(void);

#endif // __RANDOM_WRITE_CHECK__H__
