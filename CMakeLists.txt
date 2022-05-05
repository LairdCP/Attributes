# SPDX-License-Identifier: Apache-2.0

if (CONFIG_ATTR)

zephyr_include_directories(include)
zephyr_sources(
    source/attr.c
    source/attr_validator.c
)

zephyr_sources_ifdef(CONFIG_ATTR_SHELL
    source/attr_shell.c
)

zephyr_sources_ifdef(CONFIG_MCUMGR_CMD_SENTRIUS_MGMT
    source/sentrius_mgmt.c
)

if (NOT ATTR_PATH)
message(FATAL_ERROR "set(ATTR_PATH \"path\") not in application CMakeLists.txt file")
endif()

zephyr_include_directories(${ATTR_PATH}/include)
zephyr_sources(
    ${ATTR_PATH}/source/attr_table.c
)

if(EXISTS ${ATTR_PATH}/source/attr_custom_validator.c)
zephyr_compile_definitions(ATTR_CUSTOM_VALIDATOR)
zephyr_sources(
    ${ATTR_PATH}/source/attr_custom_validator.c
)
endif()

endif() # CONFIG_ATTR
