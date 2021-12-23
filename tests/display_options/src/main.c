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
#include "test_attr.h"

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
void test_main(void)
{
	test_attr_setup();

	ztest_test_suite(lcz_attr_basic_api_test,
			 ztest_unit_test(test_attr_defaults),
			 ztest_unit_test(test_attr_with_lock_setup_not_engaged),
			 ztest_unit_test(test_attr_lock_enable),
			 ztest_unit_test(test_attr_with_lock_setup_not_engaged),
			 ztest_unit_test(test_attr_lock_engage),
			 ztest_unit_test(test_attr_with_lock_engaged),
			 ztest_unit_test(test_attr_lock_disengage),
			 ztest_unit_test(test_attr_with_lock_setup_not_engaged),
			 ztest_unit_test(test_attr_lock_engage),
			 ztest_unit_test(test_attr_with_lock_engaged),
			 ztest_unit_test(test_attr_lock_disable),
			 ztest_unit_test(test_attr_with_lock_setup_not_engaged),
			 ztest_unit_test(
				test_attr_show_with_lock_setup_not_engaged),
			 ztest_unit_test(test_attr_dump),
			 ztest_unit_test(
				test_attr_dump_with_lock_setup_not_engaged),
			 ztest_unit_test(test_attr_lock_enable),
			 ztest_unit_test(test_attr_lock_engage),
			 ztest_unit_test(test_attr_dump),
			 ztest_unit_test(test_attr_dump_with_lock_engaged),
			 ztest_unit_test(test_attr_lock_disable),
			 ztest_unit_test(test_attr_lock_enable),
			 ztest_unit_test(test_attr_lock_engage),
			 ztest_unit_test(test_attr_change_with_lock_engaged),
			 ztest_unit_test(test_attr_lock_disable),
			 ztest_unit_test(
				test_attr_change_with_lock_setup_not_engaged),
			 ztest_unit_test(test_attr_lock_enable),
			 ztest_unit_test(test_attr_lock_engage),
			 ztest_unit_test(test_attr_show_with_lock_engaged),
			 ztest_unit_test(test_attr_lock_disable)
			);
	ztest_run_test_suite(lcz_attr_basic_api_test);
}
