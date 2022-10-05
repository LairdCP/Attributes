/**
 * @file attr.c
 * @brief
 *
 * Copyright (c) 2021-2022 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <logging/log.h>
LOG_MODULE_REGISTER(attr, CONFIG_ATTR_LOG_LEVEL);

#define LOG_SHOW LOG_INF

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <zephyr.h>
#include <init.h>
#include <stdio.h>
#include <stdlib.h>
#include <logging/log_ctrl.h>
#include <sys/util.h>
#include <sys/crc.h>

#include "lcz_kvp.h"
#include "file_system_utilities.h"

#include "attr_table.h"
#include "attr_table_private.h"
#include "attr_hash.h"

#include "attr.h"

#ifdef CONFIG_ATTR_BROADCAST
#include "fwk_includes.h"
#endif

/******************************************************************************/
/* Global Constants, Macros and type Definitions                              */
/******************************************************************************/
BUILD_ASSERT(CONFIG_LCZ_KVP_INIT_PRIORITY < CONFIG_ATTR_INIT_PRIORITY,
	     "Invalid init priority");

#if ATTR_ENABLE_FPU_CHECK
BUILD_ASSERT(IS_ENABLED(CONFIG_FPU),
	     "FPU must be enabled for string conversion of floats");
#endif

#define BREAK_ON_ERROR(x)                                                      \
	if (x < 0) {                                                           \
		break;                                                         \
	}

#define TAKE_MUTEX(m)                                                          \
	while (!attr_initialized) {                                            \
		k_yield();                                                     \
	}                                                                      \
	k_mutex_lock(&m, K_FOREVER)

#define TAKE_MUTEX_DURING_INIT(m) k_mutex_lock(&m, K_FOREVER)

#define GIVE_MUTEX(m) k_mutex_unlock(&m)

#define ATTR_ENTRY_DECL(x)                                                     \
	const struct attr_table_entry *const entry = attr_map(x);

#define ATTR_ABS_PATH CONFIG_FSU_MOUNT_POINT "/" CONFIG_ATTR_FILE_NAME

#define ATTR_QUIET_ABS_PATH CONFIG_FSU_MOUNT_POINT "/quiet.bin"

#define LOG_ALT_USED() LOG_DBG("Alt value used ID [%u]: %d", id, r)

#define GET_ENUM_STRING(x) (entry->gsf == NULL) ? EMPTY_STRING : entry->gsf(x)

#define ATOMIC_ARRAY_SIZE(num_bits) (1 + ((num_bits)-1) / ATOMIC_BITS)

enum { DISABLE_NOTIFICATIONS = 0, NOTIFY = 1 };

enum { DISABLE_BROADCAST = 0, BROADCAST = 1 };

static const char EMPTY_STRING[] = "";

/* Time (in ms) between checks if a save is currently in progress */
#define ATTR_SAVE_EXECUTING_CHECK_TIME_MS 200

#ifdef CONFIG_ATTR_DEFERRED_SAVE
#define ATTR_SAVE_NOW true
#define ATTR_SAVE_LATER false
#else
#define ATTR_SAVE_NOW
#define ATTR_SAVE_LATER
#endif

static const lcz_kvp_cfg_t KVP_CFG = {
	.max_file_size = (ATTR_MAX_FILE_SIZE +
			  CONFIG_ATTR_FILE_ADDITIONAL_COMMENT_CHARS),
	.max_key_len = (ATTR_MAX_KEY_NAME_SIZE - 1),
	.max_val_len = (ATTR_MAX_VALUE_SIZE - 1),
	.encrypted = IS_ENABLED(CONFIG_ATTR_ENCRYPTED) ? true : false,
};

/******************************************************************************/
/* Global Data Definitions                                                    */
/******************************************************************************/
extern const struct attr_table_entry ATTR_TABLE[ATTR_TABLE_SIZE];

ATOMIC_DEFINE(attr_modified, ATTR_TABLE_SIZE);
ATOMIC_DEFINE(attr_unchanged, ATTR_TABLE_SIZE);

K_MUTEX_DEFINE(attr_mutex);

/******************************************************************************/
/* Local Data Definitions                                                     */
/******************************************************************************/
static ATOMIC_DEFINE(quiet, ATTR_TABLE_SIZE);
static ATOMIC_DEFINE(notify, ATTR_TABLE_SIZE);

#ifdef CONFIG_ATTR_DEFERRED_SAVE
static struct k_work_delayable attr_save_delayed_work;
#endif

static volatile bool attr_initialized;

static struct {
	attr_id_t id;
	const struct attr_table_entry *entry;
	char key[ATTR_MAX_KEY_NAME_SIZE];
	char value[ATTR_MAX_VALUE_SIZE];
	union {
		uint8_t data[ATTR_MAX_VALUE_SIZE];
		uint64_t u64;
		int64_t s64;
		float flt;
	} result;
	size_t result_len;
} conversion;

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

static void change_single(const ate_t *const entry, bool send_notifications,
			  bool skip_broadcast);
static void broadcast_single(attr_id_t id);
static void change_multiple(bool send_notifications);
static void notification_handler(attr_id_t id);

static int load_attributes(const char *fname, bool validate_first,
			   bool mask_modified, bool enforce_writable);

static int loader(lcz_kvp_t *kvp, char *fstr, size_t pairs, bool do_write,
		  bool mask_modified, bool enforce_writable);

static int validate(const ate_t *const entry, enum attr_type type, void *pv,
		    size_t vlen);

static int attr_write(const ate_t *const entry, enum attr_type type, void *pv,
		      size_t vlen);

static int show(const ate_t *const entry);
static int allow_show(const ate_t *const entry);
static int allow_get(const ate_t *const entry);

static bool is_dump_rw(attr_index_t index);
static bool is_dump_w(attr_index_t index);
static bool is_dump_ro(attr_index_t index);

static int prepare_for_read(const ate_t *const entry);

static bool is_writable(const ate_t *const entry);

static int64_t sign_extend64(const ate_t *const entry);

