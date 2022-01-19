/**
 * @file test_lock.c
 * @brief
 * @note: Mock logging for tests was adapted from
 * zephyr/tests/subsys/logging/log_output
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <stdlib.h>
#include <zephyr.h>
#include <ztest.h>
#include <logging/log.h>
#include <logging/log_output.h>
#include <logging/log_backend.h>
#include <logging/log_core.h>
#include <logging/log_msg.h>
#include <logging/log_backend_std.h>
#include <sys/byteorder.h>
#include <mgmt/mgmt.h>
#include <net/buf.h>
#include <file_system_utilities.h>
#include <attr.h>
#include <attr_table.h>
#include "test_lock.h"
#include <tinycbor/cbor.h>
#include <tinycbor/cbor_buf_writer.h>
#include "cborattr/cborattr.h"
#include "attribute_mgmt.h"
#include "decode_lock.h"
#include "encode_lock.h"
#include "dummy_smp.h"

/******************************************************************************/
/* Local Constant, Macro and Type Definitions                                 */
/******************************************************************************/
#define DEFAULT_LOCK_CODE 123456
#define TEST_LOCK_CODE 421731
#define ASTERISK_RESPONSE "******"
#define MIN_DUMP_FILE_SIZE 64
#define DATA_BUFFER_SIZE 384

#define LOCK_CODE_A 123456
#define LOCK_CODE_B 23848
#define LOCK_CODE_C 594729

#define SMP_DATA_READY_WAIT_TIME_S 3

/* Empty CBOR map size */
#define CBOR_MAP_DATA_SIZE 2
/* "r" = 2 bytes, parameter = 1 byte */
#define CBOR_RESULT_MIN_SIZE 3
/* "r" = 2 bytes, parameter = 4 bytes */
#define CBOR_RESULT_MAX_SIZE 6

/* Base SMP command size */
#define SMP_PACKET_SIZE 8
/* 16-bit packet size, 16-bit CRC */
#define SMP_CONSOLE_PADDING 4
/* 0x06, 0x09 at start, 0x0a at end */
#define SMP_CONSOLE_EXTRA_SIZE 3

/* Raw SMP data sizes prior to being added to messages */
#define CBOR_COMMAND_CHECK_LOCK_STATUS_SIZE (CBOR_MAP_DATA_SIZE)
#define CBOR_COMMAND_LOCK_SIZE (CBOR_MAP_DATA_SIZE)
/* "p1" = 3 bytes, parameter = 2 bytes, "p2" = 3 bytes, parameter = 1 byte */
#define CBOR_COMMAND_UNLOCK_MIN_SIZE (CBOR_MAP_DATA_SIZE + 3 + 2 + 3 + 1)
/* "p1" = 3 bytes, parameter = 5 bytes, "p2" = 3 bytes, parameter = 1 byte */
#define CBOR_COMMAND_UNLOCK_MAX_SIZE (CBOR_MAP_DATA_SIZE + 3 + 5 + 3 + 1)
/* "p1" = 3 bytes, parameter = 2 bytes */
#define CBOR_COMMAND_SET_LOCK_CODE_MIN_SIZE (CBOR_MAP_DATA_SIZE + 3 + 2)
/* "p1" = 3 bytes, parameter = 5 bytes */
#define CBOR_COMMAND_SET_LOCK_CODE_MAX_SIZE (CBOR_MAP_DATA_SIZE + 3 + 5)
#define CBOR_COMMAND_GET_UNLOCK_ERROR_CODE_SIZE (CBOR_MAP_DATA_SIZE)
/* "p1" = 3 bytes, parameter = 1 byte, "p2" = 3 bytes, parameter = 1 byte */
#define CBOR_COMMAND_SET_PARAMETER_MIN_SIZE (CBOR_MAP_DATA_SIZE + 3 + 1 + 3 + 1)
/* "p1" = 3 bytes, parameter = 2 bytes, "p2" = 3 bytes, parameter = 16 bytes */
#define CBOR_COMMAND_SET_PARAMETER_MAX_SIZE                                    \
	(CBOR_MAP_DATA_SIZE + 3 + 2 + 3 + 16)
/* "p1" = 3 bytes, parameter = 1 byte */
#define CBOR_COMMAND_GET_PARAMETER_MIN_SIZE (CBOR_MAP_DATA_SIZE + 3 + 1)
/* "p1" = 3 bytes, parameter = 2 bytes */
#define CBOR_COMMAND_GET_PARAMETER_MAX_SIZE (CBOR_MAP_DATA_SIZE + 3 + 2)

/* "r" = 2 bytes, parameter = 1 byte, "r1" = 3 bytes, parameter = 1 byte,
 * "r2" = 3 bytes, parameter = 1 byte
 */
#define CBOR_RESPONSE_CHECK_LOCK_STATUS_SIZE                                   \
	(CBOR_MAP_DATA_SIZE + 2 + 1 + 3 + 1 + 3 + 1)
#define CBOR_RESPONSE_LOCK_MIN_SIZE (CBOR_MAP_DATA_SIZE + CBOR_RESULT_MIN_SIZE)
#define CBOR_RESPONSE_LOCK_MAX_SIZE (CBOR_MAP_DATA_SIZE + CBOR_RESULT_MAX_SIZE)
#define CBOR_RESPONSE_UNLOCK_MIN_SIZE                                          \
	(CBOR_MAP_DATA_SIZE + CBOR_RESULT_MIN_SIZE)
#define CBOR_RESPONSE_UNLOCK_MAX_SIZE                                          \
	(CBOR_MAP_DATA_SIZE + CBOR_RESULT_MAX_SIZE)
#define CBOR_RESPONSE_SET_LOCK_CODE_MIN_SIZE                                   \
	(CBOR_MAP_DATA_SIZE + CBOR_RESULT_MIN_SIZE)
#define CBOR_RESPONSE_SET_LOCK_CODE_MAX_SIZE                                   \
	(CBOR_MAP_DATA_SIZE + CBOR_RESULT_MAX_SIZE)
/* "r" = 2 bytes, parameter = 1 byte, "r1" = 3 bytes, parameter = 1 byte */
#define CBOR_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE                               \
	(CBOR_MAP_DATA_SIZE + 2 + 1 + 3 + 1)
/* "id" = 3 bytes, parameter = 1 byte, "r" = 2 bytes, parameter = 1 byte */
#define CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE                                   \
	(CBOR_MAP_DATA_SIZE + 3 + 1 + 2 + 1)
/* "id" = 3 bytes, parameter = 2 bytes, "r" = 2 bytes, parameter = 16 bytes */
#define CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE                                   \
	(CBOR_MAP_DATA_SIZE + 3 + 2 + 2 + 16)
/* "id" = 3 bytes, parameter = 1 byte, "r1" = 3 bytes, parameter = 1 byte,
 * "r" = 2 bytes, parameter = 1 byte
 */
#define CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE                                   \
	(CBOR_MAP_DATA_SIZE + 3 + 1 + 3 + 1 + 2 + 1)
/* "id" = 3 bytes, parameter = 2 bytes, "r1" = 3 bytes, parameter = 16 bytes,
 * "r" = 2 bytes, parameter = 2 bytes
 */
#define CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE                                   \
	(CBOR_MAP_DATA_SIZE + 3 + 2 + 3 + 16 + 2 + 2)

/* Raw SMP command sizes prior to adding CRC or base64 encoding */
#define SMP_COMMAND_CHECK_LOCK_STATUS_SIZE                                     \
	(SMP_PACKET_SIZE + CBOR_COMMAND_CHECK_LOCK_STATUS_SIZE)
#define SMP_COMMAND_LOCK_SIZE (SMP_PACKET_SIZE + CBOR_COMMAND_LOCK_SIZE)
#define SMP_COMMAND_UNLOCK_MIN_SIZE                                            \
	(SMP_PACKET_SIZE + CBOR_COMMAND_UNLOCK_MIN_SIZE)
#define SMP_COMMAND_UNLOCK_MAX_SIZE                                            \
	(SMP_PACKET_SIZE + CBOR_COMMAND_UNLOCK_MAX_SIZE)
#define SMP_COMMAND_SET_LOCK_CODE_MIN_SIZE                                     \
	(SMP_PACKET_SIZE + CBOR_COMMAND_SET_LOCK_CODE_MIN_SIZE)
#define SMP_COMMAND_SET_LOCK_CODE_MAX_SIZE                                     \
	(SMP_PACKET_SIZE + CBOR_COMMAND_SET_LOCK_CODE_MAX_SIZE)
#define SMP_COMMAND_GET_UNLOCK_ERROR_CODE_SIZE                                 \
	(SMP_PACKET_SIZE + CBOR_COMMAND_GET_UNLOCK_ERROR_CODE_SIZE)
#define SMP_COMMAND_SET_PARAMETER_MIN_SIZE                                     \
	(SMP_PACKET_SIZE + CBOR_COMMAND_SET_PARAMETER_MIN_SIZE)
#define SMP_COMMAND_SET_PARAMETER_MAX_SIZE                                     \
	(SMP_PACKET_SIZE + CBOR_COMMAND_SET_PARAMETER_MAX_SIZE)
#define SMP_COMMAND_GET_PARAMETER_MIN_SIZE                                     \
	(SMP_PACKET_SIZE + CBOR_COMMAND_GET_PARAMETER_MIN_SIZE)
#define SMP_COMMAND_GET_PARAMETER_MAX_SIZE                                     \
	(SMP_PACKET_SIZE + CBOR_COMMAND_GET_PARAMETER_MAX_SIZE)

