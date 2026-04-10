#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(chibios)

# set include directories for ChibiOS HAL (RP2040)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/common/ARMCMx)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/include)
# >>> To use nanoFramework overlay EFL driver, uncomment the next line and comment the one after <<<
# list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/RP/LLD/EFLv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/EFLv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/RP2040)

list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/portability/GCC)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARM-common)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv6-M)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv6-M/compilers/GCC)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/devices/RP2040)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ext/ARM/CMSIS/Core/Include)

# RP2040 LLD (Low Level Driver) include directories
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/ADCv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/DMAv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/GPIOv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/I2Cv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/PIOv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/PWMv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/RTCv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/SPIv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/TIMERv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/UARTv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/USBv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/WDGv1)

list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${CHIBIOS_BOARD_DEFINITIONS_PATH})

# append dummy include directory when not using ChibiOS-Contrib
if(NOT CHIBIOS_CONTRIB_REQUIRED)
    list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/include/dummy_includes)
endif()

# OSHAL sources need to be added for ChibiOS RT-NIL or ThreadX depending on build
if(RTOS_CHIBIOS_CHECK)
    set(OSHAL_PATH ${chibios_SOURCE_DIR}/os/hal/osal/rt-nil)
elseif(RTOS_THREADX_CHECK)
    set(OSHAL_PATH ${CMAKE_SOURCE_DIR}/targets/ThreadX/ChibiOS/HAL)
else()
    message(FATAL_ERROR "RTOS not configured to use ChibiOS HAL")
endif()

# set include path for OSHAL header
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${OSHAL_PATH})

# source files for ChibiOS HAL (RP2040)
set(CHIBIOS_HAL_SRCS

    # startup code
    crt1.c
    vectors.S
    crt0_v6m.S

    # RP2040 2nd stage bootloader (required for XIP flash)
    bs2_default_padded_checksummed.S

    # RP2040 platform-specific ISR and clock init
    hal_lld.c
    rp_clocks.c
    rp_isr.c
    rp_pll.c
    rp_xosc.c

    # NVIC driver (common ARMCMx)
    nvic.c

    # HAL-OSAL files
    hal.c
    hal_st.c
    
    hal_buffers.c
    hal_queues.c
    hal_flash.c
    hal_mmcsd.c
    hal_safety.c
    
    hal_adc.c
    hal_can.c
    hal_crypto.c
    hal_dac.c
    hal_efl.c
    hal_gpt.c
    hal_i2c.c
    hal_i2s.c
    hal_icu.c
    hal_mac.c
    hal_mmc_spi.c
    hal_pal.c
    hal_pwm.c
    hal_rtc.c
    hal_sdc.c
    hal_serial.c
    hal_serial_usb.c
    hal_sio.c
    hal_spi.c
    hal_trng.c
    hal_uart.c
    hal_usb.c
    hal_wdg.c
    hal_wspi.c

    # RP2040 LLD (Low Level Driver) sources
    hal_adc_lld.c
    rp_dma.c
    hal_pal_lld.c
    hal_i2c_lld.c
    hal_spi_lld.c
    hal_st_lld.c
    hal_pwm_lld.c
    hal_sio_lld.c
    hal_usb_lld.c
    hal_wdg_lld.c
    hal_efl_lld.c
    hal_rtc_lld.c

    # OSAL
    osal.c

    board.c
)

foreach(SRC_FILE ${CHIBIOS_HAL_SRCS})

    set(CHIBIOS_HAL_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(CHIBIOS_HAL_SRC_FILE ${SRC_FILE}
        PATHS 

            ${chibios_SOURCE_DIR}/os/common/ports/ARMv6-M/compilers/GCC
            ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC
            ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/devices/RP2040

            ${chibios_SOURCE_DIR}/os/hal/src
            ${chibios_SOURCE_DIR}/os/hal/ports/common/ARMCMx

            # >>> To use nanoFramework overlay, uncomment next line and comment the one after <<<
            # ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/RP/LLD/EFLv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/EFLv1

            ${chibios_SOURCE_DIR}/os/hal/ports/RP/RP2040

            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/ADCv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/DMAv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/GPIOv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/I2Cv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/PIOv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/PWMv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/RTCv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/SPIv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/TIMERv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/UARTv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/USBv1
            ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/WDGv1

            ${OSHAL_PATH}

            ${CHIBIOS_BOARD_DEFINITIONS_PATH}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${CHIBIOS_HAL_SRC_FILE}")
    endif()

    list(APPEND CHIBIOS_HAL_SOURCES ${CHIBIOS_HAL_SRC_FILE})

endforeach()

# Suppress -Wshadow for ChibiOS ADC HAL (variable shadowing in macro expansion)
SET_SOURCE_FILES_PROPERTIES(${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/ADCv1/hal_adc_lld.c PROPERTIES COMPILE_FLAGS -Wno-shadow)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(ChibiOS_RP2040_HAL DEFAULT_MSG CHIBIOS_HAL_INCLUDE_DIRS CHIBIOS_HAL_SOURCES)
