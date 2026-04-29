#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# ORGPAL_PALTHREE — STM32F769ZI, AT25SF641 SPI1 external flash.
# Called from MCUboot/CMakeLists.txt via include() after the series-common
# variables (MCUBOOT_SERIES_COMMON_*) have been set for STM32F7xx.

set(MCUBOOT_EXTRA_SOURCES
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALTHREE/board.c
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALTHREE/common/target_ext_flash.c
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALTHREE/MCUboot/mcuboot_flash_map_boot.c
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALTHREE/MCUboot/mcuboot_detect_pin.c
)

if(NF_FEATURE_MCUBOOT_HAS_SDCARD)
    list(APPEND MCUBOOT_EXTRA_SOURCES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALTHREE/MCUboot/mcuboot_sdcard_boot.c
    )
endif()

nf_setup_mcuboot_target_build(

    LINKER_FILE
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALTHREE/MCUboot/mcuboot_stm32f769.ld

    EXTRA_SOURCES
        ${MCUBOOT_EXTRA_SOURCES}

    EXTRA_INCLUDES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALTHREE
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALTHREE/MCUboot

    COMPILE_DEFINITIONS
        MCUBOOT_USE_TINYCRYPT=1

    EXTRA_COMPILE_OPTIONS
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:MinSizeRel>:-Os>
        $<$<CONFIG:Debug>:-Og -g3>
        $<$<CONFIG:RelWithDebInfo>:-Os -g>
)