#define SMP_RESPONSE_CHECK_LOCK_STATUS_SIZE                                    \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_CHECK_LOCK_STATUS_SIZE)
#define SMP_RESPONSE_LOCK_MIN_SIZE                                             \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_LOCK_MIN_SIZE)
#define SMP_RESPONSE_LOCK_MAX_SIZE                                             \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_LOCK_MAX_SIZE)
#define SMP_RESPONSE_UNLOCK_MIN_SIZE                                           \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_UNLOCK_MIN_SIZE)
#define SMP_RESPONSE_UNLOCK_MAX_SIZE                                           \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_UNLOCK_MAX_SIZE)
#define SMP_RESPONSE_SET_LOCK_CODE_MIN_SIZE                                    \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_SET_LOCK_CODE_MIN_SIZE)
#define SMP_RESPONSE_SET_LOCK_CODE_MAX_SIZE                                    \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_SET_LOCK_CODE_MAX_SIZE)
#define SMP_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE                                \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE)
#define SMP_RESPONSE_SET_PARAMETER_MIN_SIZE                                    \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE)
#define SMP_RESPONSE_SET_PARAMETER_MAX_SIZE                                    \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE)
#define SMP_RESPONSE_GET_PARAMETER_MIN_SIZE                                    \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE)
#define SMP_RESPONSE_GET_PARAMETER_MAX_SIZE                                    \
	(SMP_PACKET_SIZE + CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE)

#define CEIL(A, B) (((A) + (B)-1) / (B))

#define BASE64_ENCODE_SIZE(x) (CEIL(x, 3) * 4)

