/**
 * @file sentrius_mgmt.h
 *
 * @brief SMP interface for Sentrius Command Group
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SENTRIUS_MGMT_H__
#define __SENTRIUS_MGMT_H__

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
 * Command IDs for sentrius management group.
 *
 * @note Location zero isn't used and is reserved for future use
 * @note IDs cannot be changed or re-ordered once set and all handlers must
 * exist in all products, even if they are not used, if a handler is not
 * available for a particular product and/or configuration then it should
 * return MGMT_ERR_ENOTSUP
 */
typedef enum {
	SENTRIUS_MGMT_ID_GET_PARAMETER = 1,
	SENTRIUS_MGMT_ID_SET_PARAMETER,
	SENTRIUS_MGMT_ID_SET_RTC,
	SENTRIUS_MGMT_ID_GET_RTC,
	SENTRIUS_MGMT_ID_LOAD_PARAMETER_FILE,
	SENTRIUS_MGMT_ID_DUMP_PARAMETER_FILE,
	SENTRIUS_MGMT_ID_FACTORY_RESET,
	SENTRIUS_MGMT_ID_SET_NOTIFY,
	SENTRIUS_MGMT_ID_GET_NOTIFY,
	SENTRIUS_MGMT_ID_DISABLE_NOTIFY,
	SENTRIUS_MGMT_ID_CHECK_LOCK_STATUS,
	SENTRIUS_MGMT_ID_SET_LOCK_CODE,
	SENTRIUS_MGMT_ID_LOCK,
	SENTRIUS_MGMT_ID_UNLOCK,
	SENTRIUS_MGMT_ID_GET_UNLOCK_ERROR_CODE
} sentrius_mgmt_id_t;

#define SENTRIUS_MGMT_HANDLER_CNT                                              \
	(sizeof SENTRIUS_MGMT_HANDLERS / sizeof SENTRIUS_MGMT_HANDLERS[0])

#ifdef __cplusplus
}
#endif

#endif /* __SENTRIUS_MGMT_H__ */
