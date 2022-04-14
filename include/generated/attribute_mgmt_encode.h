/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef ATTRIBUTE_MGMT_ENCODE_H__
#define ATTRIBUTE_MGMT_ENCODE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "attribute_mgmt_types.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


bool cbor_encode_empty_map(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out);


bool cbor_encode_check_lock_status_result(
		uint8_t *payload, size_t payload_len,
		const struct check_lock_status_result *input,
		size_t *payload_len_out);


bool cbor_encode_set_lock_code(
		uint8_t *payload, size_t payload_len,
		const struct set_lock_code *input,
		size_t *payload_len_out);


bool cbor_encode_set_lock_code_result(
		uint8_t *payload, size_t payload_len,
		const struct set_lock_code_result *input,
		size_t *payload_len_out);


bool cbor_encode_lock_result(
		uint8_t *payload, size_t payload_len,
		const struct lock_result *input,
		size_t *payload_len_out);


bool cbor_encode_unlock(
		uint8_t *payload, size_t payload_len,
		const struct unlock *input,
		size_t *payload_len_out);


bool cbor_encode_unlock_result(
		uint8_t *payload, size_t payload_len,
		const struct unlock_result *input,
		size_t *payload_len_out);


bool cbor_encode_get_unlock_error_code_result(
		uint8_t *payload, size_t payload_len,
		const struct get_unlock_error_code_result *input,
		size_t *payload_len_out);


bool cbor_encode_set_parameter(
		uint8_t *payload, size_t payload_len,
		const struct set_parameter *input,
		size_t *payload_len_out);


bool cbor_encode_set_parameter_result(
		uint8_t *payload, size_t payload_len,
		const struct set_parameter_result *input,
		size_t *payload_len_out);


bool cbor_encode_get_parameter(
		uint8_t *payload, size_t payload_len,
		const struct get_parameter *input,
		size_t *payload_len_out);


bool cbor_encode_get_parameter_result(
		uint8_t *payload, size_t payload_len,
		const struct get_parameter_result *input,
		size_t *payload_len_out);


bool cbor_encode_get_api_version(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out);


bool cbor_encode_get_api_version_result(
		uint8_t *payload, size_t payload_len,
		const struct get_api_version_result *input,
		size_t *payload_len_out);


bool cbor_encode_get_indices(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out);


bool cbor_encode_get_indices_result(
		uint8_t *payload, size_t payload_len,
		const struct get_indices_result *input,
		size_t *payload_len_out);


bool cbor_encode_get_entry_details(
		uint8_t *payload, size_t payload_len,
		const struct get_entry_details *input,
		size_t *payload_len_out);


bool cbor_encode_get_entry_details_result(
		uint8_t *payload, size_t payload_len,
		const struct get_entry_details_result *input,
		size_t *payload_len_out);


bool cbor_encode_load_parameter_file(
		uint8_t *payload, size_t payload_len,
		const struct load_parameter_file *input,
		size_t *payload_len_out);


bool cbor_encode_load_parameter_file_result(
		uint8_t *payload, size_t payload_len,
		const struct load_parameter_file_result *input,
		size_t *payload_len_out);


bool cbor_encode_dump_parameter_file(
		uint8_t *payload, size_t payload_len,
		const struct dump_parameter_file *input,
		size_t *payload_len_out);


bool cbor_encode_dump_parameter_file_result(
		uint8_t *payload, size_t payload_len,
		const struct dump_parameter_file_result *input,
		size_t *payload_len_out);


bool cbor_encode_set_notify(
		uint8_t *payload, size_t payload_len,
		const struct set_notify *input,
		size_t *payload_len_out);


bool cbor_encode_set_notify_result(
		uint8_t *payload, size_t payload_len,
		const struct set_notify_result *input,
		size_t *payload_len_out);


bool cbor_encode_get_notify(
		uint8_t *payload, size_t payload_len,
		const struct get_notify *input,
		size_t *payload_len_out);


bool cbor_encode_get_notify_result(
		uint8_t *payload, size_t payload_len,
		const struct get_notify_result *input,
		size_t *payload_len_out);


bool cbor_encode_disable_notify(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out);


bool cbor_encode_disable_notify_result(
		uint8_t *payload, size_t payload_len,
		const struct disable_notify_result *input,
		size_t *payload_len_out);


bool cbor_encode_factory_reset(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out);


bool cbor_encode_factory_reset_result(
		uint8_t *payload, size_t payload_len,
		const struct factory_reset_result *input,
		size_t *payload_len_out);


#endif /* ATTRIBUTE_MGMT_ENCODE_H__ */