/* SMP over console command sizes after adding CRC and base64 encoding */
#define SMP_CONSOLE_COMMAND_CHECK_LOCK_STATUS_SIZE                             \
	(BASE64_ENCODE_SIZE(SMP_COMMAND_CHECK_LOCK_STATUS_SIZE +               \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_COMMAND_LOCK_SIZE                                          \
	(BASE64_ENCODE_SIZE(SMP_COMMAND_LOCK_SIZE + SMP_CONSOLE_PADDING) +     \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_COMMAND_UNLOCK_MIN_SIZE                                    \
	(BASE64_ENCODE_SIZE(SMP_COMMAND_UNLOCK_MIN_SIZE +                      \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_COMMAND_UNLOCK_MAX_SIZE                                    \
	(BASE64_ENCODE_SIZE(SMP_COMMAND_UNLOCK_MAX_SIZE +                      \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_COMMAND_SET_LOCK_CODE_MIN_SIZE                             \
	(BASE64_ENCODE_SIZE(SMP_COMMAND_SET_LOCK_CODE_MIN_SIZE +               \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_COMMAND_SET_LOCK_CODE_MAX_SIZE                             \
	(BASE64_ENCODE_SIZE(SMP_COMMAND_SET_LOCK_CODE_MAX_SIZE +               \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_COMMAND_GET_UNLOCK_ERROR_CODE_SIZE                         \
	(BASE64_ENCODE_SIZE(SMP_COMMAND_GET_UNLOCK_ERROR_CODE_SIZE +           \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE                             \
	(BASE64_ENCODE_SIZE(SMP_COMMAND_SET_PARAMETER_MIN_SIZE +               \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE                             \
	(BASE64_ENCODE_SIZE(SMP_COMMAND_SET_PARAMETER_MAX_SIZE +               \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE                             \
	(BASE64_ENCODE_SIZE(SMP_COMMAND_GET_PARAMETER_MIN_SIZE +               \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE                             \
	(BASE64_ENCODE_SIZE(SMP_COMMAND_GET_PARAMETER_MAX_SIZE +               \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)

#define SMP_CONSOLE_RESPONSE_CHECK_LOCK_STATUS_SIZE                            \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_CHECK_LOCK_STATUS_SIZE +              \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_RESPONSE_LOCK_MIN_SIZE                                     \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_LOCK_MIN_SIZE +                       \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_RESPONSE_LOCK_MAX_SIZE                                     \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_LOCK_MAX_SIZE +                       \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_RESPONSE_UNLOCK_MIN_SIZE                                   \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_UNLOCK_MIN_SIZE +                     \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_RESPONSE_UNLOCK_MAX_SIZE                                   \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_UNLOCK_MAX_SIZE +                     \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_RESPONSE_SET_LOCK_CODE_MIN_SIZE                            \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_SET_LOCK_CODE_MIN_SIZE +              \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_RESPONSE_SET_LOCK_CODE_MAX_SIZE                            \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_SET_LOCK_CODE_MAX_SIZE +              \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE                        \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE +          \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE                            \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_SET_PARAMETER_MIN_SIZE +              \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE                            \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_SET_PARAMETER_MAX_SIZE +              \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE                            \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_GET_PARAMETER_MIN_SIZE +              \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)
#define SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE                            \
	(BASE64_ENCODE_SIZE(SMP_RESPONSE_GET_PARAMETER_MAX_SIZE +              \
			    SMP_CONSOLE_PADDING) +                             \
	 SMP_CONSOLE_EXTRA_SIZE)

#define ATTR_TEST_VAL_U8_DEFAULT 25
#define ATTR_TEST_VAL_U8_VALID 87
#define ATTR_TEST_VAL_U8_INVALID 63

#define ATTR_TEST_VAL_S8_DEFAULT 26
#define ATTR_TEST_VAL_S8_VALID -39
#define ATTR_TEST_VAL_S8_INVALID -51

#define ATTR_TEST_VAL_U16_DEFAULT 27
#define ATTR_TEST_VAL_U16_VALID 53241
#define ATTR_TEST_VAL_U16_INVALID 61020

#define ATTR_TEST_VAL_S16_DEFAULT 28
#define ATTR_TEST_VAL_S16_VALID -29888
#define ATTR_TEST_VAL_S16_INVALID -31019

#define ATTR_TEST_VAL_U32_DEFAULT 29
#define ATTR_TEST_VAL_U32_VALID 3144969195
#define ATTR_TEST_VAL_U32_INVALID 1591964431

#define ATTR_TEST_VAL_S32_DEFAULT 30
#define ATTR_TEST_VAL_S32_VALID -2117483645
#define ATTR_TEST_VAL_S32_INVALID -2021476312

#define ATTR_TEST_VAL_STRING_DEFAULT "test"
#define ATTR_TEST_VAL_STRING_VALID "success"
#define ATTR_TEST_VAL_STRING_INVALID "fails"

#define ATTR_TEST_VAL_BYTE_ARRAY_DEFAULT                                       \
	0xFA, 0xFF, 0xCC, 0x22, 0x00, 0x18, 0x32, 0x74, 0xA2, 0xBC
#define ATTR_TEST_VAL_BYTE_ARRAY_VALID                                         \
	0x22, 0x33, 0x52, 0x01, 0x08, 0x2F, 0x73, 0xBB, 0x00, 0x07
#define ATTR_TEST_VAL_BYTE_ARRAY_INVALID                                       \
	0x01, 0x00, 0xD2, 0x8A, 0x6F, 0xA2, 0xB9, 0x37, 0x28, 0x46

#define LOCK_INVALID_EXPECTED_WAIT_TIME_MS 1500
#define LOCK_INVALID_DELTA_TIME_MS 150

/******************************************************************************/
/* Local Function Prototypes                                                  */
/******************************************************************************/
static void reset_mock_buffer(void);
static int mock_output_func(uint8_t *buf, size_t size, void *ctx);
static void sync_string(const struct log_backend *const backend,
			struct log_msg_ids src_level, uint32_t timestamp,
			const char *fmt, va_list ap);

/******************************************************************************/
/* Local Data Definitions                                                     */
/******************************************************************************/
static uint8_t mock_buffer[384];
static uint8_t log_output_buf[8];
static uint32_t mock_len;
static bool enable_logging = false;
static uint8_t tmp_buffer[384];
static int16_t message_size = 0;
static uint8_t sequence = 1;
static struct net_buf *nb = NULL;

const struct log_backend_api log_backend_test_api = { .process = NULL,
						      .put = NULL,
						      .put_sync_string =
							      sync_string,
						      .put_sync_hexdump = NULL,
						      .panic = NULL,
						      .init = NULL,
						      .dropped = NULL };

LOG_OUTPUT_DEFINE(log_output, mock_output_func, log_output_buf,
		  sizeof(log_output_buf));

LOG_BACKEND_DEFINE(log_backend_test, log_backend_test_api, true);

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
static void clear_smp_buffers(void)
{
	memset(tmp_buffer, 0, sizeof(tmp_buffer));
	message_size = 0;
	smp_dummy_clear_state();
}

static int8_t create_cbor_header(uint8_t *buffer, uint8_t op,
				 uint16_t packet_size, uint16_t group,
				 uint8_t sequence, uint8_t command)
{
	uint8_t pos = 0;

	buffer[pos++] = op;
	buffer[pos++] = 0;

	/* Flip to big endian */
	packet_size = sys_cpu_to_be16(packet_size);
	group = sys_cpu_to_be16(group);

	memcpy(&buffer[pos], &packet_size, sizeof(packet_size));
	pos += sizeof(packet_size);
	memcpy(&buffer[pos], &group, sizeof(group));
	pos += sizeof(group);

	buffer[pos++] = sequence;
	buffer[pos++] = command;

	return pos;
}

static void check_cbor_header(uint8_t *buffer, uint8_t op,
			      uint16_t cbor_size_min, uint16_t cbor_size_max,
			      uint16_t group, uint8_t sequence, uint8_t command)
{
	uint8_t pos = 0;
	uint16_t cbor_data_size;

	zassert_equal(buffer[pos++], op, "Invalid op code");
	zassert_equal(buffer[pos++], 0, "Flags should be 0");

	/* Flip endian */
	group = sys_cpu_to_be16(group);
	cbor_data_size = sys_be16_to_cpu(*(uint16_t *)&buffer[pos]);

	zassert_true(cbor_data_size >= cbor_size_min &&
			     cbor_data_size <= cbor_size_max,
		     "Invalid packet size");
	pos += sizeof(cbor_data_size);
	zassert_mem_equal(&buffer[pos], &group, sizeof(group), "Invalid group");
	pos += sizeof(group);

	zassert_equal(buffer[pos++], sequence, "Invalid sequence number");
	zassert_equal(buffer[pos++], command, "Invalid command");
}

static int32_t create_check_lock_command(uint8_t *buffer, uint16_t buffer_size,
					 uint8_t sequence)
{
	int32_t rc = 0;

	/* Append empty map, required for SMP function to be processed */
	struct empty_map dummy_data = {
		._empty_map_nil_present = 0,
	};

	if (cbor_encode_empty_map(&buffer[SMP_PACKET_SIZE],
				  (buffer_size - SMP_PACKET_SIZE), &dummy_data,
				  &rc) == true) {
		rc += create_cbor_header(buffer, MGMT_OP_READ, 0,
					 CONFIG_MGMT_GROUP_ID_ATTRIBUTE,
					 sequence,
					 ATTRIBUTE_MGMT_ID_CHECK_LOCK_STATUS);
	} else {
		rc = -EINVAL;
	}

	return rc;
}

static int32_t create_set_lock_command(uint8_t *buffer, uint16_t buffer_size,
				       uint8_t sequence, uint32_t lock_code)
{
	int32_t rc = 0;

	struct set_lock_code set_lock_code_struct = {
		._set_lock_code_p1 = lock_code,
	};

	if (cbor_encode_set_lock_code(&buffer[SMP_PACKET_SIZE],
				      (buffer_size - SMP_PACKET_SIZE),
				      &set_lock_code_struct, &rc) == true) {
		rc += create_cbor_header(buffer, MGMT_OP_WRITE, rc,
					 CONFIG_MGMT_GROUP_ID_ATTRIBUTE,
					 sequence,
					 ATTRIBUTE_MGMT_ID_SET_LOCK_CODE);
	} else {
		rc = -EINVAL;
	}

	return rc;
}

static int32_t create_lock_command(uint8_t *buffer, uint16_t buffer_size,
				   uint8_t sequence)
{
	int32_t rc = 0;

	/* Append empty map, required for SMP function to be processed */
	struct empty_map dummy_data = {
		._empty_map_nil_present = 0,
	};

	if (cbor_encode_empty_map(&buffer[SMP_PACKET_SIZE],
				  (buffer_size - SMP_PACKET_SIZE), &dummy_data,
				  &rc) == true) {
		rc += create_cbor_header(buffer, MGMT_OP_WRITE, 0,
					 CONFIG_MGMT_GROUP_ID_ATTRIBUTE,
					 sequence, ATTRIBUTE_MGMT_ID_LOCK);
	} else {
		rc = -EINVAL;
	}

	return rc;
}

static int32_t create_unlock_command(uint8_t *buffer, uint16_t buffer_size,
				     uint8_t sequence, uint32_t lock_code,
				     bool permanent)
{
	int32_t rc = 0;

	struct unlock unlock_struct = {
		._unlock_p1 = lock_code,
		._unlock_p2 = permanent,
	};

	if (cbor_encode_unlock(&buffer[SMP_PACKET_SIZE],
			       (buffer_size - SMP_PACKET_SIZE), &unlock_struct,
			       &rc) == true) {
		rc += create_cbor_header(buffer, MGMT_OP_WRITE, rc,
					 CONFIG_MGMT_GROUP_ID_ATTRIBUTE,
					 sequence, ATTRIBUTE_MGMT_ID_UNLOCK);
	} else {
		rc = -EINVAL;
	}

	return rc;
}

static int32_t create_get_unlock_error_code_command(uint8_t *buffer,
						    uint16_t buffer_size,
						    uint8_t sequence)
{
	int32_t rc = 0;

	/* Append empty map, required for SMP function to be processed */
	struct empty_map dummy_data = {
		._empty_map_nil_present = 0,
	};

	if (cbor_encode_empty_map(&buffer[SMP_PACKET_SIZE],
				  (buffer_size - SMP_PACKET_SIZE), &dummy_data,
				  &rc) == true) {
		rc += create_cbor_header(
			buffer, MGMT_OP_READ, 0, CONFIG_MGMT_GROUP_ID_ATTRIBUTE,
			sequence, ATTRIBUTE_MGMT_ID_GET_UNLOCK_ERROR_CODE);
	} else {
		rc = -EINVAL;
	}

	return rc;
}

static int32_t
create_set_parameter_command(uint8_t *buffer, uint16_t buffer_size,
			     uint8_t sequence, uint16_t parameter_id,
			     enum attr_type parameter_type,
			     void *parameter_value, uint8_t parameter_size)
{
	int32_t rc = 0;

	struct set_parameter set_parameter_struct = { ._set_parameter_p1 =
							      parameter_id };

	if (parameter_type == ATTR_TYPE_UNKNOWN ||
	    parameter_type == ATTR_TYPE_ANY ||
	    parameter_type == ATTR_TYPE_ATOMIC ||
	    parameter_type == ATTR_TYPE_U64 ||
	    parameter_type == ATTR_TYPE_S64) {
		/* Type must be specified, and of a supported type */
		return -ENOTSUP;
	} else if (parameter_type == ATTR_TYPE_BOOL) {
		set_parameter_struct._set_parameter_p2_choice =
			_set_parameter_p2_bool;
		set_parameter_struct._set_parameter_p2_bool =
			*(bool *)parameter_value;
	} else if (parameter_type == ATTR_TYPE_U8 ||
		   parameter_type == ATTR_TYPE_U16 ||
		   parameter_type == ATTR_TYPE_U32) {
		set_parameter_struct._set_parameter_p2_choice =
			_set_parameter_p2_uint;

		if (parameter_type == ATTR_TYPE_U8) {
			set_parameter_struct._set_parameter_p2_uint =
				(uint32_t)(*(uint8_t *)parameter_value);
		} else if (parameter_type == ATTR_TYPE_U16) {
			set_parameter_struct._set_parameter_p2_uint =
				(uint32_t)(*(uint16_t *)parameter_value);
		} else if (parameter_type == ATTR_TYPE_U32) {
			set_parameter_struct._set_parameter_p2_uint =
				*(uint32_t *)parameter_value;
		}
	} else if (parameter_type == ATTR_TYPE_S8 ||
		   parameter_type == ATTR_TYPE_S16 ||
		   parameter_type == ATTR_TYPE_S32) {
		set_parameter_struct._set_parameter_p2_choice =
			_set_parameter_p2_int;

		if (parameter_type == ATTR_TYPE_S8) {
			set_parameter_struct._set_parameter_p2_int =
				(int32_t)(*(int8_t *)parameter_value);
		} else if (parameter_type == ATTR_TYPE_S16) {
			set_parameter_struct._set_parameter_p2_int =
				(int32_t)(*(int16_t *)parameter_value);
		} else if (parameter_type == ATTR_TYPE_S32) {
			set_parameter_struct._set_parameter_p2_int =
				*(int32_t *)parameter_value;
		}
	} else if (parameter_type == ATTR_TYPE_STRING) {
		set_parameter_struct._set_parameter_p2_choice =
			_set_parameter_p2_tstr;
		set_parameter_struct._set_parameter_p2_tstr.value =
			(uint8_t *)parameter_value;
		set_parameter_struct._set_parameter_p2_tstr.len =
			parameter_size;
	} else if (parameter_type == ATTR_TYPE_BYTE_ARRAY) {
		set_parameter_struct._set_parameter_p2_choice =
			_set_parameter_p2_bstr;
		set_parameter_struct._set_parameter_p2_bstr.value =
			(uint8_t *)parameter_value;
		set_parameter_struct._set_parameter_p2_bstr.len =
			parameter_size;
	}

	if (cbor_encode_set_parameter(&buffer[SMP_PACKET_SIZE],
				      (buffer_size - SMP_PACKET_SIZE),
				      &set_parameter_struct, &rc) == true) {
		rc += create_cbor_header(buffer, MGMT_OP_WRITE, rc,
					 CONFIG_MGMT_GROUP_ID_ATTRIBUTE,
					 sequence,
					 ATTRIBUTE_MGMT_ID_SET_PARAMETER);
	} else {
		rc = -EINVAL;
	}

	return rc;
}

static int32_t create_get_parameter_command(uint8_t *buffer,
					    uint16_t buffer_size,
					    uint8_t sequence,
					    uint16_t parameter_id)
{
	int32_t rc = 0;

	struct get_parameter get_parameter_struct = { ._get_parameter_p1 =
							      parameter_id };

	if (cbor_encode_get_parameter(&buffer[SMP_PACKET_SIZE],
				      (buffer_size - SMP_PACKET_SIZE),
				      &get_parameter_struct, &rc) == true) {
		rc += create_cbor_header(buffer, MGMT_OP_READ, rc,
					 CONFIG_MGMT_GROUP_ID_ATTRIBUTE,
					 sequence,
					 ATTRIBUTE_MGMT_ID_GET_PARAMETER);
	} else {
		rc = -EINVAL;
	}

	return rc;
}

void test_attr_setup(void)
{
	attr_factory_reset();
}

void test_attr_each_setup(void)
{
	clear_smp_buffers();
	smp_dummy_enable();
}

void test_attr_each_end(void)
{
	smp_dummy_disable();

	/* Clean up */
	mcumgr_buf_free(nb);

	/* Increment sequence number for next command/test */
	++sequence;
}

void test_lock_check_not_setup(void)
{
	int rc;
	bool success;
	struct check_lock_status_result check_lock_data;
	uint32_t size_out;

	message_size = create_check_lock_command(tmp_buffer, sizeof(tmp_buffer),
						 sequence);
	zassert_not_equal(message_size, 0,
			  "Message size of check lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating check lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating check lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_equal(message_size, SMP_COMMAND_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected CBOR message size");
	zassert_equal(smp_dummy_get_receive_pos(),
		      SMP_CONSOLE_COMMAND_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_equal(smp_dummy_get_send_pos(),
		      SMP_CONSOLE_RESPONSE_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected received SMP message size");
	zassert_equal(nb->len, SMP_RESPONSE_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_CHECK_LOCK_STATUS_SIZE,
			  CBOR_RESPONSE_CHECK_LOCK_STATUS_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_CHECK_LOCK_STATUS);

	/* Set data to garbage */
	check_lock_data._check_lock_status_result_r = 999;
	check_lock_data._check_lock_status_result_r1 = 1;
	check_lock_data._check_lock_status_result_r2 = 1;

	/* Decode response and ensure it is valid */
	success = cbor_decode_check_lock_status_result(
		&nb->data[SMP_PACKET_SIZE], (nb->len - SMP_PACKET_SIZE),
		&check_lock_data, &size_out);
	zassert_equal(success, true, "Error decoding check lock status result");
	zassert_ok(check_lock_data._check_lock_status_result_r,
		   "Check lock result status is not OK");
	zassert_equal(check_lock_data._check_lock_status_result_r1, 0,
		      "Lock is enabled when it should be disabled");
	zassert_equal(check_lock_data._check_lock_status_result_r2, 0,
		      "Lock is engaged when it should be disengaged");
}

void test_lock_check_setup_not_locked(void)
{
	int rc;
	bool success;
	struct check_lock_status_result check_lock_data;
	uint32_t size_out;

	message_size = create_check_lock_command(tmp_buffer, sizeof(tmp_buffer),
						 sequence);
	zassert_not_equal(message_size, 0,
			  "Message size of check lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating check lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating check lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_equal(message_size, SMP_COMMAND_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected CBOR message size");
	zassert_equal(smp_dummy_get_receive_pos(),
		      SMP_CONSOLE_COMMAND_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_equal(smp_dummy_get_send_pos(),
		      SMP_CONSOLE_RESPONSE_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected received SMP message size");
	zassert_equal(nb->len, SMP_RESPONSE_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_CHECK_LOCK_STATUS_SIZE,
			  CBOR_RESPONSE_CHECK_LOCK_STATUS_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_CHECK_LOCK_STATUS);

	/* Set data to garbage */
	check_lock_data._check_lock_status_result_r = 999;
	check_lock_data._check_lock_status_result_r1 = 1;
	check_lock_data._check_lock_status_result_r2 = 1;

	/* Decode response and ensure it is valid */
	success = cbor_decode_check_lock_status_result(
		&nb->data[SMP_PACKET_SIZE], (nb->len - SMP_PACKET_SIZE),
		&check_lock_data, &size_out);
	zassert_equal(success, true, "Error decoding check lock status result");
	zassert_ok(check_lock_data._check_lock_status_result_r,
		   "Check lock result status is not OK");
	zassert_equal(check_lock_data._check_lock_status_result_r1, 1,
		      "Lock is disabled when it should be enabled");
	zassert_equal(check_lock_data._check_lock_status_result_r2, 0,
		      "Lock is engaged when it should be disengaged");
}

void test_lock_check_setup_locked(void)
{
	int rc;
	bool success;
	struct check_lock_status_result check_lock_data;
	uint32_t size_out;

	message_size = create_check_lock_command(tmp_buffer, sizeof(tmp_buffer),
						 sequence);
	zassert_not_equal(message_size, 0,
			  "Message size of check lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating check lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating check lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_equal(message_size, SMP_COMMAND_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected CBOR message size");
	zassert_equal(smp_dummy_get_receive_pos(),
		      SMP_CONSOLE_COMMAND_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_equal(smp_dummy_get_send_pos(),
		      SMP_CONSOLE_RESPONSE_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected received SMP message size");
	zassert_equal(nb->len, SMP_RESPONSE_CHECK_LOCK_STATUS_SIZE,
		      "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_CHECK_LOCK_STATUS_SIZE,
			  CBOR_RESPONSE_CHECK_LOCK_STATUS_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_CHECK_LOCK_STATUS);

	/* Set data to garbage */
	check_lock_data._check_lock_status_result_r = 999;
	check_lock_data._check_lock_status_result_r1 = 1;
	check_lock_data._check_lock_status_result_r2 = 1;

	/* Decode response and ensure it is valid */
	success = cbor_decode_check_lock_status_result(
		&nb->data[SMP_PACKET_SIZE], (nb->len - SMP_PACKET_SIZE),
		&check_lock_data, &size_out);
	zassert_equal(success, true, "Error decoding check lock status result");
	zassert_ok(check_lock_data._check_lock_status_result_r,
		   "Check lock result status is not OK");
	zassert_equal(check_lock_data._check_lock_status_result_r1, 1,
		      "Lock is disabled when it should be enabled");
	zassert_equal(check_lock_data._check_lock_status_result_r2, 1,
		      "Lock is disengaged when it should be engaged");
}

void test_lock_set_code_a(void)
{
	int rc;
	bool success;
	struct set_lock_code_result set_lock_code_result_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_set_lock_command(tmp_buffer, sizeof(tmp_buffer),
					       sequence, LOCK_CODE_A);
	zassert_not_equal(
		message_size, 0,
		"Message size of set lock code command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set lock code command");
	zassert_not_equal(message_size, 0,
			  "Error generating set lock code command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_LOCK_CODE_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_LOCK_CODE_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_LOCK_CODE_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_LOCK_CODE_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_LOCK_CODE_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_LOCK_CODE_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_LOCK_CODE_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_LOCK_CODE_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_LOCK_CODE_MIN_SIZE,
			  CBOR_RESPONSE_SET_LOCK_CODE_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_LOCK_CODE);

	/* Set data to garbage */
	set_lock_code_result_data._set_lock_code_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_lock_code_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_lock_code_result_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set lock code status result");
	zassert_ok(set_lock_code_result_data._set_lock_code_result_r,
		   "Set lock code result status is not OK");
}

void test_lock_set_code_b(void)
{
	int rc;
	bool success;
	struct set_lock_code_result set_lock_code_result_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_set_lock_command(tmp_buffer, sizeof(tmp_buffer),
					       sequence, LOCK_CODE_B);
	zassert_not_equal(
		message_size, 0,
		"Message size of set lock code command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set lock code command");
	zassert_not_equal(message_size, 0,
			  "Error generating set lock code command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_LOCK_CODE_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_LOCK_CODE_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_LOCK_CODE_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_LOCK_CODE_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_LOCK_CODE_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_LOCK_CODE_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_LOCK_CODE_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_LOCK_CODE_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_LOCK_CODE_MIN_SIZE,
			  CBOR_RESPONSE_SET_LOCK_CODE_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_LOCK_CODE);

	/* Set data to garbage */
	set_lock_code_result_data._set_lock_code_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_lock_code_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_lock_code_result_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set lock code status result");
	zassert_ok(set_lock_code_result_data._set_lock_code_result_r,
		   "Set lock code result status is not OK");
}

void test_lock_set_code_c_failure(void)
{
	int rc;
	bool success;
	struct set_lock_code_result set_lock_code_result_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_set_lock_command(tmp_buffer, sizeof(tmp_buffer),
					       sequence, LOCK_CODE_C);
	zassert_not_equal(
		message_size, 0,
		"Message size of set lock code command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set lock code command");
	zassert_not_equal(message_size, 0,
			  "Error generating set lock code command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_LOCK_CODE_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_LOCK_CODE_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_LOCK_CODE_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_LOCK_CODE_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_LOCK_CODE_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_LOCK_CODE_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_LOCK_CODE_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_LOCK_CODE_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_LOCK_CODE_MIN_SIZE,
			  CBOR_RESPONSE_SET_LOCK_CODE_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_LOCK_CODE);

	/* Set data to garbage */
	set_lock_code_result_data._set_lock_code_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_lock_code_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_lock_code_result_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set lock code status result");
	zassert_equal(set_lock_code_result_data._set_lock_code_result_r,
		      -EACCES,
		      "Set lock code result status is not access denied");
}

void test_lock_enable_lock(void)
{
	int rc;
	bool success;
	struct lock_result lock_result_data;
	uint32_t size_out;
	uint16_t smp_send_pos;

	message_size =
		create_lock_command(tmp_buffer, sizeof(tmp_buffer), sequence);
	zassert_not_equal(
		message_size, 0,
		"Message size of enable lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating enable lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating enable lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_equal(message_size, SMP_COMMAND_LOCK_SIZE,
		      "Unexpected CBOR message size");
	zassert_equal(smp_dummy_get_receive_pos(),
		      SMP_CONSOLE_COMMAND_LOCK_SIZE,
		      "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(smp_send_pos >= SMP_CONSOLE_RESPONSE_LOCK_MIN_SIZE &&
			     smp_send_pos <= SMP_CONSOLE_RESPONSE_LOCK_MAX_SIZE,
		     "Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_LOCK_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_LOCK_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_LOCK_MIN_SIZE,
			  CBOR_RESPONSE_LOCK_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_LOCK);

	/* Set data to garbage */
	lock_result_data._lock_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_lock_result(&nb->data[SMP_PACKET_SIZE],
					  (nb->len - SMP_PACKET_SIZE),
					  &lock_result_data, &size_out);
	zassert_equal(success, true, "Error decoding lock status result");
	zassert_ok(lock_result_data._lock_result_r,
		   "Lock result status is not OK");
}

void test_lock_deactivate_lock_a(void)
{
	int rc;
	bool success;
	struct unlock_result unlock_result_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_unlock_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, LOCK_CODE_A, 0);
	zassert_not_equal(
		message_size, 0,
		"Message size of enable lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating enable lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating enable lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_UNLOCK_MIN_SIZE &&
		      message_size <= SMP_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >= SMP_CONSOLE_COMMAND_UNLOCK_MIN_SIZE &&
		      smp_receive_pos <= SMP_CONSOLE_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(smp_send_pos >= SMP_CONSOLE_RESPONSE_UNLOCK_MIN_SIZE &&
			     smp_send_pos <=
				     SMP_CONSOLE_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_UNLOCK_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_UNLOCK_MIN_SIZE,
			  CBOR_RESPONSE_UNLOCK_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_UNLOCK);

	/* Set data to garbage */
	unlock_result_data._unlock_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_unlock_result(&nb->data[SMP_PACKET_SIZE],
					    (nb->len - SMP_PACKET_SIZE),
					    &unlock_result_data, &size_out);
	zassert_equal(success, true,
		      "Error decoding deactivate lock status result");
	zassert_ok(unlock_result_data._unlock_result_r,
		   "Lock deactivate result status is not OK");
}

void test_lock_deactivate_lock_a_failure(void)
{
	int rc;
	bool success;
	struct unlock_result unlock_result_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_unlock_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, LOCK_CODE_A, 0);
	zassert_not_equal(
		message_size, 0,
		"Message size of enable lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating enable lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating enable lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_UNLOCK_MIN_SIZE &&
		      message_size <= SMP_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >= SMP_CONSOLE_COMMAND_UNLOCK_MIN_SIZE &&
		      smp_receive_pos <= SMP_CONSOLE_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(smp_send_pos >= SMP_CONSOLE_RESPONSE_UNLOCK_MIN_SIZE &&
			     smp_send_pos <=
				     SMP_CONSOLE_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_UNLOCK_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_UNLOCK_MIN_SIZE,
			  CBOR_RESPONSE_UNLOCK_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_UNLOCK);

	/* Set data to garbage */
	unlock_result_data._unlock_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_unlock_result(&nb->data[SMP_PACKET_SIZE],
					    (nb->len - SMP_PACKET_SIZE),
					    &unlock_result_data, &size_out);
	zassert_equal(success, true,
		      "Error decoding deactivate lock status result");
	zassert_equal(
		unlock_result_data._unlock_result_r, -EINVAL,
		"Lock deactivate with invalid code result status is not EINVAL");
}

void test_lock_deactivate_lock_b(void)
{
	int rc;
	bool success;
	struct unlock_result unlock_result_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_unlock_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, LOCK_CODE_B, 0);
	zassert_not_equal(
		message_size, 0,
		"Message size of enable lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating enable lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating enable lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_UNLOCK_MIN_SIZE &&
		      message_size <= SMP_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >= SMP_CONSOLE_COMMAND_UNLOCK_MIN_SIZE &&
		      smp_receive_pos <= SMP_CONSOLE_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(smp_send_pos >= SMP_CONSOLE_RESPONSE_UNLOCK_MIN_SIZE &&
			     smp_send_pos <=
				     SMP_CONSOLE_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_UNLOCK_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_UNLOCK_MIN_SIZE,
			  CBOR_RESPONSE_UNLOCK_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_UNLOCK);

	/* Set data to garbage */
	unlock_result_data._unlock_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_unlock_result(&nb->data[SMP_PACKET_SIZE],
					    (nb->len - SMP_PACKET_SIZE),
					    &unlock_result_data, &size_out);
	zassert_equal(success, true,
		      "Error decoding deactivate lock status result");
	zassert_ok(unlock_result_data._unlock_result_r,
		   "Lock deactivate result status is not OK");
}

void test_lock_deactivate_lock_c_failure(void)
{
	int rc;
	bool success;
	struct unlock_result unlock_result_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_unlock_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, LOCK_CODE_C, 0);
	zassert_not_equal(
		message_size, 0,
		"Message size of enable lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating enable lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating enable lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_UNLOCK_MIN_SIZE &&
		      message_size <= SMP_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >= SMP_CONSOLE_COMMAND_UNLOCK_MIN_SIZE &&
		      smp_receive_pos <= SMP_CONSOLE_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(smp_send_pos >= SMP_CONSOLE_RESPONSE_UNLOCK_MIN_SIZE &&
			     smp_send_pos <=
				     SMP_CONSOLE_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_UNLOCK_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_UNLOCK_MIN_SIZE,
			  CBOR_RESPONSE_UNLOCK_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_UNLOCK);

	/* Set data to garbage */
	unlock_result_data._unlock_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_unlock_result(&nb->data[SMP_PACKET_SIZE],
					    (nb->len - SMP_PACKET_SIZE),
					    &unlock_result_data, &size_out);
	zassert_equal(success, true,
		      "Error decoding deactivate lock status result");
	zassert_equal(
		unlock_result_data._unlock_result_r, -EINVAL,
		"Lock deactivate with invalid code result status is not EINVAL");
}

void test_lock_disable_lock_a(void)
{
	int rc;
	bool success;
	struct unlock_result unlock_result_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_unlock_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, LOCK_CODE_A, 1);
	zassert_not_equal(
		message_size, 0,
		"Message size of enable lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating enable lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating enable lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_UNLOCK_MIN_SIZE &&
		      message_size <= SMP_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >= SMP_CONSOLE_COMMAND_UNLOCK_MIN_SIZE &&
		      smp_receive_pos <= SMP_CONSOLE_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(smp_send_pos >= SMP_CONSOLE_RESPONSE_UNLOCK_MIN_SIZE &&
			     smp_send_pos <=
				     SMP_CONSOLE_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_UNLOCK_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_UNLOCK_MIN_SIZE,
			  CBOR_RESPONSE_UNLOCK_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_UNLOCK);

	/* Set data to garbage */
	unlock_result_data._unlock_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_unlock_result(&nb->data[SMP_PACKET_SIZE],
					    (nb->len - SMP_PACKET_SIZE),
					    &unlock_result_data, &size_out);
	zassert_equal(success, true,
		      "Error decoding lock disable status result");
	zassert_ok(unlock_result_data._unlock_result_r,
		   "Lock disable result status is not OK");
}

void test_lock_disable_lock_a_failure(void)
{
	int rc;
	bool success;
	struct unlock_result unlock_result_data;
	uint32_t size_out;
	int64_t time_start;
	int64_t time_difference;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_unlock_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, LOCK_CODE_A, 1);
	zassert_not_equal(
		message_size, 0,
		"Message size of enable lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating enable lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating enable lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");

	/* Get the time prior to sending the command */
	time_start = k_uptime_get();

	/* Send the command */
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_UNLOCK_MIN_SIZE &&
		      message_size <= SMP_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >= SMP_CONSOLE_COMMAND_UNLOCK_MIN_SIZE &&
		      smp_receive_pos <= SMP_CONSOLE_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get the time difference since before the packet was sent and ensure the valid is close to the expected wait time */
	time_difference = k_uptime_delta(&time_start);
	zassert_within(time_difference, LOCK_INVALID_EXPECTED_WAIT_TIME_MS,
		       LOCK_INVALID_DELTA_TIME_MS,
		       "Invalid delay time when wrong lock code was entered");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(smp_send_pos >= SMP_CONSOLE_RESPONSE_UNLOCK_MIN_SIZE &&
			     smp_send_pos <=
				     SMP_CONSOLE_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_UNLOCK_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_UNLOCK_MIN_SIZE,
			  CBOR_RESPONSE_UNLOCK_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_UNLOCK);

	/* Set data to garbage */
	unlock_result_data._unlock_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_unlock_result(&nb->data[SMP_PACKET_SIZE],
					    (nb->len - SMP_PACKET_SIZE),
					    &unlock_result_data, &size_out);
	zassert_equal(success, true,
		      "Error decoding lock disable status result");
	zassert_equal(
		unlock_result_data._unlock_result_r, -EINVAL,
		"Lock disable with invalid code result status is not EINVAL");
}

void test_lock_disable_lock_b(void)
{
	int rc;
	bool success;
	struct unlock_result unlock_result_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_unlock_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, LOCK_CODE_B, 1);
	zassert_not_equal(
		message_size, 0,
		"Message size of enable lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating enable lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating enable lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_UNLOCK_MIN_SIZE &&
		      message_size <= SMP_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >= SMP_CONSOLE_COMMAND_UNLOCK_MIN_SIZE &&
		      smp_receive_pos <= SMP_CONSOLE_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(smp_send_pos >= SMP_CONSOLE_RESPONSE_UNLOCK_MIN_SIZE &&
			     smp_send_pos <=
				     SMP_CONSOLE_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_UNLOCK_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_UNLOCK_MIN_SIZE,
			  CBOR_RESPONSE_UNLOCK_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_UNLOCK);

	/* Set data to garbage */
	unlock_result_data._unlock_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_unlock_result(&nb->data[SMP_PACKET_SIZE],
					    (nb->len - SMP_PACKET_SIZE),
					    &unlock_result_data, &size_out);
	zassert_equal(success, true,
		      "Error decoding lock disable status result");
	zassert_ok(unlock_result_data._unlock_result_r,
		   "Lock disable result status is not OK");
}

void test_lock_disable_lock_c_failure(void)
{
	int rc;
	bool success;
	struct unlock_result unlock_result_data;
	uint32_t size_out;
	int64_t time_start;
	int64_t time_difference;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_unlock_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, LOCK_CODE_C, 1);
	zassert_not_equal(
		message_size, 0,
		"Message size of enable lock command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating enable lock command");
	zassert_not_equal(message_size, 0,
			  "Error generating enable lock command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");

	/* Get the time prior to sending the command */
	time_start = k_uptime_get();

	/* Send the command */
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_UNLOCK_MIN_SIZE &&
		      message_size <= SMP_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >= SMP_CONSOLE_COMMAND_UNLOCK_MIN_SIZE &&
		      smp_receive_pos <= SMP_CONSOLE_COMMAND_UNLOCK_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get the time difference since before the packet was sent and ensure the valid is close to the expected wait time */
	time_difference = k_uptime_delta(&time_start);
	zassert_within(time_difference, LOCK_INVALID_EXPECTED_WAIT_TIME_MS,
		       LOCK_INVALID_DELTA_TIME_MS,
		       "Invalid delay time when wrong lock code was entered");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(smp_send_pos >= SMP_CONSOLE_RESPONSE_UNLOCK_MIN_SIZE &&
			     smp_send_pos <=
				     SMP_CONSOLE_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_UNLOCK_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_UNLOCK_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_UNLOCK_MIN_SIZE,
			  CBOR_RESPONSE_UNLOCK_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_UNLOCK);

	/* Set data to garbage */
	unlock_result_data._unlock_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_unlock_result(&nb->data[SMP_PACKET_SIZE],
					    (nb->len - SMP_PACKET_SIZE),
					    &unlock_result_data, &size_out);
	zassert_equal(success, true,
		      "Error decoding lock disable status result");
	zassert_equal(
		unlock_result_data._unlock_result_r, -EINVAL,
		"Lock disable with invalid code result status is not EINVAL");
}

void test_lock_check_error_undefined(void)
{
	int rc;
	bool success;
	struct get_unlock_error_code_result unlock_error_data;
	uint32_t size_out;

	message_size = create_get_unlock_error_code_command(
		tmp_buffer, sizeof(tmp_buffer), sequence);
	zassert_not_equal(
		message_size, 0,
		"Message size of check lock error command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating check lock error command");
	zassert_not_equal(message_size, 0,
			  "Error generating check lock error command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_equal(message_size, SMP_COMMAND_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected CBOR message size");
	zassert_equal(smp_dummy_get_receive_pos(),
		      SMP_CONSOLE_COMMAND_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_equal(smp_dummy_get_send_pos(),
		      SMP_CONSOLE_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected received SMP message size");
	zassert_equal(nb->len, SMP_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
			  CBOR_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_UNLOCK_ERROR_CODE);

	/* Set data to garbage */
	unlock_error_data._get_unlock_error_code_result_r = 999;
	unlock_error_data._get_unlock_error_code_result_r1 = 1;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_unlock_error_code_result(
		&nb->data[SMP_PACKET_SIZE], (nb->len - SMP_PACKET_SIZE),
		&unlock_error_data, &size_out);
	zassert_equal(success, true,
		      "Error decoding unlock error code result status");
	zassert_ok(unlock_error_data._get_unlock_error_code_result_r,
		   "Check error code status is not OK");
	zassert_equal(unlock_error_data._get_unlock_error_code_result_r1,
		      SETTINGS_PASSCODE_STATUS_UNDEFINED,
		      "Lock error is not undefined when it should be");
}

void test_lock_check_error_valid(void)
{
	int rc;
	bool success;
	struct get_unlock_error_code_result unlock_error_data;
	uint32_t size_out;

	message_size = create_get_unlock_error_code_command(
		tmp_buffer, sizeof(tmp_buffer), sequence);
	zassert_not_equal(
		message_size, 0,
		"Message size of check lock error command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating check lock error command");
	zassert_not_equal(message_size, 0,
			  "Error generating check lock error command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_equal(message_size, SMP_COMMAND_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected CBOR message size");
	zassert_equal(smp_dummy_get_receive_pos(),
		      SMP_CONSOLE_COMMAND_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_equal(smp_dummy_get_send_pos(),
		      SMP_CONSOLE_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected received SMP message size");
	zassert_equal(nb->len, SMP_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
			  CBOR_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_UNLOCK_ERROR_CODE);

	/* Set data to garbage */
	unlock_error_data._get_unlock_error_code_result_r = 999;
	unlock_error_data._get_unlock_error_code_result_r1 = 1;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_unlock_error_code_result(
		&nb->data[SMP_PACKET_SIZE], (nb->len - SMP_PACKET_SIZE),
		&unlock_error_data, &size_out);
	zassert_equal(success, true,
		      "Error decoding unlock error code result status");
	zassert_ok(unlock_error_data._get_unlock_error_code_result_r,
		   "Check error code status is not OK");
	zassert_equal(unlock_error_data._get_unlock_error_code_result_r1,
		      SETTINGS_PASSCODE_STATUS_VALID_CODE,
		      "Lock error is not valid when it should be");
}

void test_lock_check_error_invalid(void)
{
	int rc;
	bool success;
	struct get_unlock_error_code_result unlock_error_data;
	uint32_t size_out;

	message_size = create_get_unlock_error_code_command(
		tmp_buffer, sizeof(tmp_buffer), sequence);
	zassert_not_equal(
		message_size, 0,
		"Message size of check lock error command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating check lock error command");
	zassert_not_equal(message_size, 0,
			  "Error generating check lock error command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_equal(message_size, SMP_COMMAND_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected CBOR message size");
	zassert_equal(smp_dummy_get_receive_pos(),
		      SMP_CONSOLE_COMMAND_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_equal(smp_dummy_get_send_pos(),
		      SMP_CONSOLE_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected received SMP message size");
	zassert_equal(nb->len, SMP_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
		      "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
			  CBOR_RESPONSE_GET_UNLOCK_ERROR_CODE_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_UNLOCK_ERROR_CODE);

	/* Set data to garbage */
	unlock_error_data._get_unlock_error_code_result_r = 999;
	unlock_error_data._get_unlock_error_code_result_r1 = 1;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_unlock_error_code_result(
		&nb->data[SMP_PACKET_SIZE], (nb->len - SMP_PACKET_SIZE),
		&unlock_error_data, &size_out);
	zassert_equal(success, true,
		      "Error decoding unlock error code result status");
	zassert_ok(unlock_error_data._get_unlock_error_code_result_r,
		   "Check error code status is not OK");
	zassert_equal(unlock_error_data._get_unlock_error_code_result_r1,
		      SETTINGS_PASSCODE_STATUS_INVALID_CODE,
		      "Lock error is not invalid when it should be");
}

void test_attr_defaults(void)
{
	uint8_t value;
	uint32_t code;

	value = (uint8_t)attr_get_uint32(ATTR_ID_lock, 99);
	zassert_equal(value, 0, "Invalid default value for lock");
	code = attr_get_uint32(ATTR_ID_settings_passcode, 99);
	zassert_equal(code, DEFAULT_LOCK_CODE,
		      "Invalid default value for settings passcode");
	value = (uint8_t)attr_get_uint32(ATTR_ID_settings_passcode_status, 99);
	zassert_equal(value, SETTINGS_PASSCODE_STATUS_UNDEFINED,
		      "Invalid default value for settings passcode status");
	value = (uint8_t)attr_get_uint32(ATTR_ID_lock_status, 99);
	zassert_equal(value, LOCK_STATUS_NOT_SETUP,
		      "Invalid default value for lock status");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_a, 99);
	zassert_equal(value, 1, "Invalid default value for test_a");
}

void test_attr_show_with_lock_engaged(void)
{
	int rc;

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(NULL, ATTR_ID_test_u);
	enable_logging = false;
	zassert_equal(rc, -EPERM, "Failed to show obscured test_u value");
	zassert_not_null(strstr(mock_buffer, "[26] test_u " ASTERISK_RESPONSE),
			 "Expected obscured value for test_u not found");

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(NULL, ATTR_ID_test_v);
	enable_logging = false;
	zassert_equal(rc, -EACCES, "Failed to ensure test_v value is hidden");
	zassert_is_null(strstr(mock_buffer, "test_v"),
			"Unexpected value for test_x found");
	zassert_equal(strlen(mock_buffer), 0,
		      "Unexpected response for showing hidden attribute");

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(NULL, ATTR_ID_test_w);
	enable_logging = false;
	zassert_equal(rc, -EPERM, "Failed to show obscured test_w value");
	zassert_not_null(strstr(mock_buffer, "[28] test_w " ASTERISK_RESPONSE),
			 "Expected obscured value for test_w not found");

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(NULL, ATTR_ID_test_x);
	enable_logging = false;
	zassert_equal(rc, -EACCES, "Failed to ensure test_x value is hidden");
	zassert_is_null(strstr(mock_buffer, "test_x"),
			"Unexpected value for test_x found");
	zassert_equal(strlen(mock_buffer), 0,
		      "Unexpected response for showing hidden attribute");
}

void test_lock_set_uint8(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	uint8_t test_value = ATTR_TEST_VAL_U8_VALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_uint8,
					     ATTR_TYPE_U8, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_uint8,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(set_parameter_data._set_parameter_result_r, 0,
		      "Set parameter result is not correct");
}

void test_lock_set_uint8_fail(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	uint8_t test_value = ATTR_TEST_VAL_U8_INVALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_uint8,
					     ATTR_TYPE_U8, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding unlock error code result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_uint8,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(
		set_parameter_data._set_parameter_result_r, -EPERM,
		"Set parameter result attribute should fail due to active lock");
}

void test_lock_set_int8(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	int8_t test_value = ATTR_TEST_VAL_S8_VALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int8,
					     ATTR_TYPE_S8, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_int8,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(set_parameter_data._set_parameter_result_r, 0,
		      "Set parameter result is not correct");
}

void test_lock_set_int8_fail(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	int8_t test_value = ATTR_TEST_VAL_S8_INVALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int8,
					     ATTR_TYPE_S8, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_int8,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(
		set_parameter_data._set_parameter_result_r, -EPERM,
		"Set parameter result attribute should fail due to active lock");
}

void test_lock_set_uint16(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	uint16_t test_value = ATTR_TEST_VAL_U16_VALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_uint16,
					     ATTR_TYPE_U16, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_uint16,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(set_parameter_data._set_parameter_result_r, 0,
		      "Set parameter result is not correct");
}

void test_lock_set_uint16_fail(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	uint16_t test_value = ATTR_TEST_VAL_U16_INVALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_uint16,
					     ATTR_TYPE_U16, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding unlock error code result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_uint16,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(
		set_parameter_data._set_parameter_result_r, -EPERM,
		"Set parameter result attribute should fail due to active lock");
}

void test_lock_set_int16(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	int16_t test_value = ATTR_TEST_VAL_S16_VALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int16,
					     ATTR_TYPE_S16, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_int16,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(set_parameter_data._set_parameter_result_r, 0,
		      "Set parameter result is not correct");
}

void test_lock_set_int16_fail(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	int16_t test_value = ATTR_TEST_VAL_S16_INVALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int16,
					     ATTR_TYPE_S16, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_int16,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(
		set_parameter_data._set_parameter_result_r, -EPERM,
		"Set parameter result attribute should fail due to active lock");
}

void test_lock_set_uint32(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	uint32_t test_value = ATTR_TEST_VAL_U32_VALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_uint32,
					     ATTR_TYPE_U32, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_uint32,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(set_parameter_data._set_parameter_result_r, 0,
		      "Set parameter result is not correct");
}

void test_lock_set_uint32_fail(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	uint32_t test_value = ATTR_TEST_VAL_U32_INVALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_uint32,
					     ATTR_TYPE_U32, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding unlock error code result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_uint32,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(
		set_parameter_data._set_parameter_result_r, -EPERM,
		"Set parameter result attribute should fail due to active lock");
}

void test_lock_set_int32(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	int32_t test_value = ATTR_TEST_VAL_S32_VALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int32,
					     ATTR_TYPE_S32, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_int32,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(set_parameter_data._set_parameter_result_r, 0,
		      "Set parameter result is not correct");
}

void test_lock_set_int32_fail(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	int32_t test_value = ATTR_TEST_VAL_S32_INVALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int32,
					     ATTR_TYPE_S32, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_int32,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(
		set_parameter_data._set_parameter_result_r, -EPERM,
		"Set parameter result attribute should fail due to active lock");
}

void test_lock_set_bool(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	bool test_value = true;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_bool,
					     ATTR_TYPE_BOOL, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_bool,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(set_parameter_data._set_parameter_result_r, 0,
		      "Set parameter result is not correct");
}

void test_lock_set_bool_fail(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	bool test_value = true;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_bool,
					     ATTR_TYPE_BOOL, &test_value,
					     sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding unlock error code result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_bool,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(
		set_parameter_data._set_parameter_result_r, -EPERM,
		"Set parameter result attribute should fail due to active lock");
}

void test_lock_set_string(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	uint8_t test_value[] = ATTR_TEST_VAL_STRING_VALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_string,
					     ATTR_TYPE_STRING, test_value,
					     strlen(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_string,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(set_parameter_data._set_parameter_result_r, 0,
		      "Set parameter result is not correct");
}

void test_lock_set_string_fail(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	uint8_t test_value[] = ATTR_TEST_VAL_STRING_INVALID;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_set_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_string,
					     ATTR_TYPE_STRING, test_value,
					     strlen(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding unlock error code result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_string,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(
		set_parameter_data._set_parameter_result_r, -EPERM,
		"Set parameter result attribute should fail due to active lock");
}

void test_lock_set_byte_array(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	uint8_t test_value[] = { ATTR_TEST_VAL_BYTE_ARRAY_VALID };
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_set_parameter_command(
		tmp_buffer, sizeof(tmp_buffer), sequence,
		ATTR_ID_test_lock_byte_array, ATTR_TYPE_BYTE_ARRAY, test_value,
		sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding set parameter result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_byte_array,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(set_parameter_data._set_parameter_result_r, 0,
		      "Set parameter result is not correct");
}

void test_lock_set_byte_array_fail(void)
{
	int rc;
	bool success;
	struct set_parameter_result set_parameter_data;
	uint32_t size_out;
	uint8_t test_value[] = { ATTR_TEST_VAL_BYTE_ARRAY_INVALID };
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size = create_set_parameter_command(
		tmp_buffer, sizeof(tmp_buffer), sequence,
		ATTR_ID_test_lock_byte_array, ATTR_TYPE_BYTE_ARRAY, test_value,
		sizeof(test_value));
	zassert_not_equal(
		message_size, 0,
		"Message size of set parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating set parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating set parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_SET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");
	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_SET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_SET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_SET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_WRITE_RSP,
			  CBOR_RESPONSE_SET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_SET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_SET_PARAMETER);

	/* Set data to garbage */
	set_parameter_data._set_parameter_result_id = 999;
	set_parameter_data._set_parameter_result_r = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_set_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &set_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding unlock error code result status");
	zassert_equal(set_parameter_data._set_parameter_result_id,
		      ATTR_ID_test_lock_byte_array,
		      "Set parameter result attribute ID is not correct");
	zassert_equal(
		set_parameter_data._set_parameter_result_r, -EPERM,
		"Set parameter result attribute should fail due to active lock");
}

void test_lock_set_test_value_defaults(void)
{
	int rc;
	rc = attr_default(ATTR_ID_test_lock_uint8);
	zassert_ok(rc, "Error restoring default test attribute value");
	rc = attr_default(ATTR_ID_test_lock_int8);
	zassert_ok(rc, "Error restoring default test attribute value");
	rc = attr_default(ATTR_ID_test_lock_uint16);
	zassert_ok(rc, "Error restoring default test attribute value");
	rc = attr_default(ATTR_ID_test_lock_int16);
	zassert_ok(rc, "Error restoring default test attribute value");
	rc = attr_default(ATTR_ID_test_lock_uint32);
	zassert_ok(rc, "Error restoring default test attribute value");
	rc = attr_default(ATTR_ID_test_lock_int32);
	zassert_ok(rc, "Error restoring default test attribute value");
	rc = attr_default(ATTR_ID_test_lock_bool);
	zassert_ok(rc, "Error restoring default test attribute value");
	rc = attr_default(ATTR_ID_test_lock_string);
	zassert_ok(rc, "Error restoring default test attribute value");
	rc = attr_default(ATTR_ID_test_lock_byte_array);
	zassert_ok(rc, "Error restoring default test attribute value");
}

void test_lock_get_uint8(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_uint8);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_uint = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_uint8,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(uint8_t),
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_uint,
		      "Get parameter result data type is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_uint,
		      ATTR_TEST_VAL_U8_VALID,
		      "Get parameter result data value is not correct");
}

void test_lock_get_uint8_fail(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_uint8);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_uint = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_uint8,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(uint8_t),
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_uint,
		      "Get parameter result data type is not correct");
	zassert_not_equal(get_parameter_data._get_parameter_result_r1_uint,
			  ATTR_TEST_VAL_U8_INVALID,
			  "Get parameter result data value is not correct");
}

void test_lock_get_int8(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int8);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_int = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_int8,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(int8_t),
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_int,
		      "Get parameter result data type is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_int,
		      ATTR_TEST_VAL_S8_VALID,
		      "Get parameter result data value is not correct");
}

void test_lock_get_int8_fail(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int8);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_int = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_int8,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(int8_t),
		      "Get parameter result attribute size is not correct");
	zassert_true((get_parameter_data._get_parameter_result_r1_choice ==
			      _get_parameter_result_r1_int ||
		      get_parameter_data._get_parameter_result_r1_choice ==
			      _get_parameter_result_r1_uint),
		     "Get parameter result data type is not correct");
	zassert_not_equal(get_parameter_data._get_parameter_result_r1_int,
			  ATTR_TEST_VAL_S8_INVALID,
			  "Get parameter result data value is not correct");
}

void test_lock_get_uint16(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence,
					     ATTR_ID_test_lock_uint16);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_uint = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_uint16,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(uint16_t),
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_uint,
		      "Get parameter result data type is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_uint,
		      ATTR_TEST_VAL_U16_VALID,
		      "Get parameter result data value is not correct");
}

void test_lock_get_uint16_fail(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence,
					     ATTR_ID_test_lock_uint16);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_uint = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_uint16,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(uint16_t),
		      "Get parameter result attribute size is not correct");
	zassert_true((get_parameter_data._get_parameter_result_r1_choice ==
			      _get_parameter_result_r1_int ||
		      get_parameter_data._get_parameter_result_r1_choice ==
			      _get_parameter_result_r1_uint),
		     "Get parameter result data type is not correct");
	zassert_not_equal(get_parameter_data._get_parameter_result_r1_uint,
			  ATTR_TEST_VAL_U16_INVALID,
			  "Get parameter result data value is not correct");
}

void test_lock_get_int16(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int16);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_int = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_int16,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(int16_t),
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_int,
		      "Get parameter result data type is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_int,
		      ATTR_TEST_VAL_S16_VALID,
		      "Get parameter result data value is not correct");
}

void test_lock_get_int16_fail(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int16);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_int = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_int16,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(int16_t),
		      "Get parameter result attribute size is not correct");
	zassert_true((get_parameter_data._get_parameter_result_r1_choice ==
			      _get_parameter_result_r1_int ||
		      get_parameter_data._get_parameter_result_r1_choice ==
			      _get_parameter_result_r1_uint),
		     "Get parameter result data type is not correct");
	zassert_not_equal(get_parameter_data._get_parameter_result_r1_int,
			  ATTR_TEST_VAL_S16_INVALID,
			  "Get parameter result data value is not correct");
}

void test_lock_get_uint32(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence,
					     ATTR_ID_test_lock_uint32);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_uint = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_uint32,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(uint32_t),
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_uint,
		      "Get parameter result data type is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_uint,
		      ATTR_TEST_VAL_U32_VALID,
		      "Get parameter result data value is not correct");
}

void test_lock_get_uint32_fail(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence,
					     ATTR_ID_test_lock_uint32);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_uint = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_uint32,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(uint32_t),
		      "Get parameter result attribute size is not correct");
	zassert_true((get_parameter_data._get_parameter_result_r1_choice ==
			      _get_parameter_result_r1_int ||
		      get_parameter_data._get_parameter_result_r1_choice ==
			      _get_parameter_result_r1_uint),
		     "Get parameter result data type is not correct");
	zassert_not_equal(get_parameter_data._get_parameter_result_r1_uint,
			  ATTR_TEST_VAL_U32_INVALID,
			  "Get parameter result data value is not correct");
}

void test_lock_get_int32(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int32);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_int = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_int32,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(int32_t),
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_int,
		      "Get parameter result data type is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_int,
		      ATTR_TEST_VAL_S32_VALID,
		      "Get parameter result data value is not correct");
}

void test_lock_get_int32_fail(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_int32);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_int = 999;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_int32,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      sizeof(int32_t),
		      "Get parameter result attribute size is not correct");
	zassert_true((get_parameter_data._get_parameter_result_r1_choice ==
			      _get_parameter_result_r1_int ||
		      get_parameter_data._get_parameter_result_r1_choice ==
			      _get_parameter_result_r1_uint),
		     "Get parameter result data type is not correct");
	zassert_not_equal(get_parameter_data._get_parameter_result_r1_int,
			  ATTR_TEST_VAL_S32_INVALID,
			  "Get parameter result data value is not correct");
}

void test_lock_get_bool(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence, ATTR_ID_test_lock_bool);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;
	get_parameter_data._get_parameter_result_r1_bool = false;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_bool,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r, sizeof(bool),
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_bool,
		      "Get parameter result data type is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_bool, true,
		      "Get parameter result data value is not correct");
}

void test_lock_get_string(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence,
					     ATTR_ID_test_lock_string);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_string,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      ATTR_TEST_LOCK_STRING_MAX_STR_SIZE,
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_tstr,
		      "Get parameter result data type is not correct");
	zassert_equal(
		get_parameter_data._get_parameter_result_r1_tstr.len,
		strlen(ATTR_TEST_VAL_STRING_VALID),
		"Get parameter result attribute string size is not correct");
	zassert_mem_equal(
		get_parameter_data._get_parameter_result_r1_tstr.value,
		ATTR_TEST_VAL_STRING_VALID, strlen(ATTR_TEST_VAL_STRING_VALID),
		"Get parameter result attribute string is not correct");
}

void test_lock_get_string_fail(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence,
					     ATTR_ID_test_lock_string);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_string,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      ATTR_TEST_LOCK_STRING_MAX_STR_SIZE,
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_tstr,
		      "Get parameter result data type is not correct");
	zassert_not_equal(
		get_parameter_data._get_parameter_result_r1_tstr.len,
		strlen(ATTR_TEST_VAL_STRING_INVALID),
		"Get parameter result attribute string size is not correct");
	zassert_not_equal(
		memcmp(get_parameter_data._get_parameter_result_r1_tstr.value,
		       ATTR_TEST_VAL_STRING_INVALID,
		       strlen(ATTR_TEST_VAL_STRING_INVALID)),
		0, "Get parameter result attribute string is not correct");
}

void test_lock_get_byte_array(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint8_t cmp_buf[] = { ATTR_TEST_VAL_BYTE_ARRAY_VALID };
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence,
					     ATTR_ID_test_lock_byte_array);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_byte_array,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      ATTR_TEST_LOCK_BYTE_ARRAY_SIZE,
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_bstr,
		      "Get parameter result data type is not correct");
	zassert_equal(
		get_parameter_data._get_parameter_result_r1_bstr.len,
		sizeof(cmp_buf),
		"Get parameter result attribute byte array size is not correct");
	zassert_mem_equal(
		get_parameter_data._get_parameter_result_r1_bstr.value, cmp_buf,
		sizeof(cmp_buf),
		"Get parameter result attribute byte array is not correct");
}

