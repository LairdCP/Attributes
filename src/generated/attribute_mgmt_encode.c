/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "attribute_mgmt_encode.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_repeated_get_entry_details_index(
		zcbor_state_t *state, const struct get_entry_details_index *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"index", tmp_str.len = sizeof("index") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._get_entry_details_index)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_load_parameter_file_p1(
		zcbor_state_t *state, const struct load_parameter_file_p1 *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_tstr_encode(state, (&(*input)._load_parameter_file_p1)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_load_parameter_file_result_f(
		zcbor_state_t *state, const struct load_parameter_file_result_f *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"f", tmp_str.len = sizeof("f") - 1, &tmp_str)))))
	&& (zcbor_tstr_encode(state, (&(*input)._load_parameter_file_result_f)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_dump_parameter_file_p2(
		zcbor_state_t *state, const struct dump_parameter_file_p2 *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_tstr_encode(state, (&(*input)._dump_parameter_file_p2)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_dump_parameter_file_result_n(
		zcbor_state_t *state, const struct dump_parameter_file_result_n *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"n", tmp_str.len = sizeof("n") - 1, &tmp_str)))))
	&& (zcbor_tstr_encode(state, (&(*input)._dump_parameter_file_result_n)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_get_parameter_result_r1(
		zcbor_state_t *state, const struct get_parameter_result_r1_ *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_bool) ? ((zcbor_bool_encode(state, (&(*input)._get_parameter_result_r1_bool))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1__uint64) ? (((*input)._get_parameter_result_r1__uint64 <= 18446744073709551615ULL)
	&& (zcbor_uint64_encode(state, (&(*input)._get_parameter_result_r1__uint64))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1__int64) ? (((*input)._get_parameter_result_r1__int64 >= -9223372036854775807LL)
	&& ((*input)._get_parameter_result_r1__int64 <= 9223372036854775807LL)
	&& (zcbor_int64_encode(state, (&(*input)._get_parameter_result_r1__int64))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_tstr) ? ((zcbor_tstr_encode(state, (&(*input)._get_parameter_result_r1_tstr))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_float) ? ((zcbor_float64_encode(state, (&(*input)._get_parameter_result_r1_float))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_bstr) ? ((zcbor_bstr_encode(state, (&(*input)._get_parameter_result_r1_bstr))))
	: false))))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_get_entry_details_result_id(
		zcbor_state_t *state, const struct get_entry_details_result_id *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._get_entry_details_result_id)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_get_entry_details_result_name(
		zcbor_state_t *state, const struct get_entry_details_result_name *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"name", tmp_str.len = sizeof("name") - 1, &tmp_str)))))
	&& (zcbor_tstr_encode(state, (&(*input)._get_entry_details_result_name)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_get_entry_details_result_size(
		zcbor_state_t *state, const struct get_entry_details_result_size *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"size", tmp_str.len = sizeof("size") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._get_entry_details_result_size)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_get_entry_details_result_type(
		zcbor_state_t *state, const struct get_entry_details_result_type *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"type", tmp_str.len = sizeof("type") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._get_entry_details_result_type)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_get_entry_details_result_flags(
		zcbor_state_t *state, const struct get_entry_details_result_flags *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"flags", tmp_str.len = sizeof("flags") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._get_entry_details_result_flags)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_get_entry_details_result_prepared(
		zcbor_state_t *state, const struct get_entry_details_result_prepared *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"prepared", tmp_str.len = sizeof("prepared") - 1, &tmp_str)))))
	&& (zcbor_bool_encode(state, (&(*input)._get_entry_details_result_prepared)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_get_entry_details_result_min(
		zcbor_state_t *state, const struct get_entry_details_result_min_ *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"min", tmp_str.len = sizeof("min") - 1, &tmp_str)))))
	&& (((*input)._get_entry_details_result_min_choice == _get_entry_details_result_min_bool) ? ((zcbor_bool_encode(state, (&(*input)._get_entry_details_result_min_bool))))
	: (((*input)._get_entry_details_result_min_choice == _get_entry_details_result_min__uint64) ? (((*input)._get_entry_details_result_min__uint64 <= 18446744073709551615ULL)
	&& (zcbor_uint64_encode(state, (&(*input)._get_entry_details_result_min__uint64))))
	: (((*input)._get_entry_details_result_min_choice == _get_entry_details_result_min__int64) ? (((*input)._get_entry_details_result_min__int64 >= -9223372036854775807LL)
	&& ((*input)._get_entry_details_result_min__int64 <= 9223372036854775807LL)
	&& (zcbor_int64_encode(state, (&(*input)._get_entry_details_result_min__int64))))
	: (((*input)._get_entry_details_result_min_choice == _get_entry_details_result_min_float) ? ((zcbor_float64_encode(state, (&(*input)._get_entry_details_result_min_float))))
	: false))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_repeated_get_entry_details_result_max(
		zcbor_state_t *state, const struct get_entry_details_result_max_ *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"max", tmp_str.len = sizeof("max") - 1, &tmp_str)))))
	&& (((*input)._get_entry_details_result_max_choice == _get_entry_details_result_max_bool) ? ((zcbor_bool_encode(state, (&(*input)._get_entry_details_result_max_bool))))
	: (((*input)._get_entry_details_result_max_choice == _get_entry_details_result_max__uint64) ? (((*input)._get_entry_details_result_max__uint64 <= 18446744073709551615ULL)
	&& (zcbor_uint64_encode(state, (&(*input)._get_entry_details_result_max__uint64))))
	: (((*input)._get_entry_details_result_max_choice == _get_entry_details_result_max__int64) ? (((*input)._get_entry_details_result_max__int64 >= -9223372036854775807LL)
	&& ((*input)._get_entry_details_result_max__int64 <= 9223372036854775807LL)
	&& (zcbor_int64_encode(state, (&(*input)._get_entry_details_result_max__int64))))
	: (((*input)._get_entry_details_result_max_choice == _get_entry_details_result_max_float) ? ((zcbor_float64_encode(state, (&(*input)._get_entry_details_result_max_float))))
	: false))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_factory_reset_result(
		zcbor_state_t *state, const struct factory_reset_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._factory_reset_result_r)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_factory_reset(
		zcbor_state_t *state, const void *input)
{
	zcbor_print("%s\r\n", __func__);

	return true;
}

static bool encode_disable_notify_result(
		zcbor_state_t *state, const struct disable_notify_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._disable_notify_result_r)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_disable_notify(
		zcbor_state_t *state, const void *input)
{
	zcbor_print("%s\r\n", __func__);

	return true;
}

static bool encode_get_notify_result(
		zcbor_state_t *state, const struct get_notify_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._get_notify_result_id))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._get_notify_result_r)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_notify(
		zcbor_state_t *state, const struct get_notify *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._get_notify_p1)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_notify_result(
		zcbor_state_t *state, const struct set_notify_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._set_notify_result_id))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._set_notify_result_r)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_notify(
		zcbor_state_t *state, const struct set_notify *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._set_notify_p1))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_bool_encode(state, (&(*input)._set_notify_p2)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_dump_parameter_file_result(
		zcbor_state_t *state, const struct dump_parameter_file_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._dump_parameter_file_result_r))))
	&& zcbor_present_encode(&((*input)._dump_parameter_file_result_n_present), (zcbor_encoder_t *)encode_repeated_dump_parameter_file_result_n, state, (&(*input)._dump_parameter_file_result_n)));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_dump_parameter_file(
		zcbor_state_t *state, const struct dump_parameter_file *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._dump_parameter_file_p1))))
	&& zcbor_present_encode(&((*input)._dump_parameter_file_p2_present), (zcbor_encoder_t *)encode_repeated_dump_parameter_file_p2, state, (&(*input)._dump_parameter_file_p2)));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_load_parameter_file_result(
		zcbor_state_t *state, const struct load_parameter_file_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._load_parameter_file_result_r))))
	&& zcbor_present_encode(&((*input)._load_parameter_file_result_f_present), (zcbor_encoder_t *)encode_repeated_load_parameter_file_result_f, state, (&(*input)._load_parameter_file_result_f)));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_load_parameter_file(
		zcbor_state_t *state, const struct load_parameter_file *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (zcbor_present_encode(&((*input)._load_parameter_file_p1_present), (zcbor_encoder_t *)encode_repeated_load_parameter_file_p1, state, (&(*input)._load_parameter_file_p1)));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_entry_details_result(
		zcbor_state_t *state, const struct get_entry_details_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"index", tmp_str.len = sizeof("index") - 1, &tmp_str)))))
	&& ((*input)._get_entry_details_result_index <= 65535)
	&& (zcbor_uint32_encode(state, (&(*input)._get_entry_details_result_index))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"res", tmp_str.len = sizeof("res") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._get_entry_details_result_res))))
	&& zcbor_present_encode(&((*input)._get_entry_details_result_id_present), (zcbor_encoder_t *)encode_repeated_get_entry_details_result_id, state, (&(*input)._get_entry_details_result_id))
	&& zcbor_present_encode(&((*input)._get_entry_details_result_name_present), (zcbor_encoder_t *)encode_repeated_get_entry_details_result_name, state, (&(*input)._get_entry_details_result_name))
	&& zcbor_present_encode(&((*input)._get_entry_details_result_size_present), (zcbor_encoder_t *)encode_repeated_get_entry_details_result_size, state, (&(*input)._get_entry_details_result_size))
	&& zcbor_present_encode(&((*input)._get_entry_details_result_type_present), (zcbor_encoder_t *)encode_repeated_get_entry_details_result_type, state, (&(*input)._get_entry_details_result_type))
	&& zcbor_present_encode(&((*input)._get_entry_details_result_flags_present), (zcbor_encoder_t *)encode_repeated_get_entry_details_result_flags, state, (&(*input)._get_entry_details_result_flags))
	&& zcbor_present_encode(&((*input)._get_entry_details_result_prepared_present), (zcbor_encoder_t *)encode_repeated_get_entry_details_result_prepared, state, (&(*input)._get_entry_details_result_prepared))
	&& zcbor_present_encode(&((*input)._get_entry_details_result_min_present), (zcbor_encoder_t *)encode_repeated_get_entry_details_result_min, state, (&(*input)._get_entry_details_result_min))
	&& zcbor_present_encode(&((*input)._get_entry_details_result_max_present), (zcbor_encoder_t *)encode_repeated_get_entry_details_result_max, state, (&(*input)._get_entry_details_result_max)));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_entry_details(
		zcbor_state_t *state, const struct get_entry_details *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"method", tmp_str.len = sizeof("method") - 1, &tmp_str)))))
	&& (((*input)._get_entry_details_method_choice == _get_entry_details_method_index) ? ((zcbor_uint32_put(state, (0))))
	: (((*input)._get_entry_details_method_choice == _get_entry_details_method_first) ? ((zcbor_uint32_put(state, (1))))
	: (((*input)._get_entry_details_method_choice == _get_entry_details_method_last) ? ((zcbor_uint32_put(state, (2))))
	: (((*input)._get_entry_details_method_choice == _get_entry_details_method_next) ? ((zcbor_uint32_put(state, (3))))
	: (((*input)._get_entry_details_method_choice == _get_entry_details_method_previous) ? ((zcbor_uint32_put(state, (4))))
	: false))))))
	&& zcbor_present_encode(&((*input)._get_entry_details_index_present), (zcbor_encoder_t *)encode_repeated_get_entry_details_index, state, (&(*input)._get_entry_details_index)));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_indices_result(
		zcbor_state_t *state, const struct get_indices_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"table_size", tmp_str.len = sizeof("table_size") - 1, &tmp_str)))))
	&& ((*input)._get_indices_result_table_size <= 65535)
	&& (zcbor_uint32_encode(state, (&(*input)._get_indices_result_table_size))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"min_id", tmp_str.len = sizeof("min_id") - 1, &tmp_str)))))
	&& ((*input)._get_indices_result_min_id <= 65535)
	&& (zcbor_uint32_encode(state, (&(*input)._get_indices_result_min_id))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"max_id", tmp_str.len = sizeof("max_id") - 1, &tmp_str)))))
	&& ((*input)._get_indices_result_max_id <= 65535)
	&& (zcbor_uint32_encode(state, (&(*input)._get_indices_result_max_id)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_indices(
		zcbor_state_t *state, const void *input)
{
	zcbor_print("%s\r\n", __func__);

	return true;
}

static bool encode_get_api_version_result(
		zcbor_state_t *state, const struct get_api_version_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"api_version", tmp_str.len = sizeof("api_version") - 1, &tmp_str)))))
	&& (zcbor_tstr_encode(state, (&(*input)._get_api_version_result_api_version)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_api_version(
		zcbor_state_t *state, const void *input)
{
	zcbor_print("%s\r\n", __func__);

	return true;
}

static bool encode_get_parameter_result(
		zcbor_state_t *state, const struct get_parameter_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&( * input)._get_parameter_result_r))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, & tmp_str)))))
	&& (zcbor_uint32_encode(state, (&( * input)._get_parameter_result_id))))
	&& zcbor_present_encode(&((*input)._get_parameter_result_r1_present), (zcbor_encoder_t *) encode_repeated_get_parameter_result_r1, state, (&(*input)._get_parameter_result_r1)));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_parameter(
		zcbor_state_t *state, const struct get_parameter *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._get_parameter_p1)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_parameter_result(
		zcbor_state_t *state, const struct set_parameter_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._set_parameter_result_id))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._set_parameter_result_r)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_parameter(
		zcbor_state_t *state, const struct set_parameter *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._set_parameter_p1))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (((*input)._set_parameter_p2_choice == _set_parameter_p2_bool) ? ((zcbor_bool_encode(state, (&(*input)._set_parameter_p2_bool))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2__uint64) ? (((*input)._set_parameter_p2__uint64 <= 18446744073709551615ULL)
	&& (zcbor_uint64_encode(state, (&(*input)._set_parameter_p2__uint64))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2__int64) ? (((*input)._set_parameter_p2__int64 >= -9223372036854775807LL)
	&& ((*input)._set_parameter_p2__int64 <= 9223372036854775807LL)
	&& (zcbor_int64_encode(state, (&(*input)._set_parameter_p2__int64))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_tstr) ? ((zcbor_tstr_encode(state, (&(*input)._set_parameter_p2_tstr))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_float) ? ((zcbor_float64_encode(state, (&(*input)._set_parameter_p2_float))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_bstr) ? ((zcbor_bstr_encode(state, (&(*input)._set_parameter_p2_bstr))))
	: false))))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_unlock_error_code_result(
		zcbor_state_t *state, const struct get_unlock_error_code_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._get_unlock_error_code_result_r))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._get_unlock_error_code_result_r1)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_unlock_result(
		zcbor_state_t *state, const struct unlock_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._unlock_result_r)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_unlock(
		zcbor_state_t *state, const struct unlock *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& ((*input)._unlock_p1 >= 0)
	&& ((*input)._unlock_p1 <= 999999)
	&& (zcbor_uint32_encode(state, (&(*input)._unlock_p1))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_bool_encode(state, (&(*input)._unlock_p2)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_lock_result(
		zcbor_state_t *state, const struct lock_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._lock_result_r)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_lock_code_result(
		zcbor_state_t *state, const struct set_lock_code_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._set_lock_code_result_r)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_lock_code(
		zcbor_state_t *state, const struct set_lock_code *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& ((*input)._set_lock_code_p1 >= 0)
	&& ((*input)._set_lock_code_p1 <= 999999)
	&& (zcbor_uint32_encode(state, (&(*input)._set_lock_code_p1)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_check_lock_status_result(
		zcbor_state_t *state, const struct check_lock_status_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._check_lock_status_result_r))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (zcbor_bool_encode(state, (&(*input)._check_lock_status_result_r1))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r2", tmp_str.len = sizeof("r2") - 1, &tmp_str)))))
	&& (zcbor_bool_encode(state, (&(*input)._check_lock_status_result_r2)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_empty_map(
		zcbor_state_t *state, const void *input)
{
	zcbor_print("%s\r\n", __func__);

	return true;
}



bool cbor_encode_empty_map(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_empty_map(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_check_lock_status_result(
		uint8_t *payload, size_t payload_len,
		const struct check_lock_status_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_check_lock_status_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_set_lock_code(
		uint8_t *payload, size_t payload_len,
		const struct set_lock_code *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_set_lock_code(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_set_lock_code_result(
		uint8_t *payload, size_t payload_len,
		const struct set_lock_code_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_set_lock_code_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_lock_result(
		uint8_t *payload, size_t payload_len,
		const struct lock_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_lock_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_unlock(
		uint8_t *payload, size_t payload_len,
		const struct unlock *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_unlock(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_unlock_result(
		uint8_t *payload, size_t payload_len,
		const struct unlock_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_unlock_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_get_unlock_error_code_result(
		uint8_t *payload, size_t payload_len,
		const struct get_unlock_error_code_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_unlock_error_code_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_set_parameter(
		uint8_t *payload, size_t payload_len,
		const struct set_parameter *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_set_parameter(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_set_parameter_result(
		uint8_t *payload, size_t payload_len,
		const struct set_parameter_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_set_parameter_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_get_parameter(
		uint8_t *payload, size_t payload_len,
		const struct get_parameter *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_parameter(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_get_parameter_result(
		uint8_t *payload, size_t payload_len,
		const struct get_parameter_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_parameter_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_get_api_version(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_api_version(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_get_api_version_result(
		uint8_t *payload, size_t payload_len,
		const struct get_api_version_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_api_version_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_get_indices(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_indices(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_get_indices_result(
		uint8_t *payload, size_t payload_len,
		const struct get_indices_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_indices_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_get_entry_details(
		uint8_t *payload, size_t payload_len,
		const struct get_entry_details *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_entry_details(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_get_entry_details_result(
		uint8_t *payload, size_t payload_len,
		const struct get_entry_details_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_entry_details_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_load_parameter_file(
		uint8_t *payload, size_t payload_len,
		const struct load_parameter_file *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_load_parameter_file(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_load_parameter_file_result(
		uint8_t *payload, size_t payload_len,
		const struct load_parameter_file_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_load_parameter_file_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_dump_parameter_file(
		uint8_t *payload, size_t payload_len,
		const struct dump_parameter_file *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_dump_parameter_file(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_dump_parameter_file_result(
		uint8_t *payload, size_t payload_len,
		const struct dump_parameter_file_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_dump_parameter_file_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_set_notify(
		uint8_t *payload, size_t payload_len,
		const struct set_notify *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_set_notify(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_set_notify_result(
		uint8_t *payload, size_t payload_len,
		const struct set_notify_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_set_notify_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_get_notify(
		uint8_t *payload, size_t payload_len,
		const struct get_notify *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_notify(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_get_notify_result(
		uint8_t *payload, size_t payload_len,
		const struct get_notify_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_notify_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_disable_notify(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_disable_notify(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_disable_notify_result(
		uint8_t *payload, size_t payload_len,
		const struct disable_notify_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_disable_notify_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_factory_reset(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_factory_reset(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}


bool cbor_encode_factory_reset_result(
		uint8_t *payload, size_t payload_len,
		const struct factory_reset_result *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_factory_reset_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
