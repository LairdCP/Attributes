/**
 * @file attr_table.h
 *
 * @brief This is generated by attribute_generator.py
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ATTRIBUTE_TABLE_H__
#define __ATTRIBUTE_TABLE_H__

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <zephyr.h>
#include <zephyr/types.h>
#include <stddef.h>

#include "attr_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Indices                                                                    */
/******************************************************************************/

/* pystart - attribute indices */
#define ATTR_INDEX_lock                               2
#define ATTR_INDEX_load_path                          51
/* pyend */

/* pystart - attribute ids */
#define ATTR_ID_location                              1
#define ATTR_ID_passkey                               4
#define ATTR_ID_lock                                  5
#define ATTR_ID_firmware_version                      11
#define ATTR_ID_reset_reason                          12
#define ATTR_ID_bluetooth_address                     13
#define ATTR_ID_reset_count                           14
#define ATTR_ID_up_time                               16
#define ATTR_ID_tx_power                              59
#define ATTR_ID_network_id                            60
#define ATTR_ID_config_version                        61
#define ATTR_ID_hardware_version                      63
#define ATTR_ID_attribute_version                     93
#define ATTR_ID_qrtc                                  94
#define ATTR_ID_qrtc_last_set                         95
#define ATTR_ID_name                                  140
#define ATTR_ID_board                                 142
#define ATTR_ID_build_id                              143
#define ATTR_ID_app_type                              144
#define ATTR_ID_mount                                 145
#define ATTR_ID_commissioned                          146
#define ATTR_ID_cert_status                           147
#define ATTR_ID_root_ca_name                          148
#define ATTR_ID_client_cert_name                      149
#define ATTR_ID_client_key_name                       150
#define ATTR_ID_endpoint                              151
#define ATTR_ID_port                                  152
#define ATTR_ID_client_id                             153
#define ATTR_ID_topic_prefix                          154
#define ATTR_ID_gateway_state                         155
#define ATTR_ID_peer_verify                           156
#define ATTR_ID_motion_odr                            168
#define ATTR_ID_motion_thresh                         169
#define ATTR_ID_motion_scale                          170
#define ATTR_ID_motion_duration                       171
#define ATTR_ID_motion_alarm                          172
#define ATTR_ID_sd_log_max_size                       173
#define ATTR_ID_ct_aes_key                            174
#define ATTR_ID_gateway_id                            176
#define ATTR_ID_central_state                         188
#define ATTR_ID_sensor_bluetooth_address              189
#define ATTR_ID_join_delay                            190
#define ATTR_ID_join_min                              191
#define ATTR_ID_join_max                              192
#define ATTR_ID_join_interval                         193
#define ATTR_ID_delay_cloud_reconnect                 195
#define ATTR_ID_fota_control_point                    203
#define ATTR_ID_fota_status                           204
#define ATTR_ID_fota_file_name                        205
#define ATTR_ID_fota_size                             206
#define ATTR_ID_fota_count                            207
#define ATTR_ID_load_path                             208
#define ATTR_ID_dump_path                             209
#define ATTR_ID_floaty                                211
#define ATTR_ID_generate_psk                          212
#define ATTR_ID_lwm2m_psk                             213
#define ATTR_ID_lwm2m_client_id                       214
#define ATTR_ID_lwm2m_peer_url                        215
#define ATTR_ID_cloud_error                           217
#define ATTR_ID_commissioning_busy                    218
#define ATTR_ID_ethernet_init_error                   221
#define ATTR_ID_ethernet_mac                          222
#define ATTR_ID_ethernet_type                         223
#define ATTR_ID_ethernet_mode                         224
#define ATTR_ID_ethernet_cable_detected               225
#define ATTR_ID_ethernet_speed                        226
#define ATTR_ID_ethernet_duplex                       227
#define ATTR_ID_ethernet_ip_address                   228
#define ATTR_ID_ethernet_netmask_length               229
#define ATTR_ID_ethernet_gateway                      230
#define ATTR_ID_ethernet_dns                          231
#define ATTR_ID_ethernet_static_ip_address            232
#define ATTR_ID_ethernet_static_netmask_length        233
#define ATTR_ID_ethernet_static_gateway               234
#define ATTR_ID_ethernet_static_dns                   235
#define ATTR_ID_ethernet_dhcp_lease_time              236
#define ATTR_ID_ethernet_dhcp_renew_time              237
#define ATTR_ID_ethernet_dhcp_state                   238
#define ATTR_ID_ethernet_dhcp_attempts                239
#define ATTR_ID_sntp_server                           241
#define ATTR_ID_ble_prepare_timeout                   262
#define ATTR_ID_lwm2m_enable_bootstrap                264
/* pyend */