void test_lock_get_byte_array_fail(void)
{
	int rc;
	bool success;
	struct get_parameter_result get_parameter_data;
	uint32_t size_out;
	uint8_t cmp_buf[] = { ATTR_TEST_VAL_BYTE_ARRAY_INVALID };
	uint16_t smp_receive_pos;
	uint16_t smp_send_pos;

	message_size =
		create_get_parameter_command(tmp_buffer, sizeof(tmp_buffer),
					     sequence,
					     ATTR_ID_test_lock_byte_array);
	zassert_not_equal(
		message_size, 0,
		"Message size of get parameter command should not be 0");
	zassert_equal((message_size < 0), 0,
		      "Error generating get parameter command");
	zassert_not_equal(message_size, 0,
			  "Error generating get parameter command");
	rc = smp_dummy_tx_pkt(tmp_buffer, message_size);
	smp_receive_pos = smp_dummy_get_receive_pos();
	zassert_equal(rc, 0, "Error generating SMP command");
	smp_dummy_add_data();
	zassert_true((message_size >= SMP_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      message_size <= SMP_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected CBOR message size");
	zassert_true((smp_receive_pos >=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MIN_SIZE &&
		      smp_receive_pos <=
			      SMP_CONSOLE_COMMAND_GET_PARAMETER_MAX_SIZE),
		     "Unexpected SMP message size");

	/* Wait for the SMP command to be processed and a response to be received */
	zassert_ok(smp_dummy_wait_for_data(SMP_DATA_READY_WAIT_TIME_S),
		   "SMP data ready timeout");

	/* Get raw SMP data as a net buffer and check the header is valid */
	nb = smp_dummy_get_outgoing();
	smp_send_pos = smp_dummy_get_send_pos();
	zassert_not_null(nb, "Net buffer with SMP response should not be null");

	zassert_true(
		smp_send_pos >= SMP_CONSOLE_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			smp_send_pos <=
				SMP_CONSOLE_RESPONSE_GET_PARAMETER_MAX_SIZE,
		"Unexpected received SMP message size");
	zassert_true(nb->len >= SMP_RESPONSE_GET_PARAMETER_MIN_SIZE &&
			     nb->len <= SMP_RESPONSE_GET_PARAMETER_MAX_SIZE,
		     "Unexpected received SMP (base64-decoded) message size");
	check_cbor_header(nb->data, MGMT_OP_READ_RSP,
			  CBOR_RESPONSE_GET_PARAMETER_MIN_SIZE,
			  CBOR_RESPONSE_GET_PARAMETER_MAX_SIZE,
			  CONFIG_MGMT_GROUP_ID_ATTRIBUTE, sequence,
			  ATTRIBUTE_MGMT_ID_GET_PARAMETER);

	/* Set data to garbage */
	get_parameter_data._get_parameter_result_id = 999;
	get_parameter_data._get_parameter_result_r = 999;
	get_parameter_data._get_parameter_result_r1_choice = 10;

	/* Decode response and ensure it is valid */
	success = cbor_decode_get_parameter_result(&nb->data[SMP_PACKET_SIZE],
						   (nb->len - SMP_PACKET_SIZE),
						   &get_parameter_data,
						   &size_out);
	zassert_equal(success, true,
		      "Error decoding get parameter result status");
	zassert_equal(get_parameter_data._get_parameter_result_id,
		      ATTR_ID_test_lock_byte_array,
		      "Get parameter result attribute ID is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r,
		      ATTR_TEST_LOCK_BYTE_ARRAY_SIZE,
		      "Get parameter result attribute size is not correct");
	zassert_equal(get_parameter_data._get_parameter_result_r1_choice,
		      _get_parameter_result_r1_bstr,
		      "Get parameter result data type is not correct");
	zassert_equal(
		get_parameter_data._get_parameter_result_r1_bstr.len,
		sizeof(cmp_buf),
		"Get parameter result attribute byte array size is not correct");
	zassert_not_equal(
		memcmp(get_parameter_data._get_parameter_result_r1_bstr.value,
		       cmp_buf, sizeof(cmp_buf)),
		0, "Get parameter result attribute byte array is not correct");
}

/******************************************************************************/
/* Local Function Definitions                                                 */
/******************************************************************************/
static void reset_mock_buffer(void)
{
	mock_len = 0U;
	memset(mock_buffer, 0, sizeof(mock_buffer));
}

static int mock_output_func(uint8_t *buf, size_t size, void *ctx)
{
	memcpy(&mock_buffer[mock_len], buf, size);
	mock_len += size;

	return size;
}

static void sync_string(const struct log_backend *const backend,
			struct log_msg_ids src_level, uint32_t timestamp,
			const char *fmt, va_list ap)
{
	if (enable_logging == true) {
		log_backend_std_sync_string(&log_output, 0, src_level,
					    timestamp, fmt, ap);
	}
}
