/**
 * @file attr_table.c
 * @brief
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <zephyr.h>
#include <string.h>

#ifdef ATTR_CUSTOM_VALIDATOR
#include "attr_custom_validator.h"
#endif

#include "attr_validator.h"
#include "attr_table.h"
#include "errno_str.h"

/* clang-format off */

/******************************************************************************/
/* Local Constant, Macro and Type Definitions                                 */
/******************************************************************************/
#define DRW DEFAULT_RW_ATTRIBUTE_VALUES
#define DRO DEFAULT_RO_ATTRIBUTE_VALUES

/* Add things to the end of the API document.
 * Do not remove items. Change them to deprecated.
 */
typedef struct rw_attribute {
	/* pystart - rw attributes */
	char sensor_name[23 + 1];
	char location[32 + 1];
	uint16_t advertising_interval;
	uint16_t advertising_duration;
	uint32_t passkey;
	bool lock;
	char load_path[32 + 1];
	char dump_path[32 + 1];
	/* pyend */
} rw_attribute_t;

static const rw_attribute_t DEFAULT_RW_ATTRIBUTE_VALUES = {
	/* pystart - rw defaults */
	.sensor_name = "",
	.location = "",
	.advertising_interval = 1000,
	.advertising_duration = 15000,
	.passkey = 123456,
	.lock = false,
	.load_path = "/lfs/params.txt",
	.dump_path = "/lfs/dump.txt"
	/* pyend */
};

typedef struct ro_attribute {
	/* pystart - ro attributes */
	int64_t up_time;
	uint32_t qrtc;
	uint32_t qrtc_last_set;
	/* pyend */
} ro_attribute_t;

static const ro_attribute_t DEFAULT_RO_ATTRIBUTE_VALUES = {
	/* pystart - ro defaults */
	.up_time = 0,
	.qrtc = 0,
	.qrtc_last_set = 0
	/* pyend */
};

/* pystart - remap */
/* pyend */

/******************************************************************************/
/* Local Data Definitions                                                     */
/******************************************************************************/
static rw_attribute_t rw;
static ro_attribute_t ro;

/******************************************************************************/
/* Global Data Definitions                                                    */
/******************************************************************************/

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

#define y true
#define n false

/* If min == max then range isn't checked. */

/* index....id.name.....................type.savable.writable.readable.lockable.broadcast.deprecated.validator..min.max. */
const struct attr_table_entry ATTR_TABLE[ATTR_TABLE_SIZE] = {
	/* pystart - attribute table */
	[0  ] = { 0  , RW_ATTRS(sensor_name)                   , ATTR_TYPE_STRING        , y, y, y, y, y, n, av_string           , NULL                                , .min.ux = 1         , .max.ux = 23         },
	[1  ] = { 1  , RW_ATTRS(location)                      , ATTR_TYPE_STRING        , y, y, y, y, n, n, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[2  ] = { 2  , RW_ATTRX(advertising_interval)          , ATTR_TYPE_U16           , y, y, y, y, y, n, av_uint16           , NULL                                , .min.ux = 20        , .max.ux = 10000      },
	[3  ] = { 3  , RW_ATTRX(advertising_duration)          , ATTR_TYPE_U16           , y, y, y, y, y, n, av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[4  ] = { 4  , RW_ATTRX(passkey)                       , ATTR_TYPE_U32           , y, y, y, y, y, n, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 999999     },
	[5  ] = { 5  , RW_ATTRX(lock)                          , ATTR_TYPE_BOOL          , y, y, y, y, n, n, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[6  ] = { 16 , RO_ATTRX(up_time)                       , ATTR_TYPE_S64           , n, n, y, n, n, n, av_int64            , attr_prepare_up_time                , .min.ux = 0         , .max.ux = 0          },
	[7  ] = { 94 , RO_ATTRX(qrtc)                          , ATTR_TYPE_U32           , n, n, y, n, n, n, av_uint32           , attr_prepare_qrtc                   , .min.ux = 0         , .max.ux = 0          },
	[8  ] = { 95 , RO_ATTRX(qrtc_last_set)                 , ATTR_TYPE_U32           , n, n, y, n, n, n, av_uint32           , attr_prepare_qrtc_last_set          , .min.ux = 0         , .max.ux = 0          },
	[9  ] = { 208, RW_ATTRS(load_path)                     , ATTR_TYPE_STRING        , y, y, y, n, n, n, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[10 ] = { 209, RW_ATTRS(dump_path)                     , ATTR_TYPE_STRING        , y, y, y, n, n, n, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         }
	/* pyend */
};

/**
 * @brief map id to table entry (invalid entries are NULL)
 */
static const struct attr_table_entry * const ATTR_MAP[] = {
	/* pystart - attribute map */
	[0  ] = &ATTR_TABLE[0  ],
	[1  ] = &ATTR_TABLE[1  ],
	[2  ] = &ATTR_TABLE[2  ],
	[3  ] = &ATTR_TABLE[3  ],
	[4  ] = &ATTR_TABLE[4  ],
	[5  ] = &ATTR_TABLE[5  ],
	[16 ] = &ATTR_TABLE[6  ],
	[94 ] = &ATTR_TABLE[7  ],
	[95 ] = &ATTR_TABLE[8  ],
	[208] = &ATTR_TABLE[9  ],
	[209] = &ATTR_TABLE[10 ]
	/* pyend */
};
BUILD_ASSERT(ARRAY_SIZE(ATTR_MAP) == (ATTR_TABLE_MAX_ID + 1),
	     "Invalid attribute map");

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
void attr_table_initialize(void)
{
	memcpy(&rw, &DRW, sizeof(rw_attribute_t));
	memcpy(&ro, &DRO, sizeof(ro_attribute_t));
}

void attr_table_factory_reset(void)
{
	size_t i = 0;
	for (i = 0; i < ATTR_TABLE_SIZE; i++) {
		memcpy(ATTR_TABLE[i].pData, ATTR_TABLE[i].pDefault,
		       ATTR_TABLE[i].size);
	}
}

const struct attr_table_entry *const attr_map(attr_id_t id)
{
	if (id > ATTR_TABLE_MAX_ID) {
		return NULL;
	} else {
		return ATTR_MAP[id];
	}
}

attr_index_t attr_table_index(const struct attr_table_entry *const entry)
{
	__ASSERT(PART_OF_ARRAY(ATTR_TABLE, entry), "Invalid entry");
	return (entry - &ATTR_TABLE[0]);
}

/******************************************************************************/
/* Local Function Definitions                                                 */
/******************************************************************************/
/* pystart - prepare for read - weak implementations */
__weak int attr_prepare_up_time(void)
{
	return 0;
}

__weak int attr_prepare_qrtc(void)
{
	return 0;
}

__weak int attr_prepare_qrtc_last_set(void)
{
	return 0;
}

/* pyend */

/* pystart - get string */
/* pyend */