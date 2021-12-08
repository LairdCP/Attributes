/**
 * @file attr.c
 * @brief
 *
 * Copyright (c) 2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <logging/log.h>
LOG_MODULE_REGISTER(attr, 4);

#define LOG_SHOW LOG_INF

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <zephyr.h>
#include <init.h>
#include <stdio.h>
#include <logging/log_ctrl.h>
#include <sys/util.h>
#include <sys/crc.h>

#include "lcz_param_file.h"
#include "file_system_utilities.h"

#include "attr_table.h"
#include "attr_table_private.h"
#include "attr.h"

#ifdef CONFIG_ATTR_BROADCAST
#include "BufferPool.h"
#endif

/******************************************************************************/
/* Global Constants, Macros and type Definitions                              */
/******************************************************************************/
#define BREAK_ON_ERROR(x)                                                      \
	if (x < 0) {                                                           \
		break;                                                         \
	}

/* Sys init order isn't always respected */
#define TAKE_MUTEX(m)                                                          \
	while (!attr_initialized) {                                            \
		k_yield();                                                     \
	}                                                                      \
	k_mutex_lock(&m, K_FOREVER)

#define TAKE_MUTEX_NO_INIT(m) k_mutex_lock(&m, K_FOREVER)

#define GIVE_MUTEX(m) k_mutex_unlock(&m)

#define ATTR_ENTRY_DECL(x)                                                     \
	const struct attr_table_entry *const entry = attr_map(x);

#define ATTR_ABS_PATH                                                          \
	CONFIG_LCZ_PARAM_FILE_MOUNT_POINT "/" CONFIG_LCZ_PARAM_FILE_PATH       \
					  "/" CONFIG_ATTR_FILE_NAME

#define ATTR_QUIET_ABS_PATH CONFIG_FSU_MOUNT_POINT "/quiet.bin"

BUILD_ASSERT(CONFIG_LCZ_PARAM_FILE_INIT_PRIORITY < CONFIG_ATTR_INIT_PRIORITY,
	     "Invalid init priority");

#define LOG_ALT_USED() LOG_DBG("alt value used id [%u]: %d", id, r)

#define GET_ENUM_STRING(x) (entry->gsf == NULL) ? EMPTY_STRING : entry->gsf(x)

#define ATOMIC_ARRAY_SIZE(num_bits) (1 + ((num_bits)-1) / ATOMIC_BITS)

enum { DISABLE_NOTIFICATIONS = 0, ENABLE_NOTIFICATIONS = 1 };

static const char EMPTY_STRING[] = "";

/* Each feedback file entry consists of a 2 character id, an equals to sign
 * character, a two character error code and a new line character.
 */
#define ATTR_LOAD_FEEDBACK_ENTRY_SIZE 6

/* Malloc always wants to align on a 4 byte boundary */
#define ATTR_LOAD_FEEDBACK_ALIGN_SIZE 4

/* Time (in ms) between checks if a save is currently in progress */
#define ATTR_SAVE_EXECUTING_CHECK_TIME_MS 200

#ifdef CONFIG_ATTR_DEFERRED_SAVE
#define ATTR_SAVE_NOW true
#define ATTR_SAVE_LATER false
#else
#define ATTR_SAVE_NOW
#define ATTR_SAVE_LATER
#endif

/******************************************************************************/
/* Global Data Definitions                                                    */
/******************************************************************************/
extern const struct attr_table_entry ATTR_TABLE[ATTR_TABLE_SIZE];

ATOMIC_DEFINE(attr_modified, ATTR_TABLE_SIZE);

/******************************************************************************/
/* Local Data Definitions                                                     */
/******************************************************************************/
#ifdef CONFIG_ATTR_SHELL
static struct k_work work_show;
#endif

static ATOMIC_DEFINE(quiet, ATTR_TABLE_SIZE);
static ATOMIC_DEFINE(notify, ATTR_TABLE_SIZE);

static struct k_mutex attr_mutex;
static struct k_mutex attr_work_mutex;
static struct k_mutex attr_save_change_mutex;

#ifdef CONFIG_ATTR_DEFERRED_SAVE
static struct k_work_delayable attr_save_delayed_work;
#endif

static bool attr_initialized;

/******************************************************************************/
/* Local Function Prototypes                                                  */
/******************************************************************************/
static int set_internal(attr_id_t id, enum attr_type type, void *pv,
			size_t vlen, bool broadcast, bool *modified);

static int save_single(const ate_t *const entry);

#ifdef CONFIG_ATTR_DEFERRED_SAVE
static int save_attributes(bool immediately);
#else
static int save_attributes(void);
#endif

static void change_single(const ate_t *const entry, bool send_notifications);
static void change_handler(bool send_notifications);
static void notification_handler(attr_id_t id);

static int load_attributes(const char *fname, const char *feedback_path,
			   bool validate_first, bool mask_modified,
			   bool skip_non_writeable);

static int loader(param_kvp_t *kvp, char *fstr, size_t pairs, bool do_write,
		  bool mask_modified, uint16_t *error_count,
		  bool skip_non_writeable);

static int validate(const ate_t *const entry, enum attr_type type, void *pv,
		    size_t vlen);

static int attr_write(const ate_t *const entry, enum attr_type type, void *pv,
		      size_t vlen);

static void show(const ate_t *const entry);

static param_t convert_attr_type(attr_index_t index);
static size_t get_attr_length(attr_index_t index);
static bool is_dump_rw(attr_index_t index);
static bool is_dump_w(attr_index_t index);
static bool is_dump_ro(attr_index_t index);

static int prepare_for_read(const ate_t *const entry);

static bool is_writable(const ate_t *const entry);

static int64_t sign_extend64(const ate_t *const entry);

static int initialize_quiet(void);

static int attr_init(const struct device *device);

#ifdef CONFIG_ATTR_SHELL
static void sys_workq_show_handler(struct k_work *item);
#endif

extern void attr_table_initialize(void);
extern void attr_table_factory_reset(void);

static enum attr_write_error diagnose_parameter_write_error(param_kvp_t *kvp);
static enum attr_write_error diagnose_numeric_write_error(param_kvp_t *kvp,
							  enum attr_type Type);
static enum attr_write_error diagnose_string_write_error(param_kvp_t *kvp);

static int build_empty_feedback_file(const char *feedback_path);
static int build_feedback_file(const char *feedback_path, char *fstr,
			       param_kvp_t *kvp, size_t pairs);

#ifdef CONFIG_ATTR_DEFERRED_SAVE
static void save_attributes_work(struct k_work *item);
#endif

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
SYS_INIT(attr_init, APPLICATION, CONFIG_ATTR_INIT_PRIORITY);

int attr_factory_reset(void)
{
	fsu_delete_abs(ATTR_QUIET_ABS_PATH);
	attr_table_factory_reset();
	return save_attributes(ATTR_SAVE_NOW);
}

