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
	char location[32 + 1];
	uint32_t passkey;
	bool lock;
	int8_t tx_power;
	uint16_t network_id;
	uint8_t hardware_minor_version;
	uint32_t qrtc_last_set;
	bool commissioned;
	char root_ca_name[48 + 1];
	char client_cert_name[48 + 1];
	char client_key_name[48 + 1];
	char endpoint[254 + 1];
	char port[16 + 1];
	char client_id[32 + 1];
	char topic_prefix[32 + 1];
	enum peer_verify peer_verify;
	uint16_t battery_low_threshold;
	uint16_t battery_alarm_threshold;
	uint16_t battery_4;
	uint16_t battery_3;
	uint16_t battery_2;
	uint16_t battery_1;
	uint16_t battery_0;
	uint8_t motion_odr;
	uint8_t motion_thresh;
	uint8_t motion_scale;
	uint8_t motion_duration;
	uint8_t sd_log_max_size;
	uint8_t ct_aes_key[16];
	uint32_t join_delay;
	uint16_t join_min;
	uint16_t join_max;
	uint32_t join_interval;
	bool delay_cloud_reconnect;
	uint32_t modem_desired_log_level;
	char load_path[32 + 1];
	char dump_path[32 + 1];
	bool nv_imported;
	float floaty;
	uint8_t lwm2m_psk[16];
	char lwm2m_client_id[32 + 1];
	char lwm2m_peer_url[128 + 1];
	uint32_t gps_rate;
	char polte_user[16 + 1];
	char polte_password[16 + 1];
	uint32_t ble_prepare_timeout;
	int8_t temperature_offset;
	bool lwm2m_enable_bootstrap;
	/* pyend */
} rw_attribute_t;

