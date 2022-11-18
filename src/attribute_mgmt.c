/**
 * @file attribute_mgmt.c
 * @brief
 *
 * Copyright (c) 2021-2022 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <logging/log.h>
LOG_MODULE_REGISTER(attr_mgmt, CONFIG_ATTR_LOG_LEVEL);

#include <zephyr.h>
#include <init.h>
#include <limits.h>
#include <string.h>
#include <zcbor_common.h>
#include <zcbor_decode.h>
#include <zcbor_encode.h>
#include "mgmt/mgmt.h"

#ifdef CONFIG_SHELL
#include <shell/shell.h>
#include <shell/shell_uart.h>
#endif

#ifdef CONFIG_MCUMGR_SMP_BT
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <mgmt/mcumgr/smp_bt.h>
#include <bluetooth/services/dfu_smp.h>
#include "lcz_bluetooth.h"
#endif

#include "attr.h"
#include "file_system_utilities.h"
#include "lcz_memfault.h"

#ifdef CONFIG_FRAMEWORK
#include <Framework.h>
#include <FrameworkMacros.h>
#include <framework_ids.h>
#include <framework_msgcodes.h>
#endif

#include "attribute_mgmt.h"
#include "attribute_mgmt_decode.h"
#include "attribute_mgmt_encode.h"
#include "mgmt/mcumgr/buf.h"

/******************************************************************************/
/* Local Constant, Macro and Type Definitions                                 */
/******************************************************************************/
#define MGMT_OP_NOTIFY 4

#define INVALID_PARAM_ID (ATTR_TABLE_SIZE + 1)

#define MAX_PBUF_SIZE MAX(ATTR_MAX_STR_SIZE, ATTR_MAX_BIN_SIZE)

/* Overhead estimate (21 bytes) for notification buffer size.
 * A3                 # map(3)
 *    62              # text(2)
 *       6964         # "id"
 *    19 03E7         # unsigned(999)  - id less than 1000
 *    62              # text(2)
 *       7231         # "r1"
 *    60              # text(0)        - empty string
 *                    # ""
 *    66              # text(6)
 *       726573756C74 # "result"
 *    19 0400         # unsigned(1024) - size
 */
#define CBOR_NOTIFICATION_OVERHEAD 32

#define LOCK_INVALID_WAIT_TIME_MS 1500

#ifdef CONFIG_FRAMEWORK
#define ATTR_FRAMEWORK_BROADCAST(msg)                                          \
	FRAMEWORK_MSG_CREATE_AND_BROADCAST(FWK_ID_ATTR, msg)
#else
#define ATTR_FRAMEWORK_BROADCAST(msg)
#endif

/******************************************************************************/
/* Local Function Prototypes                                                  */
/******************************************************************************/
static int get_parameter(struct mgmt_ctxt *ctxt);
static int set_parameter(struct mgmt_ctxt *ctxt);
static int load_parameter_file(struct mgmt_ctxt *ctxt);
static int dump_parameter_file(struct mgmt_ctxt *ctxt);
static int factory_reset(struct mgmt_ctxt *ctxt);
static int set_notify(struct mgmt_ctxt *ctxt);
static int get_notify(struct mgmt_ctxt *ctxt);
static int disable_notify(struct mgmt_ctxt *ctxt);
static int unsupported(struct mgmt_ctxt *ctxt);

static int attribute_mgmt_init(const struct device *device);

static void cbor_encode_attribute(uint16_t param_id,
				  struct get_parameter_result *response);

#ifdef CONFIG_MCUMGR_SMP_BT
static void smp_ble_disconnected(struct bt_conn *conn, uint8_t reason);
static void smp_ble_connected(struct bt_conn *conn, uint8_t err);
#endif

