#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# ORGPAL_PALX — STM32F769NI, W25Q512 QUADSPI external flash.
# Called from MCUboot/CMakeLists.txt via include() after the series-common
# variables (MCUBOOT_SERIES_COMMON_*) have been set for STM32F7xx.
# Always optimise for size (-Os): the bootloader slot is 64 kB.

include(FetchContent)
FetchContent_GetProperties(stm32f7_hal_driver)

nf_setup_mcuboot_target_build(

    LINKER_FILE
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot/mcuboot_stm32f769_palx.ld

    EXTRA_SOURCES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/board.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/common/target_ext_flash.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/src/stm32_qspi/hal_stm32_qspi.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/QSPIv1/qspi_lld.c
        ${stm32f7_hal_driver_SOURCE_DIR}/Src/stm32f7xx_hal_qspi.c
        ${stm32f7_hal_driver_SOURCE_DIR}/Src/stm32f7xx_hal_cortex.c
        ${stm32f7_hal_driver_SOURCE_DIR}/Src/stm32f7xx_hal_dma.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot/mcuboot_flash_map_boot.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot/mcuboot_detect_pin.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/common/usbcfg.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot/mcuboot_target_init.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot/mcuboot_heartbeat_led.c
        # ChibiOS USB HAL sources for USB CDC transport.
        ${chibios_SOURCE_DIR}/os/hal/src/hal_usb.c
        ${chibios_SOURCE_DIR}/os/hal/src/hal_serial_usb.c
        ${chibios_SOURCE_DIR}/os/hal/src/hal_buffers.c
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/OTGv1/hal_usb_lld.c

    EXTRA_INCLUDES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/common
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/include/stm32_qspi
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/QSPIv1
        ${stm32f7_hal_driver_SOURCE_DIR}/Inc
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/OTGv1

    COMPILE_DEFINITIONS
        MCUBOOT_USE_TINYCRYPT=1

    EXTRA_COMPILE_OPTIONS
        $<$<CONFIG:Release>:-Os>
        $<$<CONFIG:MinSizeRel>:-Os>
        $<$<CONFIG:Debug>:-Os -g3>
        $<$<CONFIG:RelWithDebInfo>:-Os -g>
)

# if HEX2DFU tool is available pack nanoMcubooter into a DFU package
if(HEX2DFU_TOOL_AVAILABLE)

    # bootloader address comes from the MCUboot flash layout
    nf_extract_define_from_header(
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/ORGPAL_PALX/MCUboot/mcuboot_flash_layout.h
        NF_MCUBOOT_SLOT_BOOTLOADER_OFF
        MCUBOOT_BOOTLOADER_ADDRESS)
    string(REGEX REPLACE "^0[xX]" "" MCUBOOT_BOOTLOADER_ADDRESS "${MCUBOOT_BOOTLOADER_ADDRESS}")

    nf_generate_single_dfu_package(
        ${NANOMCUBOOTER_PROJECT_NAME}.elf
        ${CMAKE_BINARY_DIR}/${NANOMCUBOOTER_PROJECT_NAME}.bin
        ${MCUBOOT_BOOTLOADER_ADDRESS}
        ${CMAKE_BINARY_DIR}/${NANOMCUBOOTER_PROJECT_NAME}.dfu
    )

endif()
