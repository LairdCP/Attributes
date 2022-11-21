/**
 * @file attr.h
 * @brief
 *
 * Copyright (c) 2021-2022 Laird Connectivity
 *
 * Copyright (c) 2011-2014, Wind River Systems, Inc.
 * Copyright (c) 2020, Nordic Semiconductor ASA
 * IS_ENABLED logic from sys/util_internal.h (modified).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ATTR_H__
#define __ATTR_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <zephyr.h>
#include <stdint.h>
#include <stddef.h>
#include <shell/shell.h>

#ifdef CONFIG_ATTR_BROADCAST
#include "Framework.h"
#endif

#include "attr_table.h"
#include "attr_util.h"

/******************************************************************************/
/* Global Constants, Macros and type Definitions                              */
/******************************************************************************/
#ifdef CONFIG_ATTR_BROADCAST

#define ATTR_BROADCAST_MSG_SIZE(count)                                         \
	(sizeof(attr_changed_msg_t) + (sizeof(attr_id_t) * count))

typedef struct attr_broadcast_msg {
	FwkMsgHeader_t header;
	size_t count;
	attr_id_t list[];
} attr_changed_msg_t;

BUILD_ASSERT(ATTR_TABLE_MAX_ID < (1 << (8 * sizeof(attr_id_t))),
	     "List element size too small");

/* A better check would be size * number of attr broadcast message receivers,
 * but that information isn't currently available at compile time.
 */
BUILD_ASSERT(ATTR_BROADCAST_MSG_SIZE(ATTR_TABLE_WRITABLE_COUNT) <
		     CONFIG_BUFFER_POOL_SIZE,
	     "Buffer pool too small for attribute changed broadcast");
#endif

enum attr_dump {
	ATTR_DUMP_RW = 0,
	ATTR_DUMP_W = 1,
	ATTR_DUMP_RO = 2,
};

/* Defines for setting/clearing flags via length parameter
 * used in the attr_set_flags and attr_clear_flags functions
 */
#define ATTR_FLAG_CLEAR 0
#define ATTR_FLAG_SET 1

/* Return 1 if attribute is present
 * This relies on names being unique enough across different projects.
 * (As does #ifdef ATTR_ID_<name>).
 */
#define IS_ATTR_ENABLED(an_id) IS_ATTR_ENABLED1(an_id)

#define IS_ATTR_ENABLED1(an_id) IS_ATTR_ENABLED2(_ATTRX##an_id)
#define IS_ATTR_ENABLED2(one_or_two_args) IS_ATTR_ENABLED3(one_or_two_args 1, 0)
#define IS_ATTR_ENABLED3(ignore_this, val, ...) val

/* If valid return ID of attribute, otherwise return invalid ID */
#define ATTR_ID(a_name) ATTR_NAME(a_name)

