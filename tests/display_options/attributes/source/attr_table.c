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

#include "attr_validator.h"
#include "attr_custom_validator.h"
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
	bool lock;
	uint32_t settings_passcode;
	char load_path[32 + 1];
	char dump_path[32 + 1];
	/* pyend */
} rw_attribute_t;

static const rw_attribute_t DEFAULT_RW_ATTRIBUTE_VALUES = {
	/* pystart - rw defaults */
	.sensor_name = "",
	.lock = false,
	.settings_passcode = 123456,
	.load_path = "/lfs/params.txt",
	.dump_path = "/lfs/dump.txt"
	/* pyend */
};

typedef struct ro_attribute {
	/* pystart - ro attributes */
	char attribute_version[11 + 1];
	enum settings_passcode_status settings_passcode_status;
	enum lock_status lock_status;
	uint8_t test_a;
	uint8_t test_b;
	uint8_t test_c;
	uint8_t test_d;
	uint8_t test_e;
	uint8_t test_f;
	uint8_t test_g;
	uint8_t test_h;
	uint8_t test_i;
	uint8_t test_j;
	uint8_t test_k;
	uint8_t test_l;
	uint8_t test_m;
	uint8_t test_n;
	uint8_t test_o;
	uint8_t test_p;
	uint8_t test_q;
	uint8_t test_r;
	uint8_t test_s;
	uint8_t test_t;
	uint8_t test_u;
	uint8_t test_v;
	uint8_t test_w;
	uint8_t test_x;
	/* pyend */
} ro_attribute_t;

static const ro_attribute_t DEFAULT_RO_ATTRIBUTE_VALUES = {
	/* pystart - ro defaults */
	.attribute_version = "0.1.00",
	.settings_passcode_status = 0,
	.lock_status = 0,
	.test_a = 1,
	.test_b = 2,
	.test_c = 3,
	.test_d = 4,
	.test_e = 5,
	.test_f = 6,
	.test_g = 7,
	.test_h = 8,
	.test_i = 9,
	.test_j = 10,
	.test_k = 11,
	.test_l = 12,
	.test_m = 13,
	.test_n = 14,
	.test_o = 15,
	.test_p = 16,
	.test_q = 17,
	.test_r = 18,
	.test_s = 19,
	.test_t = 20,
	.test_u = 21,
	.test_v = 22,
	.test_w = 23,
	.test_x = 24
	/* pyend */
};

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
	[0  ] = { 0  , RW_ATTRS(sensor_name)                   , ATTR_TYPE_STRING        , y, y, y, y, y, n, 0, av_string           , NULL                                , .min.ux = 1         , .max.ux = 23         },
	[1  ] = { 1  , RO_ATTRS(attribute_version)             , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 6         , .max.ux = 11         },
	[2  ] = { 2  , RW_ATTRX(lock)                          , ATTR_TYPE_BOOL          , y, n, y, y, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[3  ] = { 3  , RW_ATTRX(settings_passcode)             , ATTR_TYPE_U32           , y, y, n, n, y, n, 0, av_cp32             , NULL                                , .min.ux = 0         , .max.ux = 999999     },
	[4  ] = { 4  , RO_ATTRE(settings_passcode_status)      , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 2          },
	[5  ] = { 5  , RO_ATTRE(lock_status)                   , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 2          },
	[6  ] = { 6  , RO_ATTRX(test_a)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 1, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[7  ] = { 7  , RO_ATTRX(test_b)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 2, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[8  ] = { 8  , RO_ATTRX(test_c)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 5, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[9  ] = { 9  , RO_ATTRX(test_d)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 6, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[10 ] = { 10 , RO_ATTRX(test_e)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 8, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[11 ] = { 11 , RO_ATTRX(test_f)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 16, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[12 ] = { 12 , RO_ATTRX(test_g)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 40, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[13 ] = { 13 , RO_ATTRX(test_h)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 48, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[14 ] = { 14 , RO_ATTRX(test_i)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 9, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[15 ] = { 15 , RO_ATTRX(test_j)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 13, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[16 ] = { 16 , RO_ATTRX(test_k)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 41, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[17 ] = { 17 , RO_ATTRX(test_l)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 10, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[18 ] = { 18 , RO_ATTRX(test_m)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 14, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[19 ] = { 19 , RO_ATTRX(test_n)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 42, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[20 ] = { 20 , RO_ATTRX(test_o)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 17, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[21 ] = { 21 , RO_ATTRX(test_p)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 21, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[22 ] = { 22 , RO_ATTRX(test_q)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 49, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[23 ] = { 23 , RO_ATTRX(test_r)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 18, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[24 ] = { 24 , RO_ATTRX(test_s)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 22, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[25 ] = { 25 , RO_ATTRX(test_t)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 50, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[26 ] = { 26 , RO_ATTRX(test_u)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 109, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[27 ] = { 27 , RO_ATTRX(test_v)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 118, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[28 ] = { 28 , RO_ATTRX(test_w)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 73, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[29 ] = { 29 , RO_ATTRX(test_x)                        , ATTR_TYPE_U8            , n, y, y, n, n, n, 82, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[30 ] = { 50 , RW_ATTRS(load_path)                     , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[31 ] = { 51 , RW_ATTRS(dump_path)                     , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         }
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
	[6  ] = &ATTR_TABLE[6  ],
	[7  ] = &ATTR_TABLE[7  ],
	[8  ] = &ATTR_TABLE[8  ],
	[9  ] = &ATTR_TABLE[9  ],
	[10 ] = &ATTR_TABLE[10 ],
	[11 ] = &ATTR_TABLE[11 ],
	[12 ] = &ATTR_TABLE[12 ],
	[13 ] = &ATTR_TABLE[13 ],
	[14 ] = &ATTR_TABLE[14 ],
	[15 ] = &ATTR_TABLE[15 ],
	[16 ] = &ATTR_TABLE[16 ],
	[17 ] = &ATTR_TABLE[17 ],
	[18 ] = &ATTR_TABLE[18 ],
	[19 ] = &ATTR_TABLE[19 ],
	[20 ] = &ATTR_TABLE[20 ],
	[21 ] = &ATTR_TABLE[21 ],
	[22 ] = &ATTR_TABLE[22 ],
	[23 ] = &ATTR_TABLE[23 ],
	[24 ] = &ATTR_TABLE[24 ],
	[25 ] = &ATTR_TABLE[25 ],
	[26 ] = &ATTR_TABLE[26 ],
	[27 ] = &ATTR_TABLE[27 ],
	[28 ] = &ATTR_TABLE[28 ],
	[29 ] = &ATTR_TABLE[29 ],
	[50 ] = &ATTR_TABLE[30 ],
	[51 ] = &ATTR_TABLE[31 ]
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
/* pyend */

/* pystart - get string */
const char *const attr_get_string_settings_passcode_status(int value)
{
	switch (value) {
		case 0:           return "Undefined";
		case 1:           return "Valid Code";
		case 2:           return "Invalid Code";
		default:          return "?";
	}
}

const char *const attr_get_string_lock_status(int value)
{
	switch (value) {
		case 0:           return "Not Setup";
		case 1:           return "Setup Engaged";
		case 2:           return "Setup Disengaged";
		default:          return "?";
	}
}

/* pyend */
