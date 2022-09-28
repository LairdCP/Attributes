/**
 * @file attr_hash.h
 * @brief Header file for gperf hash of attribute names to IDs
 *
 * Copyright (c) 2022 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ATTR_HASH_H__
#define __ATTR_HASH_H__

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <zephyr.h>
#include <zephyr/types.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Global Constants, Macros and Type Definitions                              */
/******************************************************************************/
struct attr_hash_entry {
	const char *name;
	int id;
};

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/
const struct attr_hash_entry *attr_id_from_hash(register const char *str,
						register size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __ATTR_HASH_H__ */
