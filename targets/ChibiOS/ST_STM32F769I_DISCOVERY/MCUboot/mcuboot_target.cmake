#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

set(MCUBOOT_EXTRA_SOURCES
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/board.c
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/MCUboot/mcuboot_flash_map_boot.c
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/MCUboot/mcuboot_detect_pin.c
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/MCUboot/mcuboot_target_init.c
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/MCUboot/mcuboot_heartbeat_led.c
)

if(NF_FEATURE_MCUBOOT_HAS_SDCARD)
    list(APPEND MCUBOOT_EXTRA_SOURCES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/MCUboot/mcuboot_media_boot.c
    )
endif()

nf_setup_mcuboot_target_build(

    LINKER_FILE
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/MCUboot/mcuboot_stm32f769_disco.ld

    EXTRA_SOURCES
        ${MCUBOOT_EXTRA_SOURCES}

    EXTRA_INCLUDES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/MCUboot

    COMPILE_DEFINITIONS
        MCUBOOT_USE_TINYCRYPT=1

    EXTRA_COMPILE_OPTIONS
        $<$<CONFIG:Release>:-Os>
        $<$<CONFIG:MinSizeRel>:-Os>
        $<$<CONFIG:Debug>:-Os -g3>
        $<$<CONFIG:RelWithDebInfo>:-Os -g>
)