#ifdef CONFIG_ATTR_ID_CONDITIONAL_CODE_ENABLE
#define ATTR_NAME(a_name) COND_CODE_2_ATTR(ATTR_ID_##a_name)
#define COND_CODE_2_ATTR(an_id) COND_CODE_1_ATTR(an_id)
#define COND_CODE_1_ATTR(an_id)                                                \
	__COND_CODE_ATTR(_ATTRX##an_id, (an_id), (ATTR_TABLE_SIZE))
#define __COND_CODE_ATTR(one_or_two_args, _if_code, _else_code)                \
	__GET_ARG2_DE_BRACKET_ATTR(one_or_two_args _if_code, _else_code)
#define __GET_ARG2_DE_BRACKET_ATTR(ignore_this, val, ...) __DE_BRACKET_ATTR val
#define __DE_BRACKET_ATTR(...) __VA_ARGS__
#else
#define ATTR_NAME(a_name) ATTR_ID_##a_name
#endif

/******************************************************************************/
/* Function Definitions                                                       */
/******************************************************************************/

/**
 * @brief Set values to alt (except items configured during production).
 *
 * @note Read only values are also reset to their alt values.  Many of
 * these values are set during initialization.
 *
 * Therefore, it is most likely that a software reset is required after
 * running this function (so that status/read-only values are repopulated).
 *
 * @retval negative error code, 0 on success
 */
int attr_factory_reset(void);

/**
 * @brief Get the type of the attribute
 *
 * @retval type of variable
 */
enum attr_type attr_get_type(attr_id_t id);

/**
 * @brief Helper function
 *
 * @retval true if id is valid, false otherwise
 */
bool attr_valid_id(attr_id_t id);

/**
 * @brief Set value.  This is the only set function that should be
 * used from the SMP interface.  It requires the writable flag to be true.
 *
 * @note SMP notifications are disabled during set.
 *
 * @param id an attribute id.
 * @param type the type of attribute
 * @param pv string representation of variable
 * @param vlen The length (without null char) of the string
 * being passed in.  If the value isn't a string, then the length is
 * not used.
 * @param modified Will be set to true if parameter was modified or false if not
 * (can be supplied NULL to ignore)
 *
 * @retval negative error code, 0 on success
 */
int attr_set(attr_id_t id, enum attr_type type, void *pv, size_t vlen,
	     bool *modified);

/**
 * @brief Same as set value except broadcast is disabled.
 *
 * @note Used for control points that also reflect status.
 *
 * @retval negative error code, 0 on success
 */
int attr_set_without_broadcast(attr_id_t id, enum attr_type type, void *pv,
			       size_t vlen, bool *modified);

/**
 * @brief Copy an attribute. This can be used used from the SMP interface as it
 * checks the readable flag.
 *
 * @note Calls the prepare function on prepared read attributes
 * @note Sign extends up to int64 size when type is signed.
 *
 * @param id an attribute id.
 * @param pv pointer to location to copy string
 * @param vlen is the size of pv.
 *
 * @retval negative error code, size of value on return
 */
int attr_get(attr_id_t id, void *pv, size_t vlen);

/**
 * @brief Retrieve pointer of data item. This can be used used from the SMP
 * interface as it checks the readable flag.
 *
 * @note Calls the prepare function on prepared read attributes
 *
 * @param id an attribute id.
 * @param vlen will be updated with the size of the attribute or negative
 * error code.
 *
 * @retval pointer to value
 */
void *attr_get_pointer(attr_id_t id, int *vlen);

/**
 * @brief Get default value for an attribute.
 *
 * @param id an attribute id.
 * @param pv pointer to location to copy string
 * @param vlen is the size of pv.
 *
 * @retval negative error code, size of value on return
 */
int attr_get_default(attr_id_t id, void *pv, size_t vlen);

/**
 * @brief Set a string
 *
 * @param id an attribute id.
 * @param pv string representation of variable
 * @param vlen The length (without nul char) of the
 * string being passed in.
 *
 * @retval negative error code, 0 on success
 */
int attr_set_string(attr_id_t id, char const *pv, size_t vlen);

/**
 * @brief Set an array
 *
 * @param id an attribute id.
 * @param pv array
 * @param vlen The length of the array.
 *
 * @retval negative error code, 0 on success
 */
int attr_set_byte_array(attr_id_t id, char const *pv, size_t vlen);

/**
 * @brief Get pointer to quasi-static item.
 *
 * @param id an attribute id.
 *
 * @retval pointer if found, pointer to empty string if not found
 */

const void *attr_get_quasi_static(attr_id_t id);

/**
 * @brief Copy a string
 *
 * @param pv pointer to location to copy string
 * @param id an attribute id.
 * @param max_len is the size of pv.
 *
 * @retval negative error code, 0 on success
 */
int attr_copy_string(char *pv, attr_id_t id, size_t max_len);

/**
 * @brief Helper function for setting uint64
 *
 * @param id an attribute id.
 * @param value The value to set.
 *
 * @retval negative error code, 0 on success
 */
int attr_set_uint64(attr_id_t id, uint64_t value);

/**
 * @brief Helper function for setting int64
 *
 * @param id an attribute id.
 * @param value The value to set.
 *
 * @retval negative error code, 0 on success
 */
int attr_set_signed64(attr_id_t id, int64_t value);

/**
 * @brief Helper function for setting uint8, 16 or 32
 *
 * @param id an attribute id.
 * @param value The value to set.
 *
 * @retval negative error code, 0 on success
 */
int attr_set_uint32(attr_id_t id, uint32_t value);

/**
 * @brief Add a value to the current attribute value (uint8, 16 or 32)
 *
 * @param id An attribute id.
 * @param value The value to add.
 * @return int negative error code, 0 on success
 */
int attr_add_uint32(attr_id_t id, uint32_t value);

/**
 * @brief Helper function to set atomic_t flags.
 * With the value of bitmask to set bit(s) other bits may also be set or cleared.
 *
 * @param id an attribute id.
 * @param bitmask The bits to set.
 *
 * @retval negative error code, 0 on success
 */
int attr_set_flags(attr_id_t id, atomic_val_t bitmask);

/**
 * @brief Helper function for clearing atomic_t flags.
 * With the value of bitmask to clear bit(s) other bits may also be set or cleared.
 *
 * @param id an attribute id.
 * @param bitmask The bits to clear.
 *
 * @retval negative error code, 0 on success
 */
int attr_clear_flags(attr_id_t id, atomic_val_t bitmask);

/**
 * @brief Function to check if bits are active in the flag
 *
 * @param id an attribute id.
 * @param bitmask The bits to check.
 *
 * @retval false bits not set, true for bits set
 */
bool attr_are_flags_set(attr_id_t id, atomic_val_t bitmask);
/**
 * @brief Helper function for setting uint8, 16 or 32, without broadcast or
 * notification
 *
 * @param id an attribute id.
 * @param value The value to set.
 *
 * @retval negative error code, 0 on success
 */
int attr_set_no_broadcast_uint32(attr_id_t id, uint32_t value);

/**
 * @brief Helper function for setting int8, int16, or int32
 *
 * @param id an attribute id.
 * @param value The value to set.
 *
 * @retval negative error code, 0 on success
 */
int attr_set_signed32(attr_id_t id, int32_t value);

/**
 * @brief Accessor Function for uint64
 *
 * @param pv pointer to data
 * @param id an attribute id
 *
 * @retval negative error code, 0 on success
 */
int attr_copy_uint64(uint64_t *pv, attr_id_t id);

/**
 * @brief Accessor Function for int64
 *
 * @note Sign extends when underlying type isn't int64
 *
 * @param pv pointer to data
 * @param id an attribute id
 *
 * @retval negative error code, 0 on success
 */
int attr_copy_signed64(uint64_t *pv, attr_id_t id);

/**
 * @brief Accessor Function for uint32 (uint8, uint16, bool)
 *
 * @param pv pointer to data
 * @param id an attribute id
 *
 * @retval negative error code, 0 on success
 */
int attr_copy_uint32(uint32_t *pv, attr_id_t id);

/**
 * @brief Accessor Function for int32 (int8, int16)
 *
 * @note Sign extends when underlying type isn't int32
 *
 * @param pv pointer to data
 * @param id an attribute id
 *
 * @retval negative error code, 0 on success
 */
int attr_copy_signed32(int32_t *pv, attr_id_t id);

/**
 * @brief Used to set the value of a floating point attribute
 *
 * @param id an attribute id.
 * @param value The value to set.
 *
 * @retval negative error code, 0 on success
 */
int attr_set_float(attr_id_t id, float value);

/**
 * @brief Used to set the value of a bool attribute
 *
 * @param id an attribute id.
 * @param value The value to set.
 *
 * @retval negative error code, 0 on success
 */
int attr_set_bool(attr_id_t id, bool value);

/**
 * @brief Accessor Function for float
 *
 * @param pv pointer to data
 * @param id an attribute id
 *
 * @retval negative error code, 0 on success
 */
int attr_copy_float(float *pv, attr_id_t id);

/**
 * @brief Alternate Accessor function for uint64
 *
 * @param id an attribute id
 * @param alt value
 *
 * @retval alt value if not found, invalid id, or wrong type;
 * otherwise the attribute value
 */
uint64_t attr_get_uint64(attr_id_t id, uint64_t alt);

/**
 * @brief Alternate Accessor function for int64
 *
 * @note Sign extends when underlying type isn't int64
 *
 * @param id an attribute id
 * @param alt value
 *
 * @retval alt value if not found, invalid id, or wrong type;
 * otherwise the attribute value
 */
int64_t attr_get_signed64(attr_id_t id, int64_t alt);

/**
 * @brief Accessor function for bool
 *
 * @param id an attribute id
 *
 * @retval false if not found, otherwise value
 */
bool attr_get_bool(attr_id_t id);

/**
 * @brief Alternate Accessor function for uint32
 *
 * @param id an attribute id
 * @param alt value
 *
 * @retval alt value if not found, invalid id, or wrong type;
 * otherwise the attribute value
 */
uint32_t attr_get_uint32(attr_id_t id, uint32_t alt);

/**
 * @brief Alternate Accessor function for int32
 *
 * @note Sign extends when underlying type isn't int32
 *
 * @param id an attribute id
 * @param alt value
 *
 * @retval alt value if not found, invalid id, or wrong type;
 * otherwise the attribute value
 */
int32_t attr_get_signed32(attr_id_t id, int32_t alt);

/**
 * @brief Alternate Accessor function for float
 *
 * @param id an attribute id
 * @param alt value
 *
 * @retval alt value if not found, invalid id, or wrong type;
 * otherwise the attribute value
 */
float attr_get_float(attr_id_t id, float alt);

/**
 * @brief Get the name of an attribute
 *
 * @param id an attribute id
 *
 * @retval empty string if not found
 */
const char *attr_get_name(attr_id_t id);

/**
 * @brief Get the size of an attribute
 *
 * @param id an attribute id
 *
 * @retval size of attribute, size with null if string
 */
size_t attr_get_size(attr_id_t id);

/**
 * @brief Set/Clear bit in a 32-bit attribute
 *
 * @param id an attribute id
 * @param bit location to set
 * @param value 0 for clear, any other value for set
 *
 * @retval size of attribute, size with null if string
 */
int attr_set_mask32(attr_id_t id, uint8_t bit, uint8_t value);

/**
 * @brief Set/Clear bit in an 64-bit attribute
 *
 * @param id an attribute id
 * @param bit location to set
 * @param value 0 for clear, any other value for set
 *
 * @retval size of attribute, size with null if string
 */
int attr_set_mask64(attr_id_t id, uint8_t bit, uint8_t value);

/**
 * @brief Get the id (table index) of an attribute using the name
 *
 * @param name of the attribute
 *
 * @retval attr_id_t id of attribute
 */
attr_id_t attr_get_id(const char *name);

/**
 * @brief Print the value of an attribute
 *
 * @param shell pointer to shell instance
 * @param id an attribute id
 *
 * @retval negative error code, 0 on success
 *
 */
int attr_show(const struct shell *shell, attr_id_t id);

/**
 * @brief Print all parameters to the console using system workq.
 *
 * @param shell pointer to shell instance
 *
 * @retval negative error code, 0 on success
 *
 * @note For remote execution using mcumgr, SHELL_BACKEND_DUMMY_BUF_SIZE
 * must be set large enough to display all values
 */
int attr_show_all(const struct shell *shell);

/**
 * @brief Delete attribute file
 *
 * @note Can be used during development when layout of attributes has changed.
 *
 * @retval negative error code, 0 on success
 */
int attr_delete(void);

/**
 * @brief Print all parameters to the console using system workq.  The prepare
 * function is run for each attribute that it applies to.
 *
 * @param fstr pointer to file string
 * @param type the type of dump to perform
 *
 * @retval negative error code, number of parameters on success
 * If result is positive, then caller is responsible for freeing fstr.
 */
int attr_prepare_then_dump(char **fstr, enum attr_dump type);

/**
 * @brief Set the quiet flag for an attribute.
 * Settings are saved to filesystem.
 *
 * @param id of attribute
 * @param value true to make quiet, false allows printing
 *
 * @retval negative error code, otherwise 0
 */
int attr_set_quiet(attr_id_t id, bool value);

/**
 * @brief Load attributes from a file and save them to params.txt
 *
 * @note SMP notifications are disabled during load.
 *
 * @param abs_path Absolute file name
 * @param modified Will be set to true if parameters were modified or false if
 * not (can be supplied NULL to ignore)
 *
 * @retval negative error code, 0 on success (Bug 22990)
 */
int attr_load(const char *abs_path, bool *modified);

/**
 * @brief Notification callback
 *
 * @note override weak implementation in application
 *
 * @param id of attribute that has changed.
 *
 * @return int negative error code, zero on success
 */
int attr_notify(attr_id_t id);

/**
 * @brief Set the notify flag
 *
 * @param id of attribute
 * @param value new state
 *
 * @return int negative error code, 0 on success
 */
int attr_set_notify(attr_id_t id, bool value);

/**
 * @brief Accessor
 *
 * @param id of attribute
 *
 * @return bool true if attribute is notifiable over BLE, false if it
 * isn't or id isn't valid.
 */
bool attr_get_notify(attr_id_t id);

/**
 * @brief Disable all notifications
 *
 * @return int negative error code, 0 on success
 */
int attr_disable_notify(void);

/**
 * @brief Set an entry to its default value
 *
 * @param id of attribute
 *
 * @return int 0 on success, else negative errno.
 */
int attr_default(attr_id_t id);

/**
 * @brief Force saving of attribute data to the storage device
 *
 * @note Does nothing if deferred attribute saving is not enabled
 *
 * @return int 0 on success, else negative errno.
 */
int attr_force_save(void);

/**
 * @brief Increment configuration version attribute
 *
 * @return int 0 on success, else negative errno.
 */
int attr_increment_config_version(void);

/**
 * @brief Save status callback
 *
 * @note override weak implementation in application
 *
 * @param dirty false on attribute data saved to storage device, true on
 * attribute modification made which has yet to be saved.
 */
void attr_save_status_notification(bool dirty);

/**
 * @brief Get the string representation of an enumerated type
 *
 * @param id of attribute
 * @return const char* const If found, string representation of current
 * state, otherwise error string.
 */
const char *const attr_get_enum_string(attr_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* __ATTR_H__ */
