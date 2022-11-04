/*
 * Generated using zcbor version 0.5.1
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


int cbor_encode_set_parameter(
		uint8_t *payload, size_t payload_len,
		const struct set_parameter *input,
		size_t *payload_len_out);


int cbor_encode_set_parameter_result(
		uint8_t *payload, size_t payload_len,
		const struct set_parameter_result *input,
		size_t *payload_len_out);


int cbor_encode_get_parameter(
		uint8_t *payload, size_t payload_len,
		const struct get_parameter *input,
		size_t *payload_len_out);


int cbor_encode_get_parameter_result(
		uint8_t *payload, size_t payload_len,
		const struct get_parameter_result *input,
		size_t *payload_len_out);


int cbor_encode_load_parameter_file(
		uint8_t *payload, size_t payload_len,
		const struct load_parameter_file *input,
		size_t *payload_len_out);


int cbor_encode_load_parameter_file_result(
		uint8_t *payload, size_t payload_len,
		const struct load_parameter_file_result *input,
		size_t *payload_len_out);


int cbor_encode_dump_parameter_file(
		uint8_t *payload, size_t payload_len,
		const struct dump_parameter_file *input,
		size_t *payload_len_out);


int cbor_encode_dump_parameter_file_result(
		uint8_t *payload, size_t payload_len,
		const struct dump_parameter_file_result *input,
		size_t *payload_len_out);


int cbor_encode_factory_reset(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out);


int cbor_encode_factory_reset_result(
		uint8_t *payload, size_t payload_len,
		const struct factory_reset_result *input,
		size_t *payload_len_out);


int cbor_encode_set_notify(
		uint8_t *payload, size_t payload_len,
		const struct set_notify *input,
		size_t *payload_len_out);


int cbor_encode_set_notify_result(
		uint8_t *payload, size_t payload_len,
		const struct set_notify_result *input,
		size_t *payload_len_out);


int cbor_encode_get_notify(
		uint8_t *payload, size_t payload_len,
		const struct get_notify *input,
		size_t *payload_len_out);


int cbor_encode_get_notify_result(
		uint8_t *payload, size_t payload_len,
		const struct get_notify_result *input,
		size_t *payload_len_out);


int cbor_encode_disable_notify(
		uint8_t *payload, size_t payload_len,
		const void *input,
		size_t *payload_len_out);


int cbor_encode_disable_notify_result(
		uint8_t *payload, size_t payload_len,
		const struct disable_notify_result *input,
		size_t *payload_len_out);


#endif /* ATTRIBUTE_MGMT_ENCODE_H__ */