/******************************************************************************/
/* Local Data Definitions                                                     */
/******************************************************************************/
static const struct mgmt_handler ATTRIBUTE_MGMT_HANDLERS[] = {
	[ATTRIBUTE_MGMT_ID_GET_PARAMETER] = {
		.mh_write = NULL,
		.mh_read = get_parameter,
	},
	[ATTRIBUTE_MGMT_ID_SET_PARAMETER] = {
		.mh_write = set_parameter,
		.mh_read = NULL,
	},
	[ATTRIBUTE_MGMT_ID_LOAD_PARAMETER_FILE] = {
		.mh_write = load_parameter_file,
		.mh_read = NULL,
	},
	[ATTRIBUTE_MGMT_ID_DUMP_PARAMETER_FILE] = {
		.mh_write = dump_parameter_file,
		.mh_read = NULL,
	},
	[ATTRIBUTE_MGMT_ID_FACTORY_RESET] = {
		.mh_write = factory_reset,
		.mh_read = NULL,
	},
	[ATTRIBUTE_MGMT_ID_SET_NOTIFY] = {
		.mh_write = set_notify,
		.mh_read = NULL,
	},
	[ATTRIBUTE_MGMT_ID_GET_NOTIFY] = {
		.mh_write = NULL,
		.mh_read = get_notify,
	},
	[ATTRIBUTE_MGMT_ID_DISABLE_NOTIFY] = {
		.mh_write = disable_notify,
		.mh_read = NULL,
	},
	[DEPRECATED_ATTRIBUTE_MGMT_ID_CHECK_LOCK_STATUS] = {
		.mh_write = NULL,
		.mh_read = unsupported,
	},
	[DEPRECATED_ATTRIBUTE_MGMT_ID_SET_LOCK_CODE] = {
		.mh_write = unsupported,
		.mh_read = NULL,
	},
	[DEPRECATED_ATTRIBUTE_MGMT_ID_LOCK] = {
		.mh_write = unsupported,
		.mh_read = NULL,
	},
	[DEPRECATED_ATTRIBUTE_MGMT_ID_UNLOCK] = {
		.mh_write = unsupported,
		.mh_read = NULL,
	},
	[ATTRIBUTE_MGMT_ID_GET_UNLOCK_ERROR_CODE] = {
		.mh_write = NULL,
		.mh_read = unsupported,
	},
	[DEPRECATED_ATTRIBUTE_MGMT_ID_GET_API_VERSION] = {
		.mh_write = NULL,
		.mh_read = unsupported,
	},
	[DEPRECATED_ATTRIBUTE_MGMT_ID_GET_INDICES] = {
		.mh_write = NULL,
		.mh_read = unsupported,
	},
	[DEPRECATED_ATTRIBUTE_MGMT_ID_GET_ENTRY_DETAILS] = {
		.mh_write = NULL,
		.mh_read = unsupported,
	}
};

#define ATTRIBUTE_MGMT_HANDLER_CNT                                             \
	(sizeof ATTRIBUTE_MGMT_HANDLERS / sizeof ATTRIBUTE_MGMT_HANDLERS[0])

static struct mgmt_group attribute_mgmt_group = {
	.mg_handlers = ATTRIBUTE_MGMT_HANDLERS,
	.mg_handlers_count = ATTRIBUTE_MGMT_HANDLER_CNT,
	.mg_group_id = CONFIG_MGMT_GROUP_ID_ATTRIBUTE,
};

#ifdef CONFIG_MCUMGR_SMP_BT
struct smp_notification {
	struct bt_dfu_smp_header header;
	uint8_t buffer[MAX_PBUF_SIZE + CBOR_NOTIFICATION_OVERHEAD];
} __packed;

/* The connection callback is used to reference the connection handle.
 * This allows SMP notifications to be sent.
 * Typical SMP is command-response.
 */
static struct {
	struct bt_conn *conn_handle;
	struct bt_conn_cb conn_callbacks;
	struct smp_notification cmd;
} smp_ble;
#endif

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
SYS_INIT(attribute_mgmt_init, APPLICATION, CONFIG_ATTRIBUTE_MGMT_INIT_PRIORITY);