static const rw_attribute_t DEFAULT_RW_ATTRIBUTE_VALUES = {
	/* pystart - rw defaults */
	.location = "",
	.passkey = 123456,
	.lock = false,
	.tx_power = 0,
	.network_id = 0,
	.hardware_minor_version = 0,
	.qrtc_last_set = 0,
	.commissioned = false,
	.root_ca_name = "/lfs/root_ca.pem",
	.client_cert_name = "/lfs/client_cert.pem",
	.client_key_name = "/lfs/client_key.pem",
	.endpoint = "a3273rvo818l4w-ats.iot.us-east-1.amazonaws.com",
	.port = "8883",
	.client_id = "",
	.topic_prefix = "mg100-ct/dev/gw/",
	.peer_verify = 0,
	.battery_low_threshold = 3400,
	.battery_alarm_threshold = 3000,
	.battery_4 = 4200,
	.battery_3 = 3800,
	.battery_2 = 3400,
	.battery_1 = 3000,
	.battery_0 = 2750,
	.motion_odr = 5,
	.motion_thresh = 10,
	.motion_scale = 2,
	.motion_duration = 6,
	.sd_log_max_size = 0,
	.ct_aes_key = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
	.join_delay = 0,
	.join_min = 1,
	.join_max = 100,
	.join_interval = 1,
	.delay_cloud_reconnect = false,
	.modem_desired_log_level = 1,
	.load_path = "/lfs/params.txt",
	.dump_path = "/lfs/dump.txt",
	.nv_imported = false,
	.floaty = 0.13,
	.lwm2m_psk = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
	.lwm2m_client_id = "Client_identity",
	.lwm2m_peer_url = "uwterminalx.lairdconnect.com",
	.gps_rate = 0,
	.polte_user = "",
	.polte_password = "",
	.ble_prepare_timeout = 3600,
	.temperature_offset = 25,
	.lwm2m_enable_bootstrap = false
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
	uint16_t power_voltage_mv;
	char attribute_version[11 + 1];
	uint32_t qrtc;
	char name[32 + 1];
	char board[32 + 1];
	char build_id[64 + 1];
	char app_type[32 + 1];
	char mount[32 + 1];
	enum cert_status cert_status;
	enum gateway_state gateway_state;
	uint8_t battery_capacity;
	int16_t battery_temperature;
	uint8_t battery_charging_state;
	bool battery_alarm;
	bool motion_alarm;
	float power_supply_voltage;
	char gateway_id[15 + 1];
	enum lte_network_state lte_network_state;
	enum lte_startup_state lte_startup_state;
	int16_t lte_rsrp;
	int16_t lte_sinr;
	enum lte_sleep_state lte_sleep_state;
	enum lte_rat lte_rat;
	char iccid[20 + 1];
	char lte_serial_number[14 + 1];
	char lte_version[29 + 1];
	char bands[20 + 1];
	char active_bands[20 + 1];
	enum central_state central_state;
	char sensor_bluetooth_address[30 + 1];
	enum modem_boot modem_boot;
	char apn[64 + 1];
	char apn_username[65 + 1];
	char apn_password[65 + 1];
	uint8_t apn_control_point;
	int32_t apn_status;
	uint8_t lte_operator_index;
	enum fota_control_point fota_control_point;
	enum fota_status fota_status;
	char fota_file_name[64 + 1];
	uint32_t fota_size;
	uint32_t fota_count;
	enum generate_psk generate_psk;
	enum lte_init_error lte_init_error;
	enum cloud_error cloud_error;
	bool commissioning_busy;
	char imsi[15 + 1];
	enum modem_functionality modem_functionality;
	char gps_latitude[32 + 1];
	char gps_longitude[32 + 1];
	char gps_time[32 + 1];
	char gps_fix_type[3 + 1];
	char gps_hepe[16 + 1];
	char gps_altitude[16 + 1];
	char gps_alt_unc[16 + 1];
	char gps_heading[16 + 1];
	char gps_hor_speed[16 + 1];
	char gps_ver_speed[16 + 1];
	enum gps_status gps_status;
	enum polte_control_point polte_control_point;
	enum polte_status polte_status;
	char polte_latitude[32 + 1];
	char polte_longitude[32 + 1];
	char polte_confidence[16 + 1];
	uint32_t polte_timestamp;
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
	.power_voltage_mv = 0,
	.attribute_version = "0.4.106",
	.qrtc = 0,
	.name = "",
	.board = "",
	.build_id = "0",
	.app_type = "",
	.mount = "/lfs",
	.cert_status = 0,
	.gateway_state = 0,
	.battery_capacity = 0,
	.battery_temperature = -127,
	.battery_charging_state = 0,
	.battery_alarm = false,
	.motion_alarm = false,
	.power_supply_voltage = 0,
	.gateway_id = "",
	.lte_network_state = 0,
	.lte_startup_state = 0,
	.lte_rsrp = 0,
	.lte_sinr = 0,
	.lte_sleep_state = 0,
	.lte_rat = 0,
	.iccid = "",
	.lte_serial_number = "",
	.lte_version = "",
	.bands = "",
	.active_bands = "",
	.central_state = 0,
	.sensor_bluetooth_address = "",
	.modem_boot = 0,
	.apn = "",
	.apn_username = "",
	.apn_password = "",
	.apn_control_point = 0,
	.apn_status = 0,
	.lte_operator_index = 255,
	.fota_control_point = 0,
	.fota_status = 0,
	.fota_file_name = "",
	.fota_size = 0,
	.fota_count = 0,
	.generate_psk = 0,
	.lte_init_error = 0,
	.cloud_error = 0,
	.commissioning_busy = false,
	.imsi = "",
	.modem_functionality = 0,
	.gps_latitude = "",
	.gps_longitude = "",
	.gps_time = "",
	.gps_fix_type = "",
	.gps_hepe = "",
	.gps_altitude = "",
	.gps_alt_unc = "",
	.gps_heading = "",
	.gps_hor_speed = "",
	.gps_ver_speed = "",
	.gps_status = -1,
	.polte_control_point = 0,
	.polte_status = 0,
	.polte_latitude = "",
	.polte_longitude = "",
	.polte_confidence = "",
	.polte_timestamp = 0
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
	[0  ] = { 1  , RW_ATTRS(location)                      , ATTR_TYPE_STRING        , 0x17  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[1  ] = { 4  , RW_ATTRX(passkey)                       , ATTR_TYPE_U32           , 0x1f  , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 999999     },
	[2  ] = { 5  , RW_ATTRX(lock)                          , ATTR_TYPE_BOOL          , 0x16  , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[3  ] = { 11 , RO_ATTRS(firmware_version)              , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 3         , .max.ux = 11         },
	[4  ] = { 12 , RO_ATTRS(reset_reason)                  , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 12         },
	[5  ] = { 13 , RO_ATTRS(bluetooth_address)             , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 12        , .max.ux = 12         },
	[6  ] = { 14 , RO_ATTRX(reset_count)                   , ATTR_TYPE_U32           , 0x2   , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[7  ] = { 16 , RO_ATTRX(up_time)                       , ATTR_TYPE_S64           , 0x2   , av_int64            , attr_prepare_up_time                , .min.ux = 0         , .max.ux = 0          },
	[8  ] = { 59 , RW_ATTRX(tx_power)                      , ATTR_TYPE_S8            , 0x1b  , av_tx_power         , NULL                                , .min.sx = -40       , .max.sx = 8          },
	[9  ] = { 60 , RW_ATTRX(network_id)                    , ATTR_TYPE_U16           , 0x1f  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[10 ] = { 61 , RO_ATTRX(config_version)                , ATTR_TYPE_U8            , 0xa   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[11 ] = { 63 , RW_ATTRX(hardware_minor_version)        , ATTR_TYPE_U8            , 0x1b  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[12 ] = { 75 , RO_ATTRX(power_voltage_mv)              , ATTR_TYPE_U16           , 0x2   , av_uint16           , attr_prepare_power_voltage_mv       , .min.ux = 0         , .max.ux = 3800       },
	[13 ] = { 93 , RO_ATTRS(attribute_version)             , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 6         , .max.ux = 11         },
	[14 ] = { 94 , RO_ATTRX(qrtc)                          , ATTR_TYPE_U32           , 0x2   , av_uint32           , attr_prepare_qrtc                   , .min.ux = 0         , .max.ux = 0          },
	[15 ] = { 95 , RW_ATTRX(qrtc_last_set)                 , ATTR_TYPE_U32           , 0x1a  , av_uint32           , attr_prepare_qrtc_last_set          , .min.ux = 0         , .max.ux = 0          },
	[16 ] = { 140, RO_ATTRS(name)                          , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 1         , .max.ux = 32         },
	[17 ] = { 142, RO_ATTRS(board)                         , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 1         , .max.ux = 32         },
	[18 ] = { 143, RO_ATTRS(build_id)                      , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 1         , .max.ux = 64         },
	[19 ] = { 144, RO_ATTRS(app_type)                      , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 1         , .max.ux = 32         },
	[20 ] = { 145, RO_ATTRS(mount)                         , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[21 ] = { 146, RW_ATTRX(commissioned)                  , ATTR_TYPE_BOOL          , 0x1b  , av_cpb              , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[22 ] = { 147, RO_ATTRE(cert_status)                   , ATTR_TYPE_S32           , 0x2   , av_int32            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[23 ] = { 148, RW_ATTRS(root_ca_name)                  , ATTR_TYPE_STRING        , 0x1b  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 48         },
	[24 ] = { 149, RW_ATTRS(client_cert_name)              , ATTR_TYPE_STRING        , 0x1b  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 48         },
	[25 ] = { 150, RW_ATTRS(client_key_name)               , ATTR_TYPE_STRING        , 0x1b  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 48         },
	[26 ] = { 151, RW_ATTRS(endpoint)                      , ATTR_TYPE_STRING        , 0x1b  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 254        },
	[27 ] = { 152, RW_ATTRS(port)                          , ATTR_TYPE_STRING        , 0x1b  , av_string           , NULL                                , .min.ux = 4         , .max.ux = 16         },
	[28 ] = { 153, RW_ATTRS(client_id)                     , ATTR_TYPE_STRING        , 0x1b  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[29 ] = { 154, RW_ATTRS(topic_prefix)                  , ATTR_TYPE_STRING        , 0x1b  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[30 ] = { 155, RO_ATTRE(gateway_state)                 , ATTR_TYPE_U8            , 0x2   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[31 ] = { 156, RW_ATTRE(peer_verify)                   , ATTR_TYPE_S8            , 0x13  , av_int8             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[32 ] = { 157, RO_ATTRX(battery_capacity)              , ATTR_TYPE_U8            , 0x2   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 4          },
	[33 ] = { 158, RO_ATTRX(battery_temperature)           , ATTR_TYPE_S16           , 0x2   , av_int16            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[34 ] = { 159, RO_ATTRX(battery_charging_state)        , ATTR_TYPE_U8            , 0x2   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[35 ] = { 160, RW_ATTRX(battery_low_threshold)         , ATTR_TYPE_U16           , 0x1b  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[36 ] = { 161, RW_ATTRX(battery_alarm_threshold)       , ATTR_TYPE_U16           , 0x1b  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[37 ] = { 162, RW_ATTRX(battery_4)                     , ATTR_TYPE_U16           , 0x1b  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[38 ] = { 163, RW_ATTRX(battery_3)                     , ATTR_TYPE_U16           , 0x1b  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[39 ] = { 164, RW_ATTRX(battery_2)                     , ATTR_TYPE_U16           , 0x1b  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[40 ] = { 165, RW_ATTRX(battery_1)                     , ATTR_TYPE_U16           , 0x1b  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[41 ] = { 166, RW_ATTRX(battery_0)                     , ATTR_TYPE_U16           , 0x1b  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[42 ] = { 167, RO_ATTRX(battery_alarm)                 , ATTR_TYPE_BOOL          , 0x2   , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[43 ] = { 168, RW_ATTRX(motion_odr)                    , ATTR_TYPE_U8            , 0x1b  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[44 ] = { 169, RW_ATTRX(motion_thresh)                 , ATTR_TYPE_U8            , 0x1b  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[45 ] = { 170, RW_ATTRX(motion_scale)                  , ATTR_TYPE_U8            , 0x1b  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[46 ] = { 171, RW_ATTRX(motion_duration)               , ATTR_TYPE_U8            , 0x1b  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[47 ] = { 172, RO_ATTRX(motion_alarm)                  , ATTR_TYPE_BOOL          , 0x2   , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[48 ] = { 173, RW_ATTRX(sd_log_max_size)               , ATTR_TYPE_U8            , 0x13  , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[49 ] = { 174, RW_ATTRX(ct_aes_key)                    , ATTR_TYPE_BYTE_ARRAY    , 0x11  , av_array            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[50 ] = { 175, RO_ATTRX(power_supply_voltage)          , ATTR_TYPE_FLOAT         , 0x2   , av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 10.0       },
	[51 ] = { 176, RO_ATTRS(gateway_id)                    , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 15         },
	[52 ] = { 177, RO_ATTRE(lte_network_state)             , ATTR_TYPE_U8            , 0x2   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[53 ] = { 178, RO_ATTRE(lte_startup_state)             , ATTR_TYPE_U8            , 0x2   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[54 ] = { 179, RO_ATTRX(lte_rsrp)                      , ATTR_TYPE_S16           , 0x2   , av_int16            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[55 ] = { 180, RO_ATTRX(lte_sinr)                      , ATTR_TYPE_S16           , 0x2   , av_int16            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[56 ] = { 181, RO_ATTRE(lte_sleep_state)               , ATTR_TYPE_U8            , 0x2   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[57 ] = { 182, RO_ATTRE(lte_rat)                       , ATTR_TYPE_U8            , 0xb   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[58 ] = { 183, RO_ATTRS(iccid)                         , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 20         },
	[59 ] = { 184, RO_ATTRS(lte_serial_number)             , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 14         },
	[60 ] = { 185, RO_ATTRS(lte_version)                   , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 29         },
	[61 ] = { 186, RO_ATTRS(bands)                         , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 20        , .max.ux = 20         },
	[62 ] = { 187, RO_ATTRS(active_bands)                  , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 20        , .max.ux = 20         },
	[63 ] = { 188, RO_ATTRE(central_state)                 , ATTR_TYPE_U8            , 0x2   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[64 ] = { 189, RO_ATTRS(sensor_bluetooth_address)      , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 30         },
	[65 ] = { 190, RW_ATTRX(join_delay)                    , ATTR_TYPE_U32           , 0x1b  , av_cp32             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[66 ] = { 191, RW_ATTRX(join_min)                      , ATTR_TYPE_U16           , 0x13  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[67 ] = { 192, RW_ATTRX(join_max)                      , ATTR_TYPE_U16           , 0x13  , av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[68 ] = { 193, RW_ATTRX(join_interval)                 , ATTR_TYPE_U32           , 0x13  , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[69 ] = { 194, RO_ATTRE(modem_boot)                    , ATTR_TYPE_U8            , 0x2   , av_uint8            , attr_prepare_modem_boot             , .min.ux = 0         , .max.ux = 0          },
	[70 ] = { 195, RW_ATTRX(delay_cloud_reconnect)         , ATTR_TYPE_BOOL          , 0x13  , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[71 ] = { 196, RO_ATTRS(apn)                           , ATTR_TYPE_STRING        , 0x3   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 64         },
	[72 ] = { 197, RO_ATTRS(apn_username)                  , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 65         },
	[73 ] = { 198, RO_ATTRS(apn_password)                  , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 65         },
	[74 ] = { 199, RO_ATTRX(apn_control_point)             , ATTR_TYPE_U8            , 0x9   , av_cp8              , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[75 ] = { 200, RO_ATTRX(apn_status)                    , ATTR_TYPE_S32           , 0x2   , av_int32            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[76 ] = { 201, RW_ATTRX(modem_desired_log_level)       , ATTR_TYPE_U32           , 0x1b  , av_cp32             , NULL                                , .min.ux = 0         , .max.ux = 4          },
	[77 ] = { 202, RO_ATTRX(lte_operator_index)            , ATTR_TYPE_U8            , 0x2   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[78 ] = { 203, RO_ATTRE(fota_control_point)            , ATTR_TYPE_U8            , 0x9   , av_cp8              , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[79 ] = { 204, RO_ATTRE(fota_status)                   , ATTR_TYPE_U8            , 0x2   , av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[80 ] = { 205, RO_ATTRS(fota_file_name)                , ATTR_TYPE_STRING        , 0x3   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 64         },
	[81 ] = { 206, RO_ATTRX(fota_size)                     , ATTR_TYPE_U32           , 0x2   , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[82 ] = { 207, RO_ATTRX(fota_count)                    , ATTR_TYPE_U32           , 0x2   , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[83 ] = { 208, RW_ATTRS(load_path)                     , ATTR_TYPE_STRING        , 0x13  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[84 ] = { 209, RW_ATTRS(dump_path)                     , ATTR_TYPE_STRING        , 0x13  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[85 ] = { 210, RW_ATTRX(nv_imported)                   , ATTR_TYPE_BOOL          , 0x12  , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[86 ] = { 211, RW_ATTRX(floaty)                        , ATTR_TYPE_FLOAT         , 0x13  , av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 0.0        },
	[87 ] = { 212, RO_ATTRE(generate_psk)                  , ATTR_TYPE_U8            , 0x9   , av_cp8              , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[88 ] = { 213, RW_ATTRX(lwm2m_psk)                     , ATTR_TYPE_BYTE_ARRAY    , 0x12  , av_array            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[89 ] = { 214, RW_ATTRS(lwm2m_client_id)               , ATTR_TYPE_STRING        , 0x13  , av_string           , NULL                                , .min.ux = 1         , .max.ux = 32         },
	[90 ] = { 215, RW_ATTRS(lwm2m_peer_url)                , ATTR_TYPE_STRING        , 0x13  , av_string           , NULL                                , .min.ux = 6         , .max.ux = 128        },
	[91 ] = { 216, RO_ATTRE(lte_init_error)                , ATTR_TYPE_S8            , 0x2   , av_int8             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[92 ] = { 217, RO_ATTRE(cloud_error)                   , ATTR_TYPE_S8            , 0x2   , av_int8             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[93 ] = { 218, RO_ATTRX(commissioning_busy)            , ATTR_TYPE_BOOL          , 0x2   , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[94 ] = { 219, RO_ATTRS(imsi)                          , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 14        , .max.ux = 15         },
	[95 ] = { 220, RO_ATTRE(modem_functionality)           , ATTR_TYPE_S32           , 0x2   , av_int32            , attr_prepare_modem_functionality    , .min.ux = 0         , .max.ux = 0          },
	[96 ] = { 242, RW_ATTRX(gps_rate)                      , ATTR_TYPE_U32           , 0x19  , av_cp32             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[97 ] = { 243, RO_ATTRS(gps_latitude)                  , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[98 ] = { 244, RO_ATTRS(gps_longitude)                 , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[99 ] = { 245, RO_ATTRS(gps_time)                      , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[100] = { 246, RO_ATTRS(gps_fix_type)                  , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 3          },
	[101] = { 247, RO_ATTRS(gps_hepe)                      , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 16         },
	[102] = { 248, RO_ATTRS(gps_altitude)                  , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 16         },
	[103] = { 249, RO_ATTRS(gps_alt_unc)                   , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 16         },
	[104] = { 250, RO_ATTRS(gps_heading)                   , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 16         },
	[105] = { 251, RO_ATTRS(gps_hor_speed)                 , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 16         },
	[106] = { 252, RO_ATTRS(gps_ver_speed)                 , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 16         },
	[107] = { 253, RO_ATTRE(gps_status)                    , ATTR_TYPE_S8            , 0x2   , av_int8             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[108] = { 254, RO_ATTRE(polte_control_point)           , ATTR_TYPE_U8            , 0x9   , av_cp8              , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[109] = { 255, RO_ATTRE(polte_status)                  , ATTR_TYPE_S32           , 0x2   , av_int32            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[110] = { 256, RW_ATTRS(polte_user)                    , ATTR_TYPE_STRING        , 0x13  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 16         },
	[111] = { 257, RW_ATTRS(polte_password)                , ATTR_TYPE_STRING        , 0x13  , av_string           , NULL                                , .min.ux = 0         , .max.ux = 16         },
	[112] = { 258, RO_ATTRS(polte_latitude)                , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[113] = { 259, RO_ATTRS(polte_longitude)               , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[114] = { 260, RO_ATTRS(polte_confidence)              , ATTR_TYPE_STRING        , 0x2   , av_string           , NULL                                , .min.ux = 0         , .max.ux = 16         },
	[115] = { 261, RO_ATTRX(polte_timestamp)               , ATTR_TYPE_U32           , 0x2   , av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[116] = { 262, RW_ATTRX(ble_prepare_timeout)           , ATTR_TYPE_U32           , 0x13  , av_uint32           , NULL                                , .min.ux = 180       , .max.ux = 172800     },
	[117] = { 263, RW_ATTRX(temperature_offset)            , ATTR_TYPE_S8            , 0x13  , av_int8             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[118] = { 264, RW_ATTRX(lwm2m_enable_bootstrap)        , ATTR_TYPE_BOOL          , 0x17  , av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          }
	/* pyend */
};

/**
 * @brief map id to table entry (invalid entries are NULL)
 */
static const struct attr_table_entry * const ATTR_MAP[] = {
	/* pystart - attribute map */
	[1  ] = &ATTR_TABLE[0  ],
	[4  ] = &ATTR_TABLE[1  ],
	[5  ] = &ATTR_TABLE[2  ],
	[11 ] = &ATTR_TABLE[3  ],
	[12 ] = &ATTR_TABLE[4  ],
	[13 ] = &ATTR_TABLE[5  ],
	[14 ] = &ATTR_TABLE[6  ],
	[16 ] = &ATTR_TABLE[7  ],
	[59 ] = &ATTR_TABLE[8  ],
	[60 ] = &ATTR_TABLE[9  ],
	[61 ] = &ATTR_TABLE[10 ],
	[63 ] = &ATTR_TABLE[11 ],
	[75 ] = &ATTR_TABLE[12 ],
	[93 ] = &ATTR_TABLE[13 ],
	[94 ] = &ATTR_TABLE[14 ],
	[95 ] = &ATTR_TABLE[15 ],
	[140] = &ATTR_TABLE[16 ],
	[142] = &ATTR_TABLE[17 ],
	[143] = &ATTR_TABLE[18 ],
	[144] = &ATTR_TABLE[19 ],
	[145] = &ATTR_TABLE[20 ],
	[146] = &ATTR_TABLE[21 ],
	[147] = &ATTR_TABLE[22 ],
	[148] = &ATTR_TABLE[23 ],
	[149] = &ATTR_TABLE[24 ],
	[150] = &ATTR_TABLE[25 ],
	[151] = &ATTR_TABLE[26 ],
	[152] = &ATTR_TABLE[27 ],
	[153] = &ATTR_TABLE[28 ],
	[154] = &ATTR_TABLE[29 ],
	[155] = &ATTR_TABLE[30 ],
	[156] = &ATTR_TABLE[31 ],
	[157] = &ATTR_TABLE[32 ],
	[158] = &ATTR_TABLE[33 ],
	[159] = &ATTR_TABLE[34 ],
	[160] = &ATTR_TABLE[35 ],
	[161] = &ATTR_TABLE[36 ],
	[162] = &ATTR_TABLE[37 ],
	[163] = &ATTR_TABLE[38 ],
	[164] = &ATTR_TABLE[39 ],
	[165] = &ATTR_TABLE[40 ],
	[166] = &ATTR_TABLE[41 ],
	[167] = &ATTR_TABLE[42 ],
	[168] = &ATTR_TABLE[43 ],
	[169] = &ATTR_TABLE[44 ],
	[170] = &ATTR_TABLE[45 ],
	[171] = &ATTR_TABLE[46 ],
	[172] = &ATTR_TABLE[47 ],
	[173] = &ATTR_TABLE[48 ],
	[174] = &ATTR_TABLE[49 ],
	[175] = &ATTR_TABLE[50 ],
	[176] = &ATTR_TABLE[51 ],
	[177] = &ATTR_TABLE[52 ],
	[178] = &ATTR_TABLE[53 ],
	[179] = &ATTR_TABLE[54 ],
	[180] = &ATTR_TABLE[55 ],
	[181] = &ATTR_TABLE[56 ],
	[182] = &ATTR_TABLE[57 ],
	[183] = &ATTR_TABLE[58 ],
	[184] = &ATTR_TABLE[59 ],
	[185] = &ATTR_TABLE[60 ],
	[186] = &ATTR_TABLE[61 ],
	[187] = &ATTR_TABLE[62 ],
	[188] = &ATTR_TABLE[63 ],
	[189] = &ATTR_TABLE[64 ],
	[190] = &ATTR_TABLE[65 ],
	[191] = &ATTR_TABLE[66 ],
	[192] = &ATTR_TABLE[67 ],
	[193] = &ATTR_TABLE[68 ],
	[194] = &ATTR_TABLE[69 ],
	[195] = &ATTR_TABLE[70 ],
	[196] = &ATTR_TABLE[71 ],
	[197] = &ATTR_TABLE[72 ],
	[198] = &ATTR_TABLE[73 ],
	[199] = &ATTR_TABLE[74 ],
	[200] = &ATTR_TABLE[75 ],
	[201] = &ATTR_TABLE[76 ],
	[202] = &ATTR_TABLE[77 ],
	[203] = &ATTR_TABLE[78 ],
	[204] = &ATTR_TABLE[79 ],
	[205] = &ATTR_TABLE[80 ],
	[206] = &ATTR_TABLE[81 ],
	[207] = &ATTR_TABLE[82 ],
	[208] = &ATTR_TABLE[83 ],
	[209] = &ATTR_TABLE[84 ],
	[210] = &ATTR_TABLE[85 ],
	[211] = &ATTR_TABLE[86 ],
	[212] = &ATTR_TABLE[87 ],
	[213] = &ATTR_TABLE[88 ],
	[214] = &ATTR_TABLE[89 ],
	[215] = &ATTR_TABLE[90 ],
	[216] = &ATTR_TABLE[91 ],
	[217] = &ATTR_TABLE[92 ],
	[218] = &ATTR_TABLE[93 ],
	[219] = &ATTR_TABLE[94 ],
	[220] = &ATTR_TABLE[95 ],
	[242] = &ATTR_TABLE[96 ],
	[243] = &ATTR_TABLE[97 ],
	[244] = &ATTR_TABLE[98 ],
	[245] = &ATTR_TABLE[99 ],
	[246] = &ATTR_TABLE[100],
	[247] = &ATTR_TABLE[101],
	[248] = &ATTR_TABLE[102],
	[249] = &ATTR_TABLE[103],
	[250] = &ATTR_TABLE[104],
	[251] = &ATTR_TABLE[105],
	[252] = &ATTR_TABLE[106],
	[253] = &ATTR_TABLE[107],
	[254] = &ATTR_TABLE[108],
	[255] = &ATTR_TABLE[109],
	[256] = &ATTR_TABLE[110],
	[257] = &ATTR_TABLE[111],
	[258] = &ATTR_TABLE[112],
	[259] = &ATTR_TABLE[113],
	[260] = &ATTR_TABLE[114],
	[261] = &ATTR_TABLE[115],
	[262] = &ATTR_TABLE[116],
	[263] = &ATTR_TABLE[117],
	[264] = &ATTR_TABLE[118]
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

__weak int attr_prepare_modem_boot(void)
{
	return 0;
}

__weak int attr_prepare_modem_functionality(void)
{
	return 0;
}

/* pyend */

#ifdef CONFIG_ATTR_STRING_NAME
/* pystart - get string */
const char *const attr_get_string_cert_status(int value)
{
	switch (value) {
		case 1:           return "Busy";
		case 0:           return "Success";
		default:          return errno_str_get(value);
	}
}

const char *const attr_get_string_gateway_state(int value)
{
	switch (value) {
		case 0:           return "Power Up Init";
		case 1:           return "Network Init";
		case 2:           return "Wait For Network";
		case 3:           return "Network Connected";
		case 4:           return "Network Disconnected";
		case 5:           return "Network Error";
		case 6:           return "Wait For Commission";
		case 7:           return "Resolve Server";
		case 8:           return "Wait Before Cloud Connect";
		case 9:           return "Cloud Connected";
		case 10:          return "Cloud Wait For Disconnect";
		case 11:          return "Cloud Disconnected";
		case 12:          return "Cloud Error";
		case 13:          return "Fota Busy ";
		case 14:          return "Decommission";
		case 15:          return "Cloud Request Disconnect";
		case 16:          return "Cloud Connecting";
		case 17:          return "Modem Init";
		case 18:          return "Modem Error";
		default:          return "?";
	}
}

const char *const attr_get_string_peer_verify(int value)
{
	switch (value) {
		case 0:           return "None";
		case 1:           return "Optional";
		case 2:           return "Required";
		case 3:           return "Unset";
		default:          return "?";
	}
}

const char *const attr_get_string_lte_network_state(int value)
{
	switch (value) {
		case 0:           return "Not Registered";
		case 1:           return "Home Network";
		case 2:           return "Searching";
		case 3:           return "Registration Denied";
		case 4:           return "Out Of Coverage";
		case 5:           return "Roaming";
		case 8:           return "Emergency";
		case 240:         return "Unable To Configure";
		default:          return "?";
	}
}

const char *const attr_get_string_lte_startup_state(int value)
{
	switch (value) {
		case 0:           return "Ready";
		case 1:           return "Waiting For Access Code";
		case 2:           return "Sim Not Present";
		case 3:           return "Sim Lock";
		case 4:           return "Unrecoverable Error";
		case 5:           return "Unknown";
		case 6:           return "Inactive Sim";
		default:          return "?";
	}
}

const char *const attr_get_string_lte_sleep_state(int value)
{
	switch (value) {
		case 0:           return "Uninitialized";
		case 1:           return "Hibernate";
		case 2:           return "Awake";
		case 3:           return "Lite Hibernate";
		case 4:           return "Sleep";
		default:          return "?";
	}
}

const char *const attr_get_string_lte_rat(int value)
{
	switch (value) {
		case 0:           return "Cat M1";
		case 1:           return "Cat NB1";
		default:          return "?";
	}
}

const char *const attr_get_string_central_state(int value)
{
	switch (value) {
		case 0:           return "Finding Device";
		case 1:           return "Finding Service";
		case 2:           return "Finding Ess Temperature Char";
		case 3:           return "Finding Ess Humidity Char";
		case 4:           return "Finding Ess Pressure Char";
		case 5:           return "Connected And Configured";
		case 6:           return "Finding Smp Char";
		case 7:           return "Challenge Request";
		case 8:           return "Challenge Response";
		case 9:           return "Log Download";
		default:          return "?";
	}
}

const char *const attr_get_string_modem_boot(int value)
{
	switch (value) {
		case 0:           return "Normal";
		case 1:           return "Delayed";
		case 2:           return "Airplane";
		default:          return "?";
	}
}

const char *const attr_get_string_fota_control_point(int value)
{
	switch (value) {
		case 0:           return "Nop";
		case 2:           return "Modem Start";
		case 16:          return "Ble Prepare";
		case 17:          return "Ble Abort";
		default:          return "?";
	}
}

const char *const attr_get_string_fota_status(int value)
{
	switch (value) {
		case 0:           return "Success";
		case 1:           return "Busy";
		case 2:           return "Error";
		default:          return "?";
	}
}

const char *const attr_get_string_generate_psk(int value)
{
	switch (value) {
		case 0:           return "LwM2M Default";
		case 1:           return "LwM2M Random";
		default:          return "?";
	}
}

const char *const attr_get_string_lte_init_error(int value)
{
	switch (value) {
		case 0:           return "None";
		case -1:          return "No Iface";
		case -2:          return "Iface Cfg";
		case -3:          return "Dns Cfg";
		case -4:          return "Modem";
		case -5:          return "Airplane";
		default:          return "?";
	}
}

const char *const attr_get_string_cloud_error(int value)
{
	switch (value) {
		case 0:           return "None";
		case -1:          return "Init Endpoint";
		case -2:          return "Init Client";
		case -3:          return "Init Root Ca";
		case -4:          return "Read Cred Fs";
		case -5:          return "Cred Size";
		case -6:          return "Init Topic Prefix";
		case -7:          return "Init Client Cert";
		case -8:          return "Init Client Key";
		default:          return "?";
	}
}

const char *const attr_get_string_modem_functionality(int value)
{
	switch (value) {
		case 0:           return "Minimum";
		case 1:           return "Full";
		case 4:           return "Airplane";
		default:          return errno_str_get(value);
	}
}

const char *const attr_get_string_gps_status(int value)
{
	switch (value) {
		case -1:          return "Invalid";
		case 0:           return "Fix Lost Or Not Available";
		case 1:           return "Prediction Available";
		case 2:           return "2D Available";
		case 3:           return "3D Available";
		case 4:           return "Fixed To Invalid";
		default:          return "?";
	}
}

const char *const attr_get_string_polte_control_point(int value)
{
	switch (value) {
		case 0:           return "Reserved";
		case 1:           return "Register";
		case 2:           return "Enable";
		case 3:           return "Locate";
		default:          return "?";
	}
}

const char *const attr_get_string_polte_status(int value)
{
	switch (value) {
		case 0:           return "Success";
		case 1:           return "Modem Invalid State";
		case 2:           return "Bad Number Of Frames To Capture";
		case 3:           return "Not Enough Memory";
		case 4:           return "Pending Response From Modem";
		case 5:           return "Retrying Capture Attempt";
		case 6:           return "Reserved";
		case 7:           return "Device Id Unavailable";
		case 8:           return "Delaying Capture Attempt Because Of Ongoing Paging";
		case 9:           return "Flash Write Failure";
		case 10:          return "Server Error";
		case 100:         return "Locate In Progress";
		case 127:         return "Busy";
		default:          return errno_str_get(value);
	}
}

/* pyend */
#endif
