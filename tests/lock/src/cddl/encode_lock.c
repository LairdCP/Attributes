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
#include "encode_lock.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_repeated_empty_map_nil(
		zcbor_state_t *state, const void *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = "nil", tmp_str.len = sizeof("nil") - 1, &tmp_str)))))
	&& (zcbor_nil_put(state, NULL))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_parameter_result(
		zcbor_state_t *state, const struct get_parameter_result *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 3) && (int_res = ((((zcbor_tstr_encode(state, ((tmp_str.value = "id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._get_parameter_result_id))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_bool) ? ((zcbor_bool_encode(state, (&(*input)._get_parameter_result_r1_bool))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_uint) ? ((zcbor_uint32_encode(state, (&(*input)._get_parameter_result_r1_uint))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_int) ? ((zcbor_int32_encode(state, (&(*input)._get_parameter_result_r1_int))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_tstr) ? ((zcbor_tstr_encode(state, (&(*input)._get_parameter_result_r1_tstr))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_bstr) ? ((zcbor_bstr_encode(state, (&(*input)._get_parameter_result_r1_bstr))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_nil) ? ((zcbor_nil_put(state, NULL)))
	: false)))))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._get_parameter_result_r))))), ((zcbor_map_end_encode(state, 3)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_parameter(
		zcbor_state_t *state, const struct get_parameter *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 1) && (int_res = ((((zcbor_tstr_encode(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._get_parameter_p1))))), ((zcbor_map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_parameter_result(
		zcbor_state_t *state, const struct set_parameter_result *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (int_res = ((((zcbor_tstr_encode(state, ((tmp_str.value = "id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._set_parameter_result_id))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._set_parameter_result_r))))), ((zcbor_map_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_parameter(
		zcbor_state_t *state, const struct set_parameter *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (int_res = ((((zcbor_tstr_encode(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input)._set_parameter_p1))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = "p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (((*input)._set_parameter_p2_choice == _set_parameter_p2_bool) ? ((zcbor_bool_encode(state, (&(*input)._set_parameter_p2_bool))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_uint) ? ((zcbor_uint32_encode(state, (&(*input)._set_parameter_p2_uint))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_int) ? ((zcbor_int32_encode(state, (&(*input)._set_parameter_p2_int))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_tstr) ? ((zcbor_tstr_encode(state, (&(*input)._set_parameter_p2_tstr))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_bstr) ? ((zcbor_bstr_encode(state, (&(*input)._set_parameter_p2_bstr))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_nil) ? ((zcbor_nil_put(state, NULL)))
	: false)))))))), ((zcbor_map_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_unlock_error_code_result(
		zcbor_state_t *state, const struct get_unlock_error_code_result *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (int_res = ((((zcbor_tstr_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._get_unlock_error_code_result_r))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._get_unlock_error_code_result_r1))))), ((zcbor_map_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_unlock_result(
		zcbor_state_t *state, const struct unlock_result *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 1) && (int_res = ((((zcbor_tstr_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._unlock_result_r))))), ((zcbor_map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_unlock(
		zcbor_state_t *state, const struct unlock *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (int_res = ((((zcbor_tstr_encode(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& ((*input)._unlock_p1 >= 0)
	&& ((*input)._unlock_p1 <= 999999)
	&& (zcbor_uint32_encode(state, (&(*input)._unlock_p1))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = "p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_bool_encode(state, (&(*input)._unlock_p2))))), ((zcbor_map_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_lock_result(
		zcbor_state_t *state, const struct lock_result *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 1) && (int_res = ((((zcbor_tstr_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._lock_result_r))))), ((zcbor_map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_lock_code_result(
		zcbor_state_t *state, const struct set_lock_code_result *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 1) && (int_res = ((((zcbor_tstr_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._set_lock_code_result_r))))), ((zcbor_map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_lock_code(
		zcbor_state_t *state, const struct set_lock_code *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 1) && (int_res = ((((zcbor_tstr_encode(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& ((*input)._set_lock_code_p1 >= 0)
	&& ((*input)._set_lock_code_p1 <= 999999)
	&& (zcbor_uint32_encode(state, (&(*input)._set_lock_code_p1))))), ((zcbor_map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_check_lock_status_result(
		zcbor_state_t *state, const struct check_lock_status_result *input)
{
	zcbor_print("%s\n", __func__);
	zcbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 3) && (int_res = ((((zcbor_tstr_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input)._check_lock_status_result_r))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (zcbor_bool_encode(state, (&(*input)._check_lock_status_result_r1))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = "r2", tmp_str.len = sizeof("r2") - 1, &tmp_str)))))
	&& (zcbor_bool_encode(state, (&(*input)._check_lock_status_result_r2))))), ((zcbor_map_end_encode(state, 3)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_empty_map(
		zcbor_state_t *state, const struct empty_map *input)
{
	zcbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_map_start_encode(state, 1) && (int_res = (zcbor_present_encode(&((*input)._empty_map_nil_present), (void *)encode_repeated_empty_map_nil, state, NULL)), ((zcbor_map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



bool cbor_encode_empty_map(
		uint8_t *payload, uint32_t payload_len,
		const struct empty_map *input,
		uint32_t *payload_len_out)
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
		uint8_t *payload, uint32_t payload_len,
		const struct check_lock_status_result *input,
		uint32_t *payload_len_out)
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
		uint8_t *payload, uint32_t payload_len,
		const struct set_lock_code *input,
		uint32_t *payload_len_out)
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
		uint8_t *payload, uint32_t payload_len,
		const struct set_lock_code_result *input,
		uint32_t *payload_len_out)
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
		uint8_t *payload, uint32_t payload_len,
		const struct lock_result *input,
		uint32_t *payload_len_out)
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
		uint8_t *payload, uint32_t payload_len,
		const struct unlock *input,
		uint32_t *payload_len_out)
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
		uint8_t *payload, uint32_t payload_len,
		const struct unlock_result *input,
		uint32_t *payload_len_out)
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
		uint8_t *payload, uint32_t payload_len,
		const struct get_unlock_error_code_result *input,
		uint32_t *payload_len_out)
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
		uint8_t *payload, uint32_t payload_len,
		const struct set_parameter *input,
		uint32_t *payload_len_out)
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
		uint8_t *payload, uint32_t payload_len,
		const struct set_parameter_result *input,
		uint32_t *payload_len_out)
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
		uint8_t *payload, uint32_t payload_len,
		const struct get_parameter *input,
		uint32_t *payload_len_out)
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
		uint8_t *payload, uint32_t payload_len,
		const struct get_parameter_result *input,
		uint32_t *payload_len_out)
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
