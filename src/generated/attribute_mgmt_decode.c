/*
 * Generated using zcbor version 0.5.1
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

static bool decode_repeated_load_parameter_file_p1(zcbor_state_t *state, struct load_parameter_file_p1 *result);
static bool decode_repeated_load_parameter_file_result_f(zcbor_state_t *state, struct load_parameter_file_result_f *result);
static bool decode_repeated_dump_parameter_file_p2(zcbor_state_t *state, struct dump_parameter_file_p2 *result);
static bool decode_repeated_dump_parameter_file_result_n(zcbor_state_t *state, struct dump_parameter_file_result_n *result);
static bool decode_repeated_get_parameter_result_r1(zcbor_state_t *state, struct get_parameter_result_r1_ *result);
static bool decode_disable_notify_result(zcbor_state_t *state, struct disable_notify_result *result);
static bool decode_disable_notify(zcbor_state_t *state, void *result);
static bool decode_get_notify_result(zcbor_state_t *state, struct get_notify_result *result);
static bool decode_get_notify(zcbor_state_t *state, struct get_notify *result);
static bool decode_set_notify_result(zcbor_state_t *state, struct set_notify_result *result);
static bool decode_set_notify(zcbor_state_t *state, struct set_notify *result);
static bool decode_factory_reset_result(zcbor_state_t *state, struct factory_reset_result *result);
static bool decode_factory_reset(zcbor_state_t *state, void *result);
static bool decode_dump_parameter_file_result(zcbor_state_t *state, struct dump_parameter_file_result *result);
static bool decode_dump_parameter_file(zcbor_state_t *state, struct dump_parameter_file *result);
static bool decode_load_parameter_file_result(zcbor_state_t *state, struct load_parameter_file_result *result);
static bool decode_load_parameter_file(zcbor_state_t *state, struct load_parameter_file *result);
static bool decode_get_parameter_result(zcbor_state_t *state, struct get_parameter_result *result);
static bool decode_get_parameter(zcbor_state_t *state, struct get_parameter *result);
static bool decode_set_parameter_result(zcbor_state_t *state, struct set_parameter_result *result);
static bool decode_set_parameter(zcbor_state_t *state, struct set_parameter *result);


static bool decode_repeated_load_parameter_file_p1(
		zcbor_state_t *state, struct load_parameter_file_p1 *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_tstr_decode(state, (&(*result).p1)))));

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
	&& (zcbor_tstr_decode(state, (&(*result).f)))));

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
	&& (zcbor_tstr_decode(state, (&(*result).p2)))));

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
	&& (zcbor_tstr_decode(state, (&(*result).n)))));

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
	&& (zcbor_union_start_code(state) && (int_res = ((((zcbor_bool_decode(state, (&(*result)._bool)))) && (((*result).r1_choice = _get_parameter_result_r1_bool) || 1))
	|| (((zcbor_uint64_decode(state, (&(*result)._uint64)))
	&& ((((((*result)._uint64 <= 18446744073709551615ULL)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) && (((*result).r1_choice = _get_parameter_result_r1__uint64) || 1))
	|| (((zcbor_int64_decode(state, (&(*result)._int64)))
	&& ((((((*result)._int64 >= -9223372036854775807LL)
	&& ((*result)._int64 <= 9223372036854775807LL)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) && (((*result).r1_choice = _get_parameter_result_r1__int64) || 1))
	|| (((zcbor_tstr_decode(state, (&(*result).tstr)))) && (((*result).r1_choice = _get_parameter_result_r1_tstr) || 1))
	|| (((zcbor_float_decode(state, (&(*result)._float)))) && (((*result).r1_choice = _get_parameter_result_r1_float) || 1))
	|| (((zcbor_bstr_decode(state, (&(*result).bstr)))) && (((*result).r1_choice = _get_parameter_result_r1_bstr) || 1))), zcbor_union_end_code(state), int_res))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_disable_notify_result(
		zcbor_state_t *state, struct disable_notify_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result).r))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

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

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result).id))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result).r))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_notify(
		zcbor_state_t *state, struct get_notify *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result).p1))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_notify_result(
		zcbor_state_t *state, struct set_notify_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result).id))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result).r))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_notify(
		zcbor_state_t *state, struct set_notify *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result).p1))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_bool_decode(state, (&(*result).p2))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_factory_reset_result(
		zcbor_state_t *state, struct factory_reset_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result).r))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

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

static bool decode_dump_parameter_file_result(
		zcbor_state_t *state, struct dump_parameter_file_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result).r))))
	&& zcbor_present_decode(&((*result).n_present), (zcbor_decoder_t *)decode_repeated_dump_parameter_file_result_n, state, (&(*result).n))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_dump_parameter_file(
		zcbor_state_t *state, struct dump_parameter_file *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result).p1))))
	&& zcbor_present_decode(&((*result).p2_present), (zcbor_decoder_t *)decode_repeated_dump_parameter_file_p2, state, (&(*result).p2))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_load_parameter_file_result(
		zcbor_state_t *state, struct load_parameter_file_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result).r))))
	&& zcbor_present_decode(&((*result).f_present), (zcbor_decoder_t *)decode_repeated_load_parameter_file_result_f, state, (&(*result).f))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_load_parameter_file(
		zcbor_state_t *state, struct load_parameter_file *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_decode(state) && ((zcbor_present_decode(&((*result).p1_present), (zcbor_decoder_t *)decode_repeated_load_parameter_file_p1, state, (&(*result).p1))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_parameter_result(
		zcbor_state_t *state, struct get_parameter_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result).r))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result).id))))
	&& zcbor_present_decode(&((*result).r1_present), (zcbor_decoder_t *)decode_repeated_get_parameter_result_r1, state, (&(*result).r1))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_parameter(
		zcbor_state_t *state, struct get_parameter *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result).p1))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_parameter_result(
		zcbor_state_t *state, struct set_parameter_result *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result).id))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result).r))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

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

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result).p1))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_union_start_code(state) && (int_res = ((((zcbor_bool_decode(state, (&(*result)._bool)))) && (((*result).p2_choice = _set_parameter_p2_bool) || 1))
	|| (((zcbor_uint64_decode(state, (&(*result)._uint64)))
	&& ((((((*result)._uint64 <= 18446744073709551615ULL)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) && (((*result).p2_choice = _set_parameter_p2__uint64) || 1))
	|| (((zcbor_int64_decode(state, (&(*result)._int64)))
	&& ((((((*result)._int64 >= -9223372036854775807LL)
	&& ((*result)._int64 <= 9223372036854775807LL)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) && (((*result).p2_choice = _set_parameter_p2__int64) || 1))
	|| (((zcbor_tstr_decode(state, (&(*result).tstr)))) && (((*result).p2_choice = _set_parameter_p2_tstr) || 1))
	|| (((zcbor_float_decode(state, (&(*result)._float)))) && (((*result).p2_choice = _set_parameter_p2_float) || 1))
	|| (((zcbor_bstr_decode(state, (&(*result).bstr)))) && (((*result).p2_choice = _set_parameter_p2_bstr) || 1))), zcbor_union_end_code(state), int_res)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



int cbor_decode_set_parameter(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_set_parameter_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_get_parameter(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_get_parameter_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_load_parameter_file(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_load_parameter_file_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_dump_parameter_file(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_dump_parameter_file_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_factory_reset(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_factory_reset_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_set_notify(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_set_notify_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_get_notify(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_get_notify_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_disable_notify(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_decode_disable_notify_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}
