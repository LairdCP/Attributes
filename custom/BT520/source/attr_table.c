/**
 * @file attr_table.c
 * @brief
 *
 * Copyright (c) 2022 Laird Connectivity
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
	char location[32 + 1];
	uint16_t advertising_interval;
	uint16_t advertising_duration;
	uint32_t passkey;
	bool lock;
	uint32_t power_sense_interval;
	uint32_t temperature_sense_interval;
	bool active_mode;
	enum advertising_phy advertising_phy;
	int8_t tx_power;
	uint16_t network_id;
	enum config_type config_type;
	uint8_t hardware_minor_version;
	uint32_t qrtc_last_set;
	uint32_t settings_passcode;
	bool data_logging_enable;
	bool factory_reset_enable;
	enum boot_phy boot_phy;
	bool block_downgrades;
	uint8_t motion_odr;
	uint8_t motion_thresh;
	uint8_t motion_scale;
	uint8_t motion_duration;
	char load_path[32 + 1];
	float floaty;
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
	.power_sense_interval = 0,
	.temperature_sense_interval = 60,
	.active_mode = false,
	.advertising_phy = true,
	.tx_power = 0,
	.network_id = 0,
	.config_type = 0,
	.hardware_minor_version = 0,
	.qrtc_last_set = 0,
	.settings_passcode = 123456,
	.data_logging_enable = false,
	.factory_reset_enable = true,
	.boot_phy = 0,
	.block_downgrades = 0,
	.motion_odr = 5,
	.motion_thresh = 10,
	.motion_scale = 2,
	.motion_duration = 6,
	.load_path = "/lfs/params.txt",
	.floaty = 0.13
	/* pyend */
};

typedef struct ro_attribute {
	/* pystart - ro attributes */
	char firmware_version[11 + 1];
	char reset_reason[12 + 1];
	char bluetooth_address[12 + 1];
	uint32_t reset_count;
	int64_t up_time;
	uint8_t config_version;
	float temperature_result_1;
	uint16_t power_voltage_mv;
	uint32_t flags;
	enum magnet_state magnet_state;
	char param_path[8 + 1];
	uint32_t battery_age;
	char attribute_version[11 + 1];
	uint32_t qrtc;
	enum log_file_status log_file_status;
	bool mag_switch_simulated;
	bool mag_switch_simulated_value;
	bool mobile_app_disconnect;
	int32_t attr_save_error_code;
	int8_t security_level;
	char build_id[64 + 1];
	char app_type[32 + 1];
	char mount[32 + 1];
	bool motion_alarm;
	char dump_path[32 + 1];
	enum settings_passcode_status settings_passcode_status;
	enum lock_status lock_status;
	/* pyend */
} ro_attribute_t;

