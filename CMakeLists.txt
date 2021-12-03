# SPDX-License-Identifier: Apache-2.0

if (CONFIG_ATTR)

zephyr_include_directories(universal/include)
zephyr_sources(
    universal/source/attr.c
    universal/source/attr_validator.c
)

zephyr_sources_ifdef(CONFIG_ATTR_SHELL
    universal/source/attr_shell.c
)

zephyr_sources_ifdef(CONFIG_MCUMGR_CMD_SENTRIUS_MGMT
    universal/source/sentrius_mgmt.c
)


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

zephyr_sources_ifdef(CONFIG_ATTR_SHELL_LOCK universal/source/shell_lock.c)

endif() # CONFIG_ATTR
