/**
 * @file attribute_mgmt.h
 *
 * @brief SMP interface for Attribute Command Group
 *
 * Copyright (c) 2021-2022 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATTRIBUTE_MGMT_H__
#define __ATTRIBUTE_MGMT_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <zephyr/types.h>

/******************************************************************************/
/* Local Constant, Macro and Type Definitions                                 */
/******************************************************************************/
/**
 * Command IDs for attribute management group.
 *
 * @note Location zero isn't used and is reserved for future use
 * @note IDs cannot be changed or re-ordered once set and all handlers must
 * exist in all products, even if they are not used, if a handler is not
 * available for a particular product and/or configuration then it should
 * return MGMT_ERR_ENOTSUP
 */
typedef enum {
	ATTRIBUTE_MGMT_ID_GET_PARAMETER = 1,
	ATTRIBUTE_MGMT_ID_SET_PARAMETER,
	ATTRIBUTE_MGMT_ID_LOAD_PARAMETER_FILE,
	ATTRIBUTE_MGMT_ID_DUMP_PARAMETER_FILE,
	ATTRIBUTE_MGMT_ID_FACTORY_RESET,
	ATTRIBUTE_MGMT_ID_SET_NOTIFY,
	ATTRIBUTE_MGMT_ID_GET_NOTIFY,
	ATTRIBUTE_MGMT_ID_DISABLE_NOTIFY,
	DEPRECATED_ATTRIBUTE_MGMT_ID_CHECK_LOCK_STATUS,
	DEPRECATED_ATTRIBUTE_MGMT_ID_SET_LOCK_CODE,
	DEPRECATED_ATTRIBUTE_MGMT_ID_LOCK,
	DEPRECATED_ATTRIBUTE_MGMT_ID_UNLOCK,
	ATTRIBUTE_MGMT_ID_GET_UNLOCK_ERROR_CODE,
	DEPRECATED_ATTRIBUTE_MGMT_ID_GET_API_VERSION,
	DEPRECATED_ATTRIBUTE_MGMT_ID_GET_INDICES,
	DEPRECATED_ATTRIBUTE_MGMT_ID_GET_ENTRY_DETAILS
} ATTRIBUTE_MGMT_id_t;

#ifdef __cplusplus
}
#endif

#endif /* __ATTRIBUTE_MGMT_H__ */
