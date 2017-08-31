#include "../core.h"


/**
 * @brief	Set the value of a global config key.
 * @note	This function will also free the value of the global config key if it has already previously been set.
 * @param	setting		a pointer to the global key to have its value adjusted.
 * @param	value		a managed string containing the new key value, or if NULL, the key's default value will be used.
 * @return	true if the specified key's value was set successfully, or false on failure.
 */
bool_t cu_value_set(magma_keys_t *setting, stringer_t *value) {

	bool_t result = true;

	switch (setting->norm.type) {

	// Strings
	case (M_TYPE_NULLER):
		if (!ns_empty(*((char **)(setting->store)))) {
			ns_free(*((char **)(setting->store)));
			*((char **)(setting->store)) = NULL;
		}
		if (!st_empty(value))
			*((char **)(setting->store)) = ns_import(st_char_get(value), st_length_get(value));
		else if (!ns_empty(setting->norm.val.ns))
			*((char **)(setting->store)) = ns_dupe(setting->norm.val.ns);
		break;

	case (M_TYPE_STRINGER):
		if (!st_empty(*((stringer_t **)(setting->store)))) {
			st_free(*((stringer_t **)(setting->store)));
			*((stringer_t **)(setting->store)) = NULL;
		}
		if (!st_empty(value))
			*((stringer_t **)(setting->store)) = st_dupe_opts(MANAGED_T | CONTIGUOUS | HEAP, value);
		else if (!st_empty(setting->norm.val.st))
			*((stringer_t **)(setting->store)) = st_dupe_opts(MANAGED_T | CONTIGUOUS | HEAP, setting->norm.val.st);
		break;

		// Booleans
	case (M_TYPE_BOOLEAN):
		if (!st_empty(value)) {
			if (!st_cmp_ci_eq(value, CONSTANT("true")))
				*((bool_t *)(setting->store)) = true;
			else if (!st_cmp_ci_eq(value, CONSTANT("false")))
				*((bool_t *)(setting->store)) = false;
			else {
				mclog_critical("Invalid value for %s.", setting->name);
				result = false;
			}
		} else
			*((bool_t *)(setting->store)) = setting->norm.val.binary;
		break;

		// Integers
	case (M_TYPE_INT8):
		if (!st_empty(value)) {
			if (!int8_conv_st(value, (int8_t *)(setting->store))) {
				mclog_critical("Invalid value for %s.", setting->name);
				result = false;
			}
		} else
			*((int8_t *)(setting->store)) = setting->norm.val.i8;
		break;

	case (M_TYPE_INT16):
		if (!st_empty(value)) {
			if (!uint16_conv_st(value, (uint16_t *)(setting->store))) {
				mclog_critical("Invalid value for %s.", setting->name);
				result = false;
			}
		} else
			*((int16_t *)(setting->store)) = setting->norm.val.u16;
		break;

	case (M_TYPE_INT32):
		if (!st_empty(value)) {
			if (!int32_conv_st(value, (int32_t *)(setting->store))) {
				mclog_critical("Invalid value for %s.", setting->name);
				result = false;
			}
		} else
			*((int32_t *)(setting->store)) = setting->norm.val.i32;
		break;

	case (M_TYPE_INT64):
		if (!st_empty(value)) {
			if (!int64_conv_st(value, (int64_t *)(setting->store))) {
				mclog_critical("Invalid value for %s.", setting->name);
				result = false;
			}
		} else
			*((int64_t *)(setting->store)) = setting->norm.val.i64;
		break;

		// Unsigned Integers
	case (M_TYPE_UINT8):
		if (!st_empty(value)) {
			if (!uint8_conv_st(value, (uint8_t *)(setting->store))) {
				mclog_critical("Invalid value for %s.", setting->name);
				result = false;
			}
		} else
			*((uint8_t *)(setting->store)) = setting->norm.val.u8;
		break;

	case (M_TYPE_UINT16):
		if (!st_empty(value)) {
			if (!uint16_conv_st(value, (uint16_t *)(setting->store))) {
				mclog_critical("Invalid value for %s.", setting->name);
				result = false;
			}
		} else
			*((uint16_t *)(setting->store)) = setting->norm.val.u16;
		break;

	case (M_TYPE_UINT32):
		if (!st_empty(value)) {
			if (!uint32_conv_st(value, (uint32_t *)(setting->store))) {
				mclog_critical("Invalid value for %s.", setting->name);
				result = false;
			}
		} else
			*((uint32_t *)(setting->store)) = setting->norm.val.u32;
		break;

	case (M_TYPE_UINT64):
		if (!st_empty(value)) {
			if (!uint64_conv_st(value, (uint64_t *)(setting->store))) {
				mclog_critical("Invalid value for %s.", setting->name);
				result = false;
			}
		} else
			*((uint64_t *)(setting->store)) = setting->norm.val.u64;
		break;

	default:
		mclog_critical("The %s setting definition is using an invalid type.", setting->name);
		result = false;
		break;
	}
	return result;
}

/**
 * @brief	Load all the default values for non-required configuration options.
 * @return	true if all default magma config values were successfully loaded, or false on failure.
 */
