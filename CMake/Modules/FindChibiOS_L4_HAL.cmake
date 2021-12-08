#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(chibios)

# set include directories for ChibiOS HAL
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/common/ARMCMx)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/STM32L4xx)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/include)

list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/portability/GCC)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARM-common)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/devices/STM32L4xx)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ext/ARM/CMSIS/Core/Include)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ext/ST/STM32L4xx)

list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/ADCv3)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/CANv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/CRYPv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/DACv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/DMAv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/EXTIv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/GPIOv3)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/I2Cv2)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/OTGv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/QUADSPIv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/RCCv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/RNGv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/RTCv2)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SDMMCv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SPIv2)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SYSTICKv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/TIMv1)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/USART)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/USARTv2)
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/xWDGv1)

list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${CHIBIOS_BOARD_DEFINITIONS_PATH})

# append dummy include directory when not using ChibiOS-Contrib
if(NOT CHIBIOS_CONTRIB_REQUIRED)
    list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/include/dummy_includes)
endif()

####################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the include directory(ies) below 
####################################################################################
# component STM32_FLASH
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FLASHv4)
# component STM32_CRC
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/CRCv1)
# component STM32_RNG
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/RNGv1) 

###############################################################################################################################
# Add above the required include directory(ies) for a new nanoFramework overlay component that you are adding
# following the template below. 
#
# list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/<path-here>)
###############################################################################################################################

####################################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the source file(s) specific to this series below 
####################################################################################################
# component STM32_FLASH
list(APPEND CHIBIOS_HAL_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FLASHv4/flash_lld.c)
# component STM32_CRC
list(APPEND CHIBIOS_HAL_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/CRCv1/crc_lld.c)
# component STM32_RNG
list(APPEND CHIBIOS_HAL_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/RNGv1/rng_lld.c)

##########################################################################################################################
# Add above ALL the source code file(s) low level driver specif for a series required for a new nanoFramework 
# overlay component that you are adding following the template below. 
#
# list(APPEND CHIBIOS_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/src/<path-here>)
##########################################################################################################################

# OSHAL sources need to be added for ChibiOS RT-NIL or Azure RTOS depending on build
# adjust search path here
if(RTOS_CHIBIOS_CHECK)
    set(OSHAL_PATH ${chibios_SOURCE_DIR}/os/hal/osal/rt-nil)
elseif(RTOS_AZURERTOS_CHECK)
    set(OSHAL_PATH ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ChibiOS)
else()
    message(FATAL_ERROR "RTOS not configured to use ChibiOS HAL")
endif()

# set include path for OSHAL header
list(APPEND CHIBIOS_HAL_INCLUDE_DIRS ${OSHAL_PATH})

# source files for ChibiOS HAL
set(CHIBIOS_HAL_SRCS

    # startup code
    crt1.c
    vectors.S
    crt0_v7m.S

    nvic.c
    stm32_isr.c

    # HAL-OSAL files
    hal.c
    hal_st.c
    
    hal_buffers.c
    hal_queues.c
    hal_flash.c
    hal_mmcsd.c
    
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

    # LLD HAL files
    hal_lld.c
    hal_efl_lld.c

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

    hal_i2s_lld.c
    hal_spi_v2_lld.c

    hal_st_lld.c
    hal_gpt_lld.c
    hal_icu_lld.c
    hal_pwm_lld.c

    hal_serial_lld.c
    hal_uart_lld.c

    hal_wdg_lld.c


    # OSAL 
    osal.c

    board.c
)

foreach(SRC_FILE ${CHIBIOS_HAL_SRCS})

    set(CHIBIOS_HAL_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(CHIBIOS_HAL_SRC_FILE ${SRC_FILE}
        PATHS 

            ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M/compilers/GCC
            ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC

            ${chibios_SOURCE_DIR}/os/hal/src
            ${chibios_SOURCE_DIR}/os/hal/ports/common/ARMCMx

            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/STM32L4xx

            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/STM32L4xx
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/ADCv3
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/CANv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/CRYPv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/DACv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/DMAv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/EXTIv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/GPIOv3
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/I2Cv2
            #${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/MACv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/OTGv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/QUADSPIv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/RNGv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/RTCv2
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SDMMCv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SPIv2
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SYSTICKv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/TIMv1
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/USARTv2
            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/xWDGv1

            ${OSHAL_PATH}
            
            ${CHIBIOS_BOARD_DEFINITIONS_PATH}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${CHIBIOS_HAL_SRC_FILE}")
    endif()

    list(APPEND CHIBIOS_HAL_SOURCES ${CHIBIOS_HAL_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(ChibiOS_L4_HAL DEFAULT_MSG CHIBIOS_HAL_INCLUDE_DIRS CHIBIOS_HAL_SOURCES)
