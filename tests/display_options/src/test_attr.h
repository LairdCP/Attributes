/**
 * @file test_attr.h
 * @brief
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __TEST_ATTR_H__
#define __TEST_ATTR_H__

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
void test_attr_defaults(void);
void test_attr_lock_enable(void);
void test_attr_with_lock_setup_not_engaged(void);
void test_attr_with_lock_engaged(void);
void test_attr_lock_disengage(void);
void test_attr_lock_engage(void);
void test_attr_lock_disable(void);
void test_attr_show_with_lock_setup_not_engaged(void);
void test_attr_show_with_lock_engaged(void);
void test_attr_change_with_lock_engaged(void);
void test_attr_change_with_lock_setup_not_engaged(void);
void test_attr_dump(void);
void test_attr_dump_with_lock_setup_not_engaged(void);
void test_attr_dump_with_lock_engaged(void);

#endif /* __TEST_ATTR_H__ */