enum attr_type attr_get_type(attr_id_t id)
{
	ATTR_ENTRY_DECL(id);

	if (entry != NULL) {
		return entry->type;
	} else {
		return ATTR_TYPE_UNKNOWN;
	}
}

bool attr_valid_id(attr_id_t id)
{
	ATTR_ENTRY_DECL(id);

	return (entry != NULL);
}

int attr_set(attr_id_t id, enum attr_type type, void *pv, size_t vlen, bool *modified)
{
	return set_internal(id, type, pv, vlen, true, modified);
}

int attr_set_without_broadcast(attr_id_t id, enum attr_type type, void *pv,
			       size_t vlen, bool *modified)
{
	return set_internal(id, type, pv, vlen, false, modified);
}

int attr_get(attr_id_t id, void *pv, size_t vlen)
{
	memset(pv, 0, vlen);
	ATTR_ENTRY_DECL(id);
	size_t size;
	int64_t extended;
	int r = -EPERM;

	if (entry != NULL) {
		if (entry->readable) {
			r = prepare_for_read(entry);
			if (r >= 0) {
				TAKE_MUTEX(attr_mutex);
				if (entry->type == ATTR_TYPE_S64 ||
				    entry->type == ATTR_TYPE_S32 ||
				    entry->type == ATTR_TYPE_S16 ||
				    entry->type == ATTR_TYPE_S8) {
					extended = sign_extend64(entry);
					size = MIN(sizeof(int64_t), vlen);
					memcpy(pv, &extended, size);
				} else {
					size = MIN(entry->size, vlen);
					memcpy(pv, entry->pData, size);
				}
				r = size;
				GIVE_MUTEX(attr_mutex);
			}
		}
	}
	return r;
}

