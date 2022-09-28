# SPDX-License-Identifier: Apache-2.0

if (CONFIG_ATTR)

zephyr_include_directories(include)
zephyr_include_directories(include/generated)
zephyr_sources(
    src/attr.c
    src/attr_validator.c
    src/generated/attribute_mgmt_encode.c
    src/generated/attribute_mgmt_decode.c
)

zephyr_sources_ifdef(CONFIG_ATTR_SHELL
    src/attr_shell.c
)

zephyr_sources_ifdef(CONFIG_MCUMGR_CMD_ATTRIBUTE_MGMT
    src/attribute_mgmt.c
)

if (NOT ATTR_CUSTOM_PATH_BASE)
message(FATAL_ERROR "ATTR_CUSTOM_PATH_BASE has not been set in application CMakeLists.txt file")
endif()

zephyr_include_directories(${ATTR_CUSTOM_PATH_BASE}/include)
zephyr_sources(
    ${ATTR_CUSTOM_PATH_BASE}/src/attr_hash.c
    ${ATTR_CUSTOM_PATH_BASE}/src/attr_table.c
)

if(CONFIG_FRAMEWORK)
    # Add framework ID and msgcode files
    add_fwk_id_file(${CMAKE_CURRENT_SOURCE_DIR}/framework/attr_ids.h)
    add_fwk_msgcode_file(${CMAKE_CURRENT_SOURCE_DIR}/framework/attr_msgcodes.h)
endif()

endif() # CONFIG_ATTR
