/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef ATTRIBUTE_MGMT_TYPES_H__
#define ATTRIBUTE_MGMT_TYPES_H__

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

struct set_parameter_result {
	uint32_t _set_parameter_result_id;
	int32_t _set_parameter_result_r;
};

struct get_parameter {
	uint32_t _get_parameter_p1;
};

struct get_api_version_result {
	struct zcbor_string _get_api_version_result_api_version;
};

struct get_indices_result {
	uint32_t _get_indices_result_table_size;
	uint32_t _get_indices_result_min_id;
	uint32_t _get_indices_result_max_id;
};

struct get_entry_details_index {
	uint32_t _get_entry_details_index;
};

struct get_entry_details {
	enum {
		_get_entry_details_method_index = 0,
		_get_entry_details_method_first = 1,
		_get_entry_details_method_last = 2,
		_get_entry_details_method_next = 3,
		_get_entry_details_method_previous = 4,
	} _get_entry_details_method_choice;
	struct get_entry_details_index _get_entry_details_index;
	uint_fast32_t _get_entry_details_index_present;
};

struct load_parameter_file_p1 {
	struct zcbor_string _load_parameter_file_p1;
};

struct load_parameter_file {
	struct load_parameter_file_p1 _load_parameter_file_p1;
	uint_fast32_t _load_parameter_file_p1_present;
};

struct load_parameter_file_result_f {
	struct zcbor_string _load_parameter_file_result_f;
};

struct load_parameter_file_result {
	int32_t _load_parameter_file_result_r;
	struct load_parameter_file_result_f _load_parameter_file_result_f;
	uint_fast32_t _load_parameter_file_result_f_present;
};

struct dump_parameter_file_p2 {
	struct zcbor_string _dump_parameter_file_p2;
};

struct dump_parameter_file {
	uint32_t _dump_parameter_file_p1;
	struct dump_parameter_file_p2 _dump_parameter_file_p2;
	uint_fast32_t _dump_parameter_file_p2_present;
};

struct dump_parameter_file_result_n {
	struct zcbor_string _dump_parameter_file_result_n;
};

struct dump_parameter_file_result {
	int32_t _dump_parameter_file_result_r;
	struct dump_parameter_file_result_n _dump_parameter_file_result_n;
	uint_fast32_t _dump_parameter_file_result_n_present;
};

struct set_notify {
	uint32_t _set_notify_p1;
	bool _set_notify_p2;
};

struct set_notify_result {
	uint32_t _set_notify_result_id;
	int32_t _set_notify_result_r;
};

struct get_notify {
	uint32_t _get_notify_p1;
};

struct get_notify_result {
	uint32_t _get_notify_result_id;
	int32_t _get_notify_result_r;
};

struct disable_notify_result {
	int32_t _disable_notify_result_r;
};

struct factory_reset_result {
	int32_t _factory_reset_result_r;
};

struct set_parameter {
	uint32_t _set_parameter_p1;
	union {
		bool _set_parameter_p2_bool;
		uint64_t _set_parameter_p2__uint64;
		int64_t _set_parameter_p2__int64;
		struct zcbor_string _set_parameter_p2_tstr;
		double _set_parameter_p2_float;
		struct zcbor_string _set_parameter_p2_bstr;
	};
	enum {
		_set_parameter_p2_bool,
		_set_parameter_p2__uint64,
		_set_parameter_p2__int64,
		_set_parameter_p2_tstr,
		_set_parameter_p2_float,
		_set_parameter_p2_bstr,
	} _set_parameter_p2_choice;
};

struct get_parameter_result_r1_ {
	union {
		bool _get_parameter_result_r1_bool;
		uint64_t _get_parameter_result_r1__uint64;
		int64_t _get_parameter_result_r1__int64;
		struct zcbor_string _get_parameter_result_r1_tstr;
		double _get_parameter_result_r1_float;
		struct zcbor_string _get_parameter_result_r1_bstr;
	};
	enum {
		_get_parameter_result_r1_bool,
		_get_parameter_result_r1__uint64,
		_get_parameter_result_r1__int64,
		_get_parameter_result_r1_tstr,
		_get_parameter_result_r1_float,
		_get_parameter_result_r1_bstr,
	} _get_parameter_result_r1_choice;
};

struct get_parameter_result {
	int32_t _get_parameter_result_r;
	uint32_t _get_parameter_result_id;
	struct get_parameter_result_r1_ _get_parameter_result_r1;
	uint_fast32_t _get_parameter_result_r1_present;
};

struct get_entry_details_result_id {
	uint32_t _get_entry_details_result_id;
};

struct get_entry_details_result_name {
	struct zcbor_string _get_entry_details_result_name;
};

struct get_entry_details_result_size {
	uint32_t _get_entry_details_result_size;
};

struct get_entry_details_result_type {
	uint32_t _get_entry_details_result_type;
};

struct get_entry_details_result_flags {
	uint32_t _get_entry_details_result_flags;
};

struct get_entry_details_result_prepared {
	bool _get_entry_details_result_prepared;
};

struct get_entry_details_result_min_ {
	union {
		bool _get_entry_details_result_min_bool;
		uint64_t _get_entry_details_result_min__uint64;
		int64_t _get_entry_details_result_min__int64;
		double _get_entry_details_result_min_float;
	};
	enum {
		_get_entry_details_result_min_bool,
		_get_entry_details_result_min__uint64,
		_get_entry_details_result_min__int64,
		_get_entry_details_result_min_float,
	} _get_entry_details_result_min_choice;
};

struct get_entry_details_result_max_ {
	union {
		bool _get_entry_details_result_max_bool;
		uint64_t _get_entry_details_result_max__uint64;
		int64_t _get_entry_details_result_max__int64;
		double _get_entry_details_result_max_float;
	};
	enum {
		_get_entry_details_result_max_bool,
		_get_entry_details_result_max__uint64,
		_get_entry_details_result_max__int64,
		_get_entry_details_result_max_float,
	} _get_entry_details_result_max_choice;
};

struct get_entry_details_result {
	uint32_t _get_entry_details_result_index;
	int32_t _get_entry_details_result_res;
	struct get_entry_details_result_id _get_entry_details_result_id;
	uint_fast32_t _get_entry_details_result_id_present;
	struct get_entry_details_result_name _get_entry_details_result_name;
	uint_fast32_t _get_entry_details_result_name_present;
	struct get_entry_details_result_size _get_entry_details_result_size;
	uint_fast32_t _get_entry_details_result_size_present;
	struct get_entry_details_result_type _get_entry_details_result_type;
	uint_fast32_t _get_entry_details_result_type_present;
	struct get_entry_details_result_flags _get_entry_details_result_flags;
	uint_fast32_t _get_entry_details_result_flags_present;
	struct get_entry_details_result_prepared _get_entry_details_result_prepared;
	uint_fast32_t _get_entry_details_result_prepared_present;
	struct get_entry_details_result_min_ _get_entry_details_result_min;
	uint_fast32_t _get_entry_details_result_min_present;
	struct get_entry_details_result_max_ _get_entry_details_result_max;
	uint_fast32_t _get_entry_details_result_max_present;
};


#endif /* ATTRIBUTE_MGMT_TYPES_H__ */