static const ro_attribute_t DEFAULT_RO_ATTRIBUTE_VALUES = {
	/* pystart - ro defaults */
	.firmware_version = "0.0.0",
	.reset_reason = "RESETPIN",
	.bluetooth_address = "0",
	.reset_count = 0,
	.up_time = 0,
	.config_version = 0,
	.temperature_result_1 = 0,
	.power_voltage_mv = 0,
	.flags = 0,
	.magnet_state = false,
	.param_path = "/ramfs",
	.battery_age = 0,
	.attribute_version = "0.4.103",
	.qrtc = 0,
	.log_file_status = 0,
	.mag_switch_simulated = 0,
	.mag_switch_simulated_value = 0,
	.mobile_app_disconnect = 0,
	.attr_save_error_code = 0,
	.security_level = 0,
	.build_id = "0",
	.app_type = "",
	.mount = "/lfs",
	.motion_alarm = false,
	.dump_path = "/ramfs/dump.txt",
	.settings_passcode_status = 0,
	.lock_status = 0
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
#ifdef CONFIG_ATTR_STRING_NAME
#define RW_ATTRS(n) STRINGIFY(n), rw.n, DRW.n, sizeof(rw.n), NULL
#define RW_ATTRX(n) STRINGIFY(n), &rw.n, &DRW.n, sizeof(rw.n), NULL
#define RW_ATTRE(n) STRINGIFY(n), &rw.n, &DRW.n, sizeof(rw.n), attr_get_string_ ## n
#define RO_ATTRS(n) STRINGIFY(n), ro.n, DRO.n, sizeof(ro.n), NULL
#define RO_ATTRX(n) STRINGIFY(n), &ro.n, &DRO.n, sizeof(ro.n), NULL
#define RO_ATTRE(n) STRINGIFY(n), &ro.n, &DRO.n, sizeof(ro.n), attr_get_string_ ## n
#else
#define RW_ATTRS(n) "", rw.n, DRW.n, sizeof(rw.n), NULL
#define RW_ATTRX(n) "", &rw.n, &DRW.n, sizeof(rw.n), NULL
#define RW_ATTRE(n) "", &rw.n, &DRW.n, sizeof(rw.n), attr_get_string_ ## n
#define RO_ATTRS(n) "", ro.n, DRO.n, sizeof(ro.n), NULL
#define RO_ATTRX(n) "", &ro.n, &DRO.n, sizeof(ro.n), NULL
#define RO_ATTRE(n) "", &ro.n, &DRO.n, sizeof(ro.n), attr_get_string_ ## n
#endif

#define y true
#define n false

/* If min == max then range isn't checked. */

/* index....id.name.....................type.flags.validator..prepare..min.max. */
const struct attr_table_entry ATTR_TABLE[ATTR_TABLE_SIZE] = {
	/* pystart - attribute table */
	[0  ] = { 0  , RW_ATTRS(sensor_name)                   , ATTR_TYPE_STRING        , 0x1f  , av_string           , NULL                                , .min.ux = 1         , .max.ux = 23         },
	[1  ] = { 1  , RW_ATTRS(location)                      , ATTR_TYPE_STRING        , 0x17  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[2  ] = { 2  , RW_ATTRX(advertising_interval)          , ATTR_TYPE_U16           , 0x1f  , av_uint16           , NULL                                , .min.ux = 500       , .max.ux = 10000      },
	[3  ] = { 3  , RW_ATTRX(advertising_duration)          , ATTR_TYPE_U16           , 0x1f  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[4  ] = { 4  , RW_ATTRX(passkey)                       , ATTR_TYPE_U32           , 0x1f  , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 999999     },
	[5  ] = { 5  , RW_ATTRX(lock)                          , ATTR_TYPE_BOOL          , 0x16  , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[6  ] = { 6  , RW_ATTRX(power_sense_interval)          , ATTR_TYPE_U32           , 0x1f  , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 86400      },
	[7  ] = { 7  , RW_ATTRX(temperature_sense_interval)    , ATTR_TYPE_U32           , 0x1f  , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 86400      },
	[8  ] = { 11 , RO_ATTRS(firmware_version)              , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 3         , .max.ux = 11         },
	[9  ] = { 12 , RO_ATTRS(reset_reason)                  , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 12         },
	[10 ] = { 13 , RO_ATTRS(bluetooth_address)             , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 12        , .max.ux = 12         },
	[11 ] = { 14 , RO_ATTRX(reset_count)                   , ATTR_TYPE_U32           , 0x2   , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[12 ] = { 16 , RO_ATTRX(up_time)                       , ATTR_TYPE_S64           , 0x2   , av_int64            , attr_prepare_up_time                , .min.ux = 0         , .max.ux = 0          },
	[13 ] = { 57 , RW_ATTRX(active_mode)                   , ATTR_TYPE_BOOL          , 0x1b  , av_cp8              , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[14 ] = { 58 , RW_ATTRE(advertising_phy)               , ATTR_TYPE_BOOL          , 0x1f  , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[15 ] = { 59 , RW_ATTRX(tx_power)                      , ATTR_TYPE_S8            , 0x1b  , av_tx_power         , NULL                                , .min.sx = -40       , .max.sx = 8          },
	[16 ] = { 60 , RW_ATTRX(network_id)                    , ATTR_TYPE_U16           , 0x1f  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[17 ] = { 61 , RO_ATTRX(config_version)                , ATTR_TYPE_U8            , 0xa   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[18 ] = { 62 , RW_ATTRE(config_type)                   , ATTR_TYPE_U8            , 0x1f  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 6          },
	[19 ] = { 63 , RW_ATTRX(hardware_minor_version)        , ATTR_TYPE_U8            , 0x1b  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[20 ] = { 70 , RO_ATTRX(temperature_result_1)          , ATTR_TYPE_FLOAT         , 0x2   , av_float            , attr_prepare_temperature_result_1   , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[21 ] = { 75 , RO_ATTRX(power_voltage_mv)              , ATTR_TYPE_U16           , 0x2   , av_uint16           , attr_prepare_power_voltage_mv       , .min.ux = 0         , .max.ux = 3800       },
	[22 ] = { 89 , RO_ATTRX(flags)                         , ATTR_TYPE_U32           , 0xa   , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[23 ] = { 90 , RO_ATTRE(magnet_state)                  , ATTR_TYPE_BOOL          , 0x2   , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[24 ] = { 91 , RO_ATTRS(param_path)                    , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 2         , .max.ux = 8          },
	[25 ] = { 92 , RO_ATTRX(battery_age)                   , ATTR_TYPE_U32           , 0x2   , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[26 ] = { 93 , RO_ATTRS(attribute_version)             , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 6         , .max.ux = 11         },
	[27 ] = { 94 , RO_ATTRX(qrtc)                          , ATTR_TYPE_U32           , 0x2   , av_uint32           , attr_prepare_qrtc                   , .min.ux = 0         , .max.ux = 0          },
	[28 ] = { 95 , RW_ATTRX(qrtc_last_set)                 , ATTR_TYPE_U32           , 0x1a  , av_uint32           , attr_prepare_qrtc_last_set          , .min.ux = 0         , .max.ux = 0          },
	[29 ] = { 100, RW_ATTRX(settings_passcode)             , ATTR_TYPE_U32           , 0x458 , av_cp32             , NULL                                , .min.ux = 0         , .max.ux = 999999     },
	[30 ] = { 113, RW_ATTRX(data_logging_enable)           , ATTR_TYPE_BOOL          , 0x17  , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[31 ] = { 114, RW_ATTRX(factory_reset_enable)          , ATTR_TYPE_BOOL          , 0x17  , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[32 ] = { 115, RO_ATTRE(log_file_status)               , ATTR_TYPE_U8            , 0x2   , av_uint8            , attr_prepare_log_file_status        , .min.ux = 0         , .max.ux = 3          },
	[33 ] = { 162, RO_ATTRX(mag_switch_simulated)          , ATTR_TYPE_BOOL          , 0x407 , av_magsimen         , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[34 ] = { 163, RO_ATTRX(mag_switch_simulated_value)    , ATTR_TYPE_BOOL          , 0x407 , av_magsim           , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[35 ] = { 166, RW_ATTRE(boot_phy)                      , ATTR_TYPE_U8            , 0x417 , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 2          },
	[36 ] = { 167, RO_ATTRX(mobile_app_disconnect)         , ATTR_TYPE_BOOL          , 0x40b , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[37 ] = { 168, RO_ATTRX(attr_save_error_code)          , ATTR_TYPE_S32           , 0x40a , av_int32            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[38 ] = { 171, RW_ATTRX(block_downgrades)              , ATTR_TYPE_BOOL          , 0x17  , av_block_downgrades , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[39 ] = { 173, RO_ATTRX(security_level)                , ATTR_TYPE_S8            , 0x2   , av_int8             , attr_prepare_security_level         , .min.sx = -1        , .max.sx = 4          },
	[40 ] = { 143, RO_ATTRS(build_id)                      , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 1         , .max.ux = 64         },
	[41 ] = { 144, RO_ATTRS(app_type)                      , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 1         , .max.ux = 32         },
	[42 ] = { 145, RO_ATTRS(mount)                         , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[43 ] = { 168, RW_ATTRX(motion_odr)                    , ATTR_TYPE_U8            , 0x1b  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[44 ] = { 169, RW_ATTRX(motion_thresh)                 , ATTR_TYPE_U8            , 0x1b  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[45 ] = { 170, RW_ATTRX(motion_scale)                  , ATTR_TYPE_U8            , 0x1b  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[46 ] = { 171, RW_ATTRX(motion_duration)               , ATTR_TYPE_U8            , 0x1b  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[47 ] = { 172, RO_ATTRX(motion_alarm)                  , ATTR_TYPE_BOOL          , 0x2   , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[48 ] = { 208, RW_ATTRS(load_path)                     , ATTR_TYPE_STRING        , 0x13  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[49 ] = { 209, RO_ATTRS(dump_path)                     , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[50 ] = { 211, RW_ATTRX(floaty)                        , ATTR_TYPE_FLOAT         , 0x13  , av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 0.0        },
	[51 ] = { 265, RO_ATTRE(settings_passcode_status)      , ATTR_TYPE_U8            , 0x2   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 2          },
	[52 ] = { 266, RO_ATTRE(lock_status)                   , ATTR_TYPE_U8            , 0x2   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 2          }
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
	[11 ] = &ATTR_TABLE[8  ],
	[12 ] = &ATTR_TABLE[9  ],
	[13 ] = &ATTR_TABLE[10 ],
	[14 ] = &ATTR_TABLE[11 ],
	[16 ] = &ATTR_TABLE[12 ],
	[57 ] = &ATTR_TABLE[13 ],
	[58 ] = &ATTR_TABLE[14 ],
	[59 ] = &ATTR_TABLE[15 ],
	[60 ] = &ATTR_TABLE[16 ],
	[61 ] = &ATTR_TABLE[17 ],
	[62 ] = &ATTR_TABLE[18 ],
	[63 ] = &ATTR_TABLE[19 ],
	[70 ] = &ATTR_TABLE[20 ],
	[75 ] = &ATTR_TABLE[21 ],
	[89 ] = &ATTR_TABLE[22 ],
	[90 ] = &ATTR_TABLE[23 ],
	[91 ] = &ATTR_TABLE[24 ],
	[92 ] = &ATTR_TABLE[25 ],
	[93 ] = &ATTR_TABLE[26 ],
	[94 ] = &ATTR_TABLE[27 ],
	[95 ] = &ATTR_TABLE[28 ],
	[100] = &ATTR_TABLE[29 ],
	[113] = &ATTR_TABLE[30 ],
	[114] = &ATTR_TABLE[31 ],
	[115] = &ATTR_TABLE[32 ],
	[143] = &ATTR_TABLE[40 ],
	[144] = &ATTR_TABLE[41 ],
	[145] = &ATTR_TABLE[42 ],
	[162] = &ATTR_TABLE[33 ],
	[163] = &ATTR_TABLE[34 ],
	[166] = &ATTR_TABLE[35 ],
	[167] = &ATTR_TABLE[36 ],
	[168] = &ATTR_TABLE[37 ],
	[169] = &ATTR_TABLE[44 ],
	[170] = &ATTR_TABLE[45 ],
	[171] = &ATTR_TABLE[38 ],
	[172] = &ATTR_TABLE[47 ],
	[173] = &ATTR_TABLE[39 ],
	[208] = &ATTR_TABLE[48 ],
	[209] = &ATTR_TABLE[49 ],
	[211] = &ATTR_TABLE[50 ],
	[265] = &ATTR_TABLE[51 ],
	[266] = &ATTR_TABLE[52 ]
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

__weak int attr_prepare_temperature_result_1(void)
{
	return 0;
}

__weak int attr_prepare_power_voltage_mv(void)
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

__weak int attr_prepare_log_file_status(void)
{
	return 0;
}

__weak int attr_prepare_security_level(void)
{
	return 0;
}

/* pyend */

#ifdef CONFIG_ATTR_STRING_NAME
/* pystart - get string */
const char *const attr_get_string_advertising_phy(int value)
{
	switch (value) {
		case 0:           return "Coded";
		case 1:           return "1M";
		default:          return "?";
	}
}

const char *const attr_get_string_config_type(int value)
{
	switch (value) {
		case 0:           return "Not Configured";
		case 1:           return "Analog";
		case 2:           return "Digital";
		case 3:           return "Temperature";
		case 4:           return "Current";
		case 5:           return "Ultrasonic Pressure";
		case 6:           return "Spi Or I2 C";
		default:          return "?";
	}
}

const char *const attr_get_string_magnet_state(int value)
{
	switch (value) {
		case true:        return "Far Door Open";
		case false:       return "Near Door Closed";
		default:          return "?";
	}
}

const char *const attr_get_string_log_file_status(int value)
{
	switch (value) {
		case 0:           return "Waiting For Prepare Command";
		case 1:           return "Preparing";
		case 2:           return "Ready";
		case 3:           return "Failed";
		default:          return "?";
	}
}

const char *const attr_get_string_boot_phy(int value)
{
	switch (value) {
		case 0:           return "Default";
		case 1:           return "Coded";
		case 2:           return "1M";
		default:          return "?";
	}
}

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
#endif

