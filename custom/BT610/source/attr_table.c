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
	char location[32 + 1];
	uint16_t advertising_interval;
	uint16_t advertising_duration;
	uint32_t passkey;
	bool lock;
	uint32_t power_sense_interval;
	uint32_t temperature_sense_interval;
	uint8_t aggregation_count;
	bool digital_output_1_state;
	bool digital_output_2_state;
	float high_temp_1_thresh_1;
	float high_temp_1_thresh_2;
	float low_temp_1_thresh_1;
	float low_temp_1_thresh_2;
	float temp_1_delta_thresh;
	float high_temp_2_thresh_1;
	float high_temp_2_thresh_2;
	float low_temp_2_thresh_1;
	float low_temp_2_thresh_2;
	float temp_2_delta_thresh;
	float high_temp_3_thresh_1;
	float high_temp_3_thresh_2;
	float low_temp_3_thresh_1;
	float low_temp_3_thresh_2;
	float temp_3_delta_thresh;
	float high_temp_4_thresh_1;
	float high_temp_4_thresh_2;
	float low_temp_4_thresh_1;
	float low_temp_4_thresh_2;
	float temp_4_delta_thresh;
	float high_analog_1_thresh_1;
	float high_analog_1_thresh_2;
	float low_analog_1_thresh_1;
	float low_analog_1_thresh_2;
	float analog_1_delta_thresh;
	float high_analog_2_thresh_1;
	float high_analog_2_thresh_2;
	float low_analog_2_thresh_1;
	float low_analog_2_thresh_2;
	float analog_2_delta_thresh;
	float high_analog_3_thresh_1;
	float high_analog_3_thresh_2;
	float low_analog_3_thresh_1;
	float low_analog_3_thresh_2;
	float analog_3_delta_thresh;
	float high_analog_4_thresh_1;
	float high_analog_4_thresh_2;
	float low_analog_4_thresh_1;
	float low_analog_4_thresh_2;
	float analog_4_delta_thresh;
	bool active_mode;
	enum advertising_phy advertising_phy;
	int8_t tx_power;
	uint16_t network_id;
	enum config_type config_type;
	uint8_t hardware_minor_version;
	float old_coefficient_a;
	float old_coefficient_b;
	float old_coefficient_c;
	uint8_t thermistor_config;
	uint8_t digital_alarms;
	uint8_t digital_input_1_config;
	uint8_t digital_input_2_config;
	enum analog_input_1_type analog_input_1_type;
	uint8_t analog_input_2_type;
	uint8_t analog_input_3_type;
	uint8_t analog_input_4_type;
	uint32_t qrtc_last_set;
	float sh_offset;
	uint32_t analog_sense_interval;
	enum tamper_switch_status tamper_switch_status;
	uint32_t settings_passcode;
	float therm_1_coefficient_a;
	float therm_2_coefficient_a;
	float therm_3_coefficient_a;
	float therm_4_coefficient_a;
	float therm_1_coefficient_b;
	float therm_2_coefficient_b;
	float therm_3_coefficient_b;
	float therm_4_coefficient_b;
	float therm_1_coefficient_c;
	float therm_2_coefficient_c;
	float therm_3_coefficient_c;
	float therm_4_coefficient_c;
	bool data_logging_enable;
	bool factory_reset_enable;
	uint32_t temperature_alarms_enable;
	uint32_t analog_alarms_enable;
	enum boot_phy boot_phy;
	bool block_downgrades;
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
	.power_sense_interval = 0,
	.temperature_sense_interval = 60,
	.aggregation_count = 1,
	.digital_output_1_state = false,
	.digital_output_2_state = false,
	.high_temp_1_thresh_1 = 1.27e+2,
	.high_temp_1_thresh_2 = 1.27e+2,
	.low_temp_1_thresh_1 = -1.28e+2,
	.low_temp_1_thresh_2 = -1.28e+2,
	.temp_1_delta_thresh = 0,
	.high_temp_2_thresh_1 = 1.27e+2,
	.high_temp_2_thresh_2 = 1.27e+2,
	.low_temp_2_thresh_1 = -1.28e+2,
	.low_temp_2_thresh_2 = -1.28e+2,
	.temp_2_delta_thresh = 0,
	.high_temp_3_thresh_1 = 1.27e+2,
	.high_temp_3_thresh_2 = 1.27e+2,
	.low_temp_3_thresh_1 = -1.28e+2,
	.low_temp_3_thresh_2 = -1.28e+2,
	.temp_3_delta_thresh = 0,
	.high_temp_4_thresh_1 = 1.27e+2,
	.high_temp_4_thresh_2 = 1.27e+2,
	.low_temp_4_thresh_1 = -1.28e+2,
	.low_temp_4_thresh_2 = -1.28e+2,
	.temp_4_delta_thresh = 0,
	.high_analog_1_thresh_1 = 0,
	.high_analog_1_thresh_2 = 0,
	.low_analog_1_thresh_1 = 0,
	.low_analog_1_thresh_2 = 0,
	.analog_1_delta_thresh = 0,
	.high_analog_2_thresh_1 = 0,
	.high_analog_2_thresh_2 = 0,
	.low_analog_2_thresh_1 = 0,
	.low_analog_2_thresh_2 = 0,
	.analog_2_delta_thresh = 0,
	.high_analog_3_thresh_1 = 0,
	.high_analog_3_thresh_2 = 0,
	.low_analog_3_thresh_1 = 0,
	.low_analog_3_thresh_2 = 0,
	.analog_3_delta_thresh = 0,
	.high_analog_4_thresh_1 = 0,
	.high_analog_4_thresh_2 = 0,
	.low_analog_4_thresh_1 = 0,
	.low_analog_4_thresh_2 = 0,
	.analog_4_delta_thresh = 0,
	.active_mode = false,
	.advertising_phy = true,
	.tx_power = 0,
	.network_id = 0,
	.config_type = 0,
	.hardware_minor_version = 0,
	.old_coefficient_a = 1.132e-3,
	.old_coefficient_b = 2.338e-4,
	.old_coefficient_c = 8.780e-8,
	.thermistor_config = 0,
	.digital_alarms = 0,
	.digital_input_1_config = 0,
	.digital_input_2_config = 0,
	.analog_input_1_type = 0,
	.analog_input_2_type = 0,
	.analog_input_3_type = 0,
	.analog_input_4_type = 0,
	.qrtc_last_set = 0,
	.sh_offset = 2.7315e+2,
	.analog_sense_interval = 60,
	.tamper_switch_status = false,
	.settings_passcode = 123456,
	.therm_1_coefficient_a = 1.132e-3,
	.therm_2_coefficient_a = 1.132e-3,
	.therm_3_coefficient_a = 1.132e-3,
	.therm_4_coefficient_a = 1.132e-3,
	.therm_1_coefficient_b = 2.338e-4,
	.therm_2_coefficient_b = 2.338e-4,
	.therm_3_coefficient_b = 2.338e-4,
	.therm_4_coefficient_b = 2.338e-4,
	.therm_1_coefficient_c = 8.780e-8,
	.therm_2_coefficient_c = 8.780e-8,
	.therm_3_coefficient_c = 8.780e-8,
	.therm_4_coefficient_c = 8.780e-8,
	.data_logging_enable = false,
	.factory_reset_enable = true,
	.temperature_alarms_enable = 0,
	.analog_alarms_enable = 0,
	.boot_phy = 0,
	.block_downgrades = 0,
	.load_path = "/lfs/params.txt",
	.dump_path = "/lfs/dump.txt"
	/* pyend */
};

