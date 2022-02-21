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
#include <zephyr.h>
#include <init.h>
#include <limits.h>
#include <string.h>
#include <tinycbor/cbor.h>
#include <tinycbor/cbor_buf_writer.h>
#include "cborattr/cborattr.h"
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

/******************************************************************************/
/* Local Constant, Macro and Type Definitions                                 */
/******************************************************************************/
#define MGMT_OP_NOTIFY 4

#define FLOAT_MAX 3.4028235E38
#define INVALID_PARAM_ID (ATTR_TABLE_SIZE + 1)
#define NOT_A_BOOL -1

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

#define END_OF_CBOR_ATTR_ARRAY                                                 \
	{                                                                      \
		.attribute = NULL                                              \
	}

#define MGMT_STATUS_CHECK(x) ((x != 0) ? MGMT_ERR_ENOMEM : 0)

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
static mgmt_handler_fn get_parameter;
static mgmt_handler_fn set_parameter;
static mgmt_handler_fn load_parameter_file;
static mgmt_handler_fn dump_parameter_file;
static mgmt_handler_fn factory_reset;
static mgmt_handler_fn set_notify;
static mgmt_handler_fn get_notify;
static mgmt_handler_fn disable_notify;
static mgmt_handler_fn check_lock_status;
static mgmt_handler_fn set_lock_code;
static mgmt_handler_fn lock;
static mgmt_handler_fn unlock;
static mgmt_handler_fn get_unlock_error_code;

static int attribute_mgmt_init(const struct device *device);

static int cbor_encode_attribute(CborEncoder *encoder,
				 long long unsigned int param_id);

static enum attr_type map_attr_to_cbor_attr(attr_id_t param_id,
					    struct cbor_attr_t *cbor_attr);

static int set_attribute(attr_id_t id, struct cbor_attr_t *cbor_attr,
			 enum attr_type type, bool *modified);

static int factory_reset(struct mgmt_ctxt *ctxt);

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
		.mh_read = get_parameter
	},
	[ATTRIBUTE_MGMT_ID_SET_PARAMETER] = {
		.mh_write = set_parameter,
		.mh_read = NULL
	},
	[ATTRIBUTE_MGMT_ID_LOAD_PARAMETER_FILE] = {
		.mh_write = load_parameter_file,
		.mh_read = NULL
	},
	[ATTRIBUTE_MGMT_ID_DUMP_PARAMETER_FILE] = {
		.mh_write = dump_parameter_file,
		.mh_read = NULL
	},
	[ATTRIBUTE_MGMT_ID_FACTORY_RESET] = {
		.mh_write = factory_reset,
		.mh_read = NULL
	},
	[ATTRIBUTE_MGMT_ID_SET_NOTIFY] = {
		.mh_write = set_notify,
		.mh_read = NULL
	},
	[ATTRIBUTE_MGMT_ID_GET_NOTIFY] = {
		.mh_write = NULL,
		.mh_read = get_notify
	},
	[ATTRIBUTE_MGMT_ID_DISABLE_NOTIFY] = {
		.mh_write = disable_notify,
		.mh_read = NULL
	},
	[ATTRIBUTE_MGMT_ID_CHECK_LOCK_STATUS] = {
		.mh_write = NULL,
		.mh_read = check_lock_status,
	},
	[ATTRIBUTE_MGMT_ID_SET_LOCK_CODE] = {
		.mh_write = set_lock_code,
		.mh_read = NULL,
	},
	[ATTRIBUTE_MGMT_ID_LOCK] = {
		.mh_write = lock,
		.mh_read = NULL,
	},
	[ATTRIBUTE_MGMT_ID_UNLOCK] = {
		.mh_write = unlock,
		.mh_read = NULL,
	},
	[ATTRIBUTE_MGMT_ID_GET_UNLOCK_ERROR_CODE] = {
		.mh_write = NULL,
		.mh_read = get_unlock_error_code,
	}
};

