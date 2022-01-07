/*
 * Generated using cddl_gen version 0.3.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "encode_lock.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool encode_repeated_empty_map_nil(
		cbor_state_t *state, const void *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;

	bool tmp_result = ((((tstrx_encode(state, ((tmp_str.value = "nil", tmp_str.len = sizeof("nil") - 1, &tmp_str)))))
	&& (nilx_put(state, NULL))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_get_parameter_result(
		cbor_state_t *state, const struct get_parameter_result *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 3) && (int_res = ((((tstrx_encode(state, ((tmp_str.value = "id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (uintx32_encode(state, (&(*input)._get_parameter_result_id))))
	&& (((tstrx_encode(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_bool) ? ((boolx_encode(state, (&(*input)._get_parameter_result_r1_bool))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_uint) ? ((uintx32_encode(state, (&(*input)._get_parameter_result_r1_uint))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_int) ? ((intx32_encode(state, (&(*input)._get_parameter_result_r1_int))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_tstr) ? ((tstrx_encode(state, (&(*input)._get_parameter_result_r1_tstr))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_bstr) ? ((bstrx_encode(state, (&(*input)._get_parameter_result_r1_bstr))))
	: (((*input)._get_parameter_result_r1_choice == _get_parameter_result_r1_nil) ? ((nilx_put(state, NULL)))
	: false)))))))
	&& (((tstrx_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_encode(state, (&(*input)._get_parameter_result_r))))), ((map_end_encode(state, 3)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_get_parameter(
		cbor_state_t *state, const struct get_parameter *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 1) && (int_res = ((((tstrx_encode(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (uintx32_encode(state, (&(*input)._get_parameter_p1))))), ((map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_set_parameter_result(
		cbor_state_t *state, const struct set_parameter_result *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 2) && (int_res = ((((tstrx_encode(state, ((tmp_str.value = "id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (uintx32_encode(state, (&(*input)._set_parameter_result_id))))
	&& (((tstrx_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_encode(state, (&(*input)._set_parameter_result_r))))), ((map_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_set_parameter(
		cbor_state_t *state, const struct set_parameter *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 2) && (int_res = ((((tstrx_encode(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (uintx32_encode(state, (&(*input)._set_parameter_p1))))
	&& (((tstrx_encode(state, ((tmp_str.value = "p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (((*input)._set_parameter_p2_choice == _set_parameter_p2_bool) ? ((boolx_encode(state, (&(*input)._set_parameter_p2_bool))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_uint) ? ((uintx32_encode(state, (&(*input)._set_parameter_p2_uint))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_int) ? ((intx32_encode(state, (&(*input)._set_parameter_p2_int))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_tstr) ? ((tstrx_encode(state, (&(*input)._set_parameter_p2_tstr))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_bstr) ? ((bstrx_encode(state, (&(*input)._set_parameter_p2_bstr))))
	: (((*input)._set_parameter_p2_choice == _set_parameter_p2_nil) ? ((nilx_put(state, NULL)))
	: false)))))))), ((map_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_get_unlock_error_code_result(
		cbor_state_t *state, const struct get_unlock_error_code_result *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 2) && (int_res = ((((tstrx_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_encode(state, (&(*input)._get_unlock_error_code_result_r))))
	&& (((tstrx_encode(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (intx32_encode(state, (&(*input)._get_unlock_error_code_result_r1))))), ((map_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_unlock_result(
		cbor_state_t *state, const struct unlock_result *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 1) && (int_res = ((((tstrx_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_encode(state, (&(*input)._unlock_result_r))))), ((map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_unlock(
		cbor_state_t *state, const struct unlock *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 2) && (int_res = ((((tstrx_encode(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& ((*input)._unlock_p1 >= 0)
	&& ((*input)._unlock_p1 <= 999999)
	&& (uintx32_encode(state, (&(*input)._unlock_p1))))
	&& (((tstrx_encode(state, ((tmp_str.value = "p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (boolx_encode(state, (&(*input)._unlock_p2))))), ((map_end_encode(state, 2)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_lock_result(
		cbor_state_t *state, const struct lock_result *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 1) && (int_res = ((((tstrx_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_encode(state, (&(*input)._lock_result_r))))), ((map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_set_lock_code_result(
		cbor_state_t *state, const struct set_lock_code_result *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 1) && (int_res = ((((tstrx_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_encode(state, (&(*input)._set_lock_code_result_r))))), ((map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_set_lock_code(
		cbor_state_t *state, const struct set_lock_code *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 1) && (int_res = ((((tstrx_encode(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& ((*input)._set_lock_code_p1 >= 0)
	&& ((*input)._set_lock_code_p1 <= 999999)
	&& (uintx32_encode(state, (&(*input)._set_lock_code_p1))))), ((map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_check_lock_status_result(
		cbor_state_t *state, const struct check_lock_status_result *input)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 3) && (int_res = ((((tstrx_encode(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_encode(state, (&(*input)._check_lock_status_result_r))))
	&& (((tstrx_encode(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (boolx_encode(state, (&(*input)._check_lock_status_result_r1))))
	&& (((tstrx_encode(state, ((tmp_str.value = "r2", tmp_str.len = sizeof("r2") - 1, &tmp_str)))))
	&& (boolx_encode(state, (&(*input)._check_lock_status_result_r2))))), ((map_end_encode(state, 3)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool encode_empty_map(
		cbor_state_t *state, const struct empty_map *input)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((map_start_encode(state, 1) && (int_res = (present_encode(&((*input)._empty_map_nil_present), (void *)encode_repeated_empty_map_nil, state, NULL)), ((map_end_encode(state, 1)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_encode_empty_map(
		uint8_t *payload, uint32_t payload_len,
		const struct empty_map *input,
		uint32_t *payload_len_out)
{
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[4];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[4];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = encode_get_parameter_result(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
