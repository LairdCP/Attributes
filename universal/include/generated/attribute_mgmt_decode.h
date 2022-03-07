/*
 * Generated using zcbor version 0.3.99
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef ATTRIBUTE_MGMT_DECODE_H__
#define ATTRIBUTE_MGMT_DECODE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"
#include "attribute_mgmt_types.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


bool cbor_decode_empty_map(
		const uint8_t *payload, size_t payload_len,
		void *result,
		size_t *payload_len_out);


bool cbor_decode_check_lock_status_result(
		const uint8_t *payload, size_t payload_len,
		struct check_lock_status_result *result,
		size_t *payload_len_out);


bool cbor_decode_set_lock_code(
		const uint8_t *payload, size_t payload_len,
		struct set_lock_code *result,
		size_t *payload_len_out);


bool cbor_decode_set_lock_code_result(
		const uint8_t *payload, size_t payload_len,
		struct set_lock_code_result *result,
		size_t *payload_len_out);


bool cbor_decode_lock_result(
		const uint8_t *payload, size_t payload_len,
		struct lock_result *result,
		size_t *payload_len_out);


bool cbor_decode_unlock(
		const uint8_t *payload, size_t payload_len,
		struct unlock *result,
		size_t *payload_len_out);


bool cbor_decode_unlock_result(
		const uint8_t *payload, size_t payload_len,
		struct unlock_result *result,
		size_t *payload_len_out);


bool cbor_decode_get_unlock_error_code_result(
		const uint8_t *payload, size_t payload_len,
		struct get_unlock_error_code_result *result,
		size_t *payload_len_out);


bool cbor_decode_set_parameter(
		const uint8_t *payload, size_t payload_len,
		struct set_parameter *result,
		size_t *payload_len_out);


bool cbor_decode_set_parameter_result(
		const uint8_t *payload, size_t payload_len,
		struct set_parameter_result *result,
		size_t *payload_len_out);


bool cbor_decode_get_parameter(
		const uint8_t *payload, size_t payload_len,
		struct get_parameter *result,
		size_t *payload_len_out);


bool cbor_decode_get_parameter_result(
		const uint8_t *payload, size_t payload_len,
		struct get_parameter_result *result,
		size_t *payload_len_out);


bool cbor_decode_get_api_version(
		const uint8_t *payload, size_t payload_len,
		void *result,
		size_t *payload_len_out);


bool cbor_decode_get_api_version_result(
		const uint8_t *payload, size_t payload_len,
		struct get_api_version_result *result,
		size_t *payload_len_out);


bool cbor_decode_get_indices(
		const uint8_t *payload, size_t payload_len,
		void *result,
		size_t *payload_len_out);


bool cbor_decode_get_indices_result(
		const uint8_t *payload, size_t payload_len,
		struct get_indices_result *result,
		size_t *payload_len_out);


bool cbor_decode_get_entry_details(
		const uint8_t *payload, size_t payload_len,
		struct get_entry_details *result,
		size_t *payload_len_out);


bool cbor_decode_get_entry_details_result(
		const uint8_t *payload, size_t payload_len,
		struct get_entry_details_result *result,
		size_t *payload_len_out);


bool cbor_decode_load_parameter_file(
		const uint8_t *payload, size_t payload_len,
		struct load_parameter_file *result,
		size_t *payload_len_out);


bool cbor_decode_load_parameter_file_result(
		const uint8_t *payload, size_t payload_len,
		struct load_parameter_file_result *result,
		size_t *payload_len_out);


bool cbor_decode_dump_parameter_file(
		const uint8_t *payload, size_t payload_len,
		struct dump_parameter_file *result,
		size_t *payload_len_out);


bool cbor_decode_dump_parameter_file_result(
		const uint8_t *payload, size_t payload_len,
		struct dump_parameter_file_result *result,
		size_t *payload_len_out);


bool cbor_decode_set_notify(
		const uint8_t *payload, size_t payload_len,
		struct set_notify *result,
		size_t *payload_len_out);


bool cbor_decode_set_notify_result(
		const uint8_t *payload, size_t payload_len,
		struct set_notify_result *result,
		size_t *payload_len_out);


bool cbor_decode_get_notify(
		const uint8_t *payload, size_t payload_len,
		struct get_notify *result,
		size_t *payload_len_out);


bool cbor_decode_get_notify_result(
		const uint8_t *payload, size_t payload_len,
		struct get_notify_result *result,
		size_t *payload_len_out);


bool cbor_decode_disable_notify(
		const uint8_t *payload, size_t payload_len,
		void *result,
		size_t *payload_len_out);


bool cbor_decode_disable_notify_result(
		const uint8_t *payload, size_t payload_len,
		struct disable_notify_result *result,
		size_t *payload_len_out);


bool cbor_decode_factory_reset(
		const uint8_t *payload, size_t payload_len,
		void *result,
		size_t *payload_len_out);


bool cbor_decode_factory_reset_result(
		const uint8_t *payload, size_t payload_len,
		struct factory_reset_result *result,
		size_t *payload_len_out);


#endif /* ATTRIBUTE_MGMT_DECODE_H__ */