/******************************************************************************/
/* Constants and Enumerations                                                 */
/******************************************************************************/

/* pystart - attribute constants */
#define ATTR_TABLE_SIZE                                         82
#define ATTR_TABLE_MAX_ID                                       264
#define ATTR_TABLE_WRITABLE_COUNT                               44
#define ATTR_MAX_STR_LENGTH                                     254
#define ATTR_MAX_STR_SIZE                                       255
#define ATTR_MAX_BIN_SIZE                                       16
#define ATTR_MAX_INT_SIZE                                       8

/* Attribute Max String Lengths */
#define ATTR_LOCATION_MAX_STR_SIZE                              33
#define ATTR_FIRMWARE_VERSION_MAX_STR_SIZE                      12
#define ATTR_RESET_REASON_MAX_STR_SIZE                          13
#define ATTR_BLUETOOTH_ADDRESS_MAX_STR_SIZE                     13
#define ATTR_ATTRIBUTE_VERSION_MAX_STR_SIZE                     12
#define ATTR_NAME_MAX_STR_SIZE                                  33
#define ATTR_BOARD_MAX_STR_SIZE                                 33
#define ATTR_BUILD_ID_MAX_STR_SIZE                              65
#define ATTR_APP_TYPE_MAX_STR_SIZE                              33
#define ATTR_MOUNT_MAX_STR_SIZE                                 33
#define ATTR_ROOT_CA_NAME_MAX_STR_SIZE                          49
#define ATTR_CLIENT_CERT_NAME_MAX_STR_SIZE                      49
#define ATTR_CLIENT_KEY_NAME_MAX_STR_SIZE                       49
#define ATTR_ENDPOINT_MAX_STR_SIZE                              255
#define ATTR_PORT_MAX_STR_SIZE                                  17
#define ATTR_CLIENT_ID_MAX_STR_SIZE                             33
#define ATTR_TOPIC_PREFIX_MAX_STR_SIZE                          33
#define ATTR_GATEWAY_ID_MAX_STR_SIZE                            16
#define ATTR_SENSOR_BLUETOOTH_ADDRESS_MAX_STR_SIZE              31
#define ATTR_FOTA_FILE_NAME_MAX_STR_SIZE                        65
#define ATTR_LOAD_PATH_MAX_STR_SIZE                             33
#define ATTR_DUMP_PATH_MAX_STR_SIZE                             33
#define ATTR_LWM2M_CLIENT_ID_MAX_STR_SIZE                       33
#define ATTR_LWM2M_PEER_URL_MAX_STR_SIZE                        129
#define ATTR_ETHERNET_IP_ADDRESS_MAX_STR_SIZE                   16
#define ATTR_ETHERNET_GATEWAY_MAX_STR_SIZE                      16
#define ATTR_ETHERNET_DNS_MAX_STR_SIZE                          16
#define ATTR_ETHERNET_STATIC_IP_ADDRESS_MAX_STR_SIZE            16
#define ATTR_ETHERNET_STATIC_GATEWAY_MAX_STR_SIZE               16
#define ATTR_ETHERNET_STATIC_DNS_MAX_STR_SIZE                   16
#define ATTR_SNTP_SERVER_MAX_STR_SIZE                           65

/* Attribute Byte Array Lengths */
#define ATTR_CT_AES_KEY_SIZE                                    16
#define ATTR_LWM2M_PSK_SIZE                                     16
#define ATTR_ETHERNET_MAC_SIZE                                  6
/* pyend */

/* pystart - enumerations */
enum cert_status {
	CERT_STATUS_INCLUDE_ERRNO = INT32_MIN,
	CERT_STATUS_BUSY = 1,
	CERT_STATUS_SUCCESS = 0,
};