int attr_set_string(attr_id_t id, char const *pv, size_t vlen)
{
	ATTR_ENTRY_DECL(id);
	int r = -EPERM;

	if (entry != NULL) {
		TAKE_MUTEX(attr_mutex);
		r = attr_write(entry, ATTR_TYPE_STRING, (void *)pv, vlen);
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, ENABLE_NOTIFICATIONS);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

int attr_set_byte_array(attr_id_t id, char const *pv, size_t vlen)
{
	ATTR_ENTRY_DECL(id);
	int r = -EPERM;

	if (entry != NULL) {
		TAKE_MUTEX(attr_mutex);
		r = attr_write(entry, ATTR_TYPE_BYTE_ARRAY, (void *)pv, vlen);
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, ENABLE_NOTIFICATIONS);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

void *attr_get_quasi_static(attr_id_t id)
{
	ATTR_ENTRY_DECL(id);

	if (entry != NULL) {
		return entry->pData;
	} else {
		return (void *)EMPTY_STRING;
	}
}

int attr_copy_string(char *pv, attr_id_t id, size_t max_len)
{
	ATTR_ENTRY_DECL(id);
	int r = -EPERM;

	if (entry != NULL) {
		r = prepare_for_read(entry);
		if (r >= 0) {
			strncpy(pv, entry->pData, max_len);
			r = 0;
		}
	}
	return r;
}

int attr_set_uint64(attr_id_t id, uint64_t value)
{
	ATTR_ENTRY_DECL(id);
	uint64_t local = value;
	int r = -EPERM;

	if (entry != NULL) {
		TAKE_MUTEX(attr_mutex);
		r = attr_write(entry, ATTR_TYPE_U64, &local, sizeof(local));
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, ENABLE_NOTIFICATIONS);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

int attr_set_signed64(attr_id_t id, int64_t value)
{
	ATTR_ENTRY_DECL(id);
	int64_t local = value;
	int r = -EPERM;

	if (entry != NULL) {
		TAKE_MUTEX(attr_mutex);
		r = attr_write(entry, ATTR_TYPE_S64, &local, sizeof(local));
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, ENABLE_NOTIFICATIONS);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

int attr_set_uint32(attr_id_t id, uint32_t value)
{
	ATTR_ENTRY_DECL(id);
	uint32_t local = value;
	int r = -EPERM;

	if (entry != NULL) {
		TAKE_MUTEX(attr_mutex);
		r = attr_write(entry, ATTR_TYPE_ANY, &local, sizeof(local));
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, ENABLE_NOTIFICATIONS);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

int attr_set_signed32(attr_id_t id, int32_t value)
{
	ATTR_ENTRY_DECL(id);
	int32_t local = value;
	int r = -EPERM;

	if (entry != NULL) {
		TAKE_MUTEX(attr_mutex);
		r = attr_write(entry, ATTR_TYPE_ANY, &local, sizeof(local));
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, ENABLE_NOTIFICATIONS);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

int attr_set_float(attr_id_t id, float value)
{
	ATTR_ENTRY_DECL(id);
	float local = value;
	int r = -EPERM;

	if (entry != NULL) {
		TAKE_MUTEX(attr_mutex);
		r = attr_write(entry, ATTR_TYPE_FLOAT, &local, sizeof(local));
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, ENABLE_NOTIFICATIONS);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

int attr_copy_uint64(uint64_t *pv, attr_id_t id)
{
	ATTR_ENTRY_DECL(id);
	*pv = 0;
	int r = -EPERM;

	if (entry != NULL) {
		if (entry->type == ATTR_TYPE_U64) {
			r = prepare_for_read(entry);
			if (r >= 0) {
				TAKE_MUTEX(attr_mutex);
				memcpy(pv, entry->pData, entry->size);
				r = 0;
				GIVE_MUTEX(attr_mutex);
			}
		} else {
			r = -EINVAL;
		}
	}
	return r;
}

int attr_copy_signed64(uint64_t *pv, attr_id_t id)
{
	ATTR_ENTRY_DECL(id);
	*pv = 0;
	int r = -EPERM;

	if (entry != NULL) {
		if (entry->type == ATTR_TYPE_S64) {
			r = prepare_for_read(entry);
			if (r >= 0) {
				TAKE_MUTEX(attr_mutex);
				*pv = sign_extend64(entry);
				r = 0;
				GIVE_MUTEX(attr_mutex);
			}
		} else {
			r = -EINVAL;
		}
	}
	return r;
}

int attr_copy_uint32(uint32_t *pv, attr_id_t id)
{
	ATTR_ENTRY_DECL(id);
	*pv = 0;
	int r = -EPERM;

	if (entry != NULL) {
		if (entry->type == ATTR_TYPE_U32 ||
		    entry->type == ATTR_TYPE_U16 ||
		    entry->type == ATTR_TYPE_U8 ||
		    entry->type == ATTR_TYPE_BOOL) {
			r = prepare_for_read(entry);
			if (r >= 0) {
				TAKE_MUTEX(attr_mutex);
				memcpy(pv, entry->pData, entry->size);
				r = 0;
				GIVE_MUTEX(attr_mutex);
			}
		} else {
			r = -EINVAL;
		}
	}
	return r;
}

int attr_copy_signed32(int32_t *pv, attr_id_t id)
{
	ATTR_ENTRY_DECL(id);
	*pv = 0;
	int r = -EPERM;

	if (entry != NULL) {
		if (entry->type == ATTR_TYPE_S32 ||
		    entry->type == ATTR_TYPE_S16 ||
		    entry->type == ATTR_TYPE_S8) {
			r = prepare_for_read(entry);
			if (r >= 0) {
				TAKE_MUTEX(attr_mutex);
				*pv = (int32_t)sign_extend64(entry);
				r = 0;
				GIVE_MUTEX(attr_mutex);
			}
		} else {
			r = -EINVAL;
		}
	}
	return r;
}

int attr_copy_float(float *pv, attr_id_t id)
{
	ATTR_ENTRY_DECL(id);
	*pv = 0.0;
	int r = -EPERM;

	if (entry != NULL) {
		if (entry->type == ATTR_TYPE_FLOAT) {
			r = prepare_for_read(entry);
			if (r >= 0) {
				TAKE_MUTEX(attr_mutex);
				*pv = *((float *)entry->pData);
				r = 0;
				GIVE_MUTEX(attr_mutex);
			}
		} else {
			r = -EINVAL;
		}
	}
	return r;
}

uint64_t attr_get_uint64(attr_id_t id, uint64_t alt)
{
	uint64_t v;
	int r = attr_copy_uint64(&v, id);

	if (r != 0) {
		v = alt;
		LOG_ALT_USED();
	}
	return v;
}

int64_t attr_get_signed64(attr_id_t id, int64_t alt)
{
	int64_t v;
	int r = attr_copy_signed64(&v, id);

	if (r != 0) {
		v = alt;
		LOG_ALT_USED();
	}
	return v;
}

uint32_t attr_get_uint32(attr_id_t id, uint32_t alt)
{
	uint32_t v;
	int r = attr_copy_uint32(&v, id);

	if (r != 0) {
		v = alt;
		LOG_ALT_USED();
	}
	return v;
}

int32_t attr_get_signed32(attr_id_t id, int32_t alt)
{
	int32_t v;
	int r = attr_copy_signed32(&v, id);

	if (r != 0) {
		v = alt;
		LOG_ALT_USED();
	}
	return v;
}

float attr_get_float(attr_id_t id, float alt)
{
	float v;
	int r = attr_copy_float(&v, id);

	if (r != 0) {
		v = alt;
		LOG_ALT_USED();
	}
	return v;
}

const char *attr_get_name(attr_id_t id)
{
	ATTR_ENTRY_DECL(id);
	const char *s = EMPTY_STRING;

	if (entry != NULL) {
		s = (const char *)entry->name;
	}
	return s;
}

size_t attr_get_size(attr_id_t id)
{
	ATTR_ENTRY_DECL(id);
	size_t size = 0;

	if (entry != NULL) {
		size = entry->size;
	}
	return size;
}

int attr_set_mask32(attr_id_t id, uint8_t Bit, uint8_t value)
{
	ATTR_ENTRY_DECL(id);
	uint32_t local;
	int r = -EPERM;

	if (entry != NULL && Bit < 32) {
		TAKE_MUTEX(attr_mutex);
		local = *(uint32_t *)entry->pData;
		WRITE_BIT(local, Bit, value);
		r = attr_write(entry, ATTR_TYPE_ANY, &local, sizeof(local));
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, ENABLE_NOTIFICATIONS);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

int attr_set_mask64(attr_id_t id, uint8_t Bit, uint8_t value)
{
	ATTR_ENTRY_DECL(id);
	uint64_t local;
	int r = -EPERM;

	if (entry != NULL && Bit < 64) {
		TAKE_MUTEX(attr_mutex);
		local = *(uint64_t *)entry->pData;
		local = value ? (local | BIT64(Bit)) : (local & ~BIT64(Bit));
		r = attr_write(entry, ATTR_TYPE_ANY, &local, sizeof(local));
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, ENABLE_NOTIFICATIONS);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

#ifdef CONFIG_ATTR_SHELL

attr_id_t attr_get_id(const char *name)
{
	attr_index_t i;

	for (i = 0; i < ATTR_TABLE_SIZE; i++) {
		if (strcmp(name, ATTR_TABLE[i].name) == 0) {
			return ATTR_TABLE[i].id;
		}
	}
	return ATTR_INVALID_ID;
}

int attr_show(attr_id_t id)
{
	ATTR_ENTRY_DECL(id);

	if (entry != NULL) {
		TAKE_MUTEX(attr_mutex);
		show(entry);
		GIVE_MUTEX(attr_mutex);
		return 0;
	} else {
		return -EINVAL;
	}
}

int attr_show_all(void)
{
	TAKE_MUTEX(attr_work_mutex);
	k_work_submit(&work_show);
	return 0;
}

int attr_delete(void)
{
	return fsu_delete_abs(ATTR_ABS_PATH);
}

#endif /* CONFIG_ATTR_SHELL */

int attr_prepare_then_dump(char **fstr, enum attr_dump type)
{
	int r = -EPERM;
	int count = 0;
	bool (*dumpable)(attr_index_t) = is_dump_rw;
	attr_index_t i;

	switch (type) {
	case ATTR_DUMP_W:
		dumpable = is_dump_w;
		break;
	case ATTR_DUMP_RO:
		dumpable = is_dump_ro;
		break;
	default:
		dumpable = is_dump_rw;
		break;
	}

	/* Update all parameters that need to be prepared before reserving
	 * the mutex - we want to avoid stacking up reservations of it,
	 * and if an attribute value changes after this, it will still be
	 * up to date
	 */
	for (i = 0; i < ATTR_TABLE_SIZE; i++) {
		if (dumpable(i)) {
			(void)prepare_for_read(&ATTR_TABLE[i]);
		}
	}

	/* Only proceed if prepares were executed OK */
	if (r == 0) {
		TAKE_MUTEX(attr_mutex);

		do {
			for (i = 0; i < ATTR_TABLE_SIZE; i++) {
				if (dumpable(i)) {
					r = lcz_param_file_generate_file(
						i, convert_attr_type(i),
						ATTR_TABLE[i].pData, get_attr_length(i),
						fstr);
					if (r < 0) {
						LOG_ERR("Error converting attribute table "
							"into file (dump) [%u] status: %d",
							ATTR_TABLE[i].id, r);
						break;
					} else {
						count += 1;
					}
				}
			}

			/* Don't validate the file if any attribute errors
			 * occurred. Break here to jump out the while(0)
			 * loop
			 */
			BREAK_ON_ERROR(r);

			r = lcz_param_file_validate_file(*fstr, strlen(*fstr));
		} while (0);

		GIVE_MUTEX(attr_mutex);
	}

	if (r < 0) {
		k_free(fstr);
		LOG_ERR("Error converting attribute table into file");
	}

	return (r < 0) ? r : count;
}

int attr_set_quiet(attr_id_t id, bool value)
{
	ATTR_ENTRY_DECL(id);
	attr_index_t index;
	int r = -EPERM;

	if (entry != NULL) {
		index = entry - &ATTR_TABLE[0];
		if (atomic_test_bit(quiet, index) != value) {
			atomic_set_bit_to(quiet, index, value);
			r = fsu_write_abs(ATTR_QUIET_ABS_PATH, quiet,
					  sizeof(quiet));
		} else {
			r = 0;
		}
	}
	return r;
}

int attr_load(const char *abs_path, const char *feedback_path, bool *modified)
{
	int r = -EPERM;

	if (modified != NULL) {
		*modified = false;
	}

	TAKE_MUTEX(attr_mutex);
	do {
		r = load_attributes(abs_path, feedback_path, true, false, true);
		BREAK_ON_ERROR(r);

		if (modified != NULL) {
			/* See if any attributes were modified prior to save */
			uint16_t i = 0;
			while (i < ATTR_TABLE_SIZE) {
				if (atomic_test_bit(attr_modified, i)) == true) {
					*modified = true;
					break;
				}

				++i;
			}
		}

		/* If attributes can't be saved, then still broadcast. */
		r = save_attributes(ATTR_SAVE_NOW);
		change_handler(DISABLE_NOTIFICATIONS);
	} while (0);
	GIVE_MUTEX(attr_mutex);

	return r;
}

int attr_set_notify(attr_id_t id, bool value)
{
	ATTR_ENTRY_DECL(id);
	int r = -EPERM;

	if (entry != NULL) {
		if (entry->readable) {
			atomic_set_bit_to(notify, attr_table_index(entry),
					  value);
			r = 0;
		} else {
			r = -EINVAL;
		}
	}
	return r;
}

bool attr_get_notify(attr_id_t id)
{
	ATTR_ENTRY_DECL(id);

	if (entry != NULL) {
		return atomic_test_bit(notify, attr_table_index(entry));
	} else {
		return false;
	}
}

int attr_disable_notify(void)
{
	size_t i;

	for (i = 0; i < ATOMIC_ARRAY_SIZE(ATTR_TABLE_SIZE); i++) {
		atomic_clear(notify + i);
	}
	return 0;
}

int attr_default(attr_id_t id)
{
	ATTR_ENTRY_DECL(id);

	if (entry != NULL) {
		memcpy(entry->pData, entry->pDefault, entry->size);
		show(entry);
		return 0;
	} else {
		return -EPERM;
	}
}

#ifdef CONFIG_ATTR_SETTINGS_LOCK
bool attr_is_locked(void)
{
	bool locked = true;
	uint8_t lock_status = attr_get_uint32(CONFIG_ATTR_INDEX_LOCK_STATUS,
					      LOCK_STATUS_NOT_SETUP);

	if (lock_status == LOCK_STATUS_NOT_SETUP ||
		lock_status == LOCK_STATUS_SETUP_DISENGAGED) {
		locked = false;
	}

	return locked;
}
#endif

__weak int attr_notify(attr_id_t id)
{
	ARG_UNUSED(id);
	return 0;
}

/******************************************************************************/
/* Local Function Definitions                                                 */
/******************************************************************************/
/* Convert the attribute type into the parameter file type */
static param_t convert_attr_type(attr_index_t index)
{
	if (ATTR_TABLE[index].type == ATTR_TYPE_STRING) {
		return PARAM_STR;
	} else {
		return PARAM_BIN;
	}
}

static size_t get_attr_length(attr_index_t index)
{
	if (ATTR_TABLE[index].type == ATTR_TYPE_STRING) {
		return strlen(ATTR_TABLE[index].pData);
	} else {
		return ATTR_TABLE[index].size;
	}
}

static int set_internal(attr_id_t id, enum attr_type type, void *pv,
			size_t vlen, bool broadcast, bool *modified)
{
	ATTR_ENTRY_DECL(id);
	int r = -EPERM;

	if (entry != NULL) {
		if (is_writable(entry)) {
			TAKE_MUTEX(attr_mutex);
			r = validate(entry, type, pv, vlen);
			if (r == 0) {
				r = attr_write(entry, type, pv, vlen);

				if (modified != NULL) {
					*modified = (atomic_test_bit(
							attr_modified,
							attr_table_index(entry))
							== true);
				}

				if (r == 0) {
					r = save_single(entry);
					if (broadcast) {
						change_single(
							entry,
							DISABLE_NOTIFICATIONS);
					}
				}
			}
			GIVE_MUTEX(attr_mutex);
		}
	}
	return r;
}

/*
 * Saving or broadcasting a single change takes the same path
 * as when processing from a file.
 */

static int save_single(const ate_t *const entry)
{
	int r = 0;

	if (atomic_test_bit(attr_modified, attr_table_index(entry))) {
		if (entry->savable && !entry->deprecated) {
			r = save_attributes(ATTR_SAVE_LATER);
		}
	}
	return r;
}

static void change_single(const ate_t *const entry, bool send_notifications)
{
	if (atomic_test_bit(attr_modified, attr_table_index(entry))) {
		change_handler(send_notifications);
	}
}

#ifdef CONFIG_ATTR_DEFERRED_SAVE
static int save_attributes_internal(void)
#else
static int save_attributes(void)
#endif
{
	/* Although unlikely, the table could have zero savable items. */
	int r = 0;
	char *fstr = NULL;
	attr_index_t i;
	uint32_t checksum_file = 0;
	uint32_t checksum_ram = 0;
	uint32_t fstrlen;

	TAKE_MUTEX(attr_save_change_mutex);

	/* Converting to file format is larger, but makes it easier to go between
	 * different versions.
	 */
	do {
		for (i = 0; i < ATTR_TABLE_SIZE; i++) {
			if (ATTR_TABLE[i].savable &&
			    !ATTR_TABLE[i].deprecated) {
				r = lcz_param_file_generate_file(
					ATTR_TABLE[i].id, convert_attr_type(i),
					ATTR_TABLE[i].pData, get_attr_length(i),
					&fstr);
				if (r < 0) {
					LOG_ERR("Error converting attribute table "
						"into file (save) [%u] status: %d",
						ATTR_TABLE[i].id, r);
					break;
				}
			}
		}
		BREAK_ON_ERROR(r);

		fstrlen = strlen(fstr);
		r = lcz_param_file_validate_file(fstr, fstrlen);
		BREAK_ON_ERROR(r);

		/* Calculate a CRC32 checksum of the current settings file and
		 * pending data in RAM, which avoids an erase and write process
		 * on the storage flash if there are no changes to write
		 */
		r = fsu_crc32_abs(&checksum_file, ATTR_ABS_PATH,
				  fsu_get_file_size_abs(ATTR_ABS_PATH));

		if (r == 0) {
			checksum_ram = crc32_ieee_update(0, fstr, fstrlen);
		} else {
			/* If calculating the checksum fails, skip working out
			 * the checksum of the RAM variables and just set the
			 * checksums to be different
			 */
			checksum_ram = checksum_file + 1;
		}

		if (checksum_file != checksum_ram) {
			/* Checksums mismatch, data has changed, write the
			 * file
			 */
			r = (int)lcz_param_file_write(CONFIG_ATTR_FILE_NAME, fstr,
						      fstrlen);
			LOG_DBG("Wrote %d of %d bytes of parameters to file", r,
				strlen(fstr));
		}
	} while (0);

	k_free(fstr);

#ifdef CONFIG_ATTR_SAVE_STATUS_IN_NON_INIT_RAM
	if (r >= 0) {
		/* Clear unsaved data flag */
		non_init_set_save_flag(false);
	}
#endif

	GIVE_MUTEX(attr_save_change_mutex);

#ifdef CONFIG_ATTR_DEFERRED_SAVE
	attr_set_signed32(ATTR_ID_save_error_code, ((r < 0) ? r : 0));
#endif

	return (r < 0) ? r : 0;
}

#ifdef CONFIG_ATTR_DEFERRED_SAVE
static void save_attributes_work(struct k_work *item)
{
	save_attributes_internal();
}

static int save_attributes(bool immediately)
{
	int rc = 0;

#ifdef CONFIG_ATTR_SAVE_STATUS_IN_NON_INIT_RAM
	non_init_set_save_flag(true);
#endif

	if (immediately == true) {
		/* Flag set to immediate save, because the state of the device
		 * is unknown and could be about to reboot, cancel a pending
		 * run and do not reschedule another run, just run it in the
		 * function directly
		 */
		if (k_work_delayable_is_pending(&attr_save_delayed_work) ==
		    true) {
			(void)k_work_cancel_delayable(&attr_save_delayed_work);
		}

		rc = save_attributes_internal();
	} else {
		while (k_work_delayable_busy_get(&attr_save_delayed_work) ==
		       true) {
			/* The save task is currently running, yield by
			 * sleeping until it has finished
			 */
			k_sleep(K_MSEC(ATTR_SAVE_EXECUTING_CHECK_TIME_MS));
		}

		/* Schedule task for saving the data */
		if (k_work_delayable_is_pending(&attr_save_delayed_work) ==
		    false) {
			k_work_reschedule(&attr_save_delayed_work,
					  K_MSEC(CONFIG_ATTR_SAVE_DELAY_MS));
		}
	}

	return rc;
}
#endif

/* generate framework broadcast, show value, and BLE notification callback */
static void change_handler(bool send_notifications)
{
	attr_index_t i;
	bool modified;

#ifdef CONFIG_ATTR_BROADCAST
	const size_t MSG_SIZE = sizeof(attr_changed_msg_t);
	attr_changed_msg_t *pb = BufferPool_Take(MSG_SIZE);

	if (pb == NULL) {
		LOG_ERR("Unable to allocate memory for attr broadcast");
	} else {
		pb->header.msgCode = FMC_ATTR_CHANGED;
		pb->header.txId = FWK_ID_RESERVED;
		pb->header.rxId = FWK_ID_RESERVED;
	}
#endif

	for (i = 0; i < ATTR_TABLE_SIZE; i++) {
		modified = atomic_test_bit(attr_modified, i);

#ifdef CONFIG_ATTR_BROADCAST
		if (modified && ATTR_TABLE[i].broadcast) {
			if (pb != NULL) {
				pb->list[pb->count++] = ATTR_TABLE[i].id;
			}
		}
#endif

		if (modified && !atomic_test_bit(quiet, i)) {
			show(&ATTR_TABLE[i]);
		}

		if (modified && send_notifications &&
		    atomic_test_bit(notify, i)) {
			notification_handler(ATTR_TABLE[i].id);
		}

		atomic_clear_bit(attr_modified, i);
	}

#ifdef CONFIG_ATTR_BROADCAST
	if (pb != NULL) {
		if (pb->count == 0) {
			/* Don't send an empty messsage */
			BufferPool_Free(pb);
		} else {
			if (Framework_Broadcast((FwkMsg_t *)pb, MSG_SIZE) !=
			    FWK_SUCCESS) {
				/* most likely */
				LOG_DBG("Zero consumers for broadcast");
				BufferPool_Free(pb);
			}
		}
	}
#endif
}

static void notification_handler(attr_id_t id)
{
	int ns = attr_notify(id);

	if (ns < 0 && ns != -ENOTCONN) {
		LOG_ERR("Notification [%u]: %d", id, ns);
	} else {
		LOG_DBG("Notification [%u]: %d", id, ns);
	}
}

static void show(const ate_t *const entry)
{
	uint32_t u = 0;
	int32_t i = 0;
	uint32_t a = 0;
	uint32_t b = 0;
	float f = 0.0;
	char float_str[CONFIG_ATTR_FLOAT_MAX_STR_SIZE] = { 0 };

	switch (entry->type) {
	case ATTR_TYPE_BOOL:
		memcpy(&u, entry->pData, entry->size);
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%s", entry->id, entry->name,
			 u ? "true" : "false");
		break;

	case ATTR_TYPE_U8:
	case ATTR_TYPE_U16:
	case ATTR_TYPE_U32:
		memcpy(&u, entry->pData, entry->size);
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%u %s", entry->id, entry->name,
			 u, GET_ENUM_STRING(u));
		break;

	case ATTR_TYPE_S8:
		i = (int32_t)(*(int8_t *)entry->pData);
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%d %s", entry->id, entry->name,
			 i, GET_ENUM_STRING(i));
		break;

	case ATTR_TYPE_S16:
		i = (int32_t)(*(int16_t *)entry->pData);
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%d %s", entry->id, entry->name,
			 i, GET_ENUM_STRING(i));
		break;

	case ATTR_TYPE_S32:
		i = *(int32_t *)entry->pData;
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%d %s", entry->id, entry->name,
			 i, GET_ENUM_STRING(i));
		break;

	case ATTR_TYPE_FLOAT:
		memcpy(&f, entry->pData, entry->size);
		snprintf(float_str, sizeof(float_str), CONFIG_ATTR_FLOAT_FMT,
			 f);
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%s", entry->id, entry->name,
			 log_strdup(float_str));
		break;

	case ATTR_TYPE_STRING:
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "'%s'", entry->id, entry->name,
			 log_strdup((char *)entry->pData));
		break;

	case ATTR_TYPE_U64:
	case ATTR_TYPE_S64:
		/* These weren't printing properly */
		memcpy(&a, (uint8_t *)entry->pData, 4);
		memcpy(&b, ((uint8_t *)entry->pData) + 4, 4);
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "0x%08x %08x", entry->id,
			 entry->name, b, a);
		break;

	default:
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "size: %u", entry->id,
			 entry->name, entry->size);
		LOG_HEXDUMP_DBG(entry->pData, entry->size, "");
		break;
	}
}