static struct mgmt_group attribute_mgmt_group = {
	.mg_handlers = ATTRIBUTE_MGMT_HANDLERS,
	.mg_handlers_count = ATTRIBUTE_MGMT_HANDLER_CNT,
	.mg_group_id = CONFIG_MGMT_GROUP_ID_ATTRIBUTE,
};

/* Only one parameter is written or read at a time. */
static union {
	uint64_t uint64;
	uint32_t uint32;
	uint16_t uint16;
	uint8_t uint8;
	int64_t int64;
	int32_t int32;
	int16_t int16;
	int8_t int8;
	bool boolean;
	char buf[MAX_PBUF_SIZE];
	float fval;
} param;

static size_t buf_size;

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
SYS_INIT(attribute_mgmt_init, APPLICATION, 99);

#ifdef CONFIG_MCUMGR_SMP_BT
/* Callback from attribute module */
int attr_notify(attr_id_t Index)
{
	int err = 0;
	CborEncoder cbor;
	CborEncoder cbor_map;
	struct cbor_buf_writer writer;
	size_t payload_len;
	size_t total_len;
	uint16_t mtu;

	if (smp_ble.conn_handle == NULL) {
		return -ENOTCONN;
	}

	cbor_buf_writer_init(&writer, smp_ble.cmd.buffer,
			     sizeof(smp_ble.cmd.buffer));
	cbor_encoder_init(&cbor, &writer.enc, 0);

	do {
		err |= cbor_encoder_create_map(&cbor, &cbor_map,
					       CborIndefiniteLength);
		err |= cbor_encode_attribute(&cbor, Index);
		err |= cbor_encoder_close_container(&cbor, &cbor_map);
		if (err < 0) {
			break;
		}

		payload_len = (size_t)(writer.ptr - smp_ble.cmd.buffer);
		total_len = sizeof(smp_ble.cmd.header) + payload_len;
		mtu = bt_gatt_get_mtu(smp_ble.conn_handle);
		if (total_len > BT_MAX_PAYLOAD(mtu)) {
			err = -EMSGSIZE;
			break;
		}

		smp_ble.cmd.header.op = MGMT_OP_NOTIFY;
		smp_ble.cmd.header.flags = 0;
		smp_ble.cmd.header.len_h8 =
			(uint8_t)((payload_len >> 8) & 0xFF);
		smp_ble.cmd.header.len_l8 =
			(uint8_t)((payload_len >> 0) & 0xFF);
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
	CborError err = 0;
	long long unsigned int param_id = INVALID_PARAM_ID;

	struct cbor_attr_t params_attr[] = {
		{
			.attribute = "p1",
			.type = CborAttrUnsignedIntegerType,
			.addr.uinteger = &param_id,
			.nodefault = true,
		},
		END_OF_CBOR_ATTR_ARRAY
	};

	/* Get the parameter ID */
	if (cbor_read_object(&ctxt->it, params_attr) != 0) {
		return MGMT_ERR_EINVAL;
	}

	if (param_id == INVALID_PARAM_ID) {
		return MGMT_ERR_EINVAL;
	}

	/* Encode the response */
	err |= cbor_encode_attribute(&ctxt->encoder, param_id);

	return MGMT_STATUS_CHECK(err);
}

/*
 * Map the attribute type to the CBOR type, get the value, and then encode it.
 */
static int cbor_encode_attribute(CborEncoder *encoder,
				 long long unsigned int param_id)
{
	CborError err = 0;
	int size = -EINVAL;
	attr_id_t id = (attr_id_t)param_id;
	enum attr_type type = attr_get_type(id);

	err |= cbor_encode_text_stringz(encoder, "id");
	err |= cbor_encode_uint(encoder, param_id);
	err |= cbor_encode_text_stringz(encoder, "r1");

	switch (type) {
	case ATTR_TYPE_S8:
	case ATTR_TYPE_S16:
	case ATTR_TYPE_S32:
	case ATTR_TYPE_S64:
		size = attr_get(id, &param.int64, sizeof(param.int64));
		err |= cbor_encode_int(encoder, param.int64);
		break;

	case ATTR_TYPE_U8:
	case ATTR_TYPE_U16:
	case ATTR_TYPE_U32:
	case ATTR_TYPE_U64:
		size = attr_get(id, &param.uint64, sizeof(param.uint64));
		err |= cbor_encode_uint(encoder, param.uint64);
		break;

	case ATTR_TYPE_STRING:
		size = attr_get(id, param.buf, MAX_PBUF_SIZE);
		err |= cbor_encode_text_stringz(encoder, param.buf);
		break;

	case ATTR_TYPE_FLOAT:
		size = attr_get(id, &param.fval, sizeof(param.fval));
		err |= cbor_encode_floating_point(encoder, CborFloatType,
						  &param.fval);
		break;

	case ATTR_TYPE_BOOL:
		size = attr_get(id, &param.boolean, sizeof(param.boolean));
		err |= cbor_encode_boolean(encoder, param.boolean);
		break;

	case ATTR_TYPE_BYTE_ARRAY:
		size = attr_get(id, param.buf, MAX_PBUF_SIZE);
		err |= cbor_encode_byte_string(encoder, param.buf,
					       MAX(0, size));
		break;

	default:
		/* Add dummy r1 parameter */
		param.uint64 = 0;
		err |= cbor_encode_uint(encoder, param.uint64);
		size = -EINVAL;
		break;
	}

	err |= cbor_encode_text_stringz(encoder, "r");
	err |= cbor_encode_int(encoder, size);

	return err;
}

static int set_parameter(struct mgmt_ctxt *ctxt)
{
	CborError err = 0;
	long long unsigned int param_id = INVALID_PARAM_ID;
	struct CborValue saved_context = ctxt->it;
	int set_result = -1;
	enum attr_type type;
#ifdef CONFIG_ATTRIBUTE_MGMT_INCREMENT_CONFIG_VERSION
	bool modified = false;
#endif

	struct cbor_attr_t params_attr[] = {
		{ .attribute = "p1",
		  .type = CborAttrUnsignedIntegerType,
		  .addr.uinteger = &param_id,
		  .nodefault = true },
		END_OF_CBOR_ATTR_ARRAY
	};

	struct cbor_attr_t expected[] = {
		{ .attribute = "p2", .nodefault = true }, END_OF_CBOR_ATTR_ARRAY
	};

	if (cbor_read_object(&ctxt->it, params_attr) != 0) {
		return MGMT_ERR_EINVAL;
	}

	if (param_id == INVALID_PARAM_ID) {
		return MGMT_ERR_EINVAL;
	}

	type = map_attr_to_cbor_attr((attr_id_t)param_id, expected);

	/* If the type of object isn't found in the CBOR, then the client could
	 * be trying to write the wrong type of value.
	 */
	if (cbor_read_object(&saved_context, expected) != 0) {
		return MGMT_ERR_EINVAL;
	}

	set_result = set_attribute((attr_id_t)param_id, expected, type,
#ifdef CONFIG_ATTRIBUTE_MGMT_INCREMENT_CONFIG_VERSION
				   &modified
#else
				   NULL
#endif
	);

	err |= cbor_encode_text_stringz(&ctxt->encoder, "id");
	err |= cbor_encode_uint(&ctxt->encoder, param_id);
	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_int(&ctxt->encoder, set_result);

#ifdef CONFIG_ATTRIBUTE_MGMT_INCREMENT_CONFIG_VERSION
	if (set_result == 0 && err == 0 && modified == true) {
		(void)attr_update_config_version();
	}
#endif

	return MGMT_STATUS_CHECK(err);
}

static int load_parameter_file(struct mgmt_ctxt *ctxt)
{
	int r = 0;
	CborError err = 0;
	bool modified;

	/* The input file is an optional parameter. */
	strncpy(param.buf, attr_get_quasi_static(ATTR_ID_load_path),
		sizeof(param.buf));

	struct cbor_attr_t params_attr[] = { { .attribute = "p1",
					       .type = CborAttrTextStringType,
					       .addr.string = param.buf,
					       .len = sizeof(param.buf),
					       .nodefault = false },
					     END_OF_CBOR_ATTR_ARRAY };

	if (cbor_read_object(&ctxt->it, params_attr) != 0) {
		return MGMT_ERR_EINVAL;
	}

#ifdef CONFIG_ATTR_SETTINGS_LOCK
	if (attr_is_locked() == true) {
		r = -EACCES;
	}
#endif

	r = attr_load(param.buf,
#ifdef CONFIG_ATTR_LOAD_FEEDBACK
		      CONFIG_ATTRIBUTE_MGMT_FEEDBACK_FILE,
#else
		      NULL,
#endif
		      &modified);

	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_int(&ctxt->encoder, r);

#ifdef CONFIG_ATTR_LOAD_FEEDBACK
	/* Encode the feedback file path. */
	err |= cbor_encode_text_stringz(&ctxt->encoder, "f");
	err |= cbor_encode_text_string(
		&ctxt->encoder, CONFIG_ATTRIBUTE_MGMT_FEEDBACK_FILE,
		strlen(CONFIG_ATTRIBUTE_MGMT_FEEDBACK_FILE));

#ifdef CONFIG_ATTR_CONFIGURATION_VERSION
	/* If no error update the device configuration version */
	if (r == 0 && err == 0 && modified == true) {
		attr_update_config_version();
	}
#endif
#endif

	return MGMT_STATUS_CHECK(err);
}

static int dump_parameter_file(struct mgmt_ctxt *ctxt)
{
	CborError err = 0;
	int r = -EPERM;
	long long unsigned int type = ULLONG_MAX;
	char *fstr = NULL;

	/* The output file is an optional parameter. */
	strncpy(param.buf, attr_get_quasi_static(ATTR_ID_dump_path),
		sizeof(param.buf));

	struct cbor_attr_t params_attr[] = {
		{ .attribute = "p1",
		  .type = CborAttrUnsignedIntegerType,
		  .addr.uinteger = &type,
		  .nodefault = true },
#ifdef CONFIG_ATTRIBUTE_MGMT_DUMP_USER_FILE_NAME
		{ .attribute = "p2",
		  .type = CborAttrTextStringType,
		  .addr.string = param.buf,
		  .len = sizeof(param.buf),
		  .nodefault = false },
#endif
		END_OF_CBOR_ATTR_ARRAY
	};

	if (cbor_read_object(&ctxt->it, params_attr) != 0) {
		return MGMT_ERR_EINVAL;
	}

	/* This will malloc a string as large as maximum parameter file size. */
	if (type < UINT8_MAX) {
		/* Clear file before proceeding */
		fsu_delete_abs(param.buf);

		/* Dump parameters to file */
		r = attr_prepare_then_dump(&fstr, type);
		if (r >= 0) {
			r = fsu_write_abs(param.buf, fstr, strlen(fstr));
			k_free(fstr);
		}
	}

	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_int(&ctxt->encoder, r);
	if (r >= 0) {
		err |= cbor_encode_text_stringz(&ctxt->encoder, "n");
		err |= cbor_encode_text_string(&ctxt->encoder, param.buf,
					       strlen(param.buf));
	}

	return MGMT_STATUS_CHECK(err);
}

static int factory_reset(struct mgmt_ctxt *ctxt)
{
#ifdef CONFIG_ATTRIBUTE_MGMT_FACTORY_RESET
	CborError err = 0;
	int r;
#ifdef ATTR_ID_factory_reset_enable
	uint8_t factory_reset_enabled = 0;
#endif

#ifdef CONFIG_ATTR_SETTINGS_LOCK
	if (attr_is_locked() == true) {
		r = -EACCES;
	}
#endif

	if (r == 0) {
#ifdef ATTR_ID_factory_reset_enable
		attr_get(ATTR_ID_factory_reset_enable, &factory_reset_enabled,
			 sizeof(factory_reset_enabled));

		if (factory_reset_enabled == 0) {
			r = -EPERM;
		}
#endif

		r = attribute_mgmt_factory_reset();

		if (r == 0) {
			ATTR_FRAMEWORK_BROADCAST(FMC_ATTR_FACTORY_RESET);
		}
	}

	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_int(&ctxt->encoder, r);

	return MGMT_STATUS_CHECK(err);
#else
	return MGMT_ERR_ENOTSUP;
#endif
}

static int set_notify(struct mgmt_ctxt *ctxt)
{
	CborError err = 0;
	long long unsigned int param_id = INVALID_PARAM_ID;

	/* Use an integer to check if the boolean type was found. */
	union {
		long long int integer;
		bool boolean;
	} value;
	value.integer = NOT_A_BOOL;

	struct cbor_attr_t params_attr[] = {
		{
			.attribute = "p1",
			.type = CborAttrUnsignedIntegerType,
			.addr.uinteger = &param_id,
			.nodefault = true,
		},
		{
			.attribute = "p2",
			.type = CborAttrBooleanType,
			.addr.boolean = &value.boolean,
			.nodefault = true,
		},
		END_OF_CBOR_ATTR_ARRAY
	};

	if (cbor_read_object(&ctxt->it, params_attr) != 0) {
		return MGMT_ERR_EINVAL;
	}

	if (param_id == INVALID_PARAM_ID || value.integer == NOT_A_BOOL) {
		return MGMT_ERR_EINVAL;
	}

	err |= cbor_encode_text_stringz(&ctxt->encoder, "id");
	err |= cbor_encode_uint(&ctxt->encoder, param_id);
	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_int(&ctxt->encoder,
			       attr_set_notify((attr_id_t)param_id,
					       value.boolean));

	return MGMT_STATUS_CHECK(err);
}

static int get_notify(struct mgmt_ctxt *ctxt)
{
	CborError err = 0;
	long long unsigned int param_id = INVALID_PARAM_ID;

	struct cbor_attr_t params_attr[] = {
		{ .attribute = "p1",
		  .type = CborAttrUnsignedIntegerType,
		  .addr.uinteger = &param_id,
		  .nodefault = true },
		END_OF_CBOR_ATTR_ARRAY
	};

	if (cbor_read_object(&ctxt->it, params_attr) != 0) {
		return MGMT_ERR_EINVAL;
	}

	if (param_id == INVALID_PARAM_ID) {
		return MGMT_ERR_EINVAL;
	}

	err |= cbor_encode_text_stringz(&ctxt->encoder, "id");
	err |= cbor_encode_uint(&ctxt->encoder, param_id);
	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_boolean(&ctxt->encoder,
				   attr_get_notify((attr_id_t)param_id));

	return MGMT_STATUS_CHECK(err);
}

static int disable_notify(struct mgmt_ctxt *ctxt)
{
	CborError err = 0;

	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_int(&ctxt->encoder, attr_disable_notify());

	return MGMT_STATUS_CHECK(err);
}

/* Get the CBOR type from the attribute type.
 * Map the CBOR attribute data structure to the type that is expected.
 * (Then read the CBOR again looking for the expected type.)
 */
static enum attr_type map_attr_to_cbor_attr(attr_id_t param_id,
					    struct cbor_attr_t *cbor_attr)
{
	enum attr_type type = attr_get_type(param_id);

	switch (type) {
	case ATTR_TYPE_BOOL:
		param.boolean = false;
		cbor_attr->type = CborAttrBooleanType;
		cbor_attr->addr.boolean = &param.boolean;
		break;

	case ATTR_TYPE_S8:
	case ATTR_TYPE_S16:
	case ATTR_TYPE_S32:
	case ATTR_TYPE_S64:
		param.int64 = LLONG_MAX;
		cbor_attr->type = CborAttrIntegerType;
		cbor_attr->addr.integer = &param.int64;
		break;

	case ATTR_TYPE_U8:
	case ATTR_TYPE_U16:
	case ATTR_TYPE_U32:
	case ATTR_TYPE_U64:
		param.uint64 = ULLONG_MAX;
		cbor_attr->type = CborAttrUnsignedIntegerType;
		cbor_attr->addr.integer = &param.uint64;
		break;

	case ATTR_TYPE_STRING:
		memset(param.buf, 0, MAX_PBUF_SIZE);
		cbor_attr->type = CborAttrTextStringType;
		cbor_attr->addr.string = param.buf;
		cbor_attr->len = sizeof(param.buf);
		break;

	case ATTR_TYPE_FLOAT:
		param.fval = FLOAT_MAX;
		cbor_attr->type = CborAttrFloatType;
		cbor_attr->addr.fval = &param.fval;
		break;

	case ATTR_TYPE_BYTE_ARRAY:
		memset(param.buf, 0, MAX_PBUF_SIZE);
		cbor_attr->type = CborAttrByteStringType;
		cbor_attr->addr.bytestring.data = param.buf;
		cbor_attr->addr.bytestring.len = &buf_size;
		cbor_attr->len = sizeof(param.buf);
		break;

	default:
		cbor_attr->type = CborAttrNullType;
		cbor_attr->attribute = NULL;
		break;
	};

	return type;
}

static int set_attribute(attr_id_t id, struct cbor_attr_t *cbor_attr,
			 enum attr_type type, bool *modified)
{
	int status = -EINVAL;

	if (modified != NULL) {
		modified = false;
	}

	if (type == ATTR_TYPE_S8 || type == ATTR_TYPE_S16 ||
	    type == ATTR_TYPE_S32 || type == ATTR_TYPE_S64) {
		if (type == ATTR_TYPE_S8) {
			int8_t tmp_val = (int8_t)*cbor_attr->addr.integer;
			status = attr_set(id, type, &tmp_val, sizeof(tmp_val),
					  modified);
		} else if (type == ATTR_TYPE_S16) {
			int16_t tmp_val = (int16_t)*cbor_attr->addr.integer;
			status = attr_set(id, type, &tmp_val, sizeof(tmp_val),
					  modified);
		} else if (type == ATTR_TYPE_S32) {
			int32_t tmp_val = (int32_t)*cbor_attr->addr.integer;
			status = attr_set(id, type, &tmp_val, sizeof(tmp_val),
					  modified);
		} else if (type == ATTR_TYPE_S64) {
			status = attr_set(id, type, cbor_attr->addr.integer,
					  sizeof(int64_t), modified);
		}
	} else if (type == ATTR_TYPE_U8 || type == ATTR_TYPE_U16 ||
		   type == ATTR_TYPE_U32 || type == ATTR_TYPE_U64) {
		if (type == ATTR_TYPE_U8) {
			uint8_t tmp_val = (uint8_t)*cbor_attr->addr.uinteger;
			status = attr_set(id, type, &tmp_val, sizeof(tmp_val),
					  modified);
		} else if (type == ATTR_TYPE_U16) {
			uint16_t tmp_val = (uint16_t)*cbor_attr->addr.uinteger;
			status = attr_set(id, type, &tmp_val, sizeof(tmp_val),
					  modified);
		} else if (type == ATTR_TYPE_U32) {
			uint32_t tmp_val = (uint32_t)*cbor_attr->addr.uinteger;
			status = attr_set(id, type, &tmp_val, sizeof(tmp_val),
					  modified);
		} else if (type == ATTR_TYPE_U64) {
			status = attr_set(id, type, cbor_attr->addr.uinteger,
					  sizeof(uint64_t), modified);
		}
	} else {
		switch (cbor_attr->type) {
		case CborAttrTextStringType:
			status = attr_set(id, type, cbor_attr->addr.string,
					  strlen(cbor_attr->addr.string),
					  modified);
			break;

		case CborAttrFloatType:
			status = attr_set(id, type, cbor_attr->addr.fval,
					  sizeof(float), modified);
			break;

		case CborAttrBooleanType:
			status = attr_set(id, type, cbor_attr->addr.boolean,
					  sizeof(bool), modified);
			break;

		case CborAttrByteStringType:
			status = attr_set(id, type,
					  cbor_attr->addr.bytestring.data,
					  *(cbor_attr->addr.bytestring.len),
					  modified);
			break;

		default:
			break;
		}
	}

	return status;
}

static int check_lock_status(struct mgmt_ctxt *ctxt)
{
#ifdef CONFIG_ATTR_SETTINGS_LOCK
	bool lock_enabled;
	bool lock_active = false;
	uint8_t lock_status;
	int r = 0;

	r = attr_get(ATTR_ID_lock, &lock_enabled, sizeof(lock_enabled));

	if (r >= 0) {
		r = attr_get(ATTR_ID_lock_status, &lock_status,
			     sizeof(lock_status));

		if (r >= 0) {
			if (lock_status == LOCK_STATUS_SETUP_ENGAGED) {
				lock_active = true;
			}

			/* Completed successfully so return success result
			 * code
			 */
			r = 0;
		}
	}

	/* Cbor encode result */
	CborError err = 0;

	/* Add result */
	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_int(&ctxt->encoder, r);

	/* Add if lock is enabled */
	err |= cbor_encode_text_stringz(&ctxt->encoder, "r1");
	err |= cbor_encode_boolean(&ctxt->encoder, lock_enabled);

	/* Add if lock is engaged */
	err |= cbor_encode_text_stringz(&ctxt->encoder, "r2");
	err |= cbor_encode_boolean(&ctxt->encoder, lock_active);

	/* Exit with result */
	return MGMT_STATUS_CHECK(err);
#else
	return MGMT_ERR_ENOTSUP;
#endif
}

static int set_lock_code(struct mgmt_ctxt *ctxt)
{
#ifdef CONFIG_ATTR_SETTINGS_LOCK
	long long unsigned int lock_code_tmp = ULLONG_MAX;
	uint32_t lock_code;
	int r = 0;

	struct cbor_attr_t params_attr[] = {
		{ .attribute = "p1",
		  .type = CborAttrUnsignedIntegerType,
		  .addr.uinteger = &lock_code_tmp,
		  .nodefault = true },
		END_OF_CBOR_ATTR_ARRAY
	};

	if (cbor_read_object(&ctxt->it, params_attr) != 0) {
		return -EINVAL;
	}

	if (attr_is_locked() == true) {
		r = -EACCES;
	}

	if (r == 0) {
		lock_code = (uint32_t)lock_code_tmp;
		r = attr_set_uint32(ATTR_ID_settings_passcode, lock_code);
	}

	if (r == 0) {
		r = attr_set_uint32(ATTR_ID_lock, true);
	}

	if (r == 0) {
		/* This sets the lock ready to be engaged when the user
		 * disconnects, when the module is rebooted or when the user
		 * manually requests it with the lock command, but allows
		 * further configuration changes to the unit until then
		 */
		r = attr_set_uint32(ATTR_ID_lock_status,
				    LOCK_STATUS_SETUP_DISENGAGED);

		ATTR_FRAMEWORK_BROADCAST(FMC_ATTR_LOCK_STATUS_CHANGED);
	}

	/* Cbor encode result */
	CborError err = 0;

	/* Add result */
	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_int(&ctxt->encoder, r);

	/* Exit with result */
	return MGMT_STATUS_CHECK(err);
#else
	return MGMT_ERR_ENOTSUP;
#endif
}

static int lock(struct mgmt_ctxt *ctxt)
{
#ifdef CONFIG_ATTR_SETTINGS_LOCK
	enum settings_passcode_status passcode_status =
		SETTINGS_PASSCODE_STATUS_UNDEFINED;
	int r = 0;

	if (attr_is_locked() == false) {
		/* Lock the settings */
		attr_set_uint32(ATTR_ID_lock, true);
		attr_set_uint32(ATTR_ID_lock_status, LOCK_STATUS_SETUP_ENGAGED);
		passcode_status = SETTINGS_PASSCODE_STATUS_VALID_CODE;

		/* Send feedback about the passcode */
		attr_set_uint32(ATTR_ID_settings_passcode_status,
				passcode_status);

		ATTR_FRAMEWORK_BROADCAST(FMC_ATTR_LOCK_STATUS_CHANGED);
	}

	/* Cbor encode result */
	CborError err = 0;

	/* Add result */
	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_int(&ctxt->encoder, r);

	/* Exit with result */
	return MGMT_STATUS_CHECK(err);
#else
	return MGMT_ERR_ENOTSUP;
#endif
}

static int unlock(struct mgmt_ctxt *ctxt)
{
#ifdef CONFIG_ATTR_SETTINGS_LOCK
	enum settings_passcode_status passcode_status =
		SETTINGS_PASSCODE_STATUS_UNDEFINED;
	long long unsigned int lock_code_tmp = ULLONG_MAX;
	uint32_t lock_code;
	uint32_t real_lock_code;
	bool permanent_unlock = false;
	int r = 0;

	struct cbor_attr_t params_attr[] = {
		{ .attribute = "p1",
		  .type = CborAttrUnsignedIntegerType,
		  .addr.uinteger = &lock_code_tmp,
		  .nodefault = true },
		{ .attribute = "p2",
		  .type = CborAttrBooleanType,
		  .addr.boolean = &permanent_unlock,
		  .nodefault = true },
		END_OF_CBOR_ATTR_ARRAY
	};

	if (cbor_read_object(&ctxt->it, params_attr) != 0) {
		return -EINVAL;
	}

	if (attr_is_locked() == true) {
		real_lock_code =
			attr_get_uint32(ATTR_ID_settings_passcode, 123456);
		lock_code = (uint32_t)lock_code_tmp;

		/* Check if the passcode entered matches */
		if (real_lock_code == lock_code) {
			/* Unlock the settings */
			attr_set_uint32(ATTR_ID_lock_status,
					LOCK_STATUS_SETUP_DISENGAGED);
			passcode_status = SETTINGS_PASSCODE_STATUS_VALID_CODE;
		} else {
			passcode_status = SETTINGS_PASSCODE_STATUS_INVALID_CODE;
			r = -EINVAL;

			/* Sleep for 1.5 seconds to slow down possible
			 * attacks
			 */
			k_sleep(K_MSEC(LOCK_INVALID_WAIT_TIME_MS));
		}

		/* Send feedback to APP about the passcode */
		attr_set_uint32(ATTR_ID_settings_passcode_status,
				passcode_status);
	}

	if (permanent_unlock == true && attr_is_locked() == false && r == 0) {
		/* User has requested to remove the lock entirely */
		attr_set_uint32(ATTR_ID_lock, false);
		attr_set_uint32(ATTR_ID_lock_status, LOCK_STATUS_NOT_SETUP);
	}

	if (r == 0) {
		ATTR_FRAMEWORK_BROADCAST(FMC_ATTR_LOCK_STATUS_CHANGED);
	}

	/* Cbor encode result */
	CborError err = 0;

	/* Add result */
	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_int(&ctxt->encoder, r);

	/* Exit with result */
	return MGMT_STATUS_CHECK(err);
#else
	return MGMT_ERR_ENOTSUP;
#endif
}

static int get_unlock_error_code(struct mgmt_ctxt *ctxt)
{
#ifdef CONFIG_ATTR_SETTINGS_LOCK
	enum settings_passcode_status passcode_status =
		SETTINGS_PASSCODE_STATUS_UNDEFINED;
	int r = 0;

	r = attr_get(ATTR_ID_settings_passcode_status, &passcode_status,
		     sizeof(passcode_status));

	if (r >= 0) {
		/* Clear status */
		attr_set_uint32(ATTR_ID_settings_passcode_status,
				SETTINGS_PASSCODE_STATUS_UNDEFINED);

		r = 0;
	}

	/* Cbor encode result */
	CborError err = 0;

	/* Add result */
	err |= cbor_encode_text_stringz(&ctxt->encoder, "r");
	err |= cbor_encode_int(&ctxt->encoder, r);

	/* Add status */
	err |= cbor_encode_text_stringz(&ctxt->encoder, "r1");
	err |= cbor_encode_int(&ctxt->encoder, passcode_status);

	/* Exit with result */
	return MGMT_STATUS_CHECK(err);
#else
	return MGMT_ERR_ENOTSUP;
#endif
}
