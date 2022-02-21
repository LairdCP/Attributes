/**
 * @file attr_validator.c
 * @brief
 *
 * Copyright (c) 2021-2022 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <zephyr.h>
#include <device.h>
#include <string.h>
#include <ctype.h>

#include "attr_table.h"
#include "attr_table_private.h"

#include "attr_validator.h"

/******************************************************************************/
/* Global Constants, Macros and Type Definitions                              */
/******************************************************************************/
#define CHECK_ENTRY() __ASSERT(entry != NULL, "Invalid Entry (index)");

/******************************************************************************/
/* Global Data Definitions                                                    */
/******************************************************************************/
extern const struct attr_table_entry ATTR_TABLE[ATTR_TABLE_SIZE];

extern atomic_t attr_modified[];
extern atomic_t attr_unchanged[];

/******************************************************************************/
/* Local Function Definitions                                                 */
/******************************************************************************/
static void set_modified(const ate_t *const entry)
{
	atomic_set_bit(attr_modified, attr_table_index(entry));
}

/**
 * @brief If option is enabled, allow broadcasting/notification/printing of
 * attribute values that are unchanged during set operation.
 */
static void notify_on_unchanged(const ate_t *const entry)
{
	if (entry->flags & FLAGS_NOTIFY_IF_VALUE_UNCHANGED) {
		atomic_set_bit(attr_unchanged, attr_table_index(entry));
	}
}

static int range_check_uint(const ate_t *const entry, uint32_t value)
{
	if (entry->min.ux == entry->max.ux) {
		return 0;
	} else if (value < entry->min.ux) {
		return -ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
	} else if (value > entry->max.ux) {
		return -ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
	} else {
		return 0;
	}
}

static int range_check_signed(const ate_t *const entry, int32_t value)
{
	if (entry->min.sx == entry->max.sx) {
		return 0;
	} else if (value < entry->min.sx) {
		return -ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
	} else if (value > entry->max.sx) {
		return -ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
	} else {
		return 0;
	}
}

static int range_check_bool(const ate_t *const entry, uint32_t value)
{
	if (value == true || value == false) {
		return 0;
	} else {
		return -ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
	}
}

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
int av_string(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	CHECK_ENTRY();
	__ASSERT((entry->size == entry->max.ux + 1), "Unexpected string size");
	int r = 0;

	if (vlen < entry->min.ux) {
		r = -ATTR_WRITE_ERROR_PARAMETER_INVALID_LENGTH;
	} else if (vlen < entry->size) {
		/* Must be smaller to account for NULL character */
		size_t current_vlen = strlen(entry->pData);

		if (do_write) {
			if ((current_vlen != vlen) ||
			    (memcmp(entry->pData, pv, vlen) != 0)) {
				set_modified(entry);
				memset(entry->pData, 0, entry->size);
				strncpy(entry->pData, pv, vlen);
			} else {
				notify_on_unchanged(entry);
			}
		}
	} else {
		r = -ATTR_WRITE_ERROR_STRING_TOO_MANY_CHARACTERS;
	}
	return r;
}

int av_array(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	CHECK_ENTRY();
	int r = 0;

	/* For arrays, the entire value must be set. */
	if (entry->size == vlen) {
		if (do_write) {
			if (memcmp(entry->pData, pv, vlen) != 0) {
				set_modified(entry);
				memcpy(entry->pData, pv, vlen);
			} else {
				notify_on_unchanged(entry);
			}
		}
	} else {
		r = -ATTR_WRITE_ERROR_PARAMETER_INVALID_LENGTH;
	}
	return r;
}

int av_uint64(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	uint64_t value = *(uint64_t *)pv;
	uint64_t *current = (uint64_t *)entry->pData;

	if (do_write) {
		if (value != *current) {
			set_modified(entry);
			*current = value;
		} else {
			notify_on_unchanged(entry);
		}
	}
	return 0;
}

int av_uint32(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	uint32_t value = *(uint32_t *)pv;
	uint32_t *current = (uint32_t *)entry->pData;

	int r = range_check_uint(entry, value);
	if (r == 0) {
		if (do_write) {
			if (value != *current) {
				set_modified(entry);
				*current = value;
			} else {
				notify_on_unchanged(entry);
			}
		}
	}
	return r;
}

int av_uint16(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	uint32_t value = (uint32_t)(*(uint16_t *)pv);
	uint16_t *current = (uint16_t *)entry->pData;

	int r = range_check_uint(entry, value);
	if (r == 0) {
		if (do_write) {
			if (value != *current) {
				set_modified(entry);
				*current = (uint16_t)value;
			} else {
				notify_on_unchanged(entry);
			}
		}
	}
	return r;
}

