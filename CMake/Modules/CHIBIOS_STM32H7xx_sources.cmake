#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(chibios)

# from os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32h7xx.mk
# List of the ChibiOS generic STM32H7xx startup and CMSIS files.

################################################################################################
# WHEN ADDING A NEW BOARD add the source code file for the hal/ports/STM32/STM32H7xx/platform.mk
################################################################################################

set(CHIBIOS_PORT_SRCS
    # startup code
    crt1.c
    vectors.S
    crt0_v7m.S

    nvic.c
    stm32_isr.c
    hal_lld.c

    hal_adc_lld.c
    hal_can_lld.c
    hal_crypto_lld.c
    hal_dac_lld.c
    stm32_dma.c
    stm32_exti.c
    hal_pal_lld.c
    hal_i2c_lld.c
    #hal_mac_lld.c
    hal_usb_lld.c
    hal_wspi_lld.c 
    hal_trng_lld.c
    hal_rtc_lld.c
    hal_sdc_lld.c

    #hal_i2s_lld.c
    hal_spi_lld.c

    hal_st_lld.c
    hal_gpt_lld.c
    hal_icu_lld.c
    hal_pwm_lld.c

    hal_serial_lld.c
    hal_uart_lld.c

    hal_wdg_lld.c

    # RT
    chcore.c
    chcoreasm.S
)

foreach(SRC_FILE ${CHIBIOS_PORT_SRCS})

    set(CHIBIOS_H7_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(CHIBIOS_H7_SRC_FILE ${SRC_FILE}
        PATHS 

            ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M/compilers/GCC
            ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC
            ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M
            ${chibios_SOURCE_DIR}/os/hal/ports/common/ARMCMx

            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/STM32H7xx
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/ADCv4
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/BDMAv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/CRYPv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/DACv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/DMAv2
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/EXTIv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/FDCANv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/GPIOv2
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/I2Cv3
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/MDMAv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/OTGv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/QUADSPIv2
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/RNGv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/RTCv2
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SDMMCv2
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SPIv3
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SYSTICKv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/TIMv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/USARTv2
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/xWDGv1

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${CHIBIOS_H7_SRC_FILE}")
    endif()

    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_H7_SRC_FILE})

endforeach()

list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/portability/GCC)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARM-common)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/devices/STM32H7xx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ext/ARM/CMSIS/Core/Include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ext/ST/STM32H7xx)

list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/ADCv4)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/BDMAv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/CRYPv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/DACv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/DMAv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/EXTIv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/FDCANv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/GPIOv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/I2Cv3)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/MDMAv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/OTGv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/QUADSPIv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/RNGv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/RTCv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SDMMCv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SPIv3)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SYSTICKv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/TIMv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/USARTv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/xWDGv1)


####################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the include directory(ies) below 
####################################################################################
# component STM32_FLASH
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FLASHv2)
# component STM32_CRC
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/CRCv1)
# component STM32_RNG
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/RNGv1) 
# component STM32_FSMC (Flexible Memory Controller)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FSMCv1)
# component STM32_ONEWIRE
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/ONEWIREv1) 

###############################################################################################################################
# Add above the required include directory(ies) for a new nanoFramework overlay component that you are adding
# following the template below. 
#
# list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/<path-here>)
###############################################################################################################################


####################################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the source file(s) specific to this series below 
####################################################################################################
# component STM32_FLASH
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FLASHv2/flash_lld.c)
# component STM32_CRC
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/CRCv1/crc_lld.c)
# component STM32_RNG
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/RNGv1/rng_lld.c)
# component STM32_FSMC (Flexible Memory Controller)
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FSMCv1/fsmc_nand_lld.c)
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FSMCv1/fsmc_sdram_lld.c)
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FSMCv1/fsmc_sram_lld.c)
# component STM32_ONEWIRE
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/ONEWIREv1/onewire_lld.c)

##########################################################################################################################
# Add above ALL the source code file(s) low level driver specif for a series required for a new nanoFramework 
# overlay component that you are adding following the template below. 
#
# list(APPEND CHIBIOS_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/src/<path-here>)
##########################################################################################################################