/**
 * @brief Read parameter file from flash and load it into attributes/RAM.
 *
 * @param validate_first validate entire file when loading from an external
 * source. Otherwise, allow bad pairs when loading from a file that should be good.
 *
 * @param mask_modified Don't set modified flag during initialization
 */
static int load_attributes(const char *fname, const char *feedback_path,
			   bool validate_first, bool mask_modified,
			   bool skip_non_writeable)
{
	int r = -EPERM;
	size_t fsize;
	char *fstr = NULL;
	param_kvp_t *kvp = NULL;
	size_t pairs = 0;
	uint16_t error_count = 0;

	do {
		r = lcz_param_file_parse_from_file(fname, &fsize, &fstr, &kvp);
		LOG_INF("pairs: %d fsize: %d file: %s", r, fsize,
			log_strdup(fname));
		BREAK_ON_ERROR(r);

		pairs = r;

		if (validate_first) {
			r = loader(kvp, fstr, pairs, false, mask_modified,
				   &error_count, skip_non_writeable);

			if (error_count != 0) {
				/* Error occured during verification, no point
				 * in continuing
				 */
				r = -EINVAL;
			}
		}
		BREAK_ON_ERROR(r);

		r = loader(kvp, fstr, pairs, true, mask_modified, &error_count,
			   skip_non_writeable);

	} while (0);

	/* If we got as far as building the kvp list and any errors
	 * occurred, build a list of diagnostics for all parameters in
	 * the file.
	 */
	if ((kvp != NULL) && (error_count) && (feedback_path != NULL)) {
		build_feedback_file(feedback_path, fstr, kvp, pairs);
	}

	/* If no errors occurred and we have a feedback path, we just
	 * create an empty file.
	 */
	if ((!error_count) && (feedback_path != NULL)) {
		build_empty_feedback_file(feedback_path);
	}

	/* Free the kvp allocation only if an allocation has been made */
	if (kvp != NULL) {
		k_free(kvp);
	}

	/* Free the fstr allocation only if an allocation has been made */
	if (fstr != NULL) {
		k_free(fstr);
	}

	LOG_DBG("status %d", r);

	return r;
}