enum gateway_state {
	GATEWAY_STATE_POWER_UP_INIT = 0,
	GATEWAY_STATE_NETWORK_INIT = 1,
	GATEWAY_STATE_WAIT_FOR_NETWORK = 2,
	GATEWAY_STATE_NETWORK_CONNECTED = 3,
	GATEWAY_STATE_NETWORK_DISCONNECTED = 4,
	GATEWAY_STATE_NETWORK_ERROR = 5,
	GATEWAY_STATE_WAIT_FOR_COMMISSION = 6,
	GATEWAY_STATE_RESOLVE_SERVER = 7,
	GATEWAY_STATE_WAIT_BEFORE_CLOUD_CONNECT = 8,
	GATEWAY_STATE_CLOUD_CONNECTED = 9,
	GATEWAY_STATE_CLOUD_WAIT_FOR_DISCONNECT = 10,
	GATEWAY_STATE_CLOUD_DISCONNECTED = 11,
	GATEWAY_STATE_CLOUD_ERROR = 12,
	GATEWAY_STATE_FOTA_BUSY  = 13,
	GATEWAY_STATE_DECOMMISSION = 14,
	GATEWAY_STATE_CLOUD_REQUEST_DISCONNECT = 15,
	GATEWAY_STATE_CLOUD_CONNECTING = 16,
	GATEWAY_STATE_MODEM_INIT = 17,
	GATEWAY_STATE_MODEM_ERROR = 18,
};

enum peer_verify {
	PEER_VERIFY_NONE = 0,
	PEER_VERIFY_OPTIONAL = 1,
	PEER_VERIFY_REQUIRED = 2,
	PEER_VERIFY_UNSET = 3,
};

enum central_state {
	CENTRAL_STATE_FINDING_DEVICE = 0,
	CENTRAL_STATE_FINDING_SERVICE = 1,
	CENTRAL_STATE_FINDING_ESS_TEMPERATURE_CHAR = 2,
	CENTRAL_STATE_FINDING_ESS_HUMIDITY_CHAR = 3,
	CENTRAL_STATE_FINDING_ESS_PRESSURE_CHAR = 4,
	CENTRAL_STATE_CONNECTED_AND_CONFIGURED = 5,
	CENTRAL_STATE_FINDING_SMP_CHAR = 6,
	CENTRAL_STATE_CHALLENGE_REQUEST = 7,
	CENTRAL_STATE_CHALLENGE_RESPONSE = 8,
	CENTRAL_STATE_LOG_DOWNLOAD = 9,
};

enum fota_control_point {
	FOTA_CONTROL_POINT_NOP = 0,
	FOTA_CONTROL_POINT_MODEM_START = 2,
	FOTA_CONTROL_POINT_BLE_PREPARE = 16,
	FOTA_CONTROL_POINT_BLE_ABORT = 17,
};

enum fota_status {
	FOTA_STATUS_SUCCESS = 0,
	FOTA_STATUS_BUSY = 1,
	FOTA_STATUS_ERROR = 2,
};

enum generate_psk {
	GENERATE_PSK_LWM2M_DEFAULT = 0,
	GENERATE_PSK_LWM2M_RANDOM = 1,
};

enum cloud_error {
	CLOUD_ERROR_NONE = 0,
	CLOUD_ERROR_INIT_ENDPOINT = -1,
	CLOUD_ERROR_INIT_CLIENT_ID = -2,
	CLOUD_ERROR_INIT_ROOT_CA = -3,
	CLOUD_ERROR_READ_CRED_FS = -4,
	CLOUD_ERROR_CRED_SIZE = -5,
	CLOUD_ERROR_INIT_TOPIC_PREFIX = -6,
	CLOUD_ERROR_INIT_CLIENT_CERT = -7,
	CLOUD_ERROR_INIT_CLIENT_KEY = -8,
};

enum ethernet_init_error {
	ETHERNET_INIT_ERROR_NONE = 0,
	ETHERNET_INIT_ERROR_NO_IFACE = -1,
	ETHERNET_INIT_ERROR_IFACE_CFG = -2,
	ETHERNET_INIT_ERROR_DNS_CFG = -3,
};

