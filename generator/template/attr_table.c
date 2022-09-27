/**
 * @file attr_table.c
 * @brief
 *
 * Copyright (c) 2022 Laird Connectivity LLC
 *
 * SPDX-License-Identifier: LicenseRef-LairdConnectivity-Clause
 */

/**************************************************************************************************/
/* Includes                                                                                       */
/**************************************************************************************************/
#include <zephyr.h>
#include <string.h>
#include <errno_str.h>
#include "attr_validator.h"
#include "attr_table.h"

#ifdef CONFIG_ATTR_CUSTOM_VALIDATOR
#include "attr_custom_validator.h"
#endif

/**************************************************************************************************/
/* Local Constant, Macro and Type Definitions                                                     */
/**************************************************************************************************/
#define DRW DEFAULT_RW_ATTRIBUTE_VALUES
#define DRO DEFAULT_RO_ATTRIBUTE_VALUES
#define y true
#define n false

/* pystart - rw attributes */
/* pyend */

/* pystart - rw defaults */
/* pyend */

/* pystart - ro attributes */
/* pyend */

/* pystart - ro defaults */
/* pyend */

/**************************************************************************************************/
/* Local Data Definitions                                                                         */
/**************************************************************************************************/
static rw_attribute_t rw;
static ro_attribute_t ro;

/**************************************************************************************************/
/* Global Data Definitions                                                                        */
/**************************************************************************************************/
/**
 * @brief Table shorthand
 *
 * @ref CreateStruct (Python script)
 * Writable but non-savable values are populated using RO macro.
 *
 *.........name...value...default....size...writable..readable..get enum str
 */
#define RW_ATTRS(n) STRINGIFY(n), rw.n, DRW.n, sizeof(rw.n), NULL
#define RW_ATTRX(n) STRINGIFY(n), &rw.n, &DRW.n, sizeof(rw.n), NULL
#define RW_ATTRE(n) STRINGIFY(n), &rw.n, &DRW.n, sizeof(rw.n), attr_get_string_ ## n
#define RO_ATTRS(n) STRINGIFY(n), ro.n, DRO.n, sizeof(ro.n), NULL
#define RO_ATTRX(n) STRINGIFY(n), &ro.n, &DRO.n, sizeof(ro.n), NULL
#define RO_ATTRE(n) STRINGIFY(n), &ro.n, &DRO.n, sizeof(ro.n), attr_get_string_ ## n

/* If min == max then range isn't checked.
 *
 * index....name.....................type.flags.validator..prepare..min.max.
 */
/* pystart - table */
/* pyend */

/**************************************************************************************************/
/* Local Function Definitions                                                                     */
/**************************************************************************************************/
/* pystart - get string */
/* pyend */

/**************************************************************************************************/
/* Global Function Definitions                                                                    */
/**************************************************************************************************/
void attr_table_initialize(void)
{
	memcpy(&rw, &DRW, sizeof(rw_attribute_t));
	memcpy(&ro, &DRO, sizeof(ro_attribute_t));
}

void attr_table_factory_reset(void)
{
	size_t i = 0;
	for (i = 0; i < ATTR_TABLE_SIZE; i++) {
		memcpy(ATTR_TABLE[i].pData, ATTR_TABLE[i].pDefault, ATTR_TABLE[i].size);
	}
}

const struct attr_table_entry *const attr_map(attr_id_t id)
{
	if (id == 0 || id > ATTR_TABLE_MAX_ID) {
		return NULL;
	} else {
		return &ATTR_TABLE[id];
	}
}

attr_index_t attr_table_index(const struct attr_table_entry *const entry)
{
	__ASSERT(PART_OF_ARRAY(ATTR_TABLE, entry), "Invalid entry");
	return (entry - &ATTR_TABLE[0]);
}
