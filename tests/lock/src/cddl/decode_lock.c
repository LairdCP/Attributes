/*
 * Generated using cddl_gen version 0.3.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"
#include "decode_lock.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


static bool decode_repeated_empty_map_nil(
		cbor_state_t *state, void *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;

	bool tmp_result = ((((tstrx_expect(state, ((tmp_str.value = "nil", tmp_str.len = sizeof("nil") - 1, &tmp_str)))))
	&& (nilx_expect(state, NULL))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_get_parameter_result(
		cbor_state_t *state, struct get_parameter_result *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = ((((tstrx_expect(state, ((tmp_str.value = "id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (uintx32_decode(state, (&(*result)._get_parameter_result_id))))
	&& (((tstrx_expect(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (union_start_code(state) && (int_res = ((((boolx_decode(state, (&(*result)._get_parameter_result_r1_bool)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_bool) || 1))
	|| (((uintx32_decode(state, (&(*result)._get_parameter_result_r1_uint)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_uint) || 1))
	|| (((intx32_decode(state, (&(*result)._get_parameter_result_r1_int)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_int) || 1))
	|| (((tstrx_decode(state, (&(*result)._get_parameter_result_r1_tstr)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_tstr) || 1))
	|| (((bstrx_decode(state, (&(*result)._get_parameter_result_r1_bstr)))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_bstr) || 1))
	|| (((nilx_expect(state, NULL))) && (((*result)._get_parameter_result_r1_choice = _get_parameter_result_r1_nil) || 1))), union_end_code(state), int_res)))
	&& (((tstrx_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_decode(state, (&(*result)._get_parameter_result_r))))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_get_parameter(
		cbor_state_t *state, struct get_parameter *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = ((((tstrx_expect(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (uintx32_decode(state, (&(*result)._get_parameter_p1))))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_set_parameter_result(
		cbor_state_t *state, struct set_parameter_result *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = ((((tstrx_expect(state, ((tmp_str.value = "id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (uintx32_decode(state, (&(*result)._set_parameter_result_id))))
	&& (((tstrx_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_decode(state, (&(*result)._set_parameter_result_r))))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_set_parameter(
		cbor_state_t *state, struct set_parameter *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = ((((tstrx_expect(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (uintx32_decode(state, (&(*result)._set_parameter_p1))))
	&& (((tstrx_expect(state, ((tmp_str.value = "p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (union_start_code(state) && (int_res = ((((boolx_decode(state, (&(*result)._set_parameter_p2_bool)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_bool) || 1))
	|| (((uintx32_decode(state, (&(*result)._set_parameter_p2_uint)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_uint) || 1))
	|| (((intx32_decode(state, (&(*result)._set_parameter_p2_int)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_int) || 1))
	|| (((tstrx_decode(state, (&(*result)._set_parameter_p2_tstr)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_tstr) || 1))
	|| (((bstrx_decode(state, (&(*result)._set_parameter_p2_bstr)))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_bstr) || 1))
	|| (((nilx_expect(state, NULL))) && (((*result)._set_parameter_p2_choice = _set_parameter_p2_nil) || 1))), union_end_code(state), int_res)))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_get_unlock_error_code_result(
		cbor_state_t *state, struct get_unlock_error_code_result *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = ((((tstrx_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_decode(state, (&(*result)._get_unlock_error_code_result_r))))
	&& (((tstrx_expect(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (intx32_decode(state, (&(*result)._get_unlock_error_code_result_r1))))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_unlock_result(
		cbor_state_t *state, struct unlock_result *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = ((((tstrx_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_decode(state, (&(*result)._unlock_result_r))))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_lock_result(
		cbor_state_t *state, struct lock_result *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = ((((tstrx_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_decode(state, (&(*result)._lock_result_r))))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_unlock(
		cbor_state_t *state, struct unlock *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = ((((tstrx_expect(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (uintx32_decode(state, (&(*result)._unlock_p1)))
	&& ((*result)._unlock_p1 >= 0)
	&& ((*result)._unlock_p1 <= 999999))
	&& (((tstrx_expect(state, ((tmp_str.value = "p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (boolx_decode(state, (&(*result)._unlock_p2))))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_set_lock_code_result(
		cbor_state_t *state, struct set_lock_code_result *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = ((((tstrx_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_decode(state, (&(*result)._set_lock_code_result_r))))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_set_lock_code(
		cbor_state_t *state, struct set_lock_code *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = ((((tstrx_expect(state, ((tmp_str.value = "p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (uintx32_decode(state, (&(*result)._set_lock_code_p1)))
	&& ((*result)._set_lock_code_p1 >= 0)
	&& ((*result)._set_lock_code_p1 <= 999999))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_check_lock_status_result(
		cbor_state_t *state, struct check_lock_status_result *result)
{
	cbor_print("%s\n", __func__);
	cbor_string_type_t tmp_str;
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = ((((tstrx_expect(state, ((tmp_str.value = "r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (intx32_decode(state, (&(*result)._check_lock_status_result_r))))
	&& (((tstrx_expect(state, ((tmp_str.value = "r1", tmp_str.len = sizeof("r1") - 1, &tmp_str)))))
	&& (boolx_decode(state, (&(*result)._check_lock_status_result_r1))))
	&& (((tstrx_expect(state, ((tmp_str.value = "r2", tmp_str.len = sizeof("r2") - 1, &tmp_str)))))
	&& (boolx_decode(state, (&(*result)._check_lock_status_result_r2))))), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}

static bool decode_empty_map(
		cbor_state_t *state, struct empty_map *result)
{
	cbor_print("%s\n", __func__);
	bool int_res;

	bool tmp_result = (((map_start_decode(state) && (int_res = (present_decode(&((*result)._empty_map_nil_present), (void *)decode_repeated_empty_map_nil, state, NULL)), ((map_end_decode(state)) && int_res)))));

	if (!tmp_result)
		cbor_trace();

	return tmp_result;
}



bool cbor_decode_empty_map(
		const uint8_t *payload, uint32_t payload_len,
		struct empty_map *result,
		uint32_t *payload_len_out)
{
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = decode_set_lock_code_result(states, result);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = decode_unlock(states, result);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = decode_lock_result(states, result);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[4];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[3];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

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
	cbor_state_t states[4];

	new_state(states, sizeof(states) / sizeof(cbor_state_t), payload, payload_len, 1);

	bool ret = decode_get_parameter_result(states, result);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	return ret;
}
