/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"
#include "attribute_mgmt_decode.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_repeated_get_entry_details_index(
		zcbor_state_t *state, struct get_entry_details_index *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"index", tmp_str.len = sizeof("index") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_entry_details_index)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_load_parameter_file_p1(
		zcbor_state_t *state, struct load_parameter_file_p1 *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_tstr_decode(state, (&(*result)._load_parameter_file_p1)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_load_parameter_file_result_f(
		zcbor_state_t *state, struct load_parameter_file_result_f *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"f", tmp_str.len = sizeof("f") - 1, &tmp_str)))))
	&& (zcbor_tstr_decode(state, (&(*result)._load_parameter_file_result_f)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_dump_parameter_file_p2(
		zcbor_state_t *state, struct dump_parameter_file_p2 *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_tstr_decode(state, (&(*result)._dump_parameter_file_p2)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_dump_parameter_file_result_n(
		zcbor_state_t *state, struct dump_parameter_file_result_n *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"n", tmp_str.len = sizeof("n") - 1, &tmp_str)))))
	&& (zcbor_tstr_decode(state, (&(*result)._dump_parameter_file_result_n)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_get_parameter_result_r1(
		zcbor_state_t *state, struct get_parameter_result_r1_ *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (zcbor_union_start_code(state) && (int_res = ((((zcbor_bool_decode(state, (&(*result)._get_parameter_result_r1_bool)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_bool) || 1))
	|| (((zcbor_uint64_decode(state, (&(*result)._get_parameter_result_r1__uint64)))
	&& ((*result)._get_parameter_result_r1__uint64 <= 18446744073709551615ULL)) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1__uint64) || 1))
	|| (((zcbor_int64_decode(state, (&(*result)._get_parameter_result_r1__int64)))
	&& ((*result)._get_parameter_result_r1__int64 >= -9223372036854775807LL)
	&& ((*result)._get_parameter_result_r1__int64 <= 9223372036854775807LL)) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1__int64) || 1))
	|| (((zcbor_tstr_decode(state, (&(*result)._get_parameter_result_r1_tstr)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_tstr) || 1))
	|| (((zcbor_float_decode(state, (&(*result)._get_parameter_result_r1_float)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_float) || 1))
	|| (((zcbor_bstr_decode(state, (&(*result)._get_parameter_result_r1_bstr)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_bstr) || 1))), zcbor_union_end_code(state), int_res))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_get_entry_details_result_id(
		zcbor_state_t *state, struct get_entry_details_result_id *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_entry_details_result_id)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_get_entry_details_result_name(
		zcbor_state_t *state, struct get_entry_details_result_name *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"name", tmp_str.len = sizeof("name") - 1, &tmp_str)))))
	&& (zcbor_tstr_decode(state, (&(*result)._get_entry_details_result_name)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_get_entry_details_result_size(
		zcbor_state_t *state, struct get_entry_details_result_size *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"size", tmp_str.len = sizeof("size") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_entry_details_result_size)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_get_entry_details_result_type(
		zcbor_state_t *state, struct get_entry_details_result_type *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"type", tmp_str.len = sizeof("type") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_entry_details_result_type)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_get_entry_details_result_flags(
		zcbor_state_t *state, struct get_entry_details_result_flags *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"flags", tmp_str.len = sizeof("flags") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_entry_details_result_flags)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_get_entry_details_result_prepared(
		zcbor_state_t *state, struct get_entry_details_result_prepared *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"prepared", tmp_str.len = sizeof("prepared") - 1, &tmp_str)))))
	&& (zcbor_bool_decode(state, (&(*result)._get_entry_details_result_prepared)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_get_entry_details_result_min(
		zcbor_state_t *state, struct get_entry_details_result_min_ *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"min", tmp_str.len = sizeof("min") - 1, &tmp_str)))))
	&& (zcbor_union_start_code(state) && (int_res = ((((zcbor_bool_decode(state, (&(*result)._get_entry_details_result_min_bool)))) && (((*result)._get_entry_details_result_min_choice = _get_entry_details_result_min_bool) || 1))
	|| (((zcbor_uint64_decode(state, (&(*result)._get_entry_details_result_min__uint64)))
	&& ((*result)._get_entry_details_result_min__uint64 <= 18446744073709551615ULL)) && (((*result)._get_entry_details_result_min_choice = _get_entry_details_result_min__uint64) || 1))
	|| (((zcbor_int64_decode(state, (&(*result)._get_entry_details_result_min__int64)))
	&& ((*result)._get_entry_details_result_min__int64 >= -9223372036854775807LL)
	&& ((*result)._get_entry_details_result_min__int64 <= 9223372036854775807LL)) && (((*result)._get_entry_details_result_min_choice = _get_entry_details_result_min__int64) || 1))
	|| (((zcbor_float_decode(state, (&(*result)._get_entry_details_result_min_float)))) && (((*result)._get_entry_details_result_min_choice = _get_entry_details_result_min_float) || 1))), zcbor_union_end_code(state), int_res))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_get_entry_details_result_max(
		zcbor_state_t *state, struct get_entry_details_result_max_ *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"max", tmp_str.len = sizeof("max") - 1, &tmp_str)))))
	&& (zcbor_union_start_code(state) && (int_res = ((((zcbor_bool_decode(state, (&(*result)._get_entry_details_result_max_bool)))) && (((*result)._get_entry_details_result_max_choice = _get_entry_details_result_max_bool) || 1))
	|| (((zcbor_uint64_decode(state, (&(*result)._get_entry_details_result_max__uint64)))
	&& ((*result)._get_entry_details_result_max__uint64 <= 18446744073709551615ULL)) && (((*result)._get_entry_details_result_max_choice = _get_entry_details_result_max__uint64) || 1))
	|| (((zcbor_int64_decode(state, (&(*result)._get_entry_details_result_max__int64)))
	&& ((*result)._get_entry_details_result_max__int64 >= -9223372036854775807LL)
	&& ((*result)._get_entry_details_result_max__int64 <= 9223372036854775807LL)) && (((*result)._get_entry_details_result_max_choice = _get_entry_details_result_max__int64) || 1))
	|| (((zcbor_float_decode(state, (&(*result)._get_entry_details_result_max_float)))) && (((*result)._get_entry_details_result_max_choice = _get_entry_details_result_max_float) || 1))), zcbor_union_end_code(state), int_res))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_factory_reset_result(
		zcbor_state_t *state, struct factory_reset_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._factory_reset_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_factory_reset(
		zcbor_state_t *state, void *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_decode(state) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_disable_notify_result(
		zcbor_state_t *state, struct disable_notify_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._disable_notify_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_disable_notify(
		zcbor_state_t *state, void *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_decode(state) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_notify_result(
		zcbor_state_t *state, struct get_notify_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_notify_result_id))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._get_notify_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_notify(
		zcbor_state_t *state, struct get_notify *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_notify_p1))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_notify_result(
		zcbor_state_t *state, struct set_notify_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._set_notify_result_id))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._set_notify_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_notify(
		zcbor_state_t *state, struct set_notify *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._set_notify_p1))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_bool_decode(state, (&(*result)._set_notify_p2))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_dump_parameter_file_result(
		zcbor_state_t *state, struct dump_parameter_file_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._dump_parameter_file_result_r))))
	&& zcbor_present_decode(&((*result)._dump_parameter_file_result_n_present), (zcbor_decoder_t *)decode_repeated_dump_parameter_file_result_n, state, (&(*result)._dump_parameter_file_result_n))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_dump_parameter_file(
		zcbor_state_t *state, struct dump_parameter_file *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._dump_parameter_file_p1))))
	&& zcbor_present_decode(&((*result)._dump_parameter_file_p2_present), (zcbor_decoder_t *)decode_repeated_dump_parameter_file_p2, state, (&(*result)._dump_parameter_file_p2))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_load_parameter_file_result(
		zcbor_state_t *state, struct load_parameter_file_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._load_parameter_file_result_r))))
	&& zcbor_present_decode(&((*result)._load_parameter_file_result_f_present), (zcbor_decoder_t *)decode_repeated_load_parameter_file_result_f, state, (&(*result)._load_parameter_file_result_f))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_load_parameter_file(
		zcbor_state_t *state, struct load_parameter_file *result)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = (zcbor_present_decode(&((*result)._load_parameter_file_p1_present), (zcbor_decoder_t *)decode_repeated_load_parameter_file_p1, state, (&(*result)._load_parameter_file_p1))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_entry_details_result(
		zcbor_state_t *state, struct get_entry_details_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"index", tmp_str.len = sizeof("index") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_entry_details_result_index)))
	&& ((*result)._get_entry_details_result_index <= 65535))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"res", tmp_str.len = sizeof("res") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._get_entry_details_result_res))))
	&& zcbor_present_decode(&((*result)._get_entry_details_result_id_present), (zcbor_decoder_t *)decode_repeated_get_entry_details_result_id, state, (&(*result)._get_entry_details_result_id))
	&& zcbor_present_decode(&((*result)._get_entry_details_result_name_present), (zcbor_decoder_t *)decode_repeated_get_entry_details_result_name, state, (&(*result)._get_entry_details_result_name))
	&& zcbor_present_decode(&((*result)._get_entry_details_result_size_present), (zcbor_decoder_t *)decode_repeated_get_entry_details_result_size, state, (&(*result)._get_entry_details_result_size))
	&& zcbor_present_decode(&((*result)._get_entry_details_result_type_present), (zcbor_decoder_t *)decode_repeated_get_entry_details_result_type, state, (&(*result)._get_entry_details_result_type))
	&& zcbor_present_decode(&((*result)._get_entry_details_result_flags_present), (zcbor_decoder_t *)decode_repeated_get_entry_details_result_flags, state, (&(*result)._get_entry_details_result_flags))
	&& zcbor_present_decode(&((*result)._get_entry_details_result_prepared_present), (zcbor_decoder_t *)decode_repeated_get_entry_details_result_prepared, state, (&(*result)._get_entry_details_result_prepared))
	&& zcbor_present_decode(&((*result)._get_entry_details_result_min_present), (zcbor_decoder_t *)decode_repeated_get_entry_details_result_min, state, (&(*result)._get_entry_details_result_min))
	&& zcbor_present_decode(&((*result)._get_entry_details_result_max_present), (zcbor_decoder_t *)decode_repeated_get_entry_details_result_max, state, (&(*result)._get_entry_details_result_max))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_entry_details(
		zcbor_state_t *state, struct get_entry_details *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"method", tmp_str.len = sizeof("method") - 1, &tmp_str)))))
	&& (((zcbor_uint32_decode(state, (uint32_t *)&(*result)._get_entry_details_method_choice))) && ((((*result)._get_entry_details_method_choice == _get_entry_details_method_index) && ((1)))
	|| (((*result)._get_entry_details_method_choice == _get_entry_details_method_first) && ((1)))
	|| (((*result)._get_entry_details_method_choice == _get_entry_details_method_last) && ((1)))
	|| (((*result)._get_entry_details_method_choice == _get_entry_details_method_next) && ((1)))
	|| (((*result)._get_entry_details_method_choice == _get_entry_details_method_previous) && ((1))))))
	&& zcbor_present_decode(&((*result)._get_entry_details_index_present), (zcbor_decoder_t *)decode_repeated_get_entry_details_index, state, (&(*result)._get_entry_details_index))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_indices_result(
		zcbor_state_t *state, struct get_indices_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"table_size", tmp_str.len = sizeof("table_size") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_indices_result_table_size)))
	&& ((*result)._get_indices_result_table_size <= 65535))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"min_id", tmp_str.len = sizeof("min_id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_indices_result_min_id)))
	&& ((*result)._get_indices_result_min_id <= 65535))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"max_id", tmp_str.len = sizeof("max_id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_indices_result_max_id)))
	&& ((*result)._get_indices_result_max_id <= 65535))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_indices(
		zcbor_state_t *state, void *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_decode(state) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_api_version_result(
		zcbor_state_t *state, struct get_api_version_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"api_version", tmp_str.len = sizeof("api_version") - 1, &tmp_str)))))
	&& (zcbor_tstr_decode(state, (&(*result)._get_api_version_result_api_version))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_api_version(
		zcbor_state_t *state, void *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_decode(state) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_parameter_result(
		zcbor_state_t *state, struct get_parameter_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._get_parameter_result_r))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_parameter_result_id))))
	&& zcbor_present_decode(&((*result)._get_parameter_result_r1_present), (zcbor_decoder_t *)decode_repeated_get_parameter_result_r1, state, (&(*result)._get_parameter_result_r1))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_parameter(
		zcbor_state_t *state, struct get_parameter *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_parameter_p1))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_parameter_result(
		zcbor_state_t *state, struct set_parameter_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._set_parameter_result_id))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._set_parameter_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_parameter(
		zcbor_state_t *state, struct set_parameter *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._set_parameter_p1))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_union_start_code(state) && (int_res = ((((zcbor_bool_decode(state, (&(*result)._set_parameter_p2_bool)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_bool) || 1))
	|| (((zcbor_uint64_decode(state, (&(*result)._set_parameter_p2__uint64)))
	&& ((*result)._set_parameter_p2__uint64 <= 18446744073709551615ULL)) && (((*result)._set_parameter_p2_choice = _set_parameter_p2__uint64) || 1))
	|| (((zcbor_int64_decode(state, (&(*result)._set_parameter_p2__int64)))
	&& ((*result)._set_parameter_p2__int64 >= -9223372036854775807LL)
	&& ((*result)._set_parameter_p2__int64 <= 9223372036854775807LL)) && (((*result)._set_parameter_p2_choice = _set_parameter_p2__int64) || 1))
	|| (((zcbor_tstr_decode(state, (&(*result)._set_parameter_p2_tstr)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_tstr) || 1))
	|| (((zcbor_float_decode(state, (&(*result)._set_parameter_p2_float)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_float) || 1))
	|| (((zcbor_bstr_decode(state, (&(*result)._set_parameter_p2_bstr)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_bstr) || 1))), zcbor_union_end_code(state), int_res)))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_unlock_error_code_result(
		zcbor_state_t *state, struct get_unlock_error_code_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._get_unlock_error_code_result_r))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._get_unlock_error_code_result_r1))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_unlock_result(
		zcbor_state_t *state, struct unlock_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._unlock_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_unlock(
		zcbor_state_t *state, struct unlock *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._unlock_p1)))
	&& ((*result)._unlock_p1 >= 0)
	&& ((*result)._unlock_p1 <= 999999))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_bool_decode(state, (&(*result)._unlock_p2))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_lock_result(
		zcbor_state_t *state, struct lock_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._lock_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_lock_code_result(
		zcbor_state_t *state, struct set_lock_code_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._set_lock_code_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_lock_code(
		zcbor_state_t *state, struct set_lock_code *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._set_lock_code_p1)))
	&& ((*result)._set_lock_code_p1 >= 0)
	&& ((*result)._set_lock_code_p1 <= 999999))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_check_lock_status_result(
		zcbor_state_t *state, struct check_lock_status_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._check_lock_status_result_r))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (zcbor_bool_decode(state, (&(*result)._check_lock_status_result_r1))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r2", tmp_str.len = sizeof("r2") - 1, &tmp_str)))))
	&& (zcbor_bool_decode(state, (&(*result)._check_lock_status_result_r2))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_empty_map(
		zcbor_state_t *state, void *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_decode(state) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



bool cbor_decode_empty_map(
		const uint8_t *payload, size_t payload_len,
		void *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_empty_map(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_check_lock_status_result(
		const uint8_t *payload, size_t payload_len,
		struct check_lock_status_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_check_lock_status_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_set_lock_code(
		const uint8_t *payload, size_t payload_len,
		struct set_lock_code *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_set_lock_code(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_set_lock_code_result(
		const uint8_t *payload, size_t payload_len,
		struct set_lock_code_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_set_lock_code_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_lock_result(
		const uint8_t *payload, size_t payload_len,
		struct lock_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_lock_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_unlock(
		const uint8_t *payload, size_t payload_len,
		struct unlock *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_unlock(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_unlock_result(
		const uint8_t *payload, size_t payload_len,
		struct unlock_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_unlock_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_get_unlock_error_code_result(
		const uint8_t *payload, size_t payload_len,
		struct get_unlock_error_code_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_unlock_error_code_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_set_parameter(
		const uint8_t *payload, size_t payload_len,
		struct set_parameter *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_set_parameter(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_set_parameter_result(
		const uint8_t *payload, size_t payload_len,
		struct set_parameter_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_set_parameter_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_get_parameter(
		const uint8_t *payload, size_t payload_len,
		struct get_parameter *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_parameter(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_get_parameter_result(
		const uint8_t *payload, size_t payload_len,
		struct get_parameter_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_parameter_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_get_api_version(
		const uint8_t *payload, size_t payload_len,
		void *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_api_version(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_get_api_version_result(
		const uint8_t *payload, size_t payload_len,
		struct get_api_version_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_api_version_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_get_indices(
		const uint8_t *payload, size_t payload_len,
		void *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_indices(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_get_indices_result(
		const uint8_t *payload, size_t payload_len,
		struct get_indices_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_indices_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_get_entry_details(
		const uint8_t *payload, size_t payload_len,
		struct get_entry_details *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_entry_details(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_get_entry_details_result(
		const uint8_t *payload, size_t payload_len,
		struct get_entry_details_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_entry_details_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_load_parameter_file(
		const uint8_t *payload, size_t payload_len,
		struct load_parameter_file *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_load_parameter_file(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_load_parameter_file_result(
		const uint8_t *payload, size_t payload_len,
		struct load_parameter_file_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_load_parameter_file_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_dump_parameter_file(
		const uint8_t *payload, size_t payload_len,
		struct dump_parameter_file *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_dump_parameter_file(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_dump_parameter_file_result(
		const uint8_t *payload, size_t payload_len,
		struct dump_parameter_file_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_dump_parameter_file_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_set_notify(
		const uint8_t *payload, size_t payload_len,
		struct set_notify *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_set_notify(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_set_notify_result(
		const uint8_t *payload, size_t payload_len,
		struct set_notify_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_set_notify_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_get_notify(
		const uint8_t *payload, size_t payload_len,
		struct get_notify *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_notify(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_get_notify_result(
		const uint8_t *payload, size_t payload_len,
		struct get_notify_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_notify_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_disable_notify(
		const uint8_t *payload, size_t payload_len,
		void *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_disable_notify(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_disable_notify_result(
		const uint8_t *payload, size_t payload_len,
		struct disable_notify_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_disable_notify_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_factory_reset(
		const uint8_t *payload, size_t payload_len,
		void *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_factory_reset(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_decode_factory_reset_result(
		const uint8_t *payload, size_t payload_len,
		struct factory_reset_result *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_factory_reset_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
