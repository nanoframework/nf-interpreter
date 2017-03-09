#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# from startup_stm32f0xx.mk
# List of the ChibiOS generic STM32F0xx startup and CMSIS files.

################################################################################
# WHEN ADDING A NEW BOARD add the source code file for the STM32F0xx\platform.mk
################################################################################

set(CHIBIOS_PORT_SRCS
    # startup code
    crt1.c
    vectors.c
    crt0_v6m.s

    nvic.c
    hal_lld.c
    ext_lld_isr.c
    adc_lld.c
    can_lld.c
    dac_lld.c
    stm32_dma.c
    ext_lld.c
    pal_lld.c
    i2c_lld.c
    rtc_lld.c
    i2s_lld.c
    spi_lld.c
    gpt_lld.c
    icu_lld.c
    pwm_lld.c
    st_lld.c
    serial_lld.c
    uart_lld.c
    usb_lld.c
    wdg_lld.c

    # RT
    chcore.c
    chcore_v6m.c
    chcoreasm_v6m.s
)

foreach(SRC_FILE ${CHIBIOS_PORT_SRCS})
    set(CHIBIOS_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(CHIBIOS_SRC_FILE ${SRC_FILE}
        PATHS 

            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/STM32F0xx
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/ADCv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/CANv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DACv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DMAv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/EXTIv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/GPIOv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/I2Cv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/RTCv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/SPIv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/TIMv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/USARTv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/USBv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/xWDGv1

            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/common/ARMCMx
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/rt/ports/ARMCMx
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/rt/ports/ARMCMx/compilers/GCC
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx/compilers/GCC/

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${CHIBIOS_SRC_FILE}") # debug helper
    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_SRC_FILE})
endforeach()


list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/startup/ARMCMx/devices/STM32F0xx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx/devices/STM32F0xx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ext/CMSIS/ST/STM32F0xx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ext/CMSIS/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/STM32F0xx)

list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/ADCv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/CANv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DACv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DMAv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/EXTIv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/GPIOv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/I2Cv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/RTCv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/SPIv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/TIMv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/USARTv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/USBv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/xWDGv1)


####################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the include directory(ies) bellow 
####################################################################################
# component STM32_FLASH
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/FLASHv1)

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
list(APPEND ChibiOSnfOverlay_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/FLASHv1/flash_lld.c)

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