bool_t cu_load_defaults(magma_keys_t *keys, uint64_t keys_size) {

	for (uint64_t i = 0; i < keys_size; i++) {
		if (!keys[i].required && !cu_value_set(&keys[i], NULL)) {
			mclog_info("%s has an invalid default value.", keys[i].name);
			cu_free(keys, keys_size);
			return false;
		}
	}

	return true;
}

/**
 * @brief	Get a magma config key by name.
 * @param	name	a managed string with the name of the magma config option to be looked up.
 * @return	NULL on failure or a pointer to the found magma key object on success.
 */
magma_keys_t * cu_key_lookup(magma_keys_t *keys, uint64_t keys_size, stringer_t *name) {

	magma_keys_t *result = NULL;

	for (uint64_t i = 0; !result && name && i < keys_size; i++) {
		if (!st_cmp_ci_eq(NULLER(keys[i].name), name)) {
			result = &keys[i];
		}
	}

	return result;
}

/**
 * @brief	Load the magma configuration file specified in config_file.
 * @note	Parses the config file data into a series of name/value pairs, and make sure that for each key:
 *	 			If a config option was loaded from the file, the key must allow it to be configurable via the file, and
 * 				If the key is required, it may not contain an empty value.
 * 			Finally, this function sets the appropriate magma key corresponding to the config key.
  * @return	true if all config file options were parsed and evaluated successfully, or false on failure.
 */
bool_t cu_load_file_settings(magma_keys_t *keys, uint64_t keys_size, const chr_t *config_file) {

	multi_t name;
	magma_keys_t *key;
	inx_cursor_t *cursor;
	nvp_t *config_pairs = NULL;
	stringer_t *file_data = NULL, *value;

	// Load the config file and convert it into a name/value pair structure.
	if (!(file_data = file_load(config_file))) {
		return false;
	}
	else if (!(config_pairs = nvp_alloc())) {
		st_free(file_data);
		return false;
	}
	else if (nvp_parse(config_pairs, file_data) < 0) {
		nvp_free(config_pairs);
		st_free(file_data);
		return false;
	}
	else if (!(cursor = inx_cursor_alloc(config_pairs->pairs))) {
		nvp_free(config_pairs);
		st_free(file_data);
		return false;
	}

	// Raw file data isn't needed any longer so free.
	st_free(file_data);

	// Run through all of the keys and see if there is a matching name/value pair.
	while (!mt_is_empty(name = inx_cursor_key_next(cursor))) {

		value = inx_cursor_value_active(cursor);

		if ((key = cu_key_lookup(keys, keys_size, name.val.st))) {

			// Make sure the setting can be provided via the configuration file.
			if (!key->file && value) {
					mclog_critical("%s cannot be changed using the configuration file.", key->name);
					inx_cursor_free(cursor);
					nvp_free(config_pairs);
					return false;
			}

			// Make sure the required keys are not set to NULL.
			else if (key->required && st_empty(value)) {
				mclog_critical("%s requires a legal value.", key->name);
				inx_cursor_free(cursor);
				nvp_free(config_pairs);
				return false;
			}

			// Attempt to set the value.
			else if (!cu_value_set(key, value)) {
				inx_cursor_free(cursor);
				nvp_free(config_pairs);
				return false;
			}

			// If a legit value was provided, then record that we've set this parameter.
			key->set = true;
		}

		// If we haven't had a match yet.
		else {
			mclog_critical("%.*s is not a valid setting.", st_length_int(name.val.st), st_char_get(name.val.st));
			inx_cursor_free(cursor);
			nvp_free(config_pairs);
			return false;
		}
	}

	inx_cursor_free(cursor);
	nvp_free(config_pairs);

	return true;
}

/**
 * @brief	Free all loaded in keys configuration options.
 * @note	Keys will be freed.
 * @return	This function returns no value.
 */
void cu_free(magma_keys_t *keys, uint64_t keys_size) {

	for (uint64_t i = 0; i < keys_size; i++) {
		switch (keys[i].norm.type) {

		case (M_TYPE_BLOCK):
			if (*((void **)(keys[i].store))) {
				mm_free(*((void **)(keys[i].store)));
			}
			break;
		case (M_TYPE_NULLER):
			if (*((char **)(keys[i].store))) {
				ns_free(*((char **)(keys[i].store)));
			}
			break;
		case (M_TYPE_STRINGER):
			if (*((stringer_t **)(keys[i].store))) {
				st_free(*((stringer_t **)(keys[i].store)));
			}
			break;
		default:
#ifdef MAGMA_PEDANTIC
			if (keys[i].norm.type != M_TYPE_BOOLEAN && keys[i].norm.type != M_TYPE_DOUBLE && keys[i].norm.type != M_TYPE_FLOAT &&
					keys[i].norm.type != M_TYPE_INT16 && keys[i].norm.type != M_TYPE_INT32 && keys[i].norm.type != M_TYPE_INT64 &&
					keys[i].norm.type != M_TYPE_INT8 && keys[i].norm.type != M_TYPE_UINT8 && keys[i].norm.type != M_TYPE_UINT16 &&
					keys[i].norm.type != M_TYPE_UINT32 && keys[i].norm.type != M_TYPE_UINT64 && keys[i].norm.type != M_TYPE_ENUM) {
				mclog_pedantic("Unexpected type. {type = %s = %u}", type(keys[i].norm.type), keys[i].norm.type);
			}
#endif
			break;
		}
	}
	return;
}