#ifdef CONFIG_MCUMGR_SMP_BT
/* Callback from attribute module */
int attr_notify(attr_id_t index)
{
	int err = 0;
	size_t rsp_len = 0;
	size_t total_len;
	uint16_t mtu;
	struct get_parameter_result response;

	if (smp_ble.conn_handle == NULL) {
		return -ENOTCONN;
	}

	response.id = index;

	cbor_encode_attribute(index, &response);

	do {
		if (cbor_encode_get_parameter_result(smp_ble.cmd.buffer,
						     sizeof(smp_ble.cmd.buffer),
						     &response, &rsp_len)) {
			err = -EMSGSIZE;
			break;
		}

		total_len = sizeof(smp_ble.cmd.header) + rsp_len;
		mtu = bt_gatt_get_mtu(smp_ble.conn_handle);
		if (total_len > BT_MAX_PAYLOAD(mtu)) {
			err = -EMSGSIZE;
			break;
		}

		smp_ble.cmd.header.op = MGMT_OP_NOTIFY;
		smp_ble.cmd.header.flags = 0;
		smp_ble.cmd.header.len_h8 = (uint8_t)((rsp_len >> 8) & 0xFF);
		smp_ble.cmd.header.len_l8 = (uint8_t)((rsp_len >> 0) & 0xFF);
		smp_ble.cmd.header.group_h8 = 0;
		smp_ble.cmd.header.group_l8 = CONFIG_MGMT_GROUP_ID_ATTRIBUTE;
		smp_ble.cmd.header.seq = 0;
		smp_ble.cmd.header.id = ATTRIBUTE_MGMT_ID_GET_PARAMETER;

		err = smp_bt_notify(smp_ble.conn_handle, &smp_ble.cmd,
				    total_len);
	} while (0);

	return err;
}
#endif

/******************************************************************************/
/* Local Function Definitions                                                 */
/******************************************************************************/
static int attribute_mgmt_init(const struct device *device)
{
	ARG_UNUSED(device);

	mgmt_register_group(&attribute_mgmt_group);

#ifdef CONFIG_MCUMGR_SMP_BT
	smp_ble.conn_callbacks.connected = smp_ble_connected;
	smp_ble.conn_callbacks.disconnected = smp_ble_disconnected;
	bt_conn_cb_register(&smp_ble.conn_callbacks);
#endif

	return 0;
}

#ifdef CONFIG_MCUMGR_SMP_BT
static void smp_ble_connected(struct bt_conn *conn, uint8_t err)
{
	/* Did a central connect to us? */
	if (!lbt_peripheral_role(conn)) {
		return;
	}

	if (err) {
		bt_conn_unref(conn);
		smp_ble.conn_handle = NULL;
	} else {
		smp_ble.conn_handle = bt_conn_ref(conn);
	}
}

static void smp_ble_disconnected(struct bt_conn *conn, uint8_t reason)
{
	if (!lbt_peripheral_role(conn)) {
		return;
	}

	bt_conn_unref(conn);
	smp_ble.conn_handle = NULL;

	attr_disable_notify();
}
#endif

static int get_parameter(struct mgmt_ctxt *ctxt)
{
	uint32_t rsp_len = 0;
	struct get_parameter user_params;
	struct get_parameter_result response;
	zcbor_state_t *zse = ctxt->cnbe->zs;
	zcbor_state_t *zsd = ctxt->cnbd->zs;

	user_params.p1 = INVALID_PARAM_ID;

	if (cbor_decode_get_parameter(zsd->payload,
				      (zsd->payload_end - zsd->payload),
				      &user_params, NULL)) {
		return MGMT_ERR_EINVAL;
	}

	if (user_params.p1 == INVALID_PARAM_ID) {
		return MGMT_ERR_EINVAL;
	}

	response.id = user_params.p1;

	cbor_encode_attribute(user_params.p1, &response);

	if (cbor_encode_get_parameter_result(zse->payload_mut,
					     (zse->payload_end - zse->payload),
					     &response, &rsp_len)) {
		return MGMT_ERR_EMSGSIZE;
	}

	zse->payload += rsp_len;

	return MGMT_ERR_EOK;
}

/*
 * Map the attribute type to the CBOR type, get the value, and then encode it.
 */
static void cbor_encode_attribute(uint16_t param_id,
				  struct get_parameter_result *response)
{
	attr_id_t id = (attr_id_t)param_id;
	enum attr_type type = attr_get_type(id);

	response->r1_present = true;

