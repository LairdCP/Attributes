# SPDX-License-Identifier: Apache-2.0

if (CONFIG_ATTR)

zephyr_include_directories(universal/include)
zephyr_include_directories(universal/include/generated)
zephyr_sources(
    universal/source/attr.c
    universal/source/attr_validator.c
    universal/source/generated/lock_decode.c
    universal/source/generated/lock_encode.c
)

zephyr_sources_ifdef(CONFIG_ATTR_SHELL
    universal/source/attr_shell.c
)

zephyr_sources_ifdef(CONFIG_MCUMGR_CMD_ATTRIBUTE_MGMT
    universal/source/attribute_mgmt.c
)

if(CONFIG_ATTR_DEVICE_OVERRIDE)

if(CONFIG_ATTR_DEVICE_OVERRIDE_SOURCE_FOLDER)
    set(ATTR_CUSTOM_PATH_BASE "${CMAKE_SOURCE_DIR}")
else()
    set(ATTR_CUSTOM_PATH_BASE "custom")
endif() # ATTR_DEVICE_OVERRIDE_SOURCE_FOLDER

zephyr_include_directories(${ATTR_CUSTOM_PATH_BASE}/${CONFIG_ATTR_DEVICE_OVERRIDE_NAME}/include)
zephyr_sources(
    ${ATTR_CUSTOM_PATH_BASE}/${CONFIG_ATTR_DEVICE_OVERRIDE_NAME}/source/attr_table.c
    ${ATTR_CUSTOM_PATH_BASE}/${CONFIG_ATTR_DEVICE_OVERRIDE_NAME}/source/attr_custom_validator.c
)

else()

if(CONFIG_BOARD_PINNACLE_100_DVK)
zephyr_include_directories(custom/MG100/include)
zephyr_sources(
    custom/MG100/source/attr_table.c
    custom/MG100/source/attr_custom_validator.c
)
endif()

if(CONFIG_BOARD_MG100)
zephyr_include_directories(custom/MG100/include)
zephyr_sources(
    custom/MG100/source/attr_table.c
    custom/MG100/source/attr_custom_validator.c
)
endif()

if(CONFIG_BOARD_BL5340_DVK_CPUAPP OR CONFIG_BOARD_BL5340PA_DVK_CPUAPP)
zephyr_include_directories(custom/BL5340/include)
zephyr_sources(
    custom/BL5340/source/attr_table.c
    custom/BL5340/source/attr_custom_validator.c
)
endif()

if(CONFIG_BOARD_BT6)
zephyr_include_directories(custom/BT610/include)
zephyr_sources(
    custom/BT610/source/attr_table.c
    custom/BT610/source/attr_custom_validator.c
)
endif()

if(CONFIG_BOARD_BT520)
zephyr_include_directories(custom/BT520/include)
zephyr_sources(
    custom/BT520/source/attr_table.c
    custom/BT520/source/attr_custom_validator.c
)
endif()

endif() # CONFIG_ATTR_DEVICE_OVERRIDE

if(CONFIG_ATTR_SETTINGS_LOCK)
zephyr_sources_ifdef(CONFIG_ATTR_SHELL_LOCK universal/source/shell_lock.c)
endif()

if(CONFIG_FRAMEWORK)
    # Add framework ID and msgcode files
    add_fwk_id_file(${CMAKE_CURRENT_SOURCE_DIR}/framework/attr_ids.h)
    add_fwk_msgcode_file(${CMAKE_CURRENT_SOURCE_DIR}/framework/attr_msgcodes.h)
endif()

endif() # CONFIG_ATTR
