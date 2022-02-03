/**
 * @file attr_custom_validator.c
 * @brief
 *
 * Copyright (c) 2022 Laird Connectivity
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
#include "attr_custom_validator.h"
#include "attr_validator.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(attrval, CONFIG_ATTR_VALID_LOG_LEVEL);

/******************************************************************************/
/* Global Constants, Macros and Type Definitions                              */
/******************************************************************************/
#define CHECK_ENTRY() __ASSERT(entry != NULL, "Invalid Entry (index)");

/******************************************************************************/
/* Global Data Definitions                                                    */
/******************************************************************************/
extern const struct attr_table_entry ATTR_TABLE[ATTR_TABLE_SIZE];

extern atomic_t attr_modified[];

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
int av_tx_power(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	ARG_UNUSED(vlen);
	CHECK_ENTRY();
	int32_t value = *((int32_t *)pv);

	/* Values supported by nRF52840 */
	bool valid = false;
	switch (value) {
	case -40:
	case -20:
	case -16:
	case -12:
	case -8:
	case -4:
	case 0:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		valid = true;
		break;

	default:
		valid = false;
		break;
	}

	if (valid) {
		if (do_write && value != *((int32_t *)entry->pData)) {
			atomic_set_bit(attr_modified, attr_table_index(entry));
			*((int32_t *)entry->pData) = value;
		}

		return 1;
	}

	return 0;
}

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
int av_magsimen(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	int r = 0;
	bool last_enable_state;
	bool enable_state;
	const ate_t *attribute_entry;
	bool initial_switch_state;

	/* If do_write is set, this is the second call of this function so we
	 * can go ahead and apply changes
	 */
	if (do_write) {
		/* Get the current simulation state so we can check for a
		 * change in status.
		 */
		attribute_entry = attr_map(ATTR_ID_mag_switch_simulated);
		last_enable_state = *((bool *)(attribute_entry->pData));
		/* Also the state requested to switch to */
		enable_state = *((bool *)(pv));
		/* Is simulation being enabled? */
		if ((!last_enable_state) && (enable_state)) {
			/* If so, store the live value to avoid glitching */
			attribute_entry = attr_map(ATTR_ID_magnet_state);
			initial_switch_state =
				*((bool *)(attribute_entry->pData));
			attribute_entry =
				attr_map(ATTR_ID_mag_switch_simulated_value);
			*((bool *)(attribute_entry->pData)) =
				initial_switch_state;
		}
		/* Update simulation status. */
		attribute_entry = attr_map(ATTR_ID_mag_switch_simulated);
		/* If simulation is being enabled we update the status first so
		 * we can store the live switch value.
		 */
		if (enable_state) {
#warning "Bug #20959 Update to use framework messages"
#if 0
			UserInterfaceTask_UpdateMagSwitchSimulatedStatus(
				enable_state, last_enable_state);
#endif
			*((bool *)(attribute_entry->pData)) = enable_state;
		} else {
			/* If simulation is being disabled, we update the
			 * status last so the live switch value can be read.
			 */
			*((bool *)(attribute_entry->pData)) = enable_state;
#warning "Bug #20959 Update to use framework messages"
#if 0
			UserInterfaceTask_UpdateMagSwitchSimulatedStatus(
				enable_state, last_enable_state);
#endif
		}
	} else {
		/* If do_write is not set, this is the first call so we just
		 * call the standard type validator to check its content.
		 */
		r = av_bool(entry, pv, vlen, false);
	}

	return (r);
}

int av_magsim(const ate_t *const entry, void *pv, size_t vlen, bool do_write)
{
	int r = 0;
	bool last_simulated_value;
	const ate_t *attribute_entry;

	/* If do_write is set the data has been validated */
	if (do_write) {
		/* Get the current simulated value */
		attribute_entry =
			attr_map(ATTR_ID_mag_switch_simulated_value);
		last_simulated_value = *((bool *)(attribute_entry->pData));
		/* Update the data value */
		*((bool *)(attribute_entry->pData)) = *((bool *)(pv));
		/* Update the value in the User Interface */
#warning "Bug #20959 Update to use framework messages"
#if 0
		r = UserInterfaceTask_UpdateMagSwitchSimulatedValue(
			*((bool *)(pv)), last_simulated_value);
#endif
	} else {
		/* If do_write is not set, we just call the
		 * standard type validator to make sure the
		 * data is OK.
		 */
		r = av_bool(entry, pv, vlen, false);
	}

	return (r);
}

int av_block_downgrades(const ate_t *const entry, void *pv, size_t vlen,
			bool do_write)
{
	/* Same as UINT8 */
	ARG_UNUSED(vlen);
	int r = -EPERM;
	uint32_t value = (uint32_t)(*(uint8_t *)pv);

	if (((value >= entry->min.ux) && (value <= entry->max.ux)) ||
	    (entry->min.ux == entry->max.ux)) {
		if (value == 1 || value == *((uint8_t *)entry->pData)) {
			if (do_write && value != *((uint8_t *)entry->pData)) {
				atomic_set_bit(attr_modified,
					       attr_table_index(entry));
				*((uint8_t *)entry->pData) = value;
			}
			r = 0;
		} else {
			/* Cannot disable blocking downgrades once enabled */
			r = -EINVAL;
		}
	}

	return r;
}