	switch (type) {
	case ATTR_TYPE_S8:
	case ATTR_TYPE_S16:
	case ATTR_TYPE_S32:
	case ATTR_TYPE_S64:
		response->r1.r1_choice = _get_parameter_result_r1__int64;
		response->r = attr_get(id, &response->r1._int64,
				       sizeof(response->r1._int64));
		break;

	case ATTR_TYPE_U8:
	case ATTR_TYPE_U16:
	case ATTR_TYPE_U32:
	case ATTR_TYPE_U64:
		response->r1.r1_choice = _get_parameter_result_r1__uint64;
		response->r = attr_get(id, &response->r1._uint64,
				       sizeof(response->r1._uint64));
		break;

	case ATTR_TYPE_STRING:
		response->r1.r1_choice = _get_parameter_result_r1_tstr;
		response->r1.tstr.value = attr_get_pointer(id, &response->r);
		if (response->r1.tstr.value == NULL) {
			response->r1.tstr.len = 0;
		} else {
			response->r1.tstr.len = strlen(response->r1.tstr.value);
		}
		/* For strings the terminator must be removed from the maximum size. */
		if (response->r > 0) {
			response->r -= 1;
		}
		break;

	case ATTR_TYPE_FLOAT:
		response->r1.r1_choice = _get_parameter_result_r1_float;
		response->r = attr_get(id, &response->r1._float,
				       sizeof(response->r1._float));
		break;

	case ATTR_TYPE_BOOL:
		response->r1.r1_choice = _get_parameter_result_r1_bool;
		response->r = attr_get(id, &response->r1._bool,
				       sizeof(response->r1._bool));
		break;

	case ATTR_TYPE_BYTE_ARRAY:
		response->r1.r1_choice = _get_parameter_result_r1_bstr;
		response->r1.bstr.value = attr_get_pointer(id, &response->r);
		response->r1.bstr.len = response->r;
		break;

	default:
		response->r = -EINVAL;
		response->r1_present = false;
	}
}

