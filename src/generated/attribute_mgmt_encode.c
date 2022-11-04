/*
 * Generated using zcbor version 0.5.1
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

static bool encode_repeated_load_parameter_file_p1(zcbor_state_t *state, const struct load_parameter_file_p1 *input);
static bool encode_repeated_load_parameter_file_result_f(zcbor_state_t *state, const struct load_parameter_file_result_f *input);
static bool encode_repeated_dump_parameter_file_p2(zcbor_state_t *state, const struct dump_parameter_file_p2 *input);
static bool encode_repeated_dump_parameter_file_result_n(zcbor_state_t *state, const struct dump_parameter_file_result_n *input);
static bool encode_repeated_get_parameter_result_r1(zcbor_state_t *state, const struct get_parameter_result_r1_ *input);
static bool encode_disable_notify_result(zcbor_state_t *state, const struct disable_notify_result *input);
static bool encode_disable_notify(zcbor_state_t *state, const void *input);
static bool encode_get_notify_result(zcbor_state_t *state, const struct get_notify_result *input);
static bool encode_get_notify(zcbor_state_t *state, const struct get_notify *input);
static bool encode_set_notify_result(zcbor_state_t *state, const struct set_notify_result *input);
static bool encode_set_notify(zcbor_state_t *state, const struct set_notify *input);
static bool encode_factory_reset_result(zcbor_state_t *state, const struct factory_reset_result *input);
static bool encode_factory_reset(zcbor_state_t *state, const void *input);
static bool encode_dump_parameter_file_result(zcbor_state_t *state, const struct dump_parameter_file_result *input);
static bool encode_dump_parameter_file(zcbor_state_t *state, const struct dump_parameter_file *input);
static bool encode_load_parameter_file_result(zcbor_state_t *state, const struct load_parameter_file_result *input);
static bool encode_load_parameter_file(zcbor_state_t *state, const struct load_parameter_file *input);
static bool encode_get_parameter_result(zcbor_state_t *state, const struct get_parameter_result *input);
static bool encode_get_parameter(zcbor_state_t *state, const struct get_parameter *input);
static bool encode_set_parameter_result(zcbor_state_t *state, const struct set_parameter_result *input);
static bool encode_set_parameter(zcbor_state_t *state, const struct set_parameter *input);


static bool encode_repeated_load_parameter_file_p1(
		zcbor_state_t *state, const struct load_parameter_file_p1 *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_tstr_encode(state, (&(*input).p1)))));

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
	&& (zcbor_tstr_encode(state, (&(*input).f)))));

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
	&& (zcbor_tstr_encode(state, (&(*input).p2)))));

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
	&& (zcbor_tstr_encode(state, (&(*input).n)))));

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
	&& (((*input).r1_choice == _get_parameter_result_r1_bool) ? ((zcbor_bool_encode(state, (&(*input)._bool))))
	: (((*input).r1_choice == _get_parameter_result_r1__uint64) ? (((((((*input)._uint64 <= 18446744073709551615ULL)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint64_encode(state, (&(*input)._uint64))))
	: (((*input).r1_choice == _get_parameter_result_r1__int64) ? (((((((*input)._int64 >= -9223372036854775807LL)
	&& ((*input)._int64 <= 9223372036854775807LL)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_int64_encode(state, (&(*input)._int64))))
	: (((*input).r1_choice == _get_parameter_result_r1_tstr) ? ((zcbor_tstr_encode(state, (&(*input).tstr))))
	: (((*input).r1_choice == _get_parameter_result_r1_float) ? ((zcbor_float64_encode(state, (&(*input)._float))))
	: (((*input).r1_choice == _get_parameter_result_r1_bstr) ? ((zcbor_bstr_encode(state, (&(*input).bstr))))
	: false))))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_disable_notify_result(
		zcbor_state_t *state, const struct disable_notify_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 1) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input).r))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 1))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_disable_notify(
		zcbor_state_t *state, const void *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_encode(state, 0) && zcbor_map_end_encode(state, 0))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_notify_result(
		zcbor_state_t *state, const struct get_notify_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input).id))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input).r))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_notify(
		zcbor_state_t *state, const struct get_notify *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 1) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input).p1))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 1))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_notify_result(
		zcbor_state_t *state, const struct set_notify_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input).id))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input).r))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_notify(
		zcbor_state_t *state, const struct set_notify *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input).p1))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (zcbor_bool_encode(state, (&(*input).p2))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_factory_reset_result(
		zcbor_state_t *state, const struct factory_reset_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 1) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input).r))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 1))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_factory_reset(
		zcbor_state_t *state, const void *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_encode(state, 0) && zcbor_map_end_encode(state, 0))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_dump_parameter_file_result(
		zcbor_state_t *state, const struct dump_parameter_file_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input).r))))
	&& zcbor_present_encode(&((*input).n_present), (zcbor_encoder_t *)encode_repeated_dump_parameter_file_result_n, state, (&(*input).n))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_dump_parameter_file(
		zcbor_state_t *state, const struct dump_parameter_file *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input).p1))))
	&& zcbor_present_encode(&((*input).p2_present), (zcbor_encoder_t *)encode_repeated_dump_parameter_file_p2, state, (&(*input).p2))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_load_parameter_file_result(
		zcbor_state_t *state, const struct load_parameter_file_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input).r))))
	&& zcbor_present_encode(&((*input).f_present), (zcbor_encoder_t *)encode_repeated_load_parameter_file_result_f, state, (&(*input).f))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_load_parameter_file(
		zcbor_state_t *state, const struct load_parameter_file *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_encode(state, 1) && ((zcbor_present_encode(&((*input).p1_present), (zcbor_encoder_t *)encode_repeated_load_parameter_file_p1, state, (&(*input).p1))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 1))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_parameter_result(
		zcbor_state_t *state, const struct get_parameter_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 3) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input).r))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input).id))))
	&& zcbor_present_encode(&((*input).r1_present), (zcbor_encoder_t *)encode_repeated_get_parameter_result_r1, state, (&(*input).r1))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 3))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_get_parameter(
		zcbor_state_t *state, const struct get_parameter *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 1) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input).p1))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 1))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_parameter_result(
		zcbor_state_t *state, const struct set_parameter_result *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"id", tmp_str.len = sizeof("id") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input).id))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"r", tmp_str.len = sizeof("r") - 1, &tmp_str)))))
	&& (zcbor_int32_encode(state, (&(*input).r))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_set_parameter(
		zcbor_state_t *state, const struct set_parameter *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p1", tmp_str.len = sizeof("p1") - 1, &tmp_str)))))
	&& (zcbor_uint32_encode(state, (&(*input).p1))))
	&& (((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"p2", tmp_str.len = sizeof("p2") - 1, &tmp_str)))))
	&& (((*input).p2_choice == _set_parameter_p2_bool) ? ((zcbor_bool_encode(state, (&(*input)._bool))))
	: (((*input).p2_choice == _set_parameter_p2__uint64) ? (((((((*input)._uint64 <= 18446744073709551615ULL)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_uint64_encode(state, (&(*input)._uint64))))
	: (((*input).p2_choice == _set_parameter_p2__int64) ? (((((((*input)._int64 >= -9223372036854775807LL)
	&& ((*input)._int64 <= 9223372036854775807LL)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))
	&& (zcbor_int64_encode(state, (&(*input)._int64))))
	: (((*input).p2_choice == _set_parameter_p2_tstr) ? ((zcbor_tstr_encode(state, (&(*input).tstr))))
	: (((*input).p2_choice == _set_parameter_p2_float) ? ((zcbor_float64_encode(state, (&(*input)._float))))
	: (((*input).p2_choice == _set_parameter_p2_bstr) ? ((zcbor_bstr_encode(state, (&(*input).bstr))))
	: false)))))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



int cbor_encode_set_parameter(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_set_parameter_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_get_parameter(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_get_parameter_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_load_parameter_file(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_load_parameter_file_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_dump_parameter_file(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_dump_parameter_file_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_factory_reset(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_factory_reset_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_set_notify(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_set_notify_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_get_notify(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_get_notify_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_disable_notify(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_disable_notify_result(
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

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}
