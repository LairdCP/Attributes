/**
 * @file shell_lock.c
 * @brief
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <shell/shell.h>
#include <init.h>
#include <stdio.h>
#include <stdlib.h>

#include "attr.h"
#include "attr_table.h"

/******************************************************************************/
/* Local Function Prototypes                                                  */
/******************************************************************************/
static int lock_lock_cmd(const struct shell *shell, size_t argc, char **argv);
static int lock_unlock_cmd(const struct shell *shell, size_t argc, char **argv);
static int lock_remove_cmd(const struct shell *shell, size_t argc, char **argv);
static int lock_check_cmd(const struct shell *shell, size_t argc, char **argv);
static int lock_set_cmd(const struct shell *shell, size_t argc, char **argv);
static int lock_error_cmd(const struct shell *shell, size_t argc, char **argv);

static int shell_lock_init(const struct device *device);

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
SHELL_STATIC_SUBCMD_SET_CREATE(sub_lock,
			       SHELL_CMD(lock, NULL, "Lock configuration",
					 lock_lock_cmd),
			       SHELL_CMD(unlock, NULL, "Unlock configuration "
					 "using <key>",
					 lock_unlock_cmd),
			       SHELL_CMD(remove, NULL, "Remove configuration "
					 "lock using <key> (optional if "
					 "configuration already unlocked)",
					 lock_remove_cmd),
			       SHELL_CMD(check, NULL, "Check status of "
					 "configuration lock", lock_check_cmd),
			       SHELL_CMD(set, NULL, "Set configuration lock key"
					 " to <key>", lock_set_cmd),
			       SHELL_CMD(error, NULL, "Show error of last lock "
					 "attempt (will clear error on output)",
					 lock_error_cmd),
			       SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(lock, &sub_lock, "Configuration lock", NULL);


/******************************************************************************/
// Local Constant, Macro and Type Definitions
/******************************************************************************/
#define LOCK_KEY_MAX_SIZE 6
#define LOCK_KEY_MIN_VALUE 0
#define LOCK_KEY_MAX_VALUE 999999
#define LOCK_KEY_FAILED_WAIT_TIME_SECONDS 3
#define LOCK_DEFAULT_VALUE 123456

BUILD_ASSERT(CONFIG_ATTR_INDEX_SETTINGS_PASSCODE <= ATTR_TABLE_MAX_ID);
BUILD_ASSERT(CONFIG_ATTR_INDEX_LOCK <= ATTR_TABLE_MAX_ID);
BUILD_ASSERT(CONFIG_ATTR_INDEX_LOCK_STATUS <= ATTR_TABLE_MAX_ID);
BUILD_ASSERT(CONFIG_ATTR_INDEX_SETTINGS_PASSCODE_STATUS <= ATTR_TABLE_MAX_ID);

/******************************************************************************/
/* Local Function Definitions                                                 */
/******************************************************************************/
static int lock_check_cmd(const struct shell *shell, size_t argc, char **argv)
{
	bool lock_enabled;
	bool lock_active = false;
	uint8_t lock_status;
	int r = 0;

	r = attr_get(CONFIG_ATTR_INDEX_LOCK, &lock_enabled,
		     sizeof(lock_enabled));

	if (r >= 0) {
		r = attr_get(CONFIG_ATTR_INDEX_LOCK_STATUS, &lock_status,
				  sizeof(lock_status));

		if (r >= 0) {
			if (lock_status == LOCK_STATUS_SETUP_ENGAGED) {
				lock_active = true;
			}

			/* Completed successfully so return success result
			 * code
			 */
			r = 0;
		}
	}

	if (r == 0) {
		if (lock_enabled == false) {
			shell_print(shell, "Configuration lock is disabled");
		} else {
			shell_print(shell, "Configuration lock is enabled and "
				    "is currently %s", (lock_active == true ?
					"engaged" : "disengaged"));
		}
	} else {
		shell_error(shell, "Configuration lock status is not known");
	}

	return r;
}

static int lock_set_cmd(const struct shell *shell, size_t argc, char **argv)
{
	uint32_t lock_code;
	int r = 0;

	if ((argc == 2) && (argv[1] != NULL)) {
		if (strlen(argv[1]) > LOCK_KEY_MAX_SIZE) {
			shell_error(shell, "Invalid configuraton lock key "
				    "provided, must be between %d - %d",
				    LOCK_KEY_MIN_VALUE,
				    LOCK_KEY_MAX_VALUE);
			r = -EINVAL;
		} else {
			if (attr_is_locked() == true) {
				shell_error(shell, "Cannot change lock key when"
					    " configuration is locked");
				r = -EPERM;
			}

			if (r == 0) {
				lock_code = strtoul(argv[1], NULL, 0);

				if (!(lock_code >= LOCK_KEY_MIN_VALUE &&
				      lock_code <= LOCK_KEY_MAX_VALUE)) {
					shell_error(shell, "Invalid configuration"
						    " lock key provided, must"
						    " be between %d - %d",
						    LOCK_KEY_MIN_VALUE,
						    LOCK_KEY_MAX_VALUE);
					r = -EINVAL;
				}
			}

			if (r == 0) {
				r = attr_set_uint32(
						CONFIG_ATTR_INDEX_SETTINGS_PASSCODE,
						lock_code);
			}

			if (r == 0) {
				r = attr_set_uint32(CONFIG_ATTR_INDEX_LOCK,
						    true);

			}

			if (r == 0) {
				/* This sets the lock ready to be engaged when
				 * the user disconnects, when the module is
				 * rebooted or when the user manually requests
				 * it with the lock command, but allows further
				 * configuration changes to the unit until then
				 */
				r = attr_set_uint32(CONFIG_ATTR_INDEX_LOCK_STATUS,
						    LOCK_STATUS_SETUP_DISENGAGED);
			}

			if (r == 0) {
				shell_print(shell, "Configuration lock key has "
					    "been set");
			}
		}
	} else {
		shell_error(shell, "Configuration lock key must be provided");
		r = -EINVAL;
	}

#if defined(CONFIG_SHELL_SELECTIVE_HISTORY)
	/* Do not save this command in the shell history */
	shell->history->skip_current_line = true;
#endif

	return r;
}

static int lock_lock_cmd(const struct shell *shell, size_t argc, char **argv)
{
	enum settings_passcode_status passCodeStatus =
					SETTINGS_PASSCODE_STATUS_UNDEFINED;
	int r = 0;

	if (attr_is_locked() == false) {
		/* Lock the settings */
		attr_set_uint32(CONFIG_ATTR_INDEX_LOCK, true);
		attr_set_uint32(CONFIG_ATTR_INDEX_LOCK_STATUS,
				    LOCK_STATUS_SETUP_ENGAGED);
		passCodeStatus = SETTINGS_PASSCODE_STATUS_VALID_CODE;

		/* Send feedback about the passcode */
		attr_set_uint32(CONFIG_ATTR_INDEX_SETTINGS_PASSCODE_STATUS,
				    passCodeStatus);

		shell_print(shell, "Configuration lock has been engaged");
	} else {
		shell_print(shell, "Configuration lock is already active");
		r = -EPERM;
	}

	return r;
}

static int lock_unlock_cmd(const struct shell *shell, size_t argc, char **argv)
{
	enum settings_passcode_status passCodeStatus =
					SETTINGS_PASSCODE_STATUS_UNDEFINED;
	uint32_t lock_code;
	uint32_t real_lock_code;
	int r = 0;

	if ((argc == 2) && (argv[1] != NULL)) {
		if (strlen(argv[1]) > LOCK_KEY_MAX_SIZE) {
			shell_error(shell, "Invalid configuration lock key "
				    "provided, must be between %d - %d",
				    LOCK_KEY_MIN_VALUE,
				    LOCK_KEY_MAX_VALUE);
			r = -EINVAL;
		} else {
			if (attr_is_locked() == false) {
				shell_error(shell, "Configuration is already "
					    "unlocked");
				r = -EPERM;
			} else {
				real_lock_code = attr_get_uint32(
					CONFIG_ATTR_INDEX_SETTINGS_PASSCODE,
					LOCK_DEFAULT_VALUE);
				lock_code = strtoul(argv[1], NULL, 0);

				/* Check if the passcode entered matches */
				if (real_lock_code == lock_code) {
					/* Unlock the settings */
					attr_set_uint32(
						CONFIG_ATTR_INDEX_LOCK_STATUS,
						LOCK_STATUS_SETUP_DISENGAGED);
					passCodeStatus =
						SETTINGS_PASSCODE_STATUS_VALID_CODE;
					shell_print(shell, "Configuration is "
						    "now unlocked");
				} else {
					passCodeStatus =
						SETTINGS_PASSCODE_STATUS_INVALID_CODE;
					shell_error(shell, "Invalid configuration"
						    " lock key provided");
					r = -EINVAL;

					/* Sleep for 3 seconds to slow down
					 * possible attempts
					 */
					k_sleep(K_SECONDS(
						LOCK_KEY_FAILED_WAIT_TIME_SECONDS));
				}

				/* Send feedback to APP about the passcode */
				attr_set_uint32(
					CONFIG_ATTR_INDEX_SETTINGS_PASSCODE_STATUS,
					passCodeStatus);
			}
		}
	} else {
		shell_error(shell, "Configuration lock key must be provided");
		r = -EINVAL;
	}

#if defined(CONFIG_SHELL_SELECTIVE_HISTORY)
	/* Do not save this command in the shell history */
	shell->history->skip_current_line = true;
#endif

	return r;
}

static int lock_remove_cmd(const struct shell *shell, size_t argc, char **argv)
{
	enum settings_passcode_status passCodeStatus =
					SETTINGS_PASSCODE_STATUS_UNDEFINED;
	uint32_t lock_code;
	uint32_t real_lock_code;
	int r = 0;

	if (attr_is_locked() == false) {
		attr_set_uint32(CONFIG_ATTR_INDEX_LOCK, false);
		attr_set_uint32(CONFIG_ATTR_INDEX_LOCK_STATUS,
				    LOCK_STATUS_NOT_SETUP);
	} else {
		/* Configuration is locked, require lock key */
		if ((argc == 2) && (argv[1] != NULL)) {
			if (strlen(argv[1]) > LOCK_KEY_MAX_SIZE) {
				shell_error(shell, "Invalid configuration lock "
					    "key provided, must be between "
					    "%d - %d", LOCK_KEY_MIN_VALUE,
					    LOCK_KEY_MAX_VALUE);
				r = -EINVAL;
			} else {
				real_lock_code = attr_get_uint32(
					CONFIG_ATTR_INDEX_SETTINGS_PASSCODE,
					LOCK_DEFAULT_VALUE);
				lock_code = strtoul(argv[1], NULL, 0);

				if (!(lock_code >= LOCK_KEY_MIN_VALUE &&
				      lock_code <= LOCK_KEY_MAX_VALUE)) {
					shell_error(shell, "Invalid configuration"
						    " lock key provided, must be "
						    "between %d - %d",
						    LOCK_KEY_MIN_VALUE,
						    LOCK_KEY_MAX_VALUE);
				} else {
					/* Check if the passcode entered
					 * matches
					 */
					if (real_lock_code == lock_code) {
						attr_set_uint32(
							CONFIG_ATTR_INDEX_LOCK, false);
						attr_set_uint32(
							CONFIG_ATTR_INDEX_LOCK_STATUS,
							LOCK_STATUS_NOT_SETUP);

						shell_print(shell, "Configuration"
							    " lock has been removed"
							    " from");
					} else {
						passCodeStatus =
							SETTINGS_PASSCODE_STATUS_INVALID_CODE;
						shell_error(shell, "Invalid configuration"
							    " lock key provided");
						r = -EINVAL;

						/* Sleep for 3 seconds to slow
						 * down possible attempts
						 */
						k_sleep(K_SECONDS(
							LOCK_KEY_FAILED_WAIT_TIME_SECONDS));

						/* Send feedback to APP about
						 * the passcode
						 */
						attr_set_uint32(
							CONFIG_ATTR_INDEX_SETTINGS_PASSCODE_STATUS,
							passCodeStatus);
					}
				}
			}
		} else {
			shell_error(shell, "Configuration lock key must be provided");
			r = -EINVAL;
		}
	}

#if defined(CONFIG_SHELL_SELECTIVE_HISTORY)
	/* Do not save this command in the shell history */
	shell->history->skip_current_line = true;
#endif

	return r;
}

static int lock_error_cmd(const struct shell *shell, size_t argc, char **argv)
{
	enum settings_passcode_status passCodeStatus =
					SETTINGS_PASSCODE_STATUS_UNDEFINED;
	int r = 0;

	r = attr_get(CONFIG_ATTR_INDEX_SETTINGS_PASSCODE_STATUS,
		     &passCodeStatus,
		     sizeof(passCodeStatus));

	if (r >= 0) {
		/* Clear status */
		attr_set_uint32(CONFIG_ATTR_INDEX_SETTINGS_PASSCODE_STATUS,
				    SETTINGS_PASSCODE_STATUS_UNDEFINED);

		shell_print(shell, "Lock last error: %d (%s)", passCodeStatus,
			    (passCodeStatus == SETTINGS_PASSCODE_STATUS_VALID_CODE ?
				"Valid code" : (passCodeStatus ==
					SETTINGS_PASSCODE_STATUS_INVALID_CODE ?
					"Invalid code" : (passCodeStatus ==
						SETTINGS_PASSCODE_STATUS_UNDEFINED ?
						"No code entered" : "Unknown"))));
	} else {
		shell_error(shell, "Failed to get lock last error: %d", r);
	}

	return r;
}

static int shell_lock_init(const struct device *device)
{
	ARG_UNUSED(device);
	return 0;
}

SYS_INIT(shell_lock_init, POST_KERNEL, 0);