static int set_parameter(struct mgmt_ctxt *ctxt)
{
	uint32_t rsp_len = 0;
	struct set_parameter user_params;
	struct set_parameter_result response;
	enum attr_type type;
	bool *mod_ptr = NULL;
#ifdef CONFIG_ATTRIBUTE_MGMT_INCREMENT_CONFIG_VERSION
	bool modified = false;
	mod_ptr = &modified;
#endif

	int r = 0;
	zcbor_state_t *zse = ctxt->cnbe->zs;
	zcbor_state_t *zsd = ctxt->cnbd->zs;

	user_params.p1 = INVALID_PARAM_ID;

	if (cbor_decode_set_parameter(zsd->payload,
				      (zsd->payload_end - zsd->payload),
				      &user_params, NULL)) {
		return MGMT_ERR_EINVAL;
	}

	if (user_params.p1 == INVALID_PARAM_ID) {
		return MGMT_ERR_EINVAL;
	}

	type = attr_get_type(user_params.p1);

	if (type == ATTR_TYPE_BOOL) {
		if (user_params.p2_choice == _set_parameter_p2_bool) {
			r = attr_set((attr_id_t)user_params.p1, type,
				     &user_params._bool, sizeof(bool), mod_ptr);
		} else {
			r = -ENOMSG;
		}
	} else if ((type == ATTR_TYPE_S8 || type == ATTR_TYPE_S16 ||
		    type == ATTR_TYPE_S32 || type == ATTR_TYPE_S64)) {
		if (user_params.p2_choice == _set_parameter_p2__int64) {
			/* Check bounds */
			int64_t min = LLONG_MIN;
			int64_t max = LLONG_MAX;
			if (type == ATTR_TYPE_S8) {
				min = SCHAR_MIN;
				max = SCHAR_MAX;
			} else if (type == ATTR_TYPE_S16) {
				min = SHRT_MIN;
				max = SHRT_MAX;
			} else if (type == ATTR_TYPE_S32) {
				min = LONG_MIN;
				max = LONG_MAX;
			}

			if (user_params._int64<min || user_params._int64> max) {
				/* Outside bounds of type, reject */
				r = -EINVAL;
			} else if (type == ATTR_TYPE_S8) {
				int8_t tmp_val = (int8_t)user_params._int64;
				r = attr_set((attr_id_t)user_params.p1, type,
					     &tmp_val, sizeof(int8_t), mod_ptr);
			} else if (type == ATTR_TYPE_S16) {
				int16_t tmp_val = (int16_t)user_params._int64;
				r = attr_set((attr_id_t)user_params.p1, type,
					     &tmp_val, sizeof(int16_t),
					     mod_ptr);
			} else if (type == ATTR_TYPE_S32) {
				int32_t tmp_val = (int32_t)user_params._int64;
				r = attr_set((attr_id_t)user_params.p1, type,
					     &tmp_val, sizeof(int32_t),
					     mod_ptr);
			} else if (type == ATTR_TYPE_S64) {
				r = attr_set((attr_id_t)user_params.p1, type,
					     &user_params._int64,
					     sizeof(int64_t), mod_ptr);
			}
		} else {
			r = -ENOMSG;
		}
	} else if ((type == ATTR_TYPE_U8 || type == ATTR_TYPE_U16 ||
		    type == ATTR_TYPE_U32 || type == ATTR_TYPE_U64)) {
		if (user_params.p2_choice == _set_parameter_p2__uint64) {
			/* Check bounds */
			uint64_t max = ULLONG_MAX;
			if (type == ATTR_TYPE_U8) {
				max = UCHAR_MAX;
			} else if (type == ATTR_TYPE_U16) {
				max = USHRT_MAX;
			} else if (type == ATTR_TYPE_U32) {
				max = ULONG_MAX;
			}

			if (user_params._uint64 > max) {
				/* Outside bounds of type, reject */
				r = -EINVAL;
			} else if (type == ATTR_TYPE_U8) {
				uint8_t tmp_val = (uint8_t)user_params._uint64;
				r = attr_set((attr_id_t)user_params.p1, type,
					     &tmp_val, sizeof(uint8_t),
					     mod_ptr);
			} else if (type == ATTR_TYPE_U16) {
				uint16_t tmp_val =
					(uint16_t)user_params._uint64;
				r = attr_set((attr_id_t)user_params.p1, type,
					     &tmp_val, sizeof(uint16_t),
					     mod_ptr);
			} else if (type == ATTR_TYPE_U32) {
				uint32_t tmp_val =
					(uint32_t)user_params._uint64;
				r = attr_set((attr_id_t)user_params.p1, type,
					     &tmp_val, sizeof(uint32_t),
					     mod_ptr);
			} else if (type == ATTR_TYPE_U64) {
				r = attr_set((attr_id_t)user_params.p1, type,
					     &user_params._uint64,
					     sizeof(uint64_t), mod_ptr);
			}
		} else {
			r = -ENOMSG;
		}
	} else if (type == ATTR_TYPE_STRING) {
		if (user_params.p2_choice == _set_parameter_p2_tstr) {
			r = attr_set((attr_id_t)user_params.p1, type,
				     (void *)user_params.tstr.value,
				     user_params.tstr.len, mod_ptr);
		} else {
			r = -ENOMSG;
		}
	} else if (type == ATTR_TYPE_FLOAT) {
		if (user_params.p2_choice == _set_parameter_p2_float) {
			float tmp_val = (float)user_params._float;
			r = attr_set((attr_id_t)user_params.p1, type, &tmp_val,
				     sizeof(float), mod_ptr);
		} else {
			r = -ENOMSG;
		}
	} else if (type == ATTR_TYPE_BYTE_ARRAY) {
		if (user_params.p2_choice == _set_parameter_p2_bstr) {
			r = attr_set((attr_id_t)user_params.p1, type,
				     (void *)user_params.bstr.value,
				     user_params.bstr.len, mod_ptr);
		} else {
			r = -ENOMSG;
		}
	} else {
		/* Invalid ID or type */
		r = -EINVAL;
	}

#if defined(CONFIG_ATTRIBUTE_MGMT_INCREMENT_CONFIG_VERSION) &&                 \
	defined(ATTR_ID_config_version)
	if (r == 0 && modified == true) {
		(void)attr_add_uint32(ATTR_ID_config_version, 1);
	}
#endif

	response.id = user_params.p1;
	response.r = r;

	if (cbor_encode_set_parameter_result(zse->payload_mut,
					     (zse->payload_end - zse->payload),
					     &response, &rsp_len)) {
		return MGMT_ERR_EMSGSIZE;
	}

	zse->payload += rsp_len;

	return MGMT_ERR_EOK;
}

