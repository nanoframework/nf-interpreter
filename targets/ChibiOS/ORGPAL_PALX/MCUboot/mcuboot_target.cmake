#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# ORGPAL_PALX — STM32F769NI, W25Q512 QUADSPI external flash.
# Called from MCUboot/CMakeLists.txt via include() after the series-common
# variables (MCUBOOT_SERIES_COMMON_*) have been set for STM32F7xx.
# Always optimise for size (-Os): the bootloader slot is 32 kB.

# ORGPAL_PALX — STM32F769NI, W25Q512 QUADSPI external flash.
# Called from MCUboot/CMakeLists.txt via include() after the series-common
# variables (MCUBOOT_SERIES_COMMON_*) have been set for STM32F7xx.
# Always optimise for size (-Os): the bootloader slot is 32 kB.

nf_setup_mcuboot_target_build(

    LINKER_FILE
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot/mcuboot_stm32f769_palx.ld

    EXTRA_SOURCES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/board.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot/w25q512_qspi_chibios.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot/mcuboot_flash_map_boot.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot/mcuboot_detect_pin.c

    EXTRA_INCLUDES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot

    COMPILE_DEFINITIONS
        STM32F769xx
        MCUBOOT_USE_TINYCRYPT=1

    EXTRA_COMPILE_OPTIONS
        $<$<CONFIG:Release>:-Os>
        $<$<CONFIG:MinSizeRel>:-Os>
        $<$<CONFIG:Debug>:-Os -g3>
        $<$<CONFIG:RelWithDebInfo>:-Os -g>
)