enum ethernet_type {
	ETHERNET_TYPE_IPV4 = 1,
	ETHERNET_TYPE_IPV6 = 2,
};

enum ethernet_mode {
	ETHERNET_MODE_STATIC = 1,
	ETHERNET_MODE_DHCP = 2,
};

enum ethernet_speed {
	ETHERNET_SPEED_UNKNOWN = 0,
	ETHERNET_SPEED_10MBPS = 1,
	ETHERNET_SPEED_100MBPS = 2,
	ETHERNET_SPEED_1GBPS = 4,
};

enum ethernet_duplex {
	ETHERNET_DUPLEX_UNKNOWN = 0,
	ETHERNET_DUPLEX_HALF = 1,
	ETHERNET_DUPLEX_FULL = 2,
};

enum ethernet_dhcp_state {
	ETHERNET_DHCP_STATE_DISABLED = 0,
	ETHERNET_DHCP_STATE_INIT = 1,
	ETHERNET_DHCP_STATE_SELECTING = 2,
	ETHERNET_DHCP_STATE_REQUESTING = 3,
	ETHERNET_DHCP_STATE_RENEWING = 4,
	ETHERNET_DHCP_STATE_REBINDING = 5,
	ETHERNET_DHCP_STATE_BOUND = 6,
};

enum attr_dump {
	ATTR_DUMP_RW = 0,
	ATTR_DUMP_W = 1,
	ATTR_DUMP_RO = 2,
};

/* pyend */

/* pystart - enum size check */
BUILD_ASSERT(sizeof(enum cert_status) == ATTR_SIZE_S32);
BUILD_ASSERT(sizeof(enum gateway_state) == ATTR_SIZE_U8);
BUILD_ASSERT(sizeof(enum peer_verify) == ATTR_SIZE_S8);
BUILD_ASSERT(sizeof(enum central_state) == ATTR_SIZE_U8);
BUILD_ASSERT(sizeof(enum fota_control_point) == ATTR_SIZE_U8);
BUILD_ASSERT(sizeof(enum fota_status) == ATTR_SIZE_U8);
BUILD_ASSERT(sizeof(enum generate_psk) == ATTR_SIZE_U8);
BUILD_ASSERT(sizeof(enum cloud_error) == ATTR_SIZE_S8);
BUILD_ASSERT(sizeof(enum ethernet_init_error) == ATTR_SIZE_S8);
BUILD_ASSERT(sizeof(enum ethernet_type) == ATTR_SIZE_U8);
BUILD_ASSERT(sizeof(enum ethernet_mode) == ATTR_SIZE_U8);
BUILD_ASSERT(sizeof(enum ethernet_speed) == ATTR_SIZE_U8);
BUILD_ASSERT(sizeof(enum ethernet_duplex) == ATTR_SIZE_U8);
BUILD_ASSERT(sizeof(enum ethernet_dhcp_state) == ATTR_SIZE_U8);
/* pyend */

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/
/* The weak implementations should be overridden in application. */
/* pystart - prepare for read */
int attr_prepare_up_time(void);
int attr_prepare_qrtc(void);
int attr_prepare_qrtc_last_set(void);
/* pyend */

/* pystart - get string */
const char *const attr_get_string_cert_status(int value);
const char *const attr_get_string_gateway_state(int value);
const char *const attr_get_string_peer_verify(int value);
const char *const attr_get_string_central_state(int value);
const char *const attr_get_string_fota_control_point(int value);
const char *const attr_get_string_fota_status(int value);
const char *const attr_get_string_generate_psk(int value);
const char *const attr_get_string_cloud_error(int value);
const char *const attr_get_string_ethernet_init_error(int value);
const char *const attr_get_string_ethernet_type(int value);
const char *const attr_get_string_ethernet_mode(int value);
const char *const attr_get_string_ethernet_speed(int value);
const char *const attr_get_string_ethernet_duplex(int value);
const char *const attr_get_string_ethernet_dhcp_state(int value);
/* pyend */

#ifdef __cplusplus
}
#endif

#endif /* __ATTRIBUTE_TABLE_H__ */