static int loader(param_kvp_t *kvp, char *fstr, size_t pairs, bool do_write,
		  bool mask_modified, uint16_t *error_count,
		  bool skip_non_writeable)
{
	int r = -EPERM;
	uint8_t bin[ATTR_MAX_BIN_SIZE];
	size_t binlen;
	size_t i;
	const ate_t *entry;
	int (*validate_or_write)(const ate_t *const, enum attr_type, void *,
				 size_t) = do_write ? attr_write : validate;

	if (error_count != NULL) {
		*error_count = 0;
	}

	/* Loop through file
	 * Find entry for each ID
	 * Convert value from file into binary.
	 */
	for (i = 0; i < pairs; i++) {
		entry = attr_map(kvp[i].id);

		if (entry == NULL) {
			r = -EPERM;
		} else if (skip_non_writeable == true && !is_writable(entry)) {
			/* When importing settings from a remote source, in
			 * verification mode, skip entries that are not
			 * writeable
			 */
			r = 0;
		} else if (convert_attr_type(attr_table_index(entry)) ==
			   PARAM_STR) {
			r = validate_or_write(entry, ATTR_TYPE_STRING,
					      kvp[i].keystr, kvp[i].length);
		} else {
			/* Attribute validators for numbers don't look at the length passed
			 * into the function.  However, they do cast based on the size
			 * of the parameter.
			 */
			memset(bin, 0, sizeof(bin));

			binlen = hex2bin(kvp[i].keystr, kvp[i].length, bin,
					 sizeof(bin));
			if (binlen <= 0) {
				r = -EINVAL;
				LOG_ERR("Unable to convert hex->bin for id: %d",
					entry->id);
			} else {
				r = validate_or_write(entry, ATTR_TYPE_ANY, bin,
						      binlen);
			}
		}

		if (r < 0) {
			/* We always update the error count here regardless
			 * of whether break out is enabled.
			 */
			if (error_count != NULL) {
				*error_count = *error_count + 1;
			}

			if (IS_ENABLED(CONFIG_ATTR_BREAK_ON_LOAD_FAILURE)) {
				break;
			}
		}

		if (mask_modified) {
			if (entry != NULL) {
				atomic_clear_bit(attr_modified,
						 attr_table_index(entry));
			}
		}
	}

	return r;
}

