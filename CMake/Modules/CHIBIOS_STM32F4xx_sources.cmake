#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# from startup_stm32f4xx.mk
# List of the ChibiOS generic STM32F4xx startup and CMSIS files.

################################################################################################
# WHEN ADDING A NEW BOARD add the source code file for the hal/ports/STM32/STM32F4xx/platform.mk
################################################################################################

set(CHIBIOS_PORT_SRCS
    # startup code
    crt1.c
    vectors.c
    crt0_v7m.S

    nvic.c
    hal_lld.c
    hal_ext_lld_isr.c

    hal_adc_lld.c
    hal_can_lld.c
    hal_ext_lld.c
    hal_dac_lld.c
    stm32_dma.c
    hal_pal_lld.c
    hal_i2c_lld.c
    hal_mac_lld.c
    hal_usb_lld.c
    hal_rtc_lld.c
    hal_sdc_lld.c
    hal_i2s_lld.c
    hal_spi_lld.c
    hal_gpt_lld.c
    hal_icu_lld.c
    hal_pwm_lld.c
    hal_st_lld.c
    hal_serial_lld.c
    hal_uart_lld.c
    hal_usb_lld.c
    hal_wdg_lld.c

    # RT
    chcore.c
    chcore_v7m.c
    chcoreasm_v7m.S
)

foreach(SRC_FILE ${CHIBIOS_PORT_SRCS})
    set(CHIBIOS_F4_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(CHIBIOS_F4_SRC_FILE ${SRC_FILE}
        PATHS 

            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx/compilers/GCC
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/startup/ARMCMx/compilers/GCC
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/common/ARMCMx

            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/STM32F4xx
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/ADCv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/CANv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/EXTIv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DACv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DMAv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/GPIOv2
            if(${CHIBIOS_BOARD} STREQUAL "ST_STM32F429I_DISCOVERY" OR ${CHIBIOS_BOARD} STREQUAL "MBN_QUAIL")
                ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/I2Cv1
            else()
                ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/I2Cv2
            endif()
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/MACv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/OTGv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/RTCv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/SDIOv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/SPIv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/TIMv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/USARTv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/USBv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/xWDGv1

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${CHIBIOS_F4_SRC_FILE}") # debug helper
    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_F4_SRC_FILE})
endforeach()

list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/startup/ARMCMx/devices/STM32F4xx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ext/CMSIS/ST/STM32F4xx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/STM32F4xx)

list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/ADCv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/CANv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/EXTIv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DACv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DMAv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/GPIOv2)
if(${CHIBIOS_BOARD} STREQUAL "ST_STM32F429I_DISCOVERY" OR ${CHIBIOS_BOARD} STREQUAL "MBN_QUAIL")
    list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/I2Cv1)
else()
    list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/I2Cv2)
endif()
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/MACv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/OTGv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/RTCv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/SDIOv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/SPIv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/TIMv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/USARTv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/USBv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/xWDGv1)


####################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the include directory(ies) bellow 
####################################################################################
# component STM32_FLASH
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/FLASHv2)

###############################################################################################################################
# Add above the required include directory(ies) for a new nanoFramework overlay component that you are adding
# following the template bellow. 
#
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/<path-here>)
###############################################################################################################################


####################################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the source file(s) specific to this series bellow 
####################################################################################################
# component STM32_FLASH
list(APPEND ChibiOSnfOverlay_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/FLASHv2/flash_lld.c)

##########################################################################################################################
# Add above ALL the source code file(s) low level driver specif for a series required for a new nanoFramework 
# overlay component that you are adding following the template bellow. 
#
# list(APPEND CHIBIOS_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/<path-here>)
##########################################################################################################################


#######################################################################################################################################
# this function sets the linker options AND a specific linker file (full path and name, including extension)
function(CHIBIOS_SET_LINKER_OPTIONS_AND_FILE TARGET LINKER_FILE_NAME)

    get_target_property(TARGET_LD_FLAGS ${TARGET} LINK_FLAGS)
    if(TARGET_LD_FLAGS)
        set(TARGET_LD_FLAGS "-T${LINKER_FILE_NAME} ${TARGET_LD_FLAGS}")
    else()
        set(TARGET_LD_FLAGS "-T${LINKER_FILE_NAME}")
    endif()
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS ${TARGET_LD_FLAGS})

endfunction()
