#include "random_generator_check.h"

#define log_pedantic(a)

static
int randomBytes(unsigned char *buffer, int num);

int randomBytes(unsigned char *buffer, int num) {
  
    if( ! (num >0) ) {
	log_pedantic("Size of buffer is NEGATIVE!!!");
	return -1;
    }
  
    /// buffer validity is already checked OK
    int placeTo = num;
    while(placeTo) {
	unsigned char chArbitrary = (unsigned char) rand();
	buffer[--placeTo] = chArbitrary;
    }
  
  return 1;
  
}

size_t random_write_check(stringer_t *s) {
  
  size_t len = 0;
	uchr_t *p = NULL;
	uint32_t opts = 0;

	if (!s || !(p = st_data_get(s)) || !st_valid_destination((opts = *((uint32_t *)s)))) {
		log_pedantic("The supplied string does not have a buffer capable of being written to.");
		return 0;
	}

	// If the string type supports it, store the buffer length instead of using the data length.
	else if (st_valid_avail(opts)) {
		len = st_avail_get(s);
	}
	else {
		len = st_length_get(s);
	}

/**********************
	/// belong to another domain : providers
	/// replaced with similar behaves one
	if (!RAND_bytes_d || RAND_bytes_d(p, len) != 1) {
		log_pedantic("Could not generate a random string of bytes.");
		return 0;
	}
*/

      if(randomBytes(p, len) != 1) {
	
		log_pedantic("Could not generate a random string of bytes.");
		  return 0;
 
      }
	

	if (st_valid_tracked(opts)) {
		st_length_set(s, len);
	}

	return len;
	
}

/// random get int content

int8_t rand_get_int8_check(void) {

	int8_t result;

	if (!randomBytes((unsigned char *)&result, sizeof(int8_t)) != 1) {

			log_pedantic("Entropy failure");

		}

	return result;
}

int16_t rand_get_int16_check(void) {

	int16_t result;

	if (!randomBytes((unsigned char *)&result, sizeof(int16_t)) != 1) {

			log_pedantic("Entropy failure");

		}


	return result;
}


int32_t rand_get_int32_check(void) {

	int32_t result;

	if (!randomBytes((unsigned char *)&result, sizeof(int32_t)) != 1) {

			log_pedantic("Entropy failure");

		}


	return result;
}

int64_t rand_get_int64_check(void) {

	int64_t result;

	if (!randomBytes((unsigned char *)&result, sizeof(int64_t)) != 1) {

			log_pedantic("Entropy failure");

		}


	return result;
}

/// random get unsigned int content

uint8_t rand_get_uint8_check(void) {

	uint8_t result;

	if (!randomBytes((unsigned char *)&result, sizeof(uint8_t)) != 1) {

			log_pedantic("Entropy failure");

		}


	return result;
}

uint16_t rand_get_uint16_check(void) {

	int16_t result;

	if (!randomBytes((unsigned char *)&result, sizeof(uint16_t)) != 1) {

			log_pedantic("Entropy failure");

		}


	return result;
}


uint32_t rand_get_uint32_check(void) {

	uint32_t result;

	if (!randomBytes((unsigned char *)&result, sizeof(uint32_t)) != 1) {

			log_pedantic("Entropy failure");

		}


	return result;
}

uint64_t rand_get_uint64_check(void) {

	uint64_t result;

	if (!randomBytes((unsigned char *)&result, sizeof(uint64_t)) != 1) {

			log_pedantic("Entropy failure");

		}


	return result;
}

