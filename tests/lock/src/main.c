/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include "test_lock.h"

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
void test_main(void)
{
#ifdef CONFIG_MCUMGR_CMD_OS_MGMT
        os_mgmt_register_group();
#endif

	test_attr_setup();

	ztest_test_suite(lcz_attr_lock_test,
			 /* Ensure lock is disabled by default */
			 ztest_unit_test_setup_teardown(test_lock_check_not_setup, test_attr_each_setup, test_attr_each_end),

			 /* Ensure invalid attribute values are not present */
			 ztest_unit_test_setup_teardown(test_lock_get_uint8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array_fail, test_attr_each_setup, test_attr_each_end),

			 /* Ensure attributes can be updated with the lock not setup */
			 ztest_unit_test_setup_teardown(test_lock_set_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_byte_array, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array, test_attr_each_setup, test_attr_each_end),

			 /* Reset attribute values to default */
			 ztest_unit_test(test_lock_set_test_value_defaults),

			 /* Set a lock code and ensure the lock has been setup but is disengaged */
			 ztest_unit_test_setup_teardown(test_lock_set_code_a, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_check_setup_not_locked, test_attr_each_setup, test_attr_each_end),

			 /* Try to change parameters which require the device to be unlocked, expect success */
			 ztest_unit_test_setup_teardown(test_lock_set_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_byte_array, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array, test_attr_each_setup, test_attr_each_end),

			 /* Enable the lock */
			 ztest_unit_test_setup_teardown(test_lock_enable_lock, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_check_setup_locked, test_attr_each_setup, test_attr_each_end),

			 /* Ensure parameters can no longer be updated */
			 ztest_unit_test_setup_teardown(test_lock_set_uint8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_bool_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_string_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_byte_array_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array_fail, test_attr_each_setup, test_attr_each_end),

			 /* Ensure previous set values remain */
			 ztest_unit_test_setup_teardown(test_lock_get_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array, test_attr_each_setup, test_attr_each_end),

			 /* Try to unlock using an invalid password and expect failure, check error code status */
			 ztest_unit_test_setup_teardown(test_lock_deactivate_lock_c_failure, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_check_error_invalid, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_check_error_undefined, test_attr_each_setup, test_attr_each_end),

			 /* Ensure parameters still can't be updated */
			 ztest_unit_test_setup_teardown(test_lock_set_uint8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_bool_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_string_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_byte_array_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array_fail, test_attr_each_setup, test_attr_each_end),

			 /* Try to set a new lock code with the lock already engaged, expect failure */
			 ztest_unit_test_setup_teardown(test_lock_set_code_c_failure, test_attr_each_setup, test_attr_each_end),

			 /* Ensure parameters still can't be updated */
			 ztest_unit_test_setup_teardown(test_lock_set_uint8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_bool_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_string_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_byte_array_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array_fail, test_attr_each_setup, test_attr_each_end),

			 /* Ensure previous set values remain */
			 ztest_unit_test_setup_teardown(test_lock_get_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array, test_attr_each_setup, test_attr_each_end),


			 /* Restore attributes to defaults */
			 ztest_unit_test(test_lock_set_test_value_defaults),

			 /* Deactivate the lock without removing it */
			 ztest_unit_test_setup_teardown(test_lock_deactivate_lock_a, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_check_error_valid, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_check_error_undefined, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_check_setup_not_locked, test_attr_each_setup, test_attr_each_end),

			 /* Try to change parameters which require the device to be unlocked, expect success */
			 ztest_unit_test_setup_teardown(test_lock_set_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_byte_array, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array, test_attr_each_setup, test_attr_each_end),

			 /* Restore attributes to defaults */
			 ztest_unit_test(test_lock_set_test_value_defaults),

			 /* Disable the lock and ensure it has been removed */
			 ztest_unit_test_setup_teardown(test_lock_disable_lock_a, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_check_not_setup, test_attr_each_setup, test_attr_each_end),

			 /* Try to change parameters which require the device to be unlocked, expect success */
			 ztest_unit_test_setup_teardown(test_lock_set_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_byte_array, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array, test_attr_each_setup, test_attr_each_end),

			 /* Restore attributes to defaults */
			 ztest_unit_test(test_lock_set_test_value_defaults),

			 /* Try changing to a new lock code and locking the module */
			 ztest_unit_test_setup_teardown(test_lock_set_code_b, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_enable_lock, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_check_setup_locked, test_attr_each_setup, test_attr_each_end),

			 /* Ensure an invalid and the previous unlock keys do not work */
			 ztest_unit_test_setup_teardown(test_lock_disable_lock_a_failure, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_deactivate_lock_a_failure, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_disable_lock_a_failure, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_deactivate_lock_c_failure, test_attr_each_setup, test_attr_each_end),

			 /* Ensure parameters still can't be updated after an invalid code was used */
			 ztest_unit_test_setup_teardown(test_lock_set_uint8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_bool_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_string_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_byte_array_fail, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array_fail, test_attr_each_setup, test_attr_each_end),

			 /* Disengage the lock and ensure it can be removed without the unlock key if the lock is disengaged */
			 ztest_unit_test_setup_teardown(test_lock_deactivate_lock_b, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_check_setup_not_locked, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_disable_lock_a, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_check_not_setup, test_attr_each_setup, test_attr_each_end),

			 /* Try to change parameters which require the device to be unlocked, expect success */
			 ztest_unit_test_setup_teardown(test_lock_set_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int8, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int16, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_uint32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_int32, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_bool, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_string, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_set_byte_array, test_attr_each_setup, test_attr_each_end),
			 ztest_unit_test_setup_teardown(test_lock_get_byte_array, test_attr_each_setup, test_attr_each_end)
			);
	ztest_run_test_suite(lcz_attr_lock_test);
}
