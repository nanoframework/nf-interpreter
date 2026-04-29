#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# ST_STM32F769I_DISCOVERY — STM32F769NI, SD card FatFs secondary slots (stub).
# Called from MCUboot/CMakeLists.txt via include() after the series-common
# variables (MCUBOOT_SERIES_COMMON_*) have been set for STM32F7xx.
# Secondary slots use SD card FatFs files; FatFs integration is deferred —
# mcuboot_ext_flash_init() returns -1 and MCUboot boots the primary slot.

# ST_STM32F769I_DISCOVERY — STM32F769NI, SD card FatFs secondary slots (stub).
# Called from MCUboot/CMakeLists.txt via include() after the series-common
# variables (MCUBOOT_SERIES_COMMON_*) have been set for STM32F7xx.
# Secondary slots use SD card FatFs files; FatFs integration is deferred —
# mcuboot_ext_flash_init() returns -1 and MCUboot boots the primary slot.

nf_setup_mcuboot_target_build(

    LINKER_FILE
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/MCUboot/mcuboot_stm32f769_disco.ld

    EXTRA_SOURCES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/board.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/MCUboot/mcuboot_flash_map_boot.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ST_STM32F769I_DISCOVERY/MCUboot/mcuboot_detect_pin.c

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