static int load_parameter_file(struct mgmt_ctxt *ctxt)
{
	uint32_t rsp_len = 0;
	struct load_parameter_file user_params;
	struct load_parameter_file_result load_parameter_file_data = { 0 };
	bool modified;
	int r = 0;
	zcbor_state_t *zse = ctxt->cnbe->zs;
	zcbor_state_t *zsd = ctxt->cnbd->zs;
	size_t length;
	const char *load_path;
	char path[FSU_MAX_ABS_PATH_SIZE];

	if (cbor_decode_load_parameter_file(zsd->payload,
					    (zsd->payload_end - zsd->payload),
					    &user_params, NULL)) {
		return MGMT_ERR_EINVAL;
	}

	if (user_params.p1_present) {
		load_path = user_params.p1.p1.value;
		length = user_params.p1.p1.len;
	} else {
#ifdef ATTR_ID_load_path
		load_path = attr_get_quasi_static(ATTR_ID_load_path);
#else
		load_path = "/lfs/attr.txt";
#endif
		length = strlen(load_path);
	}

	if (length < FSU_MAX_ABS_PATH_SIZE) {
		memcpy(path, load_path, length);
		path[length] = 0;
	} else {
		r = -EINVAL;
	}

	if (r == 0) {
		r = attr_load(path, &modified);
	}

	/* Bug 23168 - External (SMP) API doesn't match internal API */
	load_parameter_file_data.r = r >= 0 ? 0 : r;

	if (cbor_encode_load_parameter_file_result(
		    zse->payload_mut, (zse->payload_end - zse->payload),
		    &load_parameter_file_data, &rsp_len)) {
		return MGMT_ERR_EMSGSIZE;
	}

	zse->payload += rsp_len;

	return MGMT_ERR_EOK;
}

static int dump_parameter_file(struct mgmt_ctxt *ctxt)
{
	uint32_t rsp_len = 0;
	struct dump_parameter_file user_params;
	struct dump_parameter_file_result dump_parameter_file_data = { 0 };
	int r = MGMT_ERR_EOK;
	char *fstr = NULL;
	size_t file_name_length;
	const char *file_name;
	zcbor_state_t *zse = ctxt->cnbe->zs;
	zcbor_state_t *zsd = ctxt->cnbd->zs;
	char path[FSU_MAX_ABS_PATH_SIZE];

	if (cbor_decode_dump_parameter_file(zsd->payload,
					    (zsd->payload_end - zsd->payload),
					    &user_params, NULL)) {
		return MGMT_ERR_EINVAL;
	}

	do {
		file_name = NULL;
#ifdef CONFIG_ATTRIBUTE_MGMT_DUMP_USER_FILE_NAME
		/* The output file is an optional parameter */
		if (user_params.p2_present == true) {
			file_name = user_params.p2.p2.value;
			file_name_length = user_params.p2.p2.len;
		}
#endif
#ifdef ATTR_ID_dump_path
		if (file_name == NULL) {
			file_name = attr_get_quasi_static(ATTR_ID_dump_path);
			file_name_length = strlen(file_name);
		}
#else
		if (file_name == NULL) {
			file_name = "/lfs/dump.txt";
			file_name_length = strlen(file_name);
		}
#endif

		if (file_name_length < FSU_MAX_ABS_PATH_SIZE) {
			memcpy(path, file_name, file_name_length);
			path[file_name_length] = 0;
		} else {
			r = -EINVAL;
			break;
		}

		/* This will malloc a string as large as maximum parameter file size. */
		if (user_params.p1 < UINT8_MAX) {
			/* Clear file before proceeding */
			fsu_delete_abs(path);

			/* Dump parameters to file */
			r = attr_prepare_then_dump(&fstr, user_params.p1);
			if (r >= 0) {
				r = fsu_write_abs(path, fstr, strlen(fstr));
				k_free(fstr);
			} else {
				break;
			}
		}

		dump_parameter_file_data.n.n.value = path;
		dump_parameter_file_data.n.n.len = strlen(path);
		dump_parameter_file_data.n_present = true;
	} while (0);

	dump_parameter_file_data.r = r;

	if (cbor_encode_dump_parameter_file_result(
		    zse->payload_mut, (zse->payload_end - zse->payload),
		    &dump_parameter_file_data, &rsp_len)) {
		return MGMT_ERR_EMSGSIZE;
	}

	zse->payload += rsp_len;

	return MGMT_ERR_EOK;
}