static int initialize_quiet(void);

#ifdef CONFIG_ATTR_DEFERRED_SAVE
static void save_attributes_work(struct k_work *item);
#endif

static const ate_t *kvp_find_entry(lcz_kvp_t *kvp);
static int kvp_convert(lcz_kvp_t *kvp);
static int entry_to_kvp(lcz_kvp_t *kvp, const ate_t *const entry);

static void start_feedback_file(void);
static void append_feedback_file(lcz_kvp_t *kvp, int code);
static void create_unable_to_parse_feedback_file(void);

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/
extern void attr_table_initialize(void);
extern void attr_table_factory_reset(void);

/******************************************************************************/
/* SYS INIT                                                                   */
/******************************************************************************/
static int attr_init(const struct device *device)
{
	ARG_UNUSED(device);
	int r = -EPERM;

	TAKE_MUTEX_DURING_INIT(attr_mutex);

	attr_table_initialize();

#ifdef ATTR_INDEX_load_path
	if (strcmp(ATTR_ABS_PATH, ATTR_TABLE[ATTR_INDEX_load_path].pData) ==
	    0) {
		LOG_WRN("SMP load path should be different from attribute source");
	}
#endif

	if (fsu_get_file_size_abs(ATTR_ABS_PATH) < 0) {
		r = 0;
		LOG_INF("Attribute file doesn't exist");
	} else {
		LOG_DBG("Attempting to load from: " ATTR_ABS_PATH);
		r = load_attributes(ATTR_ABS_PATH, true, true, false);
	}

#ifdef CONFIG_ATTR_DEFERRED_SAVE
	k_work_init_delayable(&attr_save_delayed_work, save_attributes_work);
#endif

	initialize_quiet();

	attr_initialized = true;
	GIVE_MUTEX(attr_mutex);

	return r;
}

SYS_INIT(attr_init, APPLICATION, CONFIG_ATTR_INIT_PRIORITY);

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
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

int attr_set(attr_id_t id, enum attr_type type, void *pv, size_t vlen,
	     bool *modified)
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
		if (entry->flags & FLAGS_READABLE) {
			r = allow_get(entry);
			if (r == 0) {
				r = prepare_for_read(entry);
				if (r >= 0) {
					TAKE_MUTEX(attr_mutex);
					if (entry->type == ATTR_TYPE_S64 ||
					    entry->type == ATTR_TYPE_S32 ||
					    entry->type == ATTR_TYPE_S16 ||
					    entry->type == ATTR_TYPE_S8) {
						extended = sign_extend64(entry);
						size = MIN(sizeof(int64_t),
							   vlen);
						memcpy(pv, &extended, size);

						/* Return size of actual
						 * datatype once the signed
						 * value has been copied
						 */
						size = MIN(entry->size, size);
					} else {
						size = MIN(entry->size, vlen);
						memcpy(pv, entry->pData, size);
					}
					r = size;
					GIVE_MUTEX(attr_mutex);
				}
			}
		}
	}
	return r;
}

void *attr_get_pointer(attr_id_t id, int *vlen)
{
	ATTR_ENTRY_DECL(id);
	int r = -EPERM;

	if (entry != NULL) {
		if (entry->flags & FLAGS_READABLE) {
			r = allow_get(entry);
			if (r == 0) {
				r = prepare_for_read(entry);
				if (r >= 0) {
					*vlen = entry->size;
					return entry->pData;
				}
			}
		}
	}

	*vlen = r;

	return NULL;
}

