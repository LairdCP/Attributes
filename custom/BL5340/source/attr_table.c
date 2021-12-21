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
	char load_path[32 + 1];
	char dump_path[32 + 1];
	float floaty;
	uint8_t lwm2m_psk[16];
	char lwm2m_client_id[32 + 1];
	char lwm2m_peer_url[128 + 1];
	enum ethernet_type ethernet_type;
	enum ethernet_mode ethernet_mode;
	char ethernet_static_ip_address[15 + 1];
	uint8_t ethernet_static_netmask_length;
	char ethernet_static_gateway[15 + 1];
	char ethernet_static_dns[15 + 1];
	char sntp_server[64 + 1];
	uint32_t ble_prepare_timeout;
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
	.load_path = "/lfs/params.txt",
	.dump_path = "/lfs/dump.txt",
	.floaty = 0.13,
	.lwm2m_psk = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
	.lwm2m_client_id = "Client_identity",
	.lwm2m_peer_url = "uwterminalx.lairdconnect.com",
	.ethernet_type = 1,
	.ethernet_mode = 2,
	.ethernet_static_ip_address = "0.0.0.0",
	.ethernet_static_netmask_length = 0,
	.ethernet_static_gateway = "0.0.0.0",
	.ethernet_static_dns = "0.0.0.0",
	.sntp_server = "time.windows.com",
	.ble_prepare_timeout = 3600,
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
	char attribute_version[11 + 1];
	uint32_t qrtc;
	char name[32 + 1];
	char board[32 + 1];
	char build_id[64 + 1];
	char app_type[32 + 1];
	char mount[32 + 1];
	enum cert_status cert_status;
	enum gateway_state gateway_state;
	bool motion_alarm;
	char gateway_id[15 + 1];
	enum central_state central_state;
	char sensor_bluetooth_address[30 + 1];
	enum fota_control_point fota_control_point;
	enum fota_status fota_status;
	char fota_file_name[64 + 1];
	uint32_t fota_size;
	uint32_t fota_count;
	enum generate_psk generate_psk;
	enum cloud_error cloud_error;
	bool commissioning_busy;
	enum ethernet_init_error ethernet_init_error;
	uint8_t ethernet_mac[6];
	bool ethernet_cable_detected;
	enum ethernet_speed ethernet_speed;
	enum ethernet_duplex ethernet_duplex;
	char ethernet_ip_address[15 + 1];
	uint8_t ethernet_netmask_length;
	char ethernet_gateway[15 + 1];
	char ethernet_dns[15 + 1];
	uint32_t ethernet_dhcp_lease_time;
	uint32_t ethernet_dhcp_renew_time;
	enum ethernet_dhcp_state ethernet_dhcp_state;
	uint8_t ethernet_dhcp_attempts;
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
	.attribute_version = "0.4.85",
	.qrtc = 0,
	.name = "",
	.board = "",
	.build_id = "0",
	.app_type = "",
	.mount = "/lfs",
	.cert_status = 0,
	.gateway_state = 0,
	.motion_alarm = false,
	.gateway_id = "",
	.central_state = 0,
	.sensor_bluetooth_address = "",
	.fota_control_point = 0,
	.fota_status = 0,
	.fota_file_name = "",
	.fota_size = 0,
	.fota_count = 0,
	.generate_psk = 0,
	.cloud_error = 0,
	.commissioning_busy = false,
	.ethernet_init_error = 0,
	.ethernet_mac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
	.ethernet_cable_detected = false,
	.ethernet_speed = 0,
	.ethernet_duplex = 0,
	.ethernet_ip_address = "0.0.0.0",
	.ethernet_netmask_length = 0,
	.ethernet_gateway = "0.0.0.0",
	.ethernet_dns = "0.0.0.0",
	.ethernet_dhcp_lease_time = 0,
	.ethernet_dhcp_renew_time = 0,
	.ethernet_dhcp_state = 0,
	.ethernet_dhcp_attempts = 0
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
	[0  ] = { 1  , RW_ATTRS(location)                      , ATTR_TYPE_STRING        , y, y, y, y, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[1  ] = { 4  , RW_ATTRX(passkey)                       , ATTR_TYPE_U32           , y, y, y, y, y, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 999999     },
	[2  ] = { 5  , RW_ATTRX(lock)                          , ATTR_TYPE_BOOL          , y, n, y, y, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[3  ] = { 11 , RO_ATTRS(firmware_version)              , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 3         , .max.ux = 11         },
	[4  ] = { 12 , RO_ATTRS(reset_reason)                  , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 12         },
	[5  ] = { 13 , RO_ATTRS(bluetooth_address)             , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 12        , .max.ux = 12         },
	[6  ] = { 14 , RO_ATTRX(reset_count)                   , ATTR_TYPE_U32           , n, n, y, n, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[7  ] = { 16 , RO_ATTRX(up_time)                       , ATTR_TYPE_S64           , n, n, y, n, n, n, 0, av_int64            , attr_prepare_up_time                , .min.ux = 0         , .max.ux = 0          },
	[8  ] = { 59 , RW_ATTRX(tx_power)                      , ATTR_TYPE_S8            , y, y, y, n, y, n, 0, av_int8             , NULL                                , .min.sx = -40       , .max.sx = 8          },
	[9  ] = { 60 , RW_ATTRX(network_id)                    , ATTR_TYPE_U16           , y, y, y, y, y, n, 0, av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 65535      },
	[10 ] = { 61 , RO_ATTRX(config_version)                , ATTR_TYPE_U8            , n, n, y, n, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[11 ] = { 63 , RW_ATTRX(hardware_minor_version)        , ATTR_TYPE_U8            , y, y, y, n, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[12 ] = { 93 , RO_ATTRS(attribute_version)             , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 6         , .max.ux = 11         },
	[13 ] = { 94 , RO_ATTRX(qrtc)                          , ATTR_TYPE_U32           , n, n, y, n, n, n, 0, av_uint32           , attr_prepare_qrtc                   , .min.ux = 0         , .max.ux = 0          },
	[14 ] = { 95 , RW_ATTRX(qrtc_last_set)                 , ATTR_TYPE_U32           , y, n, y, n, y, n, 0, av_uint32           , attr_prepare_qrtc_last_set          , .min.ux = 0         , .max.ux = 0          },
	[15 ] = { 140, RO_ATTRS(name)                          , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 1         , .max.ux = 32         },
	[16 ] = { 142, RO_ATTRS(board)                         , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 1         , .max.ux = 32         },
	[17 ] = { 143, RO_ATTRS(build_id)                      , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 1         , .max.ux = 64         },
	[18 ] = { 144, RO_ATTRS(app_type)                      , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 1         , .max.ux = 32         },
	[19 ] = { 145, RO_ATTRS(mount)                         , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[20 ] = { 146, RW_ATTRX(commissioned)                  , ATTR_TYPE_BOOL          , y, y, y, n, y, n, 0, av_cpb              , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[21 ] = { 147, RO_ATTRE(cert_status)                   , ATTR_TYPE_S32           , n, n, y, n, n, n, 0, av_int32            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[22 ] = { 148, RW_ATTRS(root_ca_name)                  , ATTR_TYPE_STRING        , y, y, y, n, y, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 48         },
	[23 ] = { 149, RW_ATTRS(client_cert_name)              , ATTR_TYPE_STRING        , y, y, y, n, y, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 48         },
	[24 ] = { 150, RW_ATTRS(client_key_name)               , ATTR_TYPE_STRING        , y, y, y, n, y, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 48         },
	[25 ] = { 151, RW_ATTRS(endpoint)                      , ATTR_TYPE_STRING        , y, y, y, n, y, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 254        },
	[26 ] = { 152, RW_ATTRS(port)                          , ATTR_TYPE_STRING        , y, y, y, n, y, n, 0, av_string           , NULL                                , .min.ux = 4         , .max.ux = 16         },
	[27 ] = { 153, RW_ATTRS(client_id)                     , ATTR_TYPE_STRING        , y, y, y, n, y, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[28 ] = { 154, RW_ATTRS(topic_prefix)                  , ATTR_TYPE_STRING        , y, y, y, n, y, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[29 ] = { 155, RO_ATTRE(gateway_state)                 , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[30 ] = { 156, RW_ATTRE(peer_verify)                   , ATTR_TYPE_S8            , y, y, y, n, n, n, 0, av_int8             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[31 ] = { 168, RW_ATTRX(motion_odr)                    , ATTR_TYPE_U8            , y, y, y, n, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[32 ] = { 169, RW_ATTRX(motion_thresh)                 , ATTR_TYPE_U8            , y, y, y, n, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[33 ] = { 170, RW_ATTRX(motion_scale)                  , ATTR_TYPE_U8            , y, y, y, n, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[34 ] = { 171, RW_ATTRX(motion_duration)               , ATTR_TYPE_U8            , y, y, y, n, y, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 255        },
	[35 ] = { 172, RO_ATTRX(motion_alarm)                  , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[36 ] = { 173, RW_ATTRX(sd_log_max_size)               , ATTR_TYPE_U8            , y, y, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[37 ] = { 174, RW_ATTRX(ct_aes_key)                    , ATTR_TYPE_BYTE_ARRAY    , y, y, n, n, n, n, 0, av_array            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[38 ] = { 176, RO_ATTRS(gateway_id)                    , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 15         },
	[39 ] = { 188, RO_ATTRE(central_state)                 , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[40 ] = { 189, RO_ATTRS(sensor_bluetooth_address)      , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 30         },
	[41 ] = { 190, RW_ATTRX(join_delay)                    , ATTR_TYPE_U32           , y, y, y, n, y, n, 0, av_cp32             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[42 ] = { 191, RW_ATTRX(join_min)                      , ATTR_TYPE_U16           , y, y, y, n, n, n, 0, av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[43 ] = { 192, RW_ATTRX(join_max)                      , ATTR_TYPE_U16           , y, y, y, n, n, n, 0, av_uint16           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[44 ] = { 193, RW_ATTRX(join_interval)                 , ATTR_TYPE_U32           , y, y, y, n, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[45 ] = { 195, RW_ATTRX(delay_cloud_reconnect)         , ATTR_TYPE_BOOL          , y, y, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[46 ] = { 203, RO_ATTRE(fota_control_point)            , ATTR_TYPE_U8            , n, y, n, n, y, n, 0, av_cp8              , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[47 ] = { 204, RO_ATTRE(fota_status)                   , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[48 ] = { 205, RO_ATTRS(fota_file_name)                , ATTR_TYPE_STRING        , n, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 64         },
	[49 ] = { 206, RO_ATTRX(fota_size)                     , ATTR_TYPE_U32           , n, n, y, n, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[50 ] = { 207, RO_ATTRX(fota_count)                    , ATTR_TYPE_U32           , n, n, y, n, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[51 ] = { 208, RW_ATTRS(load_path)                     , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[52 ] = { 209, RW_ATTRS(dump_path)                     , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[53 ] = { 211, RW_ATTRX(floaty)                        , ATTR_TYPE_FLOAT         , y, y, y, n, n, n, 0, av_float            , NULL                                , .min.fx = 0.0       , .max.fx = 0.0        },
	[54 ] = { 212, RO_ATTRE(generate_psk)                  , ATTR_TYPE_U8            , n, y, n, n, y, n, 0, av_cp8              , NULL                                , .min.ux = 0         , .max.ux = 1          },
	[55 ] = { 213, RW_ATTRX(lwm2m_psk)                     , ATTR_TYPE_BYTE_ARRAY    , y, n, y, n, n, n, 0, av_array            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[56 ] = { 214, RW_ATTRS(lwm2m_client_id)               , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 1         , .max.ux = 32         },
	[57 ] = { 215, RW_ATTRS(lwm2m_peer_url)                , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 6         , .max.ux = 128        },
	[58 ] = { 217, RO_ATTRE(cloud_error)                   , ATTR_TYPE_S8            , n, n, y, n, n, n, 0, av_int8             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[59 ] = { 218, RO_ATTRX(commissioning_busy)            , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[60 ] = { 221, RO_ATTRE(ethernet_init_error)           , ATTR_TYPE_S8            , n, n, y, n, n, n, 0, av_int8             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[61 ] = { 222, RO_ATTRX(ethernet_mac)                  , ATTR_TYPE_BYTE_ARRAY    , n, n, y, n, n, n, 0, av_array            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[62 ] = { 223, RW_ATTRE(ethernet_type)                 , ATTR_TYPE_U8            , y, y, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 1         , .max.ux = 2          },
	[63 ] = { 224, RW_ATTRE(ethernet_mode)                 , ATTR_TYPE_U8            , y, y, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 1         , .max.ux = 2          },
	[64 ] = { 225, RO_ATTRX(ethernet_cable_detected)       , ATTR_TYPE_BOOL          , n, n, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[65 ] = { 226, RO_ATTRE(ethernet_speed)                , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[66 ] = { 227, RO_ATTRE(ethernet_duplex)               , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 0          },
	[67 ] = { 228, RO_ATTRS(ethernet_ip_address)           , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 7         , .max.ux = 15         },
	[68 ] = { 229, RO_ATTRX(ethernet_netmask_length)       , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[69 ] = { 230, RO_ATTRS(ethernet_gateway)              , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 7         , .max.ux = 15         },
	[70 ] = { 231, RO_ATTRS(ethernet_dns)                  , ATTR_TYPE_STRING        , n, n, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 7         , .max.ux = 15         },
	[71 ] = { 232, RW_ATTRS(ethernet_static_ip_address)    , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 7         , .max.ux = 15         },
	[72 ] = { 233, RW_ATTRX(ethernet_static_netmask_length), ATTR_TYPE_U8            , y, y, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 32         },
	[73 ] = { 234, RW_ATTRS(ethernet_static_gateway)       , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 7         , .max.ux = 15         },
	[74 ] = { 235, RW_ATTRS(ethernet_static_dns)           , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 7         , .max.ux = 15         },
	[75 ] = { 236, RO_ATTRX(ethernet_dhcp_lease_time)      , ATTR_TYPE_U32           , n, n, y, n, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 4294967294 },
	[76 ] = { 237, RO_ATTRX(ethernet_dhcp_renew_time)      , ATTR_TYPE_U32           , n, n, y, n, n, n, 0, av_uint32           , NULL                                , .min.ux = 0         , .max.ux = 4294967294 },
	[77 ] = { 238, RO_ATTRE(ethernet_dhcp_state)           , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 6          },
	[78 ] = { 239, RO_ATTRX(ethernet_dhcp_attempts)        , ATTR_TYPE_U8            , n, n, y, n, n, n, 0, av_uint8            , NULL                                , .min.ux = 0         , .max.ux = 100        },
	[79 ] = { 241, RW_ATTRS(sntp_server)                   , ATTR_TYPE_STRING        , y, y, y, n, n, n, 0, av_string           , NULL                                , .min.ux = 7         , .max.ux = 64         },
	[80 ] = { 262, RW_ATTRX(ble_prepare_timeout)           , ATTR_TYPE_U32           , y, y, y, n, n, n, 0, av_uint32           , NULL                                , .min.ux = 180       , .max.ux = 172800     },
	[81 ] = { 264, RW_ATTRX(lwm2m_enable_bootstrap)        , ATTR_TYPE_BOOL          , y, y, y, n, n, n, 0, av_bool             , NULL                                , .min.ux = 0         , .max.ux = 0          }
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
	[93 ] = &ATTR_TABLE[12 ],
	[94 ] = &ATTR_TABLE[13 ],
	[95 ] = &ATTR_TABLE[14 ],
	[140] = &ATTR_TABLE[15 ],
	[142] = &ATTR_TABLE[16 ],
	[143] = &ATTR_TABLE[17 ],
	[144] = &ATTR_TABLE[18 ],
	[145] = &ATTR_TABLE[19 ],
	[146] = &ATTR_TABLE[20 ],
	[147] = &ATTR_TABLE[21 ],
	[148] = &ATTR_TABLE[22 ],
	[149] = &ATTR_TABLE[23 ],
	[150] = &ATTR_TABLE[24 ],
	[151] = &ATTR_TABLE[25 ],
	[152] = &ATTR_TABLE[26 ],
	[153] = &ATTR_TABLE[27 ],
	[154] = &ATTR_TABLE[28 ],
	[155] = &ATTR_TABLE[29 ],
	[156] = &ATTR_TABLE[30 ],
	[168] = &ATTR_TABLE[31 ],
	[169] = &ATTR_TABLE[32 ],
	[170] = &ATTR_TABLE[33 ],
	[171] = &ATTR_TABLE[34 ],
	[172] = &ATTR_TABLE[35 ],
	[173] = &ATTR_TABLE[36 ],
	[174] = &ATTR_TABLE[37 ],
	[176] = &ATTR_TABLE[38 ],
	[188] = &ATTR_TABLE[39 ],
	[189] = &ATTR_TABLE[40 ],
	[190] = &ATTR_TABLE[41 ],
	[191] = &ATTR_TABLE[42 ],
	[192] = &ATTR_TABLE[43 ],
	[193] = &ATTR_TABLE[44 ],
	[195] = &ATTR_TABLE[45 ],
	[203] = &ATTR_TABLE[46 ],
	[204] = &ATTR_TABLE[47 ],
	[205] = &ATTR_TABLE[48 ],
	[206] = &ATTR_TABLE[49 ],
	[207] = &ATTR_TABLE[50 ],
	[208] = &ATTR_TABLE[51 ],
	[209] = &ATTR_TABLE[52 ],
	[211] = &ATTR_TABLE[53 ],
	[212] = &ATTR_TABLE[54 ],
	[213] = &ATTR_TABLE[55 ],
	[214] = &ATTR_TABLE[56 ],
	[215] = &ATTR_TABLE[57 ],
	[217] = &ATTR_TABLE[58 ],
	[218] = &ATTR_TABLE[59 ],
	[221] = &ATTR_TABLE[60 ],
	[222] = &ATTR_TABLE[61 ],
	[223] = &ATTR_TABLE[62 ],
	[224] = &ATTR_TABLE[63 ],
	[225] = &ATTR_TABLE[64 ],
	[226] = &ATTR_TABLE[65 ],
	[227] = &ATTR_TABLE[66 ],
	[228] = &ATTR_TABLE[67 ],
	[229] = &ATTR_TABLE[68 ],
	[230] = &ATTR_TABLE[69 ],
	[231] = &ATTR_TABLE[70 ],
	[232] = &ATTR_TABLE[71 ],
	[233] = &ATTR_TABLE[72 ],
	[234] = &ATTR_TABLE[73 ],
	[235] = &ATTR_TABLE[74 ],
	[236] = &ATTR_TABLE[75 ],
	[237] = &ATTR_TABLE[76 ],
	[238] = &ATTR_TABLE[77 ],
	[239] = &ATTR_TABLE[78 ],
	[241] = &ATTR_TABLE[79 ],
	[262] = &ATTR_TABLE[80 ],
	[264] = &ATTR_TABLE[81 ]
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

const char *const attr_get_string_ethernet_init_error(int value)
{
	switch (value) {
		case 0:           return "None";
		case -1:          return "No Iface";
		case -2:          return "Iface Cfg";
		case -3:          return "Dns Cfg";
		default:          return "?";
	}
}

const char *const attr_get_string_ethernet_type(int value)
{
	switch (value) {
		case 1:           return "IPv4";
		case 2:           return "IPv6";
		default:          return "?";
	}
}

const char *const attr_get_string_ethernet_mode(int value)
{
	switch (value) {
		case 1:           return "Static";
		case 2:           return "DHCP";
		default:          return "?";
	}
}

const char *const attr_get_string_ethernet_speed(int value)
{
	switch (value) {
		case 0:           return "Unknown";
		case 1:           return "10 Mbps";
		case 2:           return "100 Mbps";
		case 4:           return "1 Gbps";
		default:          return "?";
	}
}

const char *const attr_get_string_ethernet_duplex(int value)
{
	switch (value) {
		case 0:           return "Unknown";
		case 1:           return "Half";
		case 2:           return "Full";
		default:          return "?";
	}
}

const char *const attr_get_string_ethernet_dhcp_state(int value)
{
	switch (value) {
		case 0:           return "Disabled";
		case 1:           return "Init";
		case 2:           return "Selecting";
		case 3:           return "Requesting";
		case 4:           return "Renewing";
		case 5:           return "Rebinding";
		case 6:           return "Bound";
		default:          return "?";
	}
}

/* pyend */
