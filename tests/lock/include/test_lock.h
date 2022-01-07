/**
 * @file test_lock.h
 * @brief
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __TEST_LOCK_H__
#define __TEST_LOCK_H__

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <zephyr.h>
#include <sys/util.h>
#include <ztest.h>

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/
void test_attr_setup(void);
void test_attr_each_setup(void);
void test_attr_each_end(void);
void test_attr_defaults(void);
void test_lock_enable(void);
void test_attr_with_lock_setup_not_engaged(void);
void test_attr_with_lock_engaged(void);
void test_lock_disengage(void);
void test_lock_engage(void);
void test_lock_disable(void);
void test_attr_show_with_lock_setup_not_engaged(void);
void test_attr_show_with_lock_engaged(void);
void test_attr_change_with_lock_engaged(void);
void test_attr_change_with_lock_setup_not_engaged(void);
void test_attr_dump(void);
void test_attr_dump_with_lock_setup_not_engaged(void);
void test_attr_dump_with_lock_engaged(void);
void test_lock_check_not_setup(void);
void test_lock_check_setup_locked(void);
void test_lock_check_setup_not_locked(void);
void test_lock_set_code_a(void);
void test_lock_set_code_b(void);
void test_lock_set_code_c_failure(void);
void test_lock_enable_lock(void);
void test_lock_deactivate_lock_a(void);
void test_lock_deactivate_lock_a_failure(void);
void test_lock_deactivate_lock_b(void);
void test_lock_deactivate_lock_c_failure(void);
void test_lock_disable_lock_a(void);
void test_lock_disable_lock_a_failure(void);
void test_lock_disable_lock_b(void);
void test_lock_disable_lock_c_failure(void);
void test_lock_set_code(void);
void test_lock_enable_lock(void);
void test_lock_deactivate_lock(void);
void test_lock_disable_lock(void);
void test_lock_check_error_undefined(void);
void test_lock_check_error_valid(void);
void test_lock_check_error_invalid(void);
void test_lock_set_uint8(void);
void test_lock_set_uint8_fail(void);
void test_lock_set_int8(void);
void test_lock_set_int8_fail(void);
void test_lock_set_uint16(void);
void test_lock_set_uint16_fail(void);
void test_lock_set_int16(void);
void test_lock_set_int16_fail(void);
void test_lock_set_uint32(void);
void test_lock_set_uint32_fail(void);
void test_lock_set_int32(void);
void test_lock_set_int32_fail(void);
void test_lock_set_bool(void);
void test_lock_set_bool_fail(void);
void test_lock_set_string(void);
void test_lock_set_string_fail(void);
void test_lock_set_byte_array(void);
void test_lock_set_byte_array_fail(void);
void test_lock_get_uint8(void);
void test_lock_get_uint8_fail(void);
void test_lock_get_int8(void);
void test_lock_get_int8_fail(void);
void test_lock_get_uint16(void);
void test_lock_get_uint16_fail(void);
void test_lock_get_int16(void);
void test_lock_get_int16_fail(void);
void test_lock_get_uint32(void);
void test_lock_get_uint32_fail(void);
void test_lock_get_int32(void);
void test_lock_get_int32_fail(void);
void test_lock_get_bool(void);
void test_lock_get_bool_fail(void);
void test_lock_get_string(void);
void test_lock_get_string_fail(void);
void test_lock_get_byte_array(void);
void test_lock_get_byte_array_fail(void);
void test_lock_set_test_value_defaults(void);
void dummy_mcumgr_add_data(uint8_t *data, uint16_t data_size);
int dummy_mcumgr_send_raw(const void *data, int len, void *arg);

#endif /* __TEST_LOCK_H__ */