static int validate(const ate_t *const entry, enum attr_type type, void *pv,
		    size_t vlen)
{
	int r = -EPERM;

	if (type == entry->type || type == ATTR_TYPE_ANY) {
		r = entry->validator(entry, pv, vlen, false);
	}

	if (r < 0) {
		LOG_WRN("failure id: %u %s", entry->id, entry->name);
		LOG_HEXDUMP_DBG(pv, vlen, "attr data");
	}
	return r;
}

static int attr_write(const ate_t *const entry, enum attr_type type, void *pv,
		      size_t vlen)
{
	int r = -EPERM;

#ifdef CONFIG_ATTR_DEFERRED_SAVE
	TAKE_MUTEX_NO_INIT(attr_save_change_mutex);
#endif

	if (type == entry->type || type == ATTR_TYPE_ANY) {
		r = entry->validator(entry, pv, vlen, true);
	}

#ifdef CONFIG_ATTR_DEFERRED_SAVE
	GIVE_MUTEX(attr_save_change_mutex);
#endif

	if (r < 0) {
		LOG_WRN("validation failure id: %u %s", entry->id, entry->name);
		LOG_HEXDUMP_DBG(pv, vlen, "attr data");
	}
	return r;
}

/**
 * @brief Cause actions that will update an attribute.
 * For the majority of attributes, this function doesn't do anything.
 */
static int prepare_for_read(const ate_t *const entry)
{
	int r = 0;

	if (entry->prepare != NULL) {
		r = entry->prepare();
	}
	return r;
}

