/*
 * Generated using cddl_gen version 0.3.99
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef ENCODE_LOCK_H__
#define ENCODE_LOCK_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_encode.h"
#include "lock_types.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


bool cbor_encode_empty_map(
		uint8_t *payload, uint32_t payload_len,
		const struct empty_map *input,
		uint32_t *payload_len_out);


bool cbor_encode_check_lock_status_result(
		uint8_t *payload, uint32_t payload_len,
		const struct check_lock_status_result *input,
		uint32_t *payload_len_out);


bool cbor_encode_set_lock_code(
		uint8_t *payload, uint32_t payload_len,
		const struct set_lock_code *input,
		uint32_t *payload_len_out);


bool cbor_encode_set_lock_code_result(
		uint8_t *payload, uint32_t payload_len,
		const struct set_lock_code_result *input,
		uint32_t *payload_len_out);


bool cbor_encode_lock_result(
		uint8_t *payload, uint32_t payload_len,
		const struct lock_result *input,
		uint32_t *payload_len_out);


bool cbor_encode_unlock(
		uint8_t *payload, uint32_t payload_len,
		const struct unlock *input,
		uint32_t *payload_len_out);


bool cbor_encode_unlock_result(
		uint8_t *payload, uint32_t payload_len,
		const struct unlock_result *input,
		uint32_t *payload_len_out);


bool cbor_encode_get_unlock_error_code_result(
		uint8_t *payload, uint32_t payload_len,
		const struct get_unlock_error_code_result *input,
		uint32_t *payload_len_out);


bool cbor_encode_set_parameter(
		uint8_t *payload, uint32_t payload_len,
		const struct set_parameter *input,
		uint32_t *payload_len_out);


bool cbor_encode_set_parameter_result(
		uint8_t *payload, uint32_t payload_len,
		const struct set_parameter_result *input,
		uint32_t *payload_len_out);


bool cbor_encode_get_parameter(
		uint8_t *payload, uint32_t payload_len,
		const struct get_parameter *input,
		uint32_t *payload_len_out);


bool cbor_encode_get_parameter_result(
		uint8_t *payload, uint32_t payload_len,
		const struct get_parameter_result *input,
		uint32_t *payload_len_out);


#endif /* ENCODE_LOCK_H__ */
