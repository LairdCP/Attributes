/*
 * Generated using zcbor version 0.5.1
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

struct set_parameter_result {
	uint32_t id;
	int32_t r;
};

struct get_parameter {
	uint32_t p1;
};

struct load_parameter_file_p1 {
	struct zcbor_string p1;
};

struct load_parameter_file {
	struct load_parameter_file_p1 p1;
	uint_fast32_t p1_present;
};

struct load_parameter_file_result_f {
	struct zcbor_string f;
};

struct load_parameter_file_result {
	int32_t r;
	struct load_parameter_file_result_f f;
	uint_fast32_t f_present;
};

struct dump_parameter_file_p2 {
	struct zcbor_string p2;
};

struct dump_parameter_file {
	uint32_t p1;
	struct dump_parameter_file_p2 p2;
	uint_fast32_t p2_present;
};

struct dump_parameter_file_result_n {
	struct zcbor_string n;
};

struct dump_parameter_file_result {
	int32_t r;
	struct dump_parameter_file_result_n n;
	uint_fast32_t n_present;
};

struct factory_reset_result {
	int32_t r;
};

struct set_notify {
	uint32_t p1;
	bool p2;
};

struct set_notify_result {
	uint32_t id;
	int32_t r;
};

struct get_notify {
	uint32_t p1;
};

struct get_notify_result {
	uint32_t id;
	int32_t r;
};

struct disable_notify_result {
	int32_t r;
};

struct set_parameter {
	uint32_t p1;
	union {
		bool _bool;
		uint64_t _uint64;
		int64_t _int64;
		struct zcbor_string tstr;
		double _float;
		struct zcbor_string bstr;
	};
	enum {
		_set_parameter_p2_bool,
		_set_parameter_p2__uint64,
		_set_parameter_p2__int64,
		_set_parameter_p2_tstr,
		_set_parameter_p2_float,
		_set_parameter_p2_bstr,
	} p2_choice;
};

struct get_parameter_result_r1_ {
	union {
		bool _bool;
		uint64_t _uint64;
		int64_t _int64;
		struct zcbor_string tstr;
		double _float;
		struct zcbor_string bstr;
	};
	enum {
		_get_parameter_result_r1_bool,
		_get_parameter_result_r1__uint64,
		_get_parameter_result_r1__int64,
		_get_parameter_result_r1_tstr,
		_get_parameter_result_r1_float,
		_get_parameter_result_r1_bstr,
	} r1_choice;
};

struct get_parameter_result {
	int32_t r;
	uint32_t id;
	struct get_parameter_result_r1_ r1;
	uint_fast32_t r1_present;
};


#endif /* ATTRIBUTE_MGMT_TYPES_H__ */