static bool is_writable(const ate_t *const entry)
{
	bool r = false;
	bool settings_locked = 0;

	if (entry->writable) {
		if (entry->lockable) {
#ifdef CONFIG_ATTR_SETTINGS_LOCK
			settings_locked = attr_is_locked();

			if (settings_locked == false) {
#endif
				r = true;
#ifdef CONFIG_ATTR_SETTINGS_LOCK
			}
#endif
		} else {
			r = true;
		}
	}

	if (!r) {
		LOG_DBG("Id [%u] %s is %s", entry->id, entry->name,
			(settings_locked == false ? "not writable" :
			 "locked by settings passcode"));
	}

	return r;
}

static bool is_dump_rw(attr_index_t index)
{
	const ate_t *const entry = &ATTR_TABLE[index];
	bool b = false;

	if (entry->readable && !entry->deprecated) {
		b = true;
	}

	return b;
}

static bool is_dump_w(attr_index_t index)
{
	const ate_t *const entry = &ATTR_TABLE[index];
	bool b = false;

	if (entry->readable && !entry->deprecated && entry->writable) {
		b = true;
	}
	return b;
}

static bool is_dump_ro(attr_index_t index)
{
	const ate_t *const entry = &ATTR_TABLE[index];
	bool b = false;

	if (entry->readable && !entry->deprecated && !entry->writable) {
		b = true;
	}
	return b;
}

/**
 * @brief Use a file to determine if attribute should be printed by show
 * or made 'quiet'.
 */
static int initialize_quiet(void)
{
	int r = -EPERM;

	r = fsu_lfs_mount();
	if (r >= 0) {
		r = fsu_read_abs(ATTR_QUIET_ABS_PATH, &quiet, sizeof(quiet));

		if (r != sizeof(quiet)) {
			LOG_WRN("Unexpected quiet file size");
			r = -1;
		}

		/* If file doesn't exists, generate alt quiet settings. */
		if (r < 0) {
			r = fsu_write_abs(ATTR_QUIET_ABS_PATH, quiet,
					  sizeof(quiet));

			if (r < 0) {
				LOG_ERR("Unable to write quiet file: %d", r);
			}
		}
	}

	return r;
}

static int64_t sign_extend64(const ate_t *const entry)
{
	int64_t v;

	switch (entry->size) {
	case sizeof(int8_t):
		v = (int64_t)(*((int8_t *)entry->pData));
		break;
	case sizeof(int16_t):
		v = (int64_t)(*((int16_t *)entry->pData));
		break;
	case sizeof(int32_t):
		v = (int64_t)(*((int32_t *)entry->pData));
		break;
	default:
		v = *(int64_t *)entry->pData;
		break;
	}

	return v;
}

#ifdef CONFIG_ATTR_SETTINGS_LOCK
static void attr_load_settings_lock(void)
{
	int r;
	uint8_t lock_enabled;
	const struct attr_table_entry *const entry_lock = attr_map(
						CONFIG_ATTR_INDEX_LOCK);
	const struct attr_table_entry *const entry_lock_status = attr_map(
						CONFIG_ATTR_INDEX_LOCK_STATUS);

	if (attr_initialized == true) {
		/* Setup is already complete */
		return;
	}

	/* Check settings lock status */
	if (entry_lock == NULL || entry_lock_status == NULL) {
		/* Missing required attributes */
		if (entry_lock == NULL) {
			LOG_ERR("Missing lock attribute (%d), settings lock inoperable",
				CONFIG_ATTR_INDEX_LOCK);
		}

		if (entry_lock_status == NULL) {
			LOG_ERR("Missing lock status attribute (%d), settings lock inoperable",
				CONFIG_ATTR_INDEX_LOCK_STATUS);
		}
	} else {
		/* Set the current status of the lock */
		lock_enabled = *(uint8_t*)entry_lock->pData;

		r = attr_write(entry_lock_status, ATTR_TYPE_ANY, &lock_enabled,
			       sizeof(lock_enabled));

		if (r == 0) {
			r = save_single(entry_lock_status);
			change_single(entry_lock_status, ENABLE_NOTIFICATIONS);
		}
	}

	/* Turn off the display on the terminal of the passcode */
	attr_set_quiet(CONFIG_ATTR_INDEX_SETTINGS_PASSCODE, true);
}
#endif

static enum attr_write_error diagnose_parameter_write_error(param_kvp_t *kvp)
{
	enum attr_write_error result = ATTR_WRITE_ERROR_OK;
	enum attr_type attribute_type;

	/* The following apply to all parameter types.
	 * Known parameter index?
	 */
	if (!attr_valid_id(kvp->id)) {
		result = ATTR_WRITE_ERROR_PARAMETER_UNKNOWN;
	}
	if (result == ATTR_WRITE_ERROR_OK) {
		/* Writable parameter? */
		if (!is_writable(attr_map(kvp->id))) {
			result = ATTR_WRITE_ERROR_PARAMETER_READ_ONLY;
		}
	}
	/* If not diagnosed used type specific handlers */
	if (result == ATTR_WRITE_ERROR_OK) {
		attribute_type = attr_get_type(kvp->id);
		switch (attribute_type) {
			case (ATTR_TYPE_BOOL):
			case (ATTR_TYPE_U8):
			case (ATTR_TYPE_U16):
			case (ATTR_TYPE_U32):
			case (ATTR_TYPE_U64):
			case (ATTR_TYPE_S8):
			case (ATTR_TYPE_S16):
			case (ATTR_TYPE_S32):
			case (ATTR_TYPE_S64):
			case (ATTR_TYPE_FLOAT):
				result = diagnose_numeric_write_error(
				kvp, attribute_type);
				break;
			case (ATTR_TYPE_STRING):
				result = diagnose_string_write_error(kvp);
				break;
			default:
				break;
		}
	}
	return (result);
}

static enum attr_write_error diagnose_numeric_write_error(param_kvp_t *kvp,
							  enum attr_type Type)
{
	enum attr_write_error result = ATTR_WRITE_ERROR_OK;
	const struct attr_table_entry *attribute_entry;
	uint8_t bin[ATTR_MAX_HEX_SIZE];
	size_t binlen;

	attribute_entry = attr_map(kvp->id);