int attr_get_default(attr_id_t id, void *pv, size_t vlen)
{
	ATTR_ENTRY_DECL(id);
	size_t size;
	int r = -EPERM;
	memset(pv, 0, vlen);

	if (entry != NULL) {
		size = MIN(entry->size, vlen);
		memcpy(pv, entry->pDefault, size);
		r = size;
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
			change_single(entry, NOTIFY, BROADCAST);
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
			change_single(entry, NOTIFY, BROADCAST);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

const void *attr_get_quasi_static(attr_id_t id)
{
	ATTR_ENTRY_DECL(id);

	if (entry != NULL) {
		return (const void *)entry->pData;
	} else {
		LOG_DBG("Empty string used for id: %d", id);
		return (const void *)EMPTY_STRING;
	}
}

const char *const attr_get_enum_string(attr_id_t id)
{
	/* Use status of copy to simplify type handling */
	union {
		int32_t s;
		uint32_t u;
	} d;

	ATTR_ENTRY_DECL(id);

	if (entry == NULL) {
		return "ID not found";
	} else if (entry->gsf == NULL) {
		return "ID doesn't have get string function";
	} else if (attr_copy_uint32(&d.u, id) == 0) {
		return entry->gsf(d.u);
	} else if (attr_copy_signed32(&d.s, id) == 0) {
		return entry->gsf(d.s);
	} else {
		return "Enum string not supported for type";
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
			change_single(entry, NOTIFY, BROADCAST);
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
			change_single(entry, NOTIFY, BROADCAST);
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
			change_single(entry, NOTIFY, BROADCAST);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

int attr_add_uint32(attr_id_t id, uint32_t value)
{
	ATTR_ENTRY_DECL(id);
	int r = -EPERM;
	uint32_t current_val;

	if (entry != NULL) {
		r = attr_copy_uint32(&current_val, id);
		if (r != 0) {
			return r;
		}
		current_val += value;
		TAKE_MUTEX(attr_mutex);
		r = attr_write(entry, ATTR_TYPE_ANY, &current_val,
			       sizeof(current_val));
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, NOTIFY, BROADCAST);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

int attr_set_flags(attr_id_t id, atomic_val_t bitmask)
{
	ATTR_ENTRY_DECL(id);
	int r = -ENOENT;

	if (entry != NULL) {
		TAKE_MUTEX(attr_mutex);
		/* Writing flags may cause other flags to be set or cleared
		 * depending on the associations
		 */
		r = attr_write(entry, ATTR_TYPE_ATOMIC, &bitmask,
			       ATTR_FLAG_SET);
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, NOTIFY, BROADCAST);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

int attr_clear_flags(attr_id_t id, atomic_val_t bitmask)
{
	ATTR_ENTRY_DECL(id);
	int r = -ENOENT;

	if (entry != NULL) {
		TAKE_MUTEX(attr_mutex);
		/* Writing flags may cause other flags to be set or cleared
		 * depending on the associations
		 */
		r = attr_write(entry, ATTR_TYPE_ATOMIC, &bitmask,
			       ATTR_FLAG_CLEAR);
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, NOTIFY, BROADCAST);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

bool attr_are_flags_set(attr_id_t id, atomic_val_t bitmask)
{
	ATTR_ENTRY_DECL(id);

	if (entry != NULL) {
		if (((*(atomic_val_t *)entry->pData) & bitmask) == bitmask) {
			return true;
		}
	}
	return false;
}

int attr_set_no_broadcast_uint32(attr_id_t id, uint32_t value)
{
	ATTR_ENTRY_DECL(id);
	uint32_t local = value;
	attr_index_t index;
	int r = -EPERM;

	if (entry != NULL) {
		TAKE_MUTEX(attr_mutex);
		r = attr_write(entry, ATTR_TYPE_ANY, &local, sizeof(local));
		if (r == 0) {
			r = save_single(entry);
			index = entry - &ATTR_TABLE[0];
			change_single(entry, DISABLE_NOTIFICATIONS,
				      DISABLE_BROADCAST);
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
			change_single(entry, NOTIFY, BROADCAST);
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
			change_single(entry, NOTIFY, BROADCAST);
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

bool attr_get_bool(attr_id_t id)
{
	ATTR_ENTRY_DECL(id);
	bool b = false;

	if (entry != NULL) {
		if (entry->type == ATTR_TYPE_BOOL) {
			if (prepare_for_read(entry) >= 0) {
				b = *(bool *)entry->pData;
			}
		}
	}
	return b;
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
	const char *s = EMPTY_STRING;
	ATTR_ENTRY_DECL(id);

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

int attr_set_mask32(attr_id_t id, uint8_t bit, uint8_t value)
{
	ATTR_ENTRY_DECL(id);
	uint32_t local;
	int r = -EPERM;

	if (entry != NULL && bit < 32) {
		TAKE_MUTEX(attr_mutex);
		local = *(uint32_t *)entry->pData;
		WRITE_BIT(local, bit, value);
		r = attr_write(entry, ATTR_TYPE_ANY, &local, sizeof(local));
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, NOTIFY, BROADCAST);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

int attr_set_mask64(attr_id_t id, uint8_t bit, uint8_t value)
{
	ATTR_ENTRY_DECL(id);
	uint64_t local;
	int r = -EPERM;

	if (entry != NULL && bit < 64) {
		TAKE_MUTEX(attr_mutex);
		local = *(uint64_t *)entry->pData;
		local = value ? (local | BIT64(bit)) : (local & ~BIT64(bit));
		r = attr_write(entry, ATTR_TYPE_ANY, &local, sizeof(local));
		if (r == 0) {
			r = save_single(entry);
			change_single(entry, NOTIFY, BROADCAST);
		}
		GIVE_MUTEX(attr_mutex);
	}
	return r;
}

attr_id_t attr_get_id(const char *name)
{
	const struct attr_hash_entry *entry;

	entry = attr_id_from_hash(name, strlen(name));
	if (entry != NULL) {
		return entry->id;
	}

	return ATTR_INVALID_ID;
}

#ifdef CONFIG_ATTR_SHELL

static int shell_show(const struct shell *shell, const ate_t *const entry)
{
	uint32_t u = 0;
	int32_t i = 0;
	int r;

	r = allow_show(entry);
	if (r < 0) {
		if (r == -EPERM) {
			shell_print(shell, CONFIG_ATTR_SHOW_FMT "******",
				    attr_table_index(entry), entry->name);
		}
		return r;
	}

	switch (entry->type) {
	case ATTR_TYPE_BOOL:
		memcpy(&u, entry->pData, entry->size);
		shell_print(shell, CONFIG_ATTR_SHOW_FMT "%s",
			    attr_table_index(entry), entry->name,
			    u ? "true" : "false");
		break;

	case ATTR_TYPE_U8:
	case ATTR_TYPE_U16:
	case ATTR_TYPE_U32:
		memcpy(&u, entry->pData, entry->size);
		shell_print(shell, CONFIG_ATTR_SHOW_FMT "%u %s",
			    attr_table_index(entry), entry->name, u,
			    GET_ENUM_STRING(u));
		break;

	case ATTR_TYPE_U64:
		shell_print(shell, CONFIG_ATTR_SHOW_FMT "%" PRIu64,
			    attr_table_index(entry), entry->name,
			    *(uint64_t *)entry->pData);
		break;

	case ATTR_TYPE_S8:
		i = (int32_t)(*(int8_t *)entry->pData);
		shell_print(shell, CONFIG_ATTR_SHOW_FMT "%d %s",
			    attr_table_index(entry), entry->name, i,
			    GET_ENUM_STRING(i));
		break;

	case ATTR_TYPE_S16:
		i = (int32_t)(*(int16_t *)entry->pData);
		shell_print(shell, CONFIG_ATTR_SHOW_FMT "%d %s",
			    attr_table_index(entry), entry->name, i,
			    GET_ENUM_STRING(i));
		break;

	case ATTR_TYPE_S32:
		i = *(int32_t *)entry->pData;
		shell_print(shell, CONFIG_ATTR_SHOW_FMT "%d %s",
			    attr_table_index(entry), entry->name, i,
			    GET_ENUM_STRING(i));
		break;

	case ATTR_TYPE_S64:
		shell_print(shell, CONFIG_ATTR_SHOW_FMT "%" PRId64,
			    attr_table_index(entry), entry->name,
			    *(int64_t *)entry->pData);
		break;

	case ATTR_TYPE_FLOAT:
		shell_print(shell, CONFIG_ATTR_SHOW_FMT "%e",
			    attr_table_index(entry), entry->name,
			    *(float *)entry->pData);
		break;

	case ATTR_TYPE_STRING:
		shell_print(shell, CONFIG_ATTR_SHOW_FMT "'%s'",
			    attr_table_index(entry), entry->name,
			    (char *)entry->pData);
		break;

	default:
		shell_print(shell, CONFIG_ATTR_SHOW_FMT "size: %u",
			    attr_table_index(entry), entry->name, entry->size);
		shell_hexdump(shell, entry->pData, entry->size);
		break;
	}

	return 0;
}

int attr_show(const struct shell *shell, attr_id_t id)
{
	int r = -EINVAL;
	ATTR_ENTRY_DECL(id);

	if (entry != NULL) {
		if (!attr_initialized) {
			r = -EAGAIN;
		} else if (k_mutex_lock(&attr_mutex, K_NO_WAIT) == 0) {
			r = shell_show(shell, entry);
			GIVE_MUTEX(attr_mutex);
		} else {
			r = -EWOULDBLOCK;
		}
	}

	return r;
}

int attr_show_all(const struct shell *shell)
{
	int r = 0;
	attr_index_t i;

	if (!attr_initialized) {
		r = -EAGAIN;
	} else if (k_mutex_lock(&attr_mutex, K_NO_WAIT) == 0) {
		for (i = 0; i < ATTR_TABLE_SIZE; i++) {
			(void)shell_show(shell, &ATTR_TABLE[i]);
		}
		GIVE_MUTEX(attr_mutex);
	} else {
		r = -EWOULDBLOCK;
	}

	return r;
}

int attr_delete(void)
{
	return fsu_delete_abs(ATTR_ABS_PATH);
}

const char *const attr_get_string_set_error(int value)
{
	switch (abs(value)) {
	case ATTR_WRITE_ERROR_OK:
		return "success";
	case ATTR_WRITE_ERROR_NUMERIC_TOO_LOW:
		return "too low";
	case ATTR_WRITE_ERROR_NUMERIC_TOO_HIGH:
		return "too high";
	case ATTR_WRITE_ERROR_STRING_TOO_MANY_CHARACTERS:
		return "too many characters";
	case ATTR_WRITE_ERROR_PARAMETER_NOT_WRITABLE:
		return "value not writable";
	case ATTR_WRITE_ERROR_PARAMETER_UNKNOWN:
		return "attribute name unknown";
	case ATTR_WRITE_ERROR_PARAMETER_INVALID_LENGTH:
		return "invalid length";
	case ATTR_WRITE_ERROR_UNABLE_TO_PARSE_FILE:
		return "unable to parse file";
	case ATTR_WRITE_ERROR_UNKNOWN:
	default:
		return "unknown error";
	}
}

#endif /* CONFIG_ATTR_SHELL */

int attr_prepare_then_dump(char **fstr, enum attr_dump type)
{
	int r = 0;
	int count = 0;
	bool (*dumpable)(attr_index_t) = is_dump_rw;
	attr_index_t i;
	lcz_kvp_t kvp;

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

	/* Update all values that need to be prepared before reserving
	 * the mutex - we want to avoid stacking up reservations of it,
	 * and if an attribute value changes after this, it will still be
	 * up to date
	 */
	for (i = 0; i < ATTR_TABLE_SIZE; i++) {
		if (dumpable(i)) {
			(void)prepare_for_read(&ATTR_TABLE[i]);
		}
	}

	TAKE_MUTEX(attr_mutex);
	do {
		for (i = 0; i < ATTR_TABLE_SIZE; i++) {
			if (dumpable(i)) {
				entry_to_kvp(&kvp, &ATTR_TABLE[i]);
				r = lcz_kvp_generate_file(&KVP_CFG, &kvp, fstr);
				if (r < 0) {
					LOG_ERR("Error converting attribute table "
						"into file (dump) [%u] status: %d",
						i, r);
					break;
				} else {
					count += 1;
				}
			}
		}

		/* Don't validate the file if any attribute errors occurred. */
		BREAK_ON_ERROR(r);

		r = lcz_kvp_validate_file(&KVP_CFG, *fstr, strlen(*fstr));
	} while (0);
	GIVE_MUTEX(attr_mutex);

	if (r < 0) {
		k_free(*fstr);
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

int attr_load(const char *abs_path, bool *modified)
{
	int r = -EPERM;

	if (modified != NULL) {
		*modified = false;
	}

	TAKE_MUTEX(attr_mutex);
	do {
		r = load_attributes(abs_path, true, false, true);
		BREAK_ON_ERROR(r);

		if (modified != NULL) {
			/* See if any attributes were modified prior to save */
			uint16_t i = 0;
			while (i < ATTR_TABLE_SIZE) {
				if (atomic_test_bit(attr_modified, i)) {
					*modified = true;
					break;
				}

				++i;
			}
		}

		/* If attributes can't be saved, then still broadcast. */
		r = save_attributes(ATTR_SAVE_NOW);
		change_multiple(DISABLE_NOTIFICATIONS);
	} while (0);
	GIVE_MUTEX(attr_mutex);

	return r;
}

int attr_set_notify(attr_id_t id, bool value)
{
	ATTR_ENTRY_DECL(id);
	int r = -EPERM;

	if (entry != NULL) {
		if (entry->flags & FLAGS_READABLE) {
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
		(void)show(entry);
		return 0;
	} else {
		return -EPERM;
	}
}

__weak int attr_notify(attr_id_t id)
{
	ARG_UNUSED(id);
	return 0;
}

__weak void attr_save_status_notification(bool dirty)
{
	return;
}

int attr_force_save(void)
{
#ifdef CONFIG_ATTR_DEFERRED_SAVE
	return save_attributes(true);
#else
	return 0;
#endif
}

int attr_update_config_version(void)
{
#ifdef ATTR_ID_config_version
	uint32_t config_version = attr_get_uint32(ATTR_ID_config_version, 0);

	return attr_set_uint32(ATTR_ID_config_version,
			       (uint32_t)config_version);
#endif

	return -EIO;
}

void attr_get_indices(uint16_t *table_size, uint16_t *min_id, uint16_t *max_id)
{
	*table_size = ATTR_TABLE_SIZE;
	*min_id = 0;
	*max_id = ATTR_TABLE_MAX_ID;
}

int attr_get_entry_details(uint16_t index, attr_id_t *id, const char *name,
			   size_t *size, enum attr_type *type,
			   enum attr_flags *flags, bool *prepared,
			   const struct attr_min_max *min,
			   const struct attr_min_max *max)
{
	if (index >= ATTR_TABLE_SIZE) {
		return -EINVAL;
	}

	*id = index;
	name = ATTR_TABLE[index].name;
	*size = ATTR_TABLE[index].size;
	*type = ATTR_TABLE[index].type;
	*flags = ATTR_TABLE[index].flags;
	*prepared = ATTR_TABLE[index].prepare != NULL;
	min = &ATTR_TABLE[index].min;
	max = &ATTR_TABLE[index].max;

	return 0;
}

/******************************************************************************/
/* Local Function Definitions                                                 */
/******************************************************************************/
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
							     attr_table_index(
								     entry)) ==
						     true);
				}

				if (r == 0) {
					r = save_single(entry);
					change_single(entry,
						      DISABLE_NOTIFICATIONS,
						      broadcast);
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
		if ((entry->flags & FLAGS_SAVABLE) &&
		    !(entry->flags & FLAGS_DEPRECATED)) {
			r = save_attributes(ATTR_SAVE_LATER);
		}
	}
	return r;
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
	uint32_t fstr_len;
	lcz_kvp_t kvp;

	TAKE_MUTEX(attr_mutex);

	/* Converting to file format is larger, but makes it easier to go between
	 * different versions.
	 */
	do {
		for (i = 0; i < ATTR_TABLE_SIZE; i++) {
			if ((ATTR_TABLE[i].flags & FLAGS_SAVABLE) &&
			    !(ATTR_TABLE[i].flags & FLAGS_DEPRECATED)) {
				entry_to_kvp(&kvp, &ATTR_TABLE[i]);
				r = lcz_kvp_generate_file(&KVP_CFG, &kvp,
							  &fstr);
				if (r < 0) {
					LOG_ERR("Error converting attribute table "
						"into file (save) [%u] status: %d",
						i, r);
					break;
				}
			}
		}
		BREAK_ON_ERROR(r);

		fstr_len = strlen(fstr);
		r = lcz_kvp_validate_file(&KVP_CFG, fstr, fstr_len);
		BREAK_ON_ERROR(r);

		/* Calculate a CRC32 checksum of the current settings file and
		 * pending data in RAM, which avoids an erase and write process
		 * on the storage flash if there are no changes to write.
		 */
		r = fsu_crc32_abs(&checksum_file, ATTR_ABS_PATH,
				  fsu_get_file_size_abs(ATTR_ABS_PATH));

		if (r == 0) {
			checksum_ram = crc32_ieee_update(0, fstr, fstr_len);
		} else {
			/* If calculating the checksum fails, skip working out
			 * the checksum of the RAM variables and just set the
			 * checksums to be different
			 */
			checksum_ram = checksum_file + 1;
		}

		if (checksum_file != checksum_ram) {
			r = lcz_kvp_write(KVP_CFG.encrypted,
					  CONFIG_ATTR_FILE_NAME, fstr,
					  fstr_len);
			LOG_DBG("Wrote %d of %d bytes of key-value pairs to file",
				r, strlen(fstr));
		} else {
			LOG_DBG("No attribute changes to write");
		}
	} while (0);

	k_free(fstr);

	if (r >= 0) {
		attr_save_status_notification(false);
	}

	GIVE_MUTEX(attr_mutex);

#ifdef CONFIG_ATTR_DEFERRED_SAVE
	attr_set_signed32(ATTR_ID_attr_save_error_code, ((r < 0) ? r : 0));
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

	attr_save_status_notification(true);

	if (immediately) {
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
static void change_multiple(bool send_notifications)
{
	attr_index_t i;
	bool modified;
	bool unchanged;

#ifdef CONFIG_ATTR_BROADCAST
	size_t msg_size = ATTR_BROADCAST_MSG_SIZE(ATTR_TABLE_WRITABLE_COUNT);
	attr_changed_msg_t *pb = BufferPool_Take(msg_size);

	if (pb == NULL) {
		LOG_ERR("Unable to allocate memory for attr %s", __func__);
	} else {
		pb->header.msgCode = FMC_ATTR_CHANGED;
		pb->header.txId = FWK_ID_RESERVED;
		pb->header.rxId = FWK_ID_RESERVED;
	}
#endif

	for (i = 0; i < ATTR_TABLE_SIZE; i++) {
		modified = atomic_test_bit(attr_modified, i);
		unchanged = atomic_test_bit(attr_unchanged, i);

		if (!modified && !unchanged) {
			continue;
		}

#ifdef CONFIG_ATTR_BROADCAST
		if (ATTR_TABLE[i].flags & FLAGS_BROADCAST) {
			if (pb != NULL) {
				pb->list[pb->count++] = i;
			}
		}
#endif

		if (!atomic_test_bit(quiet, i)) {
			show(&ATTR_TABLE[i]);
		}

		if (send_notifications && atomic_test_bit(notify, i)) {
			notification_handler(i);
		}

		atomic_clear_bit(attr_modified, i);
		atomic_clear_bit(attr_unchanged, i);
	}

#ifdef CONFIG_ATTR_BROADCAST
	if (pb != NULL) {
		if (pb->count == 0) {
			/* Don't send an empty message */
			BufferPool_Free(pb);
		} else {
			if (Framework_Broadcast((FwkMsg_t *)pb, msg_size) !=
			    FWK_SUCCESS) {
				/* most likely */
				LOG_DBG("Zero consumers for broadcast");
				BufferPool_Free(pb);
			}
		}
	}
#endif
}

static void change_single(const ate_t *const entry, bool send_notifications,
			  bool broadcast)
{
	attr_index_t idx = attr_table_index(entry);
	bool modified = atomic_test_bit(attr_modified, idx);
	bool unchanged = atomic_test_bit(attr_unchanged, idx);

	if (!modified && !unchanged) {
		return;
	}

	if (!atomic_test_bit(quiet, idx)) {
		show(entry);
	}

	if (send_notifications && atomic_test_bit(notify, idx)) {
		notification_handler(idx);
	}

	atomic_clear_bit(attr_modified, idx);
	atomic_clear_bit(attr_unchanged, idx);

	if (!broadcast || ((entry->flags & FLAGS_BROADCAST) == 0)) {
		return;
	}

	/* ID is currently the same as the index */
	broadcast_single(idx);
}

static void broadcast_single(attr_id_t id)
{
#ifdef CONFIG_ATTR_BROADCAST
	size_t msg_size = ATTR_BROADCAST_MSG_SIZE(1);
	attr_changed_msg_t *pb = BufferPool_Take(msg_size);

	if (pb == NULL) {
		LOG_ERR("Unable to allocate memory for attr %s", __func__);
		return;
	} else {
		pb->header.msgCode = FMC_ATTR_CHANGED;
		pb->header.txId = FWK_ID_RESERVED;
		pb->header.rxId = FWK_ID_RESERVED;
		pb->list[0] = id;
		pb->count = 1;
	}

	if (Framework_Broadcast((FwkMsg_t *)pb, msg_size) != FWK_SUCCESS) {
		/* most likely */
		LOG_DBG("Zero consumers for broadcast");
		BufferPool_Free(pb);
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

/**
 * @brief Hide attributes you don't want seen.
 * Alternatively, obscure them to make it known that they're
 * there but not show the value.
 */
static int allow_get(const ate_t *const entry)
{
	int r = 0;
	bool prevent_show = true;

	if (entry->flags & FLAGS_HIDE_IN_SHOW) {
		/* Hidden attribute */
		if (prevent_show) {
			r = -EACCES;
		}
	} else if (entry->flags & FLAGS_OBSCURE_IN_SHOW) {
		/* Obscured attribute */
		if (prevent_show) {
			r = -EPERM;
		}
	}

	return r;
}

/**
 * @brief Use display options to determine if value should be shown
 *
 * @return int 0 - value can be shown
 * @return int -EPERM - replace value with asterisks
 * @return int -EACCES - hide value (don't display asterisks)
 */
static int allow_show(const ate_t *const entry)
{
	if (entry->flags & FLAGS_HIDE_IN_SHOW) {
		/* Hidden attribute, do not show */
		return -EACCES;
	} else if (entry->flags & FLAGS_OBSCURE_IN_SHOW) {
		/* Obscured attribute, show asterisks for value */
		return -EPERM;
	}

	return 0;
}

static int show(const ate_t *const entry)
{
	uint32_t u = 0;
	int32_t i = 0;
	int r;

	r = allow_show(entry);
	if (r < 0) {
		if (r == -EPERM) {
			LOG_SHOW(CONFIG_ATTR_SHOW_FMT "******",
				 attr_table_index(entry), entry->name);
		}
		return r;
	}

	switch (entry->type) {
	case ATTR_TYPE_BOOL:
		memcpy(&u, entry->pData, entry->size);
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%s", attr_table_index(entry),
			 entry->name, u ? "true" : "false");
		break;

	case ATTR_TYPE_U8:
	case ATTR_TYPE_U16:
	case ATTR_TYPE_U32:
		memcpy(&u, entry->pData, entry->size);
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%u %s", attr_table_index(entry),
			 entry->name, u, GET_ENUM_STRING(u));
		break;

	case ATTR_TYPE_U64:
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%" PRIu64,
			 attr_table_index(entry), entry->name,
			 *(uint64_t *)entry->pData);
		break;

	case ATTR_TYPE_S8:
		i = (int32_t)(*(int8_t *)entry->pData);
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%d %s", attr_table_index(entry),
			 entry->name, i, GET_ENUM_STRING(i));
		break;

	case ATTR_TYPE_S16:
		i = (int32_t)(*(int16_t *)entry->pData);
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%d %s", attr_table_index(entry),
			 entry->name, i, GET_ENUM_STRING(i));
		break;

	case ATTR_TYPE_S32:
		i = *(int32_t *)entry->pData;
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%d %s", attr_table_index(entry),
			 entry->name, i, GET_ENUM_STRING(i));
		break;

	case ATTR_TYPE_S64:
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%" PRId64,
			 attr_table_index(entry), entry->name,
			 *(int64_t *)entry->pData);
		break;

	case ATTR_TYPE_FLOAT:
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "%e", attr_table_index(entry),
			 entry->name, *(float *)entry->pData);
		break;

	case ATTR_TYPE_STRING:
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "'%s'", attr_table_index(entry),
			 entry->name, (char *)entry->pData);
		break;

	default:
		LOG_SHOW(CONFIG_ATTR_SHOW_FMT "size: %u",
			 attr_table_index(entry), entry->name, entry->size);
		LOG_HEXDUMP_DBG(entry->pData, entry->size, "");
		break;
	}

	return 0;
}

/**
 * @brief Read key-value pair file from flash and load it into attributes/RAM.
 *
 * @param validate_first validate entire file when loading from an external
 * source. Otherwise, allow bad pairs when loading from a file that should be good.
 *
 * @param mask_modified Don't set modified flag during initialization
 */
static int load_attributes(const char *fname, bool validate_first,
			   bool mask_modified, bool enforce_writable)
{
	int r = -EPERM;
	size_t fsize;
	char *fstr = NULL;
	lcz_kvp_t *kv = NULL;
	size_t pairs = 0;
	bool do_write = false;

	do {
		r = lcz_kvp_parse_from_file(&KVP_CFG, fname, &fsize, &fstr,
					    &kv);
		LOG_INF("pairs: %d fsize: %d file: %s", r, fsize, fname);
		if (r < 0) {
			LOG_ERR("Unable to parse KVP file");
			create_unable_to_parse_feedback_file();
			break;
		}
		pairs = r;

		/* Always run validator because it generates feedback file */
		r = loader(kv, fstr, pairs, do_write, mask_modified,
			   enforce_writable);
		if (validate_first && r < 0) {
			break;
		}

		do_write = true;
		r = loader(kv, fstr, pairs, do_write, mask_modified,
			   enforce_writable);

	} while (0);

	/* Free the kvp allocation only if an allocation has been made */
	if (kv != NULL) {
		k_free(kv);
	}

	/* Free the fstr allocation only if an allocation has been made */
	if (fstr != NULL) {
		k_free(fstr);
	}

	if (r < 0) {
		LOG_ERR("%s: status %d", __func__, r);
	} else {
		LOG_DBG("status %d", r);
	}

	return r;
}

/* Use key-value information to find attribute table entry. */
static const ate_t *kvp_find_entry(lcz_kvp_t *kvp)
{
	memset(&conversion, 0, sizeof(conversion));

	if (kvp->key_len > ATTR_MAX_KEY_NAME_SIZE) {
		LOG_ERR("%s: Key name too long", __func__);
		return NULL;
	}

	if (kvp->val_len > ATTR_MAX_VALUE_SIZE) {
		LOG_ERR("%s: Value too long", __func__);
		return NULL;
	}

	if (kvp->key == NULL) {
		LOG_ERR("%s: Invalid key pointer", __func__);
		return NULL;
	}
	if (kvp->val == NULL) {
		LOG_ERR("%s: Invalid value pointer", __func__);
		return NULL;
	}

	/* null terminate key */
	memcpy(conversion.key, kvp->key, kvp->key_len);
	conversion.key[kvp->key_len] = 0;

	/* null terminate value */
	memcpy(conversion.value, kvp->val, kvp->val_len);
	conversion.value[kvp->val_len] = 0;

	/* Use name to find ID (table index) */
	conversion.id = attr_get_id(conversion.key);
	conversion.entry = attr_map(conversion.id);

	return conversion.entry;
}

/* After entry has been found, perform data conversion for value */
static int kvp_convert(lcz_kvp_t *kvp)
{
	int r = 0;

	if (conversion.entry == NULL || kvp == NULL) {
		return -EINVAL;
	}

	/* Convert from string into type
	 * Length isn't currently used by number validators.
	 */
	switch (conversion.entry->type) {
	case ATTR_TYPE_BOOL:
	case ATTR_TYPE_U8:
	case ATTR_TYPE_U16:
	case ATTR_TYPE_U32:
	case ATTR_TYPE_U64:
		conversion.result.u64 = strtoull(conversion.value, NULL, 0);
		conversion.result_len = sizeof(conversion.result.u64);
		break;
	case ATTR_TYPE_FLOAT:
		conversion.result.flt = strtof(conversion.value, NULL);
		conversion.result_len = sizeof(conversion.result.flt);
		break;
	case ATTR_TYPE_STRING:
		/* Extra copy makes validation step uniform. */
		memcpy(conversion.result.data, kvp->val, kvp->val_len);
		conversion.result_len = kvp->val_len;
		break;
	case ATTR_TYPE_BYTE_ARRAY:
		conversion.result_len =
			hex2bin(kvp->val, kvp->val_len, conversion.result.data,
				sizeof(conversion.result.data));
		if (conversion.result_len == 0) {
			r = -EINVAL;
		}
		break;
	case ATTR_TYPE_S8:
	case ATTR_TYPE_S16:
	case ATTR_TYPE_S32:
	case ATTR_TYPE_S64:
	case ATTR_TYPE_ATOMIC:
		conversion.result.s64 = strtoll(conversion.value, NULL, 0);
		conversion.result_len = sizeof(conversion.result.s64);
	default:
	case ATTR_TYPE_ANY:
	case ATTR_TYPE_UNKNOWN:
		r = -EINVAL;
		break;
	}

	return r;
}

/* Convert entry into key-value pair
 *
 * Assumes mutex (static buffer used to store value string)
 */
static int entry_to_kvp(lcz_kvp_t *kvp, const ate_t *const entry)
{
	static char str[ATTR_MAX_VALUE_SIZE];
	int len = -1;
	int r = 0;

	memset(str, 0, sizeof(str));

	kvp->key = (char *)entry->name;
	kvp->key_len = strlen(entry->name);
	kvp->val = str;
	kvp->val_len = 0;

	switch (entry->type) {
	case ATTR_TYPE_BOOL:
		len = snprintk(str, sizeof(str), "%u", *(bool *)entry->pData);
		break;
	case ATTR_TYPE_U8:
		len = snprintk(str, sizeof(str), "%u",
			       *(uint8_t *)entry->pData);
		break;
	case ATTR_TYPE_U16:
		len = snprintk(str, sizeof(str), "%u",
			       *(uint16_t *)entry->pData);
		break;
	case ATTR_TYPE_U32:
		len = snprintk(str, sizeof(str), "%u",
			       *(uint32_t *)entry->pData);
		break;

	case ATTR_TYPE_U64:
		len = snprintk(str, sizeof(str), "%" PRIu64,
			       *(uint64_t *)entry->pData);
		break;

	case ATTR_TYPE_S8:
		len = snprintk(str, sizeof(str), "%d",
			       (int32_t)(*(int8_t *)entry->pData));
		break;

	case ATTR_TYPE_S16:
		len = snprintk(str, sizeof(str), "%d",
			       (int32_t)(*(int16_t *)entry->pData));
		break;

	case ATTR_TYPE_S32:
		len = snprintk(str, sizeof(str), "%d",
			       *(int32_t *)entry->pData);
		break;

	case ATTR_TYPE_S64:
		len = snprintk(str, sizeof(str), "%" PRId64,
			       *(int64_t *)entry->pData);
		break;

	case ATTR_TYPE_FLOAT:
		len = snprintk(str, sizeof(str), "%e", *(float *)entry->pData);
		break;

	case ATTR_TYPE_STRING:
		len = snprintk(str, sizeof(str), "%s", (char *)entry->pData);
		if (len == 0) {
			len = snprintk(str, sizeof(str), "%s",
				       LCZ_KVP_EMPTY_VALUE_STR);
		}
		break;

	case ATTR_TYPE_BYTE_ARRAY:
	default:
		len = bin2hex(entry->pData, entry->size, str, sizeof(str));
		if (len != (entry->size * 2)) {
			r = -EINVAL;
		}
		break;
	}

	/* Check output of snprintk */
	if (len <= 0 || len >= sizeof(str)) {
		r = -EINVAL;
	}

	if (r == 0) {
		kvp->val_len = len;
	} else {
		LOG_ERR("%s: Invalid conversion size %d", __func__, len);
		kvp->val_len = 0;
	}

	return r;
}

static int loader(lcz_kvp_t *kv, char *fstr, size_t pairs, bool do_write,
		  bool mask_modified, bool enforce_writable)
{
	int r = -EPERM;
	size_t i;

	/* External files are always validated */
	if (!do_write) {
		start_feedback_file();
	}

	/* Loop through file
	 * Find entry for each key-value pair
	 * Convert value from file into binary.
	 */
	for (i = 0; i < pairs; i++) {
		if (kvp_find_entry(kv + i) == NULL) {
			r = -ATTR_WRITE_ERROR_PARAMETER_UNKNOWN;
		} else if (!is_writable(conversion.entry) && enforce_writable) {
			r = -ATTR_WRITE_ERROR_PARAMETER_NOT_WRITABLE;
		} else {
			r = kvp_convert(kv + i);
			if (r == 0) {
				if (do_write) {
					r = attr_write(conversion.entry,
						       conversion.entry->type,
						       conversion.result.data,
						       conversion.result_len);
				} else {
					r = validate(conversion.entry,
						     conversion.entry->type,
						     conversion.result.data,
						     conversion.result_len);
				}
			}
		}

		if (r < 0) {
			if (!do_write) {
				append_feedback_file(kv + i, r);
			}

			/* Return errno instead of feedback code */
			r = -EINVAL;

			if (IS_ENABLED(CONFIG_ATTR_BREAK_ON_LOAD_FAILURE)) {
				break;
			}
		}

		if (do_write && mask_modified) {
			if (conversion.entry != NULL) {
				atomic_clear_bit(
					attr_modified,
					attr_table_index(conversion.entry));
				atomic_clear_bit(
					attr_unchanged,
					attr_table_index(conversion.entry));
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
		LOG_WRN("Failure id: %u %s", attr_table_index(entry),
			entry->name);
		LOG_HEXDUMP_DBG(pv, vlen, "attr data");
	}
	return r;
}

static int attr_write(const ate_t *const entry, enum attr_type type, void *pv,
		      size_t vlen)
{
	int r = -EPERM;

	if (type == entry->type || type == ATTR_TYPE_ANY) {
		r = entry->validator(entry, pv, vlen, true);
	}

	if (r < 0) {
		LOG_WRN("Validation failure id: %u %s", attr_table_index(entry),
			entry->name);
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
	return ((entry->flags & FLAGS_WRITABLE) != 0);
}

static bool is_dump_rw(attr_index_t index)
{
	const ate_t *const entry = &ATTR_TABLE[index];
	bool b = false;

	if ((entry->flags & FLAGS_READABLE) &&
	    !(entry->flags & FLAGS_DEPRECATED)) {
		b = true;
	}

	return b;
}

static bool is_dump_w(attr_index_t index)
{
	const ate_t *const entry = &ATTR_TABLE[index];
	bool b = false;

	if ((entry->flags & FLAGS_READABLE) &&
	    !(entry->flags & FLAGS_DEPRECATED) &&
	    (entry->flags & FLAGS_WRITABLE)) {
		b = true;
	}
	return b;
}

static bool is_dump_ro(attr_index_t index)
{
	const ate_t *const entry = &ATTR_TABLE[index];
	bool b = false;

	if ((entry->flags & FLAGS_READABLE) &&
	    !(entry->flags & FLAGS_DEPRECATED) &&
	    !(entry->flags & FLAGS_WRITABLE)) {
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

/* write key, delimiter, and code */
static void append_feedback_file(lcz_kvp_t *kvp, int code)
{
#ifdef CONFIG_ATTR_LOAD_FEEDBACK
	const char *file_name = CONFIG_ATTRIBUTE_FEEDBACK_FILE;
	char code_str[] = "=0\n";
	int r;

	do {
		r = fsu_append_abs(file_name, kvp->key, kvp->key_len);
		if (r < 0) {
			break;
		}

		code = abs(code);
		if (code < 0 || code >= ATTR_WRITE_ERROR_CODE_COUNT) {
			LOG_ERR("Invalid attribute write feedback code %d",
				code);
			code = ATTR_WRITE_ERROR_UNKNOWN;
		}

		code_str[1] = code + '0';
		r = fsu_append_abs(file_name, code_str, strlen(code_str));
		if (r < 0) {
			break;
		}

	} while (0);

	if (r < 0) {
		LOG_ERR("Unable to append to feedback file");
	}
#endif
}

static void start_feedback_file(void)
{
#ifdef CONFIG_ATTR_LOAD_FEEDBACK
	if (fsu_write_abs(CONFIG_ATTRIBUTE_FEEDBACK_FILE, NULL, 0) < 0) {
		LOG_ERR("Unable to start feedback file");
	}
#endif
}

static void create_unable_to_parse_feedback_file(void)
{
#ifdef CONFIG_ATTR_LOAD_FEEDBACK
	lcz_kvp_t kvp = { .key = "0", .key_len = 1, .val = 0, .val_len = 0 };

	start_feedback_file();
	append_feedback_file(&kvp, ATTR_WRITE_ERROR_UNABLE_TO_PARSE_FILE);
#endif
}