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
#include "decode_lock.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_repeated_empty_map_nil(
		zcbor_state_t *state, void *result)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;

	bool tmp_result = ((((zcbor_tstr_expect(state, ((tmp_str.value = "nil", tmp_str.len = sizeof("nil") - 1, &tmp_str)))))
	&& (zcbor_nil_expect(state, NULL))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_parameter_result(
		zcbor_state_t *state, struct get_parameter_result *result)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = "id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_parameter_result_id))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (zcbor_union_start_code(state) && (int_res = ((((zcbor_bool_decode(state, (&(*result)._get_parameter_result_r1_bool)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_bool) || 1))
	|| (((zcbor_uint32_decode(state, (&(*result)._get_parameter_result_r1_uint)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_uint) || 1))
	|| (((zcbor_int32_decode(state, (&(*result)._get_parameter_result_r1_int)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_int) || 1))
	|| (((zcbor_tstr_decode(state, (&(*result)._get_parameter_result_r1_tstr)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_tstr) || 1))
	|| (((zcbor_bstr_decode(state, (&(*result)._get_parameter_result_r1_bstr)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_bstr) || 1))
	|| (((zcbor_nil_expect(state, NULL))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_nil) || 1))), zcbor_union_end_code(state), int_res)))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._get_parameter_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_parameter(
		zcbor_state_t *state, struct get_parameter *result)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._get_parameter_p1))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_parameter_result(
		zcbor_state_t *state, struct set_parameter_result *result)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = "id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._set_parameter_result_id))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._set_parameter_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_parameter(
		zcbor_state_t *state, struct set_parameter *result)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._set_parameter_p1))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = "p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_union_start_code(state) && (int_res = ((((zcbor_bool_decode(state, (&(*result)._set_parameter_p2_bool)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_bool) || 1))
	|| (((zcbor_uint32_decode(state, (&(*result)._set_parameter_p2_uint)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_uint) || 1))
	|| (((zcbor_int32_decode(state, (&(*result)._set_parameter_p2_int)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_int) || 1))
	|| (((zcbor_tstr_decode(state, (&(*result)._set_parameter_p2_tstr)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_tstr) || 1))
	|| (((zcbor_bstr_decode(state, (&(*result)._set_parameter_p2_bstr)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_bstr) || 1))
	|| (((zcbor_nil_expect(state, NULL))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_nil) || 1))), zcbor_union_end_code(state), int_res)))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_get_unlock_error_code_result(
		zcbor_state_t *state, struct get_unlock_error_code_result *result)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._get_unlock_error_code_result_r))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._get_unlock_error_code_result_r1))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_unlock_result(
		zcbor_state_t *state, struct unlock_result *result)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._unlock_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_unlock(
		zcbor_state_t *state, struct unlock *result)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_decode(state, (&(*result)._unlock_p1)))
	&& ((*result)._unlock_p1 >= 0)
	&& ((*result)._unlock_p1 <= 999999))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = "p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_bool_decode(state, (&(*result)._unlock_p2))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_lock_result(
		zcbor_state_t *state, struct lock_result *result)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._lock_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_lock_code_result(
		zcbor_state_t *state, struct set_lock_code_result *result)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._set_lock_code_result_r))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_set_lock_code(
		zcbor_state_t *state, struct set_lock_code *result)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
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
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = ((((zcbor_tstr_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_decode(state, (&(*result)._check_lock_status_result_r))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (zcbor_bool_decode(state, (&(*result)._check_lock_status_result_r1))))
	&& (((zcbor_tstr_expect(state, ((tmp_str.value = "r2", tmp_str.len = sizeof("r2") - 1, &tmp_str)))))
	&& (zcbor_bool_decode(state, (&(*result)._check_lock_status_result_r2))))), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_empty_map(
		zcbor_state_t *state, struct empty_map *result)
{
	zcbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (int_res = (zcbor_present_decode(&((*result)._empty_map_nil_present), (void *)decode_repeated_empty_map_nil, state, NULL)), ((zcbor_map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



bool cbor_decode_empty_map(
		const uint8_t *payload, uint32_t payload_len,
		struct empty_map *result,
		uint32_t *payload_len_out)
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
		const uint8_t *payload, uint32_t payload_len,
		struct check_lock_status_result *result,
		uint32_t *payload_len_out)
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
		const uint8_t *payload, uint32_t payload_len,
		struct set_lock_code *result,
		uint32_t *payload_len_out)
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
		const uint8_t *payload, uint32_t payload_len,
		struct set_lock_code_result *result,
		uint32_t *payload_len_out)
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
		const uint8_t *payload, uint32_t payload_len,
		struct lock_result *result,
		uint32_t *payload_len_out)
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
		const uint8_t *payload, uint32_t payload_len,
		struct unlock *result,
		uint32_t *payload_len_out)
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
		const uint8_t *payload, uint32_t payload_len,
		struct unlock_result *result,
		uint32_t *payload_len_out)
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
		const uint8_t *payload, uint32_t payload_len,
		struct get_unlock_error_code_result *result,
		uint32_t *payload_len_out)
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
		const uint8_t *payload, uint32_t payload_len,
		struct set_parameter *result,
		uint32_t *payload_len_out)
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
		const uint8_t *payload, uint32_t payload_len,
		struct set_parameter_result *result,
		uint32_t *payload_len_out)
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
		const uint8_t *payload, uint32_t payload_len,
		struct get_parameter *result,
		uint32_t *payload_len_out)
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
		const uint8_t *payload, uint32_t payload_len,
		struct get_parameter_result *result,
		uint32_t *payload_len_out)
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