	/* Get numeric data and length */
	binlen = hex2bin(kvp->keystr, kvp->length, bin, sizeof(bin));
	/* Make sure the data is valid for use */
	if (binlen <= 0) {
		result = ATTR_WRITE_ERROR_PARAMETER_INVALID_LENGTH;
	}
	/* And only proceed if safe to do so */
	if (result == ATTR_WRITE_ERROR_OK) {
		switch (Type) {
			case (ATTR_TYPE_U8):
			case (ATTR_TYPE_BOOL):
				if (*((uint8_t *)(bin)) > attribute_entry->max.ux) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
				} else if (*((uint8_t *)(bin)) <
					   attribute_entry->min.ux) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
				}
				break;
			case (ATTR_TYPE_U16):
				if (*((uint16_t *)(bin)) > attribute_entry->max.ux) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
				} else if (*((uint16_t *)(bin)) <
					   attribute_entry->min.ux) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
				}
				break;
			case (ATTR_TYPE_U32):
				if (*((uint32_t *)(bin)) > attribute_entry->max.ux) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
				} else if (*((uint32_t *)(bin)) <
					   attribute_entry->min.ux) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
				}
				break;
			case (ATTR_TYPE_U64):
				if (*((uint64_t *)(bin)) > attribute_entry->max.ux) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
				} else if (*((uint64_t *)(bin)) <
					   attribute_entry->min.ux) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
				}
				break;
			case (ATTR_TYPE_S8):
				if (*((int8_t *)(bin)) > attribute_entry->max.sx) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
				} else if (*((int8_t *)(bin)) <
					   attribute_entry->min.sx) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
				}
				break;
			case (ATTR_TYPE_S16):
				if (*((int16_t *)(bin)) > attribute_entry->max.sx) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
				} else if (*((int16_t *)(bin)) <
					   attribute_entry->min.sx) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
				}
				break;
			case (ATTR_TYPE_S32):
				if (*((int32_t *)(bin)) > attribute_entry->max.sx) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
				} else if (*((int32_t *)(bin)) <
					   attribute_entry->min.sx) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
				}
				break;
			case (ATTR_TYPE_S64):
				if (*((int64_t *)(bin)) > attribute_entry->max.sx) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
				} else if (*((int64_t *)(bin)) <
					   attribute_entry->min.sx) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
				}
				break;
			case (ATTR_TYPE_FLOAT):
				if (*((float *)(bin)) > attribute_entry->max.fx) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH;
				} else if (*((float *)(bin)) <
					   attribute_entry->min.fx) {
					result = ATTR_WRITE_ERROR_NUMERIC_TOO_LOW;
				}
				break;
			default:
				break;
		}
	}

	return (result);
}

static enum attr_write_error diagnose_string_write_error(param_kvp_t *kvp)
{
	enum attr_write_error result = ATTR_WRITE_ERROR_OK;
	const struct attr_table_entry *attribute_entry;

	attribute_entry = attr_map(kvp->id);

	if (attribute_entry->size < kvp->length) {
		result = ATTR_WRITE_ERROR_STRING_TOO_MANY_CHARACTERS;
	}

	return (result);
}

static int build_feedback_file(const char *feedback_path, char *fstr,
			       param_kvp_t *kvp, size_t pairs)
{
	/* Start with a result greater than zero here
	 * so we enter the while loop
	 */
	int result = 1;
	int pair_index;
	enum attr_write_error attribute_write_error;
	param_kvp_t *next_kvp;
	uint16_t total_length = 0;
	uint8_t *write_buffer;

	/* Reserve enough space to hold details of all parameters. We add
	 * an extra character here so the append_feedback function can keep
	 * the string buffer null terminated for internal use.
	 */
	uint16_t buffer_size = (ATTR_LOAD_FEEDBACK_ENTRY_SIZE * pairs) + 1;
	/* Also be sure we align properly for malloc calls */
	buffer_size = ((buffer_size / ATTR_LOAD_FEEDBACK_ALIGN_SIZE) *
		      ATTR_LOAD_FEEDBACK_ALIGN_SIZE) +
		      ATTR_LOAD_FEEDBACK_ALIGN_SIZE;

	write_buffer = k_malloc(buffer_size);

	/* Then add our feedback if possible */
	if (write_buffer != NULL) {
		/* Start out with a blank buffer */
		memset(write_buffer, 0x0, buffer_size);
		/* Work through all load KVPs */
		for (pair_index = 0; (pair_index < pairs) && (result > 0);
		     pair_index++) {
			/* Get next KVP for diagnosis */
			next_kvp = &kvp[pair_index];
			/* Diagnose the next parameter error */
			attribute_write_error =
			diagnose_parameter_write_error(next_kvp);
			/* And add to our output file */
			result = lcz_param_file_append_feedback(
			next_kvp->id, attribute_write_error,
			write_buffer);
			/* Zero and less indicates an error occurred,
			 * greater than zero an added length.
			 */
			if (result > 0) {
				total_length += result;
			}
		}
	}

	/* OK to write our feedback file? */
	if (write_buffer != NULL) {
		/* Any data added? */
		if (total_length) {
			/* Either create or overwrite the feedback file */
			result = fsu_write_abs(feedback_path, write_buffer,
					       total_length);
		}
		/* Free memory regardless */
		k_free(write_buffer);
	}
	return (result);
}

static int build_empty_feedback_file(const char *feedback_path)
{
	int result;

	result = fsu_write_abs(feedback_path, NULL, 0);

	return (result);
}

/******************************************************************************/
/* System WorkQ context                                                       */
/******************************************************************************/
#ifdef CONFIG_ATTR_SHELL
static void sys_workq_show_handler(struct k_work *item)
{
	ARG_UNUSED(item);
	attr_index_t i;

	TAKE_MUTEX(attr_mutex);

	for (i = 0; i < ATTR_TABLE_SIZE; i++) {
		show(&ATTR_TABLE[i]);
		k_sleep(K_MSEC(CONFIG_ATTR_SHELL_SHOW_ALL_DELAY_MS));
	}
	GIVE_MUTEX(attr_mutex);

	GIVE_MUTEX(attr_work_mutex);
}
#endif

/******************************************************************************/
/* SYS INIT                                                                   */
/******************************************************************************/
static int attr_init(const struct device *device)
{
	ARG_UNUSED(device);
	int r = -EPERM;

	k_mutex_init(&attr_mutex);
	k_mutex_init(&attr_work_mutex);
	k_mutex_init(&attr_save_change_mutex);

	attr_table_initialize();

	if (strcmp(ATTR_ABS_PATH, ATTR_TABLE[ATTR_INDEX_loadPath].pData) == 0) {
		LOG_WRN("SMP load path should be different from attribute source");
	}

	if (fsu_get_file_size_abs(ATTR_ABS_PATH) < 0) {
		r = 0;
		LOG_INF("Parameter file doesn't exist");
	} else {
		LOG_DBG("Attempting to load from: " ATTR_ABS_PATH);
		r = load_attributes(ATTR_ABS_PATH, NULL, false, true, false);
	}

#ifdef CONFIG_ATTR_SHELL
	k_work_init(&work_show, sys_workq_show_handler);
#endif

#ifdef CONFIG_ATTR_DEFERRED_SAVE
	k_work_init_delayable(&attr_save_delayed_work, save_attributes_work);
#endif

	initialize_quiet();

#ifdef CONFIG_ATTR_SETTINGS_LOCK
	attr_load_settings_lock();
#endif

	attr_initialized = true;

	return r;
}