static int factory_reset(struct mgmt_ctxt *ctxt)
{
#ifdef CONFIG_ATTRIBUTE_MGMT_FACTORY_RESET
	uint32_t rsp_len = 0;
	int r = 0;
	struct factory_reset_result factory_reset_data = { 0 };
#ifdef ATTR_ID_factory_reset_enable
	uint8_t factory_reset_enabled = 0;
#endif
	zcbor_state_t *zse = ctxt->cnbe->zs;

	if (r == 0) {
#ifdef ATTR_ID_factory_reset_enable
		attr_get(ATTR_ID_factory_reset_enable, &factory_reset_enabled,
			 sizeof(factory_reset_enabled));

		if (factory_reset_enabled == 0) {
			r = -EPERM;
		}
#endif

		if (r == 0) {
			r = attr_factory_reset();

			if (r == 0) {
				ATTR_FRAMEWORK_BROADCAST(
					FMC_ATTR_FACTORY_RESET);
			}
		}
	}

	factory_reset_data.r = r;

	if (cbor_encode_factory_reset_result(zse->payload_mut,
					     (zse->payload_end - zse->payload),
					     &factory_reset_data, &rsp_len)) {
		return MGMT_ERR_EMSGSIZE;
	}

	zse->payload += rsp_len;

	return MGMT_ERR_EOK;
#else
	return MGMT_ERR_ENOTSUP;
#endif
}

static int set_notify(struct mgmt_ctxt *ctxt)
{
	uint32_t rsp_len = 0;
	struct set_notify user_params;
	struct set_notify_result set_notify_data = { 0 };
	zcbor_state_t *zse = ctxt->cnbe->zs;
	zcbor_state_t *zsd = ctxt->cnbd->zs;

	if (cbor_decode_set_notify(zsd->payload,
				   (zsd->payload_end - zsd->payload),
				   &user_params, NULL)) {
		return MGMT_ERR_EINVAL;
	}

	set_notify_data.id = user_params.p1;
	set_notify_data.r =
		attr_set_notify((attr_id_t)user_params.p1, user_params.p2);

	if (cbor_encode_set_notify_result(zse->payload_mut,
					  (zse->payload_end - zse->payload),
					  &set_notify_data, &rsp_len)) {
		return MGMT_ERR_EMSGSIZE;
	}

	zse->payload += rsp_len;

	return MGMT_ERR_EOK;
}

static int get_notify(struct mgmt_ctxt *ctxt)
{
	uint32_t rsp_len = 0;
	struct get_notify user_params;
	struct get_notify_result get_notify_data = { 0 };
	zcbor_state_t *zse = ctxt->cnbe->zs;
	zcbor_state_t *zsd = ctxt->cnbd->zs;

	if (cbor_decode_get_notify(zsd->payload,
				   (zsd->payload_end - zsd->payload),
				   &user_params, NULL)) {
		return MGMT_ERR_EINVAL;
	}

	get_notify_data.id = user_params.p1;
	get_notify_data.r = attr_get_notify((attr_id_t)user_params.p1);

	if (cbor_encode_get_notify_result(zse->payload_mut,
					  (zse->payload_end - zse->payload),
					  &get_notify_data, &rsp_len)) {
		return MGMT_ERR_EMSGSIZE;
	}

	zse->payload += rsp_len;

	return MGMT_ERR_EOK;
}

static int disable_notify(struct mgmt_ctxt *ctxt)
{
	uint32_t rsp_len = 0;
	struct disable_notify_result disable_notify_data = { 0 };
	zcbor_state_t *zse = ctxt->cnbe->zs;

	disable_notify_data.r = attr_disable_notify();

	if (cbor_encode_disable_notify_result(zse->payload_mut,
					      (zse->payload_end - zse->payload),
					      &disable_notify_data, &rsp_len)) {
		return MGMT_ERR_EMSGSIZE;
	}

	zse->payload += rsp_len;

	return MGMT_ERR_EOK;
}

static int unsupported(struct mgmt_ctxt *ctxt)
{
	return MGMT_ERR_ENOTSUP;
}