int av_uint8(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	uint32_t value = (uint32_t)(*(uint8_t *)pv);
	uint8_t *current = (uint8_t *)entry->pData;

	int r = range_check_uint(entry, value);
	if (r == 0) {
		if (do_write) {
			if (value != *current) {
				set_modified(entry);
				*current = (uint8_t)value;
			} else {
				notify_on_unchanged(entry);
			}
		}
	}
	return r;
}

int av_bool(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	uint32_t value = (uint32_t)(*(bool *)pv);
	bool *current = (bool *)entry->pData;

	int r = range_check_bool(entry, value);
	if (r == 0) {
		if (do_write) {
			if (value != *current) {
				set_modified(entry);
				*current = (bool)value;
			} else {
				notify_on_unchanged(entry);
			}
		}
	}
	return r;
}

int av_int64(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	int64_t value = *(int64_t *)pv;
	int64_t *current = (int64_t *)entry->pData;

	if (do_write) {
		if (value != *current) {
			set_modified(entry);
			*current = value;
		} else {
			notify_on_unchanged(entry);
		}
	}
	return 0;
}

int av_int32(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	int32_t value = *(int32_t *)pv;
	int32_t *current = (int32_t *)entry->pData;

	int r = range_check_signed(entry, value);
	if (r == 0) {
		if (do_write) {
			if (value != *current) {
				set_modified(entry);
				*current = value;
			} else {
				notify_on_unchanged(entry);
			}
		}
	}
	return r;
}

int av_int16(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	int32_t value = (int32_t)(*(int16_t *)pv);
	int16_t *current = (int16_t *)entry->pData;

	int r = range_check_signed(entry, value);
	if (r == 0) {
		if (do_write) {
			if (value != *current) {
				set_modified(entry);
				*current = (int16_t)value;
			} else {
				notify_on_unchanged(entry);
			}
		}
		r = 0;
	}
	return r;
}

int av_int8(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	int32_t value = (int32_t)(*(int8_t *)pv);
	int8_t *current = (int8_t *)entry->pData;

	int r = range_check_signed(entry, value);
	if (r == 0) {
		if (do_write) {
			if (value != *current) {
				set_modified(entry);
				*current = (int8_t)value;
			} else {
				notify_on_unchanged(entry);
			}
		}
		r = 0;
	}
	return r;
}

int av_float(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	float value = *((float *)pv);
	float *current = (float *)entry->pData;

	int r = 0;
	if (entry->min.fx != entry->max.fx) {
		if (value < entry->min.fx) {
			r = -ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
		} else if (value > entry->max.fx) {
			r = -ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
		}
	}

	if (r == 0) {
		if (do_write) {
			if (value != *current) {
				set_modified(entry);
				*current = value;
			} else {
				notify_on_unchanged(entry);
			}
		}
	}
	return r;
}

/**
 * @brief Control Point Validators
 * Don't check if value is the same because this is a control point.
 */
int av_cp32(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	uint32_t value = *(uint32_t *)pv;

	int r = range_check_uint(entry, value);
	if (r == 0) {
		if (do_write) {
			set_modified(entry);
			*((uint32_t *)entry->pData) = value;
		}
	}
	return r;
}

int av_cp16(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	uint32_t value = (uint32_t)(*(uint16_t *)pv);

	int r = range_check_uint(entry, value);
	if (r == 0) {
		if (do_write) {
			set_modified(entry);
			*((uint16_t *)entry->pData) = (uint16_t)value;
		}
	}
	return r;
}

int av_cp8(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	uint32_t value = (uint32_t)(*(uint8_t *)pv);

	int r = range_check_uint(entry, value);
	if (r == 0) {
		if (do_write) {
			set_modified(entry);
			*((uint8_t *)entry->pData) = (uint8_t)value;
		}
	}
	return r;
}

int av_cpi32(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	int32_t value = *(int32_t *)pv;

	int r = range_check_signed(entry, value);
	if (r == 0) {
		if (do_write) {
			set_modified(entry);
			*((int32_t *)entry->pData) = value;
		}
	}
	return r;
}

int av_cpi16(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	int32_t value = (int32_t)(*(int16_t *)pv);

	int r = range_check_signed(entry, value);
	if (r == 0) {
		if (do_write) {
			set_modified(entry);
			*((int16_t *)entry->pData) = (int16_t)value;
		}
	}
	return r;
}

int av_cpi8(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	int32_t value = (int32_t)(*(int8_t *)pv);

	int r = range_check_signed(entry, value);
	if (r == 0) {
		if (do_write) {
			set_modified(entry);
			*((int8_t *)entry->pData) = (int8_t)value;
		}
	}
	return r;
}

int av_cpb(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();

	uint32_t value = (uint32_t)(*(bool *)pv);

	int r = range_check_bool(entry, value);
	if (r == 0) {
		if (do_write) {
			set_modified(entry);
			*((bool *)entry->pData) = (bool)value;
		}
	}
	return r;
}