typedef struct ro_attribute {
	/* pystart - ro attributes */
	char firmware_version[11 + 1];
	char reset_reason[12 + 1];
	char bluetooth_address[12 + 1];
	uint32_t reset_count;
	char bootloader_version[11 + 1];
	int64_t up_time;
	uint8_t config_version;
	float ge;
	float oe;
	float temperature_result_1;
	float temperature_result_2;
	float temperature_result_3;
	float temperature_result_4;
	enum temperature_alarms temperature_alarms;
	uint16_t power_voltage_mv;
	enum digital_input digital_input;
	float analog_input_1;
	float analog_input_2;
	float analog_input_3;
	float analog_input_4;
	enum analog_alarms analog_alarms;
	uint32_t flags;
	enum magnet_state magnet_state;
	char param_path[8 + 1];
	uint32_t battery_age;
	char attribute_version[11 + 1];
	uint32_t qrtc;
	uint8_t connection_timeout_sec;
	enum log_file_status log_file_status;
	bool adc_power_simulated;
	int16_t adc_power_simulated_counts;
	bool adc_analog_sensor_simulated;
	int16_t adc_analog_sensor_simulated_counts;
	bool adc_thermistor_simulated;
	int16_t adc_thermistor_simulated_counts;
	bool adc_vref_simulated;
	int16_t adc_vref_simulated_counts;
	bool voltage_1_simulated;
	float voltage_1_simulated_value;
	bool voltage_2_simulated;
	float voltage_2_simulated_value;
	bool voltage_3_simulated;
	float voltage_3_simulated_value;
	bool voltage_4_simulated;
	float voltage_4_simulated_value;
	bool ultrasonic_simulated;
	float ultrasonic_simulated_value;
	bool pressure_simulated;
	float pressure_simulated_value;
	bool current_1_simulated;
	float current_1_simulated_value;
	bool current_2_simulated;
	float current_2_simulated_value;
	bool current_3_simulated;
	float current_3_simulated_value;
	bool current_4_simulated;
	float current_4_simulated_value;
	bool vref_simulated;
	float vref_simulated_value;
	bool temperature_1_simulated;
	float temperature_1_simulated_value;
	bool temperature_2_simulated;
	float temperature_2_simulated_value;
	bool temperature_3_simulated;
	float temperature_3_simulated_value;
	bool temperature_4_simulated;
	float temperature_4_simulated_value;
	bool powermv_simulated;
	int32_t powermv_simulated_value;
	bool digital_input_1_simulated;
	bool digital_input_1_simulated_value;
	bool digital_input_2_simulated;
	bool digital_input_2_simulated_value;
	bool mag_switch_simulated;
	bool mag_switch_simulated_value;
	bool tamper_switch_simulated;
	bool tamper_switch_simulated_value;
	bool mobile_app_disconnect;
	int32_t attr_save_error_code;
	uint8_t recover_settings_count;
	bool security_request;
	int8_t security_level;
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
	.bootloader_version = "0.0",
	.up_time = 0,
	.config_version = 0,
	.ge = 1e+0,
	.oe = 0.0,
	.temperature_result_1 = 0,
	.temperature_result_2 = 0,
	.temperature_result_3 = 0,
	.temperature_result_4 = 0,
	.temperature_alarms = 0,
	.power_voltage_mv = 0,
	.digital_input = 0,
	.analog_input_1 = 0,
	.analog_input_2 = 0,
	.analog_input_3 = 0,
	.analog_input_4 = 0,
	.analog_alarms = 0,
	.flags = 0,
	.magnet_state = false,
	.param_path = "/ext",
	.battery_age = 0,
	.attribute_version = "0.4.90",
	.qrtc = 0,
	.connection_timeout_sec = 60,
	.log_file_status = 0,
	.adc_power_simulated = 0,
	.adc_power_simulated_counts = 0,
	.adc_analog_sensor_simulated = 0,
	.adc_analog_sensor_simulated_counts = 0,
	.adc_thermistor_simulated = 0,
	.adc_thermistor_simulated_counts = 0,
	.adc_vref_simulated = 0,
	.adc_vref_simulated_counts = 0,
	.voltage_1_simulated = 0,
	.voltage_1_simulated_value = 0.0,
	.voltage_2_simulated = 0,
	.voltage_2_simulated_value = 0.0,
	.voltage_3_simulated = 0,
	.voltage_3_simulated_value = 0.0,
	.voltage_4_simulated = 0,
	.voltage_4_simulated_value = 0.0,
	.ultrasonic_simulated = 0,
	.ultrasonic_simulated_value = 0.0,
	.pressure_simulated = 0,
	.pressure_simulated_value = 0.0,
	.current_1_simulated = 0,
	.current_1_simulated_value = 0.0,
	.current_2_simulated = 0,
	.current_2_simulated_value = 0.0,
	.current_3_simulated = 0,
	.current_3_simulated_value = 0.0,
	.current_4_simulated = 0,
	.current_4_simulated_value = 0.0,
	.vref_simulated = 0,
	.vref_simulated_value = 0.0,
	.temperature_1_simulated = 0,
	.temperature_1_simulated_value = 0.0,
	.temperature_2_simulated = 0,
	.temperature_2_simulated_value = 0.0,
	.temperature_3_simulated = 0,
	.temperature_3_simulated_value = 0.0,
	.temperature_4_simulated = 0,
	.temperature_4_simulated_value = 0.0,
	.powermv_simulated = 0,
	.powermv_simulated_value = 0,
	.digital_input_1_simulated = 0,
	.digital_input_1_simulated_value = 0,
	.digital_input_2_simulated = 0,
	.digital_input_2_simulated_value = 0,
	.mag_switch_simulated = 0,
	.mag_switch_simulated_value = 0,
	.tamper_switch_simulated = 0,
	.tamper_switch_simulated_value = 0,
	.mobile_app_disconnect = 0,
	.attr_save_error_code = 0,
	.recover_settings_count = 0,
	.security_request = 0,
	.security_level = 0,
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
	[1  ] = { 1  , RW_ATTRS(location)                      , ATTR_TYPE_STRING        , y, y, y, y, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[2  ] = { 2  , RW_ATTRX(advertising_interval)          , ATTR_TYPE_U16           , y, y, y, y, y, n, 0, av_uint16           , NULL                                , .min.ux = 500       , .max.ux = 10000      },
	[3  ] = { 3  , RW_ATTRX(advertising_duration)          , ATTR_TYPE_U16           , y, y, y, y, y, n, 0, av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[4  ] = { 4  , RW_ATTRX(passkey)                       , ATTR_TYPE_U32           , y, y, y, y, y, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 999999     },
	[5  ] = { 5  , RW_ATTRX(lock)                          , ATTR_TYPE_BOOL          , y, n, y, y, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[6  ] = { 6  , RW_ATTRX(power_sense_interval)          , ATTR_TYPE_U32           , y, y, y, y, y, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 86400      },
	[7  ] = { 7  , RW_ATTRX(temperature_sense_interval)    , ATTR_TYPE_U32           , y, y, y, y, y, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 86400      },
	[8  ] = { 8  , RW_ATTRX(aggregation_count)             , ATTR_TYPE_U8            , y, y, y, y, n, n, 0, av_uint8            , NULL                                , .min.ux = 1         , .max.ux = 32         },
	[9  ] = { 9  , RW_ATTRX(digital_output_1_state)        , ATTR_TYPE_BOOL          , y, y, y, y, y, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[10 ] = { 10 , RW_ATTRX(digital_output_2_state)        , ATTR_TYPE_BOOL          , y, y, y, y, y, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[11 ] = { 11 , RO_ATTRS(firmware_version)              , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 3         , .max.ux = 11         },
	[12 ] = { 12 , RO_ATTRS(reset_reason)                  , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 12         },
	[13 ] = { 13 , RO_ATTRS(bluetooth_address)             , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 12        , .max.ux = 12         },
	[14 ] = { 14 , RO_ATTRX(reset_count)                   , ATTR_TYPE_U32           , n, n, y, n, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[15 ] = { 15 , RO_ATTRS(bootloader_version)            , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 3         , .max.ux = 11         },
	[16 ] = { 16 , RO_ATTRX(up_time)                       , ATTR_TYPE_S64           , n, n, y, n, n, n, 0, av_int64            , attr_prepare_up_time                , .min.ux = 0         , .max.ux = 0          },
	[17 ] = { 17 , RW_ATTRX(high_temp_1_thresh_1)          , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[18 ] = { 18 , RW_ATTRX(high_temp_1_thresh_2)          , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[19 ] = { 19 , RW_ATTRX(low_temp_1_thresh_1)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[20 ] = { 20 , RW_ATTRX(low_temp_1_thresh_2)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[21 ] = { 21 , RW_ATTRX(temp_1_delta_thresh)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0         , .max.fx = 255.0      },
	[22 ] = { 22 , RW_ATTRX(high_temp_2_thresh_1)          , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[23 ] = { 23 , RW_ATTRX(high_temp_2_thresh_2)          , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[24 ] = { 24 , RW_ATTRX(low_temp_2_thresh_1)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[25 ] = { 25 , RW_ATTRX(low_temp_2_thresh_2)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[26 ] = { 26 , RW_ATTRX(temp_2_delta_thresh)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0         , .max.fx = 255.0      },
	[27 ] = { 27 , RW_ATTRX(high_temp_3_thresh_1)          , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[28 ] = { 28 , RW_ATTRX(high_temp_3_thresh_2)          , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[29 ] = { 29 , RW_ATTRX(low_temp_3_thresh_1)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[30 ] = { 30 , RW_ATTRX(low_temp_3_thresh_2)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[31 ] = { 31 , RW_ATTRX(temp_3_delta_thresh)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0         , .max.fx = 255.0      },
	[32 ] = { 32 , RW_ATTRX(high_temp_4_thresh_1)          , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[33 ] = { 33 , RW_ATTRX(high_temp_4_thresh_2)          , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[34 ] = { 34 , RW_ATTRX(low_temp_4_thresh_1)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[35 ] = { 35 , RW_ATTRX(low_temp_4_thresh_2)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = -128.0    , .max.fx = 127.0      },
	[36 ] = { 36 , RW_ATTRX(temp_4_delta_thresh)           , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0         , .max.fx = 255.0      },
	[37 ] = { 37 , RW_ATTRX(high_analog_1_thresh_1)        , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[38 ] = { 38 , RW_ATTRX(high_analog_1_thresh_2)        , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[39 ] = { 39 , RW_ATTRX(low_analog_1_thresh_1)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[40 ] = { 40 , RW_ATTRX(low_analog_1_thresh_2)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[41 ] = { 41 , RW_ATTRX(analog_1_delta_thresh)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[42 ] = { 42 , RW_ATTRX(high_analog_2_thresh_1)        , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[43 ] = { 43 , RW_ATTRX(high_analog_2_thresh_2)        , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[44 ] = { 44 , RW_ATTRX(low_analog_2_thresh_1)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[45 ] = { 45 , RW_ATTRX(low_analog_2_thresh_2)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[46 ] = { 46 , RW_ATTRX(analog_2_delta_thresh)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[47 ] = { 47 , RW_ATTRX(high_analog_3_thresh_1)        , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[48 ] = { 48 , RW_ATTRX(high_analog_3_thresh_2)        , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[49 ] = { 49 , RW_ATTRX(low_analog_3_thresh_1)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[50 ] = { 50 , RW_ATTRX(low_analog_3_thresh_2)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[51 ] = { 51 , RW_ATTRX(analog_3_delta_thresh)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[52 ] = { 52 , RW_ATTRX(high_analog_4_thresh_1)        , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[53 ] = { 53 , RW_ATTRX(high_analog_4_thresh_2)        , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[54 ] = { 54 , RW_ATTRX(low_analog_4_thresh_1)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[55 ] = { 55 , RW_ATTRX(low_analog_4_thresh_2)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[56 ] = { 56 , RW_ATTRX(analog_4_delta_thresh)         , ATTR_TYPE_FLOAT         , y, y, y, y, y, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 15000.0    },
	[57 ] = { 57 , RW_ATTRX(active_mode)                   , ATTR_TYPE_BOOL          , y, y, y, n, y, n, 0, av_cp8              , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[58 ] = { 58 , RW_ATTRE(advertising_phy)               , ATTR_TYPE_BOOL          , y, y, y, y, y, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[59 ] = { 59 , RW_ATTRX(tx_power)                      , ATTR_TYPE_S8            , y, y, y, n, y, n, 0, av_int8             , NULL                                , .min.sx = -40       , .max.sx = 8          },
	[60 ] = { 60 , RW_ATTRX(network_id)                    , ATTR_TYPE_U16           , y, y, y, y, y, n, 0, av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[61 ] = { 61 , RO_ATTRX(config_version)                , ATTR_TYPE_U8            , n, n, y, n, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[62 ] = { 62 , RW_ATTRE(config_type)                   , ATTR_TYPE_U8            , y, y, y, y, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 6          },
	[63 ] = { 63 , RW_ATTRX(hardware_minor_version)        , ATTR_TYPE_U8            , y, y, y, n, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[64 ] = { 64 , RO_ATTRX(ge)                            , ATTR_TYPE_FLOAT         , n, n, y, y, n, n, 0, av_float            , NULL                                , .min.fx = -5.0      , .max.fx = 3.4e+38    },
	[65 ] = { 65 , RO_ATTRX(oe)                            , ATTR_TYPE_FLOAT         , n, n, y, y, n, n, 0, av_float            , NULL                                , .min.fx = -16.0     , .max.fx = 3.4e+38    },
	[66 ] = { 66 , RW_ATTRX(old_coefficient_a)             , ATTR_TYPE_FLOAT         , y, y, y, y, n, y, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[67 ] = { 67 , RW_ATTRX(old_coefficient_b)             , ATTR_TYPE_FLOAT         , y, y, y, y, n, y, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[68 ] = { 68 , RW_ATTRX(old_coefficient_c)             , ATTR_TYPE_FLOAT         , y, y, y, y, n, y, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[69 ] = { 69 , RW_ATTRX(thermistor_config)             , ATTR_TYPE_U8            , y, y, y, y, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 15         },
	[70 ] = { 70 , RO_ATTRX(temperature_result_1)          , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , attr_prepare_temperature_result_1   , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[71 ] = { 71 , RO_ATTRX(temperature_result_2)          , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , attr_prepare_temperature_result_2   , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[72 ] = { 72 , RO_ATTRX(temperature_result_3)          , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , attr_prepare_temperature_result_3   , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[73 ] = { 73 , RO_ATTRX(temperature_result_4)          , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , attr_prepare_temperature_result_4   , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[74 ] = { 74 , RO_ATTRE(temperature_alarms)            , ATTR_TYPE_U32           , n, y, y, y, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 1048575    },
	[75 ] = { 75 , RO_ATTRX(power_voltage_mv)              , ATTR_TYPE_U16           , n, n, y, n, n, n, 0, av_uint16           , attr_prepare_power_voltage_mv       , .min.ux = 0         , .max.ux = 3800       },
	[76 ] = { 76 , RO_ATTRE(digital_input)                 , ATTR_TYPE_U8            , n, n, y, y, n, n, 0, av_uint8            , attr_prepare_digital_input          , .min.ux = 0         , .max.ux = 3          },
	[77 ] = { 77 , RW_ATTRX(digital_alarms)                , ATTR_TYPE_U8            , y, y, y, y, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 3          },
	[78 ] = { 78 , RW_ATTRX(digital_input_1_config)        , ATTR_TYPE_U8            , y, y, y, y, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 131        },
	[79 ] = { 79 , RW_ATTRX(digital_input_2_config)        , ATTR_TYPE_U8            , y, y, y, y, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 131        },
	[80 ] = { 80 , RO_ATTRX(analog_input_1)                , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , attr_prepare_analog_input_1         , .min.fx = 0.0       , .max.fx = 15000.0    },
	[81 ] = { 81 , RO_ATTRX(analog_input_2)                , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , attr_prepare_analog_input_2         , .min.fx = 0.0       , .max.fx = 15000.0    },
	[82 ] = { 82 , RO_ATTRX(analog_input_3)                , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , attr_prepare_analog_input_3         , .min.fx = 0.0       , .max.fx = 15000.0    },
	[83 ] = { 83 , RO_ATTRX(analog_input_4)                , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , attr_prepare_analog_input_4         , .min.fx = 0.0       , .max.fx = 15000.0    },
	[84 ] = { 84 , RO_ATTRE(analog_alarms)                 , ATTR_TYPE_U32           , n, y, y, y, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 1048575    },
	[85 ] = { 85 , RW_ATTRE(analog_input_1_type)           , ATTR_TYPE_U8            , y, y, y, y, y, n, 0, av_aic              , NULL                                , .min.ux = 0         , .max.ux = 7          },
	[86 ] = { 86 , RW_ATTRX(analog_input_2_type)           , ATTR_TYPE_U8            , y, y, y, y, y, n, 0, av_aic              , NULL                                , .min.ux = 0         , .max.ux = 7          },
	[87 ] = { 87 , RW_ATTRX(analog_input_3_type)           , ATTR_TYPE_U8            , y, y, y, y, y, n, 0, av_aic              , NULL                                , .min.ux = 0         , .max.ux = 7          },
	[88 ] = { 88 , RW_ATTRX(analog_input_4_type)           , ATTR_TYPE_U8            , y, y, y, y, y, n, 0, av_aic              , NULL                                , .min.ux = 0         , .max.ux = 7          },
	[89 ] = { 89 , RO_ATTRX(flags)                         , ATTR_TYPE_U32           , n, n, y, n, y, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[90 ] = { 90 , RO_ATTRE(magnet_state)                  , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[91 ] = { 91 , RO_ATTRS(param_path)                    , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 2         , .max.ux = 8          },
	[92 ] = { 92 , RO_ATTRX(battery_age)                   , ATTR_TYPE_U32           , n, n, y, n, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[93 ] = { 93 , RO_ATTRS(attribute_version)             , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 6         , .max.ux = 11         },
	[94 ] = { 94 , RO_ATTRX(qrtc)                          , ATTR_TYPE_U32           , n, n, y, n, n, n, 0, av_uint32           , attr_prepare_qrtc                   , .min.ux = 0         , .max.ux = 0          },
	[95 ] = { 95 , RW_ATTRX(qrtc_last_set)                 , ATTR_TYPE_U32           , y, n, y, n, y, n, 0, av_uint32           , attr_prepare_qrtc_last_set          , .min.ux = 0         , .max.ux = 0          },
	[96 ] = { 96 , RW_ATTRX(sh_offset)                     , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[97 ] = { 97 , RW_ATTRX(analog_sense_interval)         , ATTR_TYPE_U32           , y, y, y, y, y, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 86400      },
	[98 ] = { 98 , RW_ATTRE(tamper_switch_status)          , ATTR_TYPE_BOOL          , y, n, y, n, y, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[99 ] = { 99 , RO_ATTRX(connection_timeout_sec)        , ATTR_TYPE_U8            , n, y, y, y, y, y, 0, av_cp8              , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[100] = { 100, RW_ATTRX(settings_passcode)             , ATTR_TYPE_U32           , y, y, n, n, y, n, 0, av_cp32             , NULL                                , .min.ux = 0         , .max.ux = 999999     },
	[101] = { 101, RW_ATTRX(therm_1_coefficient_a)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[102] = { 102, RW_ATTRX(therm_2_coefficient_a)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[103] = { 103, RW_ATTRX(therm_3_coefficient_a)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[104] = { 104, RW_ATTRX(therm_4_coefficient_a)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[105] = { 105, RW_ATTRX(therm_1_coefficient_b)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[106] = { 106, RW_ATTRX(therm_2_coefficient_b)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[107] = { 107, RW_ATTRX(therm_3_coefficient_b)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[108] = { 108, RW_ATTRX(therm_4_coefficient_b)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[109] = { 109, RW_ATTRX(therm_1_coefficient_c)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[110] = { 110, RW_ATTRX(therm_2_coefficient_c)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[111] = { 111, RW_ATTRX(therm_3_coefficient_c)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[112] = { 112, RW_ATTRX(therm_4_coefficient_c)         , ATTR_TYPE_FLOAT         , y, y, y, y, n, n, 0, av_float            , NULL                                , .min.fx = 1.2e-38   , .max.fx = 3.4e+38    },
	[113] = { 113, RW_ATTRX(data_logging_enable)           , ATTR_TYPE_BOOL          , y, y, y, y, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[114] = { 114, RW_ATTRX(factory_reset_enable)          , ATTR_TYPE_BOOL          , y, y, y, y, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[115] = { 115, RO_ATTRE(log_file_status)               , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , attr_prepare_log_file_status        , .min.ux = 0         , .max.ux = 3          },
	[116] = { 116, RW_ATTRX(temperature_alarms_enable)     , ATTR_TYPE_U32           , y, n, y, y, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 1048575    },
	[117] = { 117, RW_ATTRX(analog_alarms_enable)          , ATTR_TYPE_U32           , y, n, y, y, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 1048575    },
	[118] = { 118, RO_ATTRX(adc_power_simulated)           , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[119] = { 119, RO_ATTRX(adc_power_simulated_counts)    , ATTR_TYPE_S16           , n, n, y, n, n, n, 0, av_int16            , NULL                                , .min.ux = 0         , .max.ux = 4095       },
	[120] = { 120, RO_ATTRX(adc_analog_sensor_simulated)   , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[121] = { 121, RO_ATTRX(adc_analog_sensor_simulated_counts), ATTR_TYPE_S16           , n, n, y, n, n, n, 0, av_int16            , NULL                                , .min.ux = 0         , .max.ux = 4095       },
	[122] = { 122, RO_ATTRX(adc_thermistor_simulated)      , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[123] = { 123, RO_ATTRX(adc_thermistor_simulated_counts), ATTR_TYPE_S16           , n, n, y, n, n, n, 0, av_int16            , NULL                                , .min.ux = 0         , .max.ux = 4095       },
	[124] = { 124, RO_ATTRX(adc_vref_simulated)            , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[125] = { 125, RO_ATTRX(adc_vref_simulated_counts)     , ATTR_TYPE_S16           , n, n, y, n, n, n, 0, av_int16            , NULL                                , .min.ux = 0         , .max.ux = 4095       },
	[126] = { 126, RO_ATTRX(voltage_1_simulated)           , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[127] = { 127, RO_ATTRX(voltage_1_simulated_value)     , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[128] = { 128, RO_ATTRX(voltage_2_simulated)           , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[129] = { 129, RO_ATTRX(voltage_2_simulated_value)     , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[130] = { 130, RO_ATTRX(voltage_3_simulated)           , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[131] = { 131, RO_ATTRX(voltage_3_simulated_value)     , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[132] = { 132, RO_ATTRX(voltage_4_simulated)           , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[133] = { 133, RO_ATTRX(voltage_4_simulated_value)     , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[134] = { 134, RO_ATTRX(ultrasonic_simulated)          , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[135] = { 135, RO_ATTRX(ultrasonic_simulated_value)    , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[136] = { 136, RO_ATTRX(pressure_simulated)            , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[137] = { 137, RO_ATTRX(pressure_simulated_value)      , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[138] = { 138, RO_ATTRX(current_1_simulated)           , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[139] = { 139, RO_ATTRX(current_1_simulated_value)     , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[140] = { 140, RO_ATTRX(current_2_simulated)           , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[141] = { 141, RO_ATTRX(current_2_simulated_value)     , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[142] = { 142, RO_ATTRX(current_3_simulated)           , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[143] = { 143, RO_ATTRX(current_3_simulated_value)     , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[144] = { 144, RO_ATTRX(current_4_simulated)           , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[145] = { 145, RO_ATTRX(current_4_simulated_value)     , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[146] = { 146, RO_ATTRX(vref_simulated)                , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[147] = { 147, RO_ATTRX(vref_simulated_value)          , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[148] = { 148, RO_ATTRX(temperature_1_simulated)       , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[149] = { 149, RO_ATTRX(temperature_1_simulated_value) , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[150] = { 150, RO_ATTRX(temperature_2_simulated)       , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[151] = { 151, RO_ATTRX(temperature_2_simulated_value) , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[152] = { 152, RO_ATTRX(temperature_3_simulated)       , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[153] = { 153, RO_ATTRX(temperature_3_simulated_value) , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[154] = { 154, RO_ATTRX(temperature_4_simulated)       , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[155] = { 155, RO_ATTRX(temperature_4_simulated_value) , ATTR_TYPE_FLOAT         , n, n, y, n, n, n, 0, av_float            , NULL                                , .min.fx = -3.4e+38  , .max.fx = 3.4e+38    },
	[156] = { 156, RO_ATTRX(powermv_simulated)             , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[157] = { 157, RO_ATTRX(powermv_simulated_value)       , ATTR_TYPE_S32           , n, n, y, n, n, n, 0, av_int32            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[158] = { 158, RO_ATTRX(digital_input_1_simulated)     , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_din1simen        , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[159] = { 159, RO_ATTRX(digital_input_1_simulated_value), ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_din1sim          , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[160] = { 160, RO_ATTRX(digital_input_2_simulated)     , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_din2simen        , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[161] = { 161, RO_ATTRX(digital_input_2_simulated_value), ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_din2sim          , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[162] = { 162, RO_ATTRX(mag_switch_simulated)          , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_magsimen         , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[163] = { 163, RO_ATTRX(mag_switch_simulated_value)    , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_magsim           , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[164] = { 164, RO_ATTRX(tamper_switch_simulated)       , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_tampsimen        , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[165] = { 165, RO_ATTRX(tamper_switch_simulated_value) , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_tampsim          , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[166] = { 166, RW_ATTRE(boot_phy)                      , ATTR_TYPE_U8            , y, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 2          },
	[167] = { 167, RO_ATTRX(mobile_app_disconnect)         , ATTR_TYPE_BOOL          , n, n, y, n, y, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[168] = { 168, RO_ATTRX(attr_save_error_code)          , ATTR_TYPE_S32           , n, n, y, n, y, n, 0, av_int32            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[169] = { 170, RO_ATTRX(recover_settings_count)        , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[170] = { 171, RW_ATTRX(block_downgrades)              , ATTR_TYPE_BOOL          , y, n, y, y, n, n, 0, av_block_downgrades , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[171] = { 172, RO_ATTRX(security_request)              , ATTR_TYPE_BOOL          , n, n, n, n, y, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[172] = { 173, RO_ATTRX(security_level)                , ATTR_TYPE_S8            , n, n, y, n, n, n, 0, av_int8             , attr_prepare_security_level         , .min.sx = -1        , .max.sx = 4          },
	[173] = { 208, RW_ATTRS(load_path)                     , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[174] = { 209, RW_ATTRS(dump_path)                     , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[175] = { 265, RO_ATTRE(settings_passcode_status)      , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 2          },
	[176] = { 266, RO_ATTRE(lock_status)                   , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 2          }
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
	[30 ] = &ATTR_TABLE[30 ],
	[31 ] = &ATTR_TABLE[31 ],
	[32 ] = &ATTR_TABLE[32 ],
	[33 ] = &ATTR_TABLE[33 ],
	[34 ] = &ATTR_TABLE[34 ],
	[35 ] = &ATTR_TABLE[35 ],
	[36 ] = &ATTR_TABLE[36 ],
	[37 ] = &ATTR_TABLE[37 ],
	[38 ] = &ATTR_TABLE[38 ],
	[39 ] = &ATTR_TABLE[39 ],
	[40 ] = &ATTR_TABLE[40 ],
	[41 ] = &ATTR_TABLE[41 ],
	[42 ] = &ATTR_TABLE[42 ],
	[43 ] = &ATTR_TABLE[43 ],
	[44 ] = &ATTR_TABLE[44 ],
	[45 ] = &ATTR_TABLE[45 ],
	[46 ] = &ATTR_TABLE[46 ],
	[47 ] = &ATTR_TABLE[47 ],
	[48 ] = &ATTR_TABLE[48 ],
	[49 ] = &ATTR_TABLE[49 ],
	[50 ] = &ATTR_TABLE[50 ],
	[51 ] = &ATTR_TABLE[51 ],
	[52 ] = &ATTR_TABLE[52 ],
	[53 ] = &ATTR_TABLE[53 ],
	[54 ] = &ATTR_TABLE[54 ],
	[55 ] = &ATTR_TABLE[55 ],
	[56 ] = &ATTR_TABLE[56 ],
	[57 ] = &ATTR_TABLE[57 ],
	[58 ] = &ATTR_TABLE[58 ],
	[59 ] = &ATTR_TABLE[59 ],
	[60 ] = &ATTR_TABLE[60 ],
	[61 ] = &ATTR_TABLE[61 ],
	[62 ] = &ATTR_TABLE[62 ],
	[63 ] = &ATTR_TABLE[63 ],
	[64 ] = &ATTR_TABLE[64 ],
	[65 ] = &ATTR_TABLE[65 ],
	[66 ] = &ATTR_TABLE[66 ],
	[67 ] = &ATTR_TABLE[67 ],
	[68 ] = &ATTR_TABLE[68 ],
	[69 ] = &ATTR_TABLE[69 ],
	[70 ] = &ATTR_TABLE[70 ],
	[71 ] = &ATTR_TABLE[71 ],
	[72 ] = &ATTR_TABLE[72 ],
	[73 ] = &ATTR_TABLE[73 ],
	[74 ] = &ATTR_TABLE[74 ],
	[75 ] = &ATTR_TABLE[75 ],
	[76 ] = &ATTR_TABLE[76 ],
	[77 ] = &ATTR_TABLE[77 ],
	[78 ] = &ATTR_TABLE[78 ],
	[79 ] = &ATTR_TABLE[79 ],
	[80 ] = &ATTR_TABLE[80 ],
	[81 ] = &ATTR_TABLE[81 ],
	[82 ] = &ATTR_TABLE[82 ],
	[83 ] = &ATTR_TABLE[83 ],
	[84 ] = &ATTR_TABLE[84 ],
	[85 ] = &ATTR_TABLE[85 ],
	[86 ] = &ATTR_TABLE[86 ],
	[87 ] = &ATTR_TABLE[87 ],
	[88 ] = &ATTR_TABLE[88 ],
	[89 ] = &ATTR_TABLE[89 ],
	[90 ] = &ATTR_TABLE[90 ],
	[91 ] = &ATTR_TABLE[91 ],
	[92 ] = &ATTR_TABLE[92 ],
	[93 ] = &ATTR_TABLE[93 ],
	[94 ] = &ATTR_TABLE[94 ],
	[95 ] = &ATTR_TABLE[95 ],
	[96 ] = &ATTR_TABLE[96 ],
	[97 ] = &ATTR_TABLE[97 ],
	[98 ] = &ATTR_TABLE[98 ],
	[99 ] = &ATTR_TABLE[99 ],
	[100] = &ATTR_TABLE[100],
	[101] = &ATTR_TABLE[101],
	[102] = &ATTR_TABLE[102],
	[103] = &ATTR_TABLE[103],
	[104] = &ATTR_TABLE[104],
	[105] = &ATTR_TABLE[105],
	[106] = &ATTR_TABLE[106],
	[107] = &ATTR_TABLE[107],
	[108] = &ATTR_TABLE[108],
	[109] = &ATTR_TABLE[109],
	[110] = &ATTR_TABLE[110],
	[111] = &ATTR_TABLE[111],
	[112] = &ATTR_TABLE[112],
	[113] = &ATTR_TABLE[113],
	[114] = &ATTR_TABLE[114],
	[115] = &ATTR_TABLE[115],
	[116] = &ATTR_TABLE[116],
	[117] = &ATTR_TABLE[117],
	[118] = &ATTR_TABLE[118],
	[119] = &ATTR_TABLE[119],
	[120] = &ATTR_TABLE[120],
	[121] = &ATTR_TABLE[121],
	[122] = &ATTR_TABLE[122],
	[123] = &ATTR_TABLE[123],
	[124] = &ATTR_TABLE[124],
	[125] = &ATTR_TABLE[125],
	[126] = &ATTR_TABLE[126],
	[127] = &ATTR_TABLE[127],
	[128] = &ATTR_TABLE[128],
	[129] = &ATTR_TABLE[129],
	[130] = &ATTR_TABLE[130],
	[131] = &ATTR_TABLE[131],
	[132] = &ATTR_TABLE[132],
	[133] = &ATTR_TABLE[133],
	[134] = &ATTR_TABLE[134],
	[135] = &ATTR_TABLE[135],
	[136] = &ATTR_TABLE[136],
	[137] = &ATTR_TABLE[137],
	[138] = &ATTR_TABLE[138],
	[139] = &ATTR_TABLE[139],
	[140] = &ATTR_TABLE[140],
	[141] = &ATTR_TABLE[141],
	[142] = &ATTR_TABLE[142],
	[143] = &ATTR_TABLE[143],
	[144] = &ATTR_TABLE[144],
	[145] = &ATTR_TABLE[145],
	[146] = &ATTR_TABLE[146],
	[147] = &ATTR_TABLE[147],
	[148] = &ATTR_TABLE[148],
	[149] = &ATTR_TABLE[149],
	[150] = &ATTR_TABLE[150],
	[151] = &ATTR_TABLE[151],
	[152] = &ATTR_TABLE[152],
	[153] = &ATTR_TABLE[153],
	[154] = &ATTR_TABLE[154],
	[155] = &ATTR_TABLE[155],
	[156] = &ATTR_TABLE[156],
	[157] = &ATTR_TABLE[157],
	[158] = &ATTR_TABLE[158],
	[159] = &ATTR_TABLE[159],
	[160] = &ATTR_TABLE[160],
	[161] = &ATTR_TABLE[161],
	[162] = &ATTR_TABLE[162],
	[163] = &ATTR_TABLE[163],
	[164] = &ATTR_TABLE[164],
	[165] = &ATTR_TABLE[165],
	[166] = &ATTR_TABLE[166],
	[167] = &ATTR_TABLE[167],
	[168] = &ATTR_TABLE[168],
	[170] = &ATTR_TABLE[169],
	[171] = &ATTR_TABLE[170],
	[172] = &ATTR_TABLE[171],
	[173] = &ATTR_TABLE[172],
	[208] = &ATTR_TABLE[173],
	[209] = &ATTR_TABLE[174],
	[265] = &ATTR_TABLE[175],
	[266] = &ATTR_TABLE[176]
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

__weak int attr_prepare_temperature_result_2(void)
{
	return 0;
}

__weak int attr_prepare_temperature_result_3(void)
{
	return 0;
}

__weak int attr_prepare_temperature_result_4(void)
{
	return 0;
}

__weak int attr_prepare_power_voltage_mv(void)
{
	return 0;
}

__weak int attr_prepare_digital_input(void)
{
	return 0;
}

__weak int attr_prepare_analog_input_1(void)
{
	return 0;
}

__weak int attr_prepare_analog_input_2(void)
{
	return 0;
}

__weak int attr_prepare_analog_input_3(void)
{
	return 0;
}

__weak int attr_prepare_analog_input_4(void)
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

const char *const attr_get_string_temperature_alarms(int value)
{
	switch (value) {
		case 1:           return "Port1 H1 Bitmask";
		case 2:           return "Port1 H2 Bitmask";
		case 4:           return "Port1 L1 Bitmask";
		case 8:           return "Port1 L2 Bitmask";
		case 16:          return "Port1 Delta Bitmask";
		case 32:          return "Port2 H1 Bitmask";
		case 64:          return "Port2 H2 Bitmask";
		case 128:         return "Port2 L1 Bitmask";
		case 256:         return "Port2 L2 Bitmask";
		case 512:         return "Port2Delta Bitmask";
		case 1024:        return "Port3 H1 Bitmask";
		case 2048:        return "Port3 H2 Bitmask";
		case 4096:        return "Port3 L1 Bitmask";
		case 8192:        return "Port3 L2 Bitmask";
		case 16384:       return "Port3Delta Bitmask";
		case 32768:       return "Port4 H1 Bitmask";
		case 65535:       return "Port4 H2 Bitmask";
		case 131072:      return "Port4 L1 Bitmask";
		case 262144:      return "Port4 L2 Bitmask";
		case 524288:      return "Port4 Delta Bitmask";
		default:          return "?";
	}
}

const char *const attr_get_string_digital_input(int value)
{
	switch (value) {
		case 1:           return "Port1 Bitmask";
		case 2:           return "Port2 Bitmask";
		default:          return "?";
	}
}

const char *const attr_get_string_analog_alarms(int value)
{
	switch (value) {
		case 1:           return "Port1 H1 Bitmask";
		case 2:           return "Port1 H2 Bitmask";
		case 4:           return "Port1 L1 Bitmask";
		case 8:           return "Port1 L2 Bitmask";
		case 16:          return "Port1 Delta Bitmask";
		case 32:          return "Port2 H1 Bitmask";
		case 64:          return "Port2 H2 Bitmask";
		case 128:         return "Port2 L1 Bitmask";
		case 256:         return "Port2 L2 Bitmask";
		case 512:         return "Port2Delta Bitmask";
		case 1024:        return "Port3 H1 Bitmask";
		case 2048:        return "Port3 H2 Bitmask";
		case 4096:        return "Port3 L1 Bitmask";
		case 8192:        return "Port3 L2 Bitmask";
		case 16384:       return "Port3Delta Bitmask";
		case 32768:       return "Port4 H1 Bitmask";
		case 65535:       return "Port4 H2 Bitmask";
		case 131072:      return "Port4 L1 Bitmask";
		case 262144:      return "Port4 L2 Bitmask";
		case 524288:      return "Port4 Delta Bitmask";
		default:          return "?";
	}
}

const char *const attr_get_string_analog_input_1_type(int value)
{
	switch (value) {
		case 0:           return "Unused";
		case 1:           return "Voltage 0 V To 10 V Dc";
		case 2:           return "Current 4 Ma To 20 Ma";
		case 3:           return "Pressure";
		case 4:           return "Ultrasonic";
		case 5:           return "Ac Current 20 A";
		case 6:           return "Ac Current 150 A";
		case 7:           return "Ac Current 500 A";
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

const char *const attr_get_string_tamper_switch_status(int value)
{
	switch (value) {
		case true:        return "Active Not Pressed";
		case false:       return "Inactive Pressed";
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