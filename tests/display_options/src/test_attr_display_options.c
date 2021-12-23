/**
 * @file test_attr_display_options.c
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
#include <file_system_utilities.h>
#include <attr.h>
#include <attr_table.h>
#include "test_attr.h"

/******************************************************************************/
/* Local Constant, Macro and Type Definitions                                 */
/******************************************************************************/
#define DEFAULT_LOCK_CODE 123456
#define TEST_LOCK_CODE 421731
#define ASTERISK_RESPONSE "******"
#define MIN_DUMP_FILE_SIZE 64
#define DATA_BUFFER_SIZE 384

LOG_MODULE_REGISTER(attr_log);

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

const struct log_backend_api log_backend_test_api = {
        .process = NULL,
        .put = NULL,
        .put_sync_string = sync_string,
        .put_sync_hexdump = NULL,
        .panic = NULL,
        .init = NULL,
        .dropped = NULL
};

LOG_OUTPUT_DEFINE(log_output, mock_output_func,
		  log_output_buf, sizeof(log_output_buf));

LOG_BACKEND_DEFINE(log_backend_test, log_backend_test_api, true);

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
void test_attr_setup(void)
{
	attr_factory_reset();
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
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_b, 99);
	zassert_equal(value, 2, "Invalid default value for test_b");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_c, 99);
	zassert_equal(value, 3, "Invalid default value for test_c");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_d, 99);
	zassert_equal(value, 4, "Invalid default value for test_d");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_e, 99);
	zassert_equal(value, 5, "Invalid default value for test_e");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_f, 99);
	zassert_equal(value, 6, "Invalid default value for test_f");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_g, 99);
	zassert_equal(value, 7, "Invalid default value for test_g");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_h, 99);
	zassert_equal(value, 8, "Invalid default value for test_h");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_i, 99);
	zassert_equal(value, 9, "Invalid default value for test_i");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_j, 99);
	zassert_equal(value, 10, "Invalid default value for test_j");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_k, 99);
	zassert_equal(value, 11, "Invalid default value for test_k");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_l, 99);
	zassert_equal(value, 12, "Invalid default value for test_l");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_m, 99);
	zassert_equal(value, 13, "Invalid default value for test_m");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_n, 99);
	zassert_equal(value, 14, "Invalid default value for test_n");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_o, 99);
	zassert_equal(value, 15, "Invalid default value for test_o");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_p, 99);
	zassert_equal(value, 16, "Invalid default value for test_p");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_q, 99);
	zassert_equal(value, 17, "Invalid default value for test_q");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_r, 99);
	zassert_equal(value, 18, "Invalid default value for test_r");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_s, 99);
	zassert_equal(value, 19, "Invalid default value for test_s");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_t, 99);
	zassert_equal(value, 20, "Invalid default value for test_t");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_u, 99);
	zassert_equal(value, 21, "Invalid default value for test_u");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_v, 99);
	zassert_equal(value, 22, "Invalid default value for test_v");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_w, 99);
	zassert_equal(value, 23, "Invalid default value for test_w");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_x, 99);
	zassert_equal(value, 24, "Invalid default value for test_x");
}

void test_attr_lock_enable(void)
{
	int rc;
	rc = attr_set_uint32(ATTR_ID_settings_passcode, TEST_LOCK_CODE);
	zassert_ok(rc, "Failed to set lock code");
	rc = attr_set_uint32(ATTR_ID_lock, true);
	zassert_ok(rc, "Failed to enable lock");
}

void test_attr_with_lock_setup_not_engaged(void)
{
	int rc;
	uint8_t value = 0xff;

	rc = attr_get(ATTR_ID_test_a, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_a should be obscured");
	zassert_not_equal(value, 1, "test_a value should not have been returned");
	rc = attr_get(ATTR_ID_test_b, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_b should be hidden");
	zassert_not_equal(value, 2, "test_b value should not have been returned");
	rc = attr_get(ATTR_ID_test_c, &value, sizeof(value));
	zassert_equal(value, 3, "test_c should be visible");
	zassert_equal(rc, sizeof(value), "test_c value should have been returned");
	rc = attr_get(ATTR_ID_test_d, &value, sizeof(value));
	zassert_equal(value, 4, "test_d should be visible");
	zassert_equal(rc, sizeof(value), "test_d value should have been returned");
	rc = attr_get(ATTR_ID_test_e, &value, sizeof(value));
	zassert_equal(value, 5, "test_e should be visible");
	zassert_equal(rc, sizeof(value), "test_e value should have been returned");
	rc = attr_get(ATTR_ID_test_f, &value, sizeof(value));
	zassert_equal(value, 6, "test_f should be visible");
	zassert_equal(rc, sizeof(value), "test_f value should have been returned");
	rc = attr_get(ATTR_ID_test_g, &value, sizeof(value));
	zassert_equal(value, 7, "test_g should be visible");
	zassert_equal(rc, sizeof(value), "test_g value should have been returned");
	rc = attr_get(ATTR_ID_test_h, &value, sizeof(value));
	zassert_equal(value, 8, "test_h should be visible");
	zassert_equal(rc, sizeof(value), "test_h value should have been returned");
	rc = attr_get(ATTR_ID_test_i, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_i should be obscured");
	zassert_not_equal(value, 9, "test_i value should not have been returned");
	rc = attr_get(ATTR_ID_test_j, &value, sizeof(value));
	zassert_equal(value, 10, "test_j should be visible");
	zassert_equal(rc, sizeof(value), "test_j value should have been returned");
	rc = attr_get(ATTR_ID_test_k, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_k should be obscured");
	zassert_not_equal(value, 11, "test_k value should not have been returned");
	rc = attr_get(ATTR_ID_test_l, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_l should be hidden");
	zassert_not_equal(value, 12, "test_l value should not have been returned");
	rc = attr_get(ATTR_ID_test_m, &value, sizeof(value));
	zassert_equal(value, 13, "test_m should be visible");
	zassert_equal(rc, sizeof(value), "test_m value should have been returned");
	rc = attr_get(ATTR_ID_test_n, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_n should be hidden");
	zassert_not_equal(value, 14, "test_n value should not have been returned");
	rc = attr_get(ATTR_ID_test_o, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_o should be obscured");
	zassert_not_equal(value, 15, "test_o value should not have been returned");
	rc = attr_get(ATTR_ID_test_p, &value, sizeof(value));
	zassert_equal(value, 16, "test_p should be visible");
	zassert_equal(rc, sizeof(value), "test_p value should have been returned");
	rc = attr_get(ATTR_ID_test_q, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_q should be obscured");
	zassert_not_equal(value, 17, "test_q value should not have been returned");
	rc = attr_get(ATTR_ID_test_r, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_r should be hidden");
	zassert_not_equal(value, 18, "test_r value should not have been returned");
	rc = attr_get(ATTR_ID_test_s, &value, sizeof(value));
	zassert_equal(value, 19, "test_s should be visible");
	zassert_equal(rc, sizeof(value), "test_s value should have been returned");
	rc = attr_get(ATTR_ID_test_t, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_t should be hidden");
	zassert_not_equal(value, 20, "test_t value should not have been returned");
	rc = attr_get(ATTR_ID_test_u, &value, sizeof(value));
	zassert_equal(value, 21, "test_u should be visible");
	zassert_equal(rc, sizeof(value), "test_u value should have been returned");
	rc = attr_get(ATTR_ID_test_v, &value, sizeof(value));
	zassert_equal(value, 22, "test_v should be visible");
	zassert_equal(rc, sizeof(value), "test_v value should have been returned");
	rc = attr_get(ATTR_ID_test_w, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_w should be obscured");
	zassert_not_equal(value, 23, "test_w value should not have been returned");
	rc = attr_get(ATTR_ID_test_x, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_x should be hidden");
	zassert_not_equal(value, 24, "test_x value should not have been returned");
}

void test_attr_with_lock_engaged(void)
{
	int rc;
	uint8_t value = 0xff;

	rc = attr_get(ATTR_ID_test_a, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_a should be obscured");
	zassert_not_equal(value, 1, "test_a value should not have been returned");
	rc = attr_get(ATTR_ID_test_b, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_b should be hidden");
	zassert_not_equal(value, 2, "test_b value should not have been returned");
	rc = attr_get(ATTR_ID_test_c, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_c should be obscured");
	zassert_not_equal(value, 3, "test_c value should not have been returned");
	rc = attr_get(ATTR_ID_test_d, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_d should be hidden");
	zassert_not_equal(value, 4, "test_d value should not have been returned");
	rc = attr_get(ATTR_ID_test_e, &value, sizeof(value));
	zassert_equal(value, 5, "test_e should be visible");
	zassert_equal(rc, sizeof(value), "test_e value should have been returned");
	rc = attr_get(ATTR_ID_test_f, &value, sizeof(value));
	zassert_equal(value, 6, "test_f should be visible");
	zassert_equal(rc, sizeof(value), "test_f value should have been returned");
	rc = attr_get(ATTR_ID_test_g, &value, sizeof(value));
	zassert_equal(value, 7, "test_g should be visible");
	zassert_equal(rc, sizeof(value), "test_g value should have been returned");
	rc = attr_get(ATTR_ID_test_h, &value, sizeof(value));
	zassert_equal(value, 8, "test_h should be visible");
	zassert_equal(rc, sizeof(value), "test_h value should have been returned");
	rc = attr_get(ATTR_ID_test_i, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_i should be obscured");
	zassert_not_equal(value, 9, "test_i value should not have been returned");
	rc = attr_get(ATTR_ID_test_j, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_j should be obscured");
	zassert_not_equal(value, 10, "test_j value should not have been returned");
	rc = attr_get(ATTR_ID_test_k, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_k should be obscured");
	zassert_not_equal(value, 11, "test_k value should not have been returned");
	rc = attr_get(ATTR_ID_test_l, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_l should be hidden");
	zassert_not_equal(value, 12, "test_l value should not have been returned");
	rc = attr_get(ATTR_ID_test_m, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_m should be hidden");
	zassert_not_equal(value, 13, "test_m value should not have been returned");
	rc = attr_get(ATTR_ID_test_n, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_n should be hidden");
	zassert_not_equal(value, 14, "test_n value should not have been returned");
	rc = attr_get(ATTR_ID_test_o, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_o should be obscured");
	zassert_not_equal(value, 15, "test_o value should not have been returned");
	rc = attr_get(ATTR_ID_test_p, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_p should be obscured");
	zassert_not_equal(value, 16, "test_p value should not have been returned");
	rc = attr_get(ATTR_ID_test_q, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_q should be obscured");
	zassert_not_equal(value, 17, "test_q value should not have been returned");
	rc = attr_get(ATTR_ID_test_r, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_r should be hidden");
	zassert_not_equal(value, 18, "test_r value should not have been returned");
	rc = attr_get(ATTR_ID_test_s, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_s should be hidden");
	zassert_not_equal(value, 19, "test_s value should not have been returned");
	rc = attr_get(ATTR_ID_test_t, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_t should be hidden");
	zassert_not_equal(value, 20, "test_t value should not have been returned");
	rc = attr_get(ATTR_ID_test_u, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_u should be obscured");
	zassert_not_equal(value, 21, "test_u value should not have been returned");
	rc = attr_get(ATTR_ID_test_v, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_v should be hidden");
	zassert_not_equal(value, 22, "test_v value should not have been returned");
	rc = attr_get(ATTR_ID_test_w, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_w should be obscured");
	zassert_not_equal(value, 23, "test_w value should not have been returned");
	rc = attr_get(ATTR_ID_test_x, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_x should be hidden");
	zassert_not_equal(value, 24, "test_x value should not have been returned");
}

void test_attr_lock_disengage(void)
{
	int rc;

	rc = attr_set_uint32(ATTR_ID_lock_status, LOCK_STATUS_SETUP_DISENGAGED);
	zassert_ok(rc, "Failed to disengage lock");
}

void test_attr_lock_engage(void)
{
	int rc;

	rc = attr_set_uint32(ATTR_ID_lock_status, LOCK_STATUS_SETUP_ENGAGED);
	zassert_ok(rc, "Failed to engage lock");
}

void test_attr_lock_disable(void)
{
	int rc;

	rc = attr_set_uint32(ATTR_ID_lock, false);
	zassert_ok(rc, "Failed to disable lock");
	rc = attr_set_uint32(ATTR_ID_lock_status, LOCK_STATUS_NOT_SETUP);
	zassert_ok(rc, "Failed to disable lock");
}

void test_attr_show_with_lock_setup_not_engaged(void)
{
	int rc;

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(ATTR_ID_test_u);
	enable_logging = false;
	zassert_ok(rc, "Failed to show test_u value");
	zassert_not_null(strstr(mock_buffer, "[26] test_u 21"),
			 "Expected value for test_u not found");

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(ATTR_ID_test_v);
	enable_logging = false;
	zassert_ok(rc, "Failed to show test_v value");
	zassert_not_null(strstr(mock_buffer, "[27] test_v 22"),
			 "Expected value for test_v not found");

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(ATTR_ID_test_w);
	enable_logging = false;
	zassert_equal(rc, -EPERM, "Failed to show obscured test_w value");
	zassert_not_null(strstr(mock_buffer, "[28] test_w " ASTERISK_RESPONSE),
			 "Expected obscured value for test_w not found");

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(ATTR_ID_test_x);
	enable_logging = false;
	zassert_equal(rc, -EACCES, "Failed to ensure test_x value is hidden");
	zassert_is_null(strstr(mock_buffer, "test_x"),
			"Unexpected value for test_x found");
	zassert_equal(strlen(mock_buffer), 0,
		      "Unexpected response for showing hidden attribute");
}

void test_attr_show_with_lock_engaged(void)
{
	int rc;

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(ATTR_ID_test_u);
	enable_logging = false;
	zassert_equal(rc, -EPERM, "Failed to show obscured test_u value");
	zassert_not_null(strstr(mock_buffer, "[26] test_u " ASTERISK_RESPONSE),
			 "Expected obscured value for test_u not found");

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(ATTR_ID_test_v);
	enable_logging = false;
	zassert_equal(rc, -EACCES, "Failed to ensure test_v value is hidden");
	zassert_is_null(strstr(mock_buffer, "test_v"),
			"Unexpected value for test_x found");
	zassert_equal(strlen(mock_buffer), 0,
		      "Unexpected response for showing hidden attribute");

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(ATTR_ID_test_w);
	enable_logging = false;
	zassert_equal(rc, -EPERM, "Failed to show obscured test_w value");
	zassert_not_null(strstr(mock_buffer, "[28] test_w " ASTERISK_RESPONSE),
			 "Expected obscured value for test_w not found");

	reset_mock_buffer();
	enable_logging = true;
	rc = attr_show(ATTR_ID_test_x);
	enable_logging = false;
	zassert_equal(rc, -EACCES, "Failed to ensure test_x value is hidden");
	zassert_is_null(strstr(mock_buffer, "test_x"),
			"Unexpected value for test_x found");
	zassert_equal(strlen(mock_buffer), 0,
		      "Unexpected response for showing hidden attribute");
}

void test_attr_change_with_lock_setup_not_engaged(void)
{
	int rc;
	bool modified;
	uint8_t value;

	reset_mock_buffer();
	modified = false;
	value = 33;
	enable_logging = true;
	rc = attr_set(ATTR_ID_test_u, ATTR_TYPE_ANY, &value, sizeof(value),
		      &modified);
	enable_logging = false;
	zassert_ok(rc, "Failed to change test_u value");
	zassert_not_null(strstr(mock_buffer, "[26] test_u 33"),
			 "Expected notification for test_u change not found");
	zassert_equal(modified, true,
		      "Expected modified for test_u attribute value change");

	reset_mock_buffer();
	modified = false;
	value = 34;
	enable_logging = true;
	rc = attr_set(ATTR_ID_test_v, ATTR_TYPE_ANY, &value, sizeof(value),
		      &modified);
	enable_logging = false;
	zassert_ok(rc, "Failed to change test_v value");
	zassert_not_null(strstr(mock_buffer, "[27] test_v 34"),
			 "Expected notification for test_v change not found");
	zassert_equal(modified, true,
		      "Expected modified for test_v attribute value change");

	reset_mock_buffer();
	modified = false;
	value = 35;
	enable_logging = true;
	rc = attr_set(ATTR_ID_test_w, ATTR_TYPE_ANY, &value, sizeof(value),
		      &modified);
	enable_logging = false;
	zassert_ok(rc, "Failed to change test_w value");
	zassert_not_null(strstr(mock_buffer, "[28] test_w 35"),
			 "Expected notification for test_w change not found");
	zassert_equal(modified, true,
		      "Expected modified for test_w attribute value change");

	reset_mock_buffer();
	modified = false;
	value = 36;
	enable_logging = true;
	rc = attr_set(ATTR_ID_test_x, ATTR_TYPE_ANY, &value, sizeof(value),
		      &modified);
	enable_logging = false;
	zassert_ok(rc, "Failed to change test_v value");
	zassert_not_null(strstr(mock_buffer, "[29] test_x 36"),
			 "Expected notification for test_x change not found");
	zassert_equal(modified, true,
		      "Expected modified for test_x attribute value change");

	rc = attr_get(ATTR_ID_test_u, &value, sizeof(value));
	zassert_equal(value, 33, "test_u should be visible");
	zassert_equal(rc, sizeof(value), "test_u value should have been returned");
	rc = attr_get(ATTR_ID_test_v, &value, sizeof(value));
	zassert_equal(value, 34, "test_v should be visible");
	zassert_equal(rc, sizeof(value), "test_v value should have been returned");
	rc = attr_get(ATTR_ID_test_w, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_w should be obscured");
	zassert_not_equal(value, 35, "test_w value should not have been returned");
	rc = attr_get(ATTR_ID_test_x, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_x should be hidden");
	zassert_not_equal(value, 36, "test_x value should not have been returned");

	value = (uint8_t)attr_get_uint32(ATTR_ID_test_u, 99);
	zassert_equal(value, 33, "Invalid value for test_u");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_v, 99);
	zassert_equal(value, 34, "Invalid value for test_v");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_w, 99);
	zassert_equal(value, 35, "Invalid value for test_w");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_x, 99);
	zassert_equal(value, 36, "Invalid value for test_x");

	reset_mock_buffer();
	modified = false;
	value = 33;
	enable_logging = true;
	rc = attr_set(ATTR_ID_test_u, ATTR_TYPE_ANY, &value, sizeof(value),
		      &modified);
	enable_logging = false;
	zassert_ok(rc, "Failed to change test_u value");
	zassert_is_null(strstr(mock_buffer, "[26] test_u 33"),
			"Unexpected notification for test_u change found");
	zassert_equal(modified, false,
		      "Unexpected modified for test_u attribute value change");
}

void test_attr_change_with_lock_engaged(void)
{
	int rc;
	bool modified;
	uint8_t value;

	reset_mock_buffer();
	modified = false;
	value = 43;
	enable_logging = true;
	rc = attr_set(ATTR_ID_test_u, ATTR_TYPE_ANY, &value, sizeof(value),
		      &modified);
	enable_logging = false;
	zassert_ok(rc, "Failed to change test_u value");
	zassert_not_null(strstr(mock_buffer, "[26] test_u 43"),
			 "Expected notification for test_u change not found");
	zassert_equal(modified, true,
		      "Expected modified for test_u attribute value change");

	reset_mock_buffer();
	modified = false;
	value = 44;
	enable_logging = true;
	rc = attr_set(ATTR_ID_test_v, ATTR_TYPE_ANY, &value, sizeof(value),
		      &modified);
	enable_logging = false;
	zassert_ok(rc, "Failed to change test_v value");
	zassert_not_null(strstr(mock_buffer, "[27] test_v 44"),
			 "Expected notification for test_v change not found");
	zassert_equal(modified, true,
		      "Expected modified for test_v attribute value change");

	reset_mock_buffer();
	modified = false;
	value = 45;
	enable_logging = true;
	rc = attr_set(ATTR_ID_test_w, ATTR_TYPE_ANY, &value, sizeof(value),
		      &modified);
	enable_logging = false;
	zassert_ok(rc, "Failed to change test_w value");
	zassert_not_null(strstr(mock_buffer, "[28] test_w 45"),
			 "Expected notification for test_w change not found");
	zassert_equal(modified, true,
		      "Expected modified for test_w attribute value change");

	reset_mock_buffer();
	modified = false;
	value = 46;
	enable_logging = true;
	rc = attr_set(ATTR_ID_test_x, ATTR_TYPE_ANY, &value, sizeof(value),
		      &modified);
	enable_logging = false;
	zassert_ok(rc, "Failed to change test_v value");
	zassert_not_null(strstr(mock_buffer, "[29] test_x 46"),
			 "Expected notification for test_x change not found");
	zassert_equal(modified, true,
		      "Expected modified for test_x attribute value change");

	rc = attr_get(ATTR_ID_test_u, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_u should be obscured");
	zassert_not_equal(value, 43, "test_u value should not have been returned");
	rc = attr_get(ATTR_ID_test_v, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_v should be hidden");
	zassert_not_equal(value, 44, "test_v value should not have been returned");
	rc = attr_get(ATTR_ID_test_w, &value, sizeof(value));
	zassert_equal(rc, -EPERM, "test_w should be obscured");
	zassert_not_equal(value, 45, "test_w value should not have been returned");
	rc = attr_get(ATTR_ID_test_x, &value, sizeof(value));
	zassert_equal(rc, -EACCES, "test_x should be hidden");
	zassert_not_equal(value, 46, "test_x value should not have been returned");

	value = (uint8_t)attr_get_uint32(ATTR_ID_test_u, 99);
	zassert_equal(value, 43, "Invalid value for test_u");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_v, 99);
	zassert_equal(value, 44, "Invalid value for test_v");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_w, 99);
	zassert_equal(value, 45, "Invalid value for test_w");
	value = (uint8_t)attr_get_uint32(ATTR_ID_test_x, 99);
	zassert_equal(value, 46, "Invalid value for test_x");

	reset_mock_buffer();
	modified = false;
	value = 43;
	enable_logging = true;
	rc = attr_set(ATTR_ID_test_u, ATTR_TYPE_ANY, &value, sizeof(value),
		      &modified);
	enable_logging = false;
	zassert_ok(rc, "Failed to change test_u value");
	zassert_is_null(strstr(mock_buffer, "[26] test_u 43"),
			"Unexpected notification for test_u change found");
	zassert_equal(modified, false,
		      "Unexpected modified for test_u attribute value change");
}

void test_attr_dump(void)
{
	int rc;
	char *fstr = NULL;

	/* Clear file before proceeding */
	rc = fsu_delete_abs(attr_get_quasi_static(ATTR_ID_dump_path));
	zassert_true((rc == 0 || rc == -ENOENT),
		     "Failed to delete attribute dump file");

	/* Dump parameters to file */
	rc = attr_prepare_then_dump(&fstr, ATTR_DUMP_RW);
	zassert_false((rc < 0), "Failed to dump attributes");

	rc = fsu_write_abs(attr_get_quasi_static(ATTR_ID_dump_path), fstr,
			   strlen(fstr));
	zassert_false((rc < 0), "Failed to write attribute to dump file");
	k_free(fstr);
}

void test_attr_dump_with_lock_setup_not_engaged(void)
{
	ssize_t data_size = 0;
	uint8_t *data_buffer = NULL;

	data_size = fsu_get_file_size_abs(attr_get_quasi_static(ATTR_ID_dump_path));
	zassert_false((data_size < MIN_DUMP_FILE_SIZE),
		      "Dump file is not of a sufficient size to be valid");

	data_buffer = malloc(DATA_BUFFER_SIZE);
	zassert_not_null(data_buffer,
			 "Failed to allocate memory for data buffer");
	data_buffer[(DATA_BUFFER_SIZE - 1)] = 0;

	data_size = fsu_read_abs(attr_get_quasi_static(ATTR_ID_dump_path),
				 data_buffer, (DATA_BUFFER_SIZE - 1));
	zassert_false((data_size < MIN_DUMP_FILE_SIZE),
		      "Dump file is not of a sufficient size to be valid");
	data_buffer[data_size] = 0;

	/* Check that only valid entries are present */
	zassert_not_null(strstr(data_buffer, "\n6=01\n"),
			 "Missing test_a value in dump file");
	zassert_not_null(strstr(data_buffer, "\n7=02\n"),
			 "Missing test_b value in dump file");
	zassert_not_null(strstr(data_buffer, "\n8=03\n"),
			 "Missing test_c value in dump file");
	zassert_not_null(strstr(data_buffer, "\n9=04\n"),
			 "Missing test_d value in dump file");
	zassert_is_null(strstr(data_buffer, "\na=05\n"),
			"Unexpected test_e value in dump file");
	zassert_not_null(strstr(data_buffer, "\na=**\n"),
			 "Missing test_e value in dump file");
	zassert_is_null(strstr(data_buffer, "\nb="),
			"Unexpected test_f value in dump file");
	zassert_not_null(strstr(data_buffer, "\nc=07\n"),
			 "Missing test_g value in dump file");
	zassert_not_null(strstr(data_buffer, "\nd=08\n"),
			 "Missing test_h value in dump file");
	zassert_is_null(strstr(data_buffer, "\ne=09\n"),
			"Unexpected test_i value in dump file");
	zassert_not_null(strstr(data_buffer, "\ne=**\n"),
			 "Missing test_i value in dump file");
	zassert_is_null(strstr(data_buffer, "\nf=0a\n"),
			"Unexpected test_j value in dump file");
	zassert_not_null(strstr(data_buffer, "\nf=**\n"),
			 "Missing test_j value in dump file");
	zassert_not_null(strstr(data_buffer, "\n10=0b\n"),
			 "Missing test_k value in dump file");
	zassert_is_null(strstr(data_buffer, "\n11=0a\n"),
			"Unexpected test_l value in dump file");
	zassert_not_null(strstr(data_buffer, "\n11=**\n"),
			 "Missing test_l value in dump file");
	zassert_is_null(strstr(data_buffer, "\n12=0b\n"),
			"Unexpected test_m value in dump file");
	zassert_not_null(strstr(data_buffer, "\n12=**\n"),
			 "Missing test_m value in dump file");
	zassert_not_null(strstr(data_buffer, "\n13=0e\n"),
			 "Missing test_n value in dump file");
	zassert_is_null(strstr(data_buffer, "\n14="),
			"Unexpected test_o value in dump file");
	zassert_is_null(strstr(data_buffer, "\n15="),
			"Unexpected test_p value in dump file");
	zassert_not_null(strstr(data_buffer, "\n16=11\n"),
			 "Missing test_q value in dump file");
	zassert_is_null(strstr(data_buffer, "\n17="),
			"Unexpected test_r value in dump file");
	zassert_is_null(strstr(data_buffer, "\n18="),
			"Unexpected test_s value in dump file");
	zassert_not_null(strstr(data_buffer, "\n19=14\n"),
			 "Missing test_t value in dump file");
	zassert_not_null(strstr(data_buffer, "\n1a=15\n"),
			 "Missing test_u value in dump file");
	zassert_not_null(strstr(data_buffer, "\n1b=16\n"),
			 "Missing test_v value in dump file");
	zassert_is_null(strstr(data_buffer, "\n1c=17\n"),
			"Unexpected test_w value in dump file");
	zassert_not_null(strstr(data_buffer, "\n1c=**\n"),
			 "Missing test_w value in dump file");
	zassert_is_null(strstr(data_buffer, "\n1d="),
			"Unexpected test_x value in dump file");
}

void test_attr_dump_with_lock_engaged(void)
{
	ssize_t data_size = 0;
	uint8_t *data_buffer = NULL;

	data_size = fsu_get_file_size_abs(attr_get_quasi_static(ATTR_ID_dump_path));
	zassert_false((data_size < MIN_DUMP_FILE_SIZE),
		      "Dump file is not of a sufficient size to be valid");

	data_buffer = malloc(DATA_BUFFER_SIZE);
	zassert_not_null(data_buffer,
			 "Failed to allocate memory for data buffer");
	data_buffer[(DATA_BUFFER_SIZE - 1)] = 0;

	data_size = fsu_read_abs(attr_get_quasi_static(ATTR_ID_dump_path),
				 data_buffer, (DATA_BUFFER_SIZE - 1));
	zassert_false((data_size < MIN_DUMP_FILE_SIZE),
		      "Dump file is not of a sufficient size to be valid");
	data_buffer[data_size] = 0;

	/* Check that only valid entries are present */
	zassert_not_null(strstr(data_buffer, "\n6=01\n"),
			 "Missing test_a value in dump file");
	zassert_not_null(strstr(data_buffer, "\n7=02\n"),
			 "Missing test_b value in dump file");
	zassert_not_null(strstr(data_buffer, "\n8=03\n"),
			 "Missing test_c value in dump file");
	zassert_not_null(strstr(data_buffer, "\n9=04\n"),
			 "Missing test_d value in dump file");
	zassert_is_null(strstr(data_buffer, "\na=05\n"),
			"Unexpected test_e value in dump file");
	zassert_not_null(strstr(data_buffer, "\na=**\n"),
			 "Missing test_e value in dump file");
	zassert_is_null(strstr(data_buffer, "\nb="),
			"Unexpected test_f value in dump file");
	zassert_is_null(strstr(data_buffer, "\nc=07\n"),
			"Unexpected test_g value in dump file");
	zassert_not_null(strstr(data_buffer, "\nc=**\n"),
			 "Missing test_g value in dump file");
	zassert_is_null(strstr(data_buffer, "\nd="),
			"Unexpected test_h value in dump file");
	zassert_is_null(strstr(data_buffer, "\ne=09\n"),
			"Unexpected test_i value in dump file");
	zassert_not_null(strstr(data_buffer, "\ne=**\n"),
			 "Missing test_i value in dump file");
	zassert_is_null(strstr(data_buffer, "\nf=0a\n"),
			"Unexpected test_j value in dump file");
	zassert_not_null(strstr(data_buffer, "\nf=**\n"),
			 "Missing test_j value in dump file");
	zassert_is_null(strstr(data_buffer, "\n10=0b\n"),
			"Unexpected test_k value in dump file");
	zassert_not_null(strstr(data_buffer, "\n10=**\n"),
			 "Missing test_k value in dump file");
	zassert_is_null(strstr(data_buffer, "\n11=0a\n"),
			"Unexpected test_l value in dump file");
	zassert_not_null(strstr(data_buffer, "\n11=**\n"),
			 "Missing test_l value in dump file");
	zassert_is_null(strstr(data_buffer, "\n12=0b\n"),
			"Unexpected test_m value in dump file");
	zassert_not_null(strstr(data_buffer, "\n12=**\n"),
			 "Missing test_m value in dump file");
	zassert_is_null(strstr(data_buffer, "\n13=0e\n"),
			"Unexpected test_n value in dump file");
	zassert_not_null(strstr(data_buffer, "\n13=**\n"),
			 "Missing test_n value in dump file");
	zassert_is_null(strstr(data_buffer, "\n14="),
			"Unexpected test_o value in dump file");
	zassert_is_null(strstr(data_buffer, "\n15="),
			"Unexpected test_p value in dump file");
	zassert_is_null(strstr(data_buffer, "\n16="),
			"Unexpected test_q value in dump file");
	zassert_is_null(strstr(data_buffer, "\n17="),
			"Unexpected test_r value in dump file");
	zassert_is_null(strstr(data_buffer, "\n18="),
			"Unexpected test_s value in dump file");
	zassert_is_null(strstr(data_buffer, "\n19="),
			"Unexpected test_t value in dump file");
	zassert_is_null(strstr(data_buffer, "\n1a=15\n"),
			"Unexpected test_u value in dump file");
	zassert_not_null(strstr(data_buffer, "\n1a=**\n"),
			 "Missing test_u value in dump file");
	zassert_is_null(strstr(data_buffer, "\n1b="),
			"Unexpected test_v value in dump file");
	zassert_is_null(strstr(data_buffer, "\n1c=17\n"),
			"Unexpected test_w value in dump file");
	zassert_not_null(strstr(data_buffer, "\n1c=**\n"),
			 "Missing test_w value in dump file");
	zassert_is_null(strstr(data_buffer, "\n1d="),
			"Unexpected test_x value in dump file");
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
	if (enable_logging == true)
	{
	        log_backend_std_sync_string(&log_output, 0, src_level,
	                                    timestamp, fmt, ap);
	}
}
