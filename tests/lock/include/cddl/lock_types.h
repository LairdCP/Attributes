/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef ENCODE_LOCK_TYPES_H__
#define ENCODE_LOCK_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 3

struct empty_map {
	uint32_t _empty_map_nil_present;
};

struct check_lock_status_result {
	int32_t _check_lock_status_result_r;
	bool _check_lock_status_result_r1;
	bool _check_lock_status_result_r2;
};

struct set_lock_code {
	uint32_t _set_lock_code_p1;
};

struct set_lock_code_result {
	int32_t _set_lock_code_result_r;
};

struct lock_result {
	int32_t _lock_result_r;
};

struct unlock {
	uint32_t _unlock_p1;
	bool _unlock_p2;
};

struct unlock_result {
	int32_t _unlock_result_r;
};

struct get_unlock_error_code_result {
	int32_t _get_unlock_error_code_result_r;
	int32_t _get_unlock_error_code_result_r1;
};

struct set_parameter {
	uint32_t _set_parameter_p1;
	union {
		bool _set_parameter_p2_bool;
		uint32_t _set_parameter_p2_uint;
		int32_t _set_parameter_p2_int;
		zcbor_string_type_t _set_parameter_p2_tstr;
		zcbor_string_type_t _set_parameter_p2_bstr;
	};
	enum {
		_set_parameter_p2_bool,
		_set_parameter_p2_uint,
		_set_parameter_p2_int,
		_set_parameter_p2_tstr,
		_set_parameter_p2_bstr,
		_set_parameter_p2_nil,
	} _set_parameter_p2_choice;
};

struct set_parameter_result {
	uint32_t _set_parameter_result_id;
	int32_t _set_parameter_result_r;
};

struct get_parameter {
	uint32_t _get_parameter_p1;
};

struct get_parameter_result {
	uint32_t _get_parameter_result_id;
	union {
		bool _get_parameter_result_r1_bool;
		uint32_t _get_parameter_result_r1_uint;
		int32_t _get_parameter_result_r1_int;
		zcbor_string_type_t _get_parameter_result_r1_tstr;
		zcbor_string_type_t _get_parameter_result_r1_bstr;
	};
	enum {
		_get_parameter_result_r1_bool,
		_get_parameter_result_r1_uint,
		_get_parameter_result_r1_int,
		_get_parameter_result_r1_tstr,
		_get_parameter_result_r1_bstr,
		_get_parameter_result_r1_nil,
	} _get_parameter_result_r1_choice;
	int32_t _get_parameter_result_r;
};


#endif /* ENCODE_LOCK_TYPES_H__ */
