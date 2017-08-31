/**
 * @file /magma/src/core/config/config_utils.h
 *
 * @brief	The global configuration structure used for overall system settings, and functions to initialize it at startup and free it at shutdown.
 */

#ifndef MAGMA_CORE_CONFIG_UTILS_H
#define MAGMA_CORE_CONFIG_UTILS_H

typedef struct {
	void *store; /* The location in memory to store the setting value. */
	multi_t norm; /* The default value. */
	chr_t *name; /* The search key/name of the setting. */
	chr_t *description; /* Description of the setting and its default value. */
	bool_t file; /* Can this value be set using the config file? */
	bool_t database; /* Can this value be set using the config file? */
	bool_t overwrite; /* Can this value be overwritten? */
	bool_t set; /* Has this setting already been provided? */
	bool_t required; /* Is this setting required? */
} magma_keys_t;

bool_t        cu_value_set(magma_keys_t *setting, stringer_t *value);
bool_t        cu_load_defaults(magma_keys_t *keys, uint64_t keys_size);
magma_keys_t* cu_key_lookup(magma_keys_t *keys, uint64_t keys_size, stringer_t *name);
bool_t        cu_load_file_settings(magma_keys_t *keys, uint64_t keys_size, const chr_t *config_file);
void          cu_free(magma_keys_t *keys, uint64_t keys_size);

#endif// MAGMA_CORE_CONFIG_UTILS_H
