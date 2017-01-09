# from startup_stm32f4xx.mk
# List of the ChibiOS generic STM32F4xx startup and CMSIS files.

list(APPEND CHIBIOS_SOURCES ${PROJECT_BINARY_DIR}/ChibiOS_Source/common/ports/ARMCMx/compilers/GCC/crt1.c)
list(APPEND CHIBIOS_SOURCES ${PROJECT_BINARY_DIR}/ChibiOS_Source/common/ports/ARMCMx/compilers/GCC/vectors.c)
list(APPEND CHIBIOS_SOURCES ${PROJECT_BINARY_DIR}/ChibiOS_Source/common/ports/ARMCMx/compilers/GCC/crt0_v7m.s)

################################################################################################
# WHEN ADDING A NEW BOARD add the source code file for the hal/ports/STM32/STM32F4xx/platform.mk
################################################################################################

set(CHIBIOS_PORT_SRCS
   
    nvic.c
    hal_lld.c
    ext_lld_isr.c

    adc_lld.c
    can_lld.c
    ext_lld.c
    dac_lld.c
    stm32_dma.c
    pal_lld.c
    i2c_lld.c
    mac_lld.c
    usb_lld.c
    rtc_lld.c
    sdc_lld.c
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
    chcore_v7m.c
    chcoreasm_v7m.s
)

foreach(SRC_FILE ${CHIBIOS_PORT_SRCS})
    set(CHIBIOS_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(CHIBIOS_SRC_FILE ${SRC_FILE}
        PATHS 

            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/common/ARMCMx
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/STM32F4xx
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/ADCv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/CANv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/EXTIv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/DACv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/DMAv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/GPIOv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/I2Cv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/MACv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/OTGv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/RTCv2
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/SDIOv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/SPIv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/TIMv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/USARTv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/USBv1
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/xWDGv1

            ${PROJECT_BINARY_DIR}/ChibiOS_Source/rt/ports/ARMCMx
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/rt/ports/ARMCMx/compilers/GCC

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${CHIBIOS_SRC_FILE}") # debug helper
    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_SRC_FILE})
endforeach()


list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/common/ports/ARMCMx/devices/STM32F4xx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/STM32F4xx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/ADCv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/CANv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/EXTIv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/DACv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/DMAv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/GPIOv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/I2Cv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/MACv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/OTGv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/RTCv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/SDIOv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/SPIv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/TIMv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/USARTv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/USBv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/LLD/xWDGv1)


#######################################################################################################################################
# WHEN ADDING A NEW BOARD add the respective if clause bellow along with the startup file and the linker file name (without extension)
#######################################################################################################################################

# linker file
if(${CHIBIOS_BOARD} STREQUAL "ST_STM32F4_DISCOVERY")
    set(LINKER_FILE "STM32F407xG")
endif()

#######################################################################################################################################

function(CHIBIOS_SET_LINKER_OPTIONS TARGET)

    get_target_property(TARGET_LD_FLAGS ${TARGET} LINK_FLAGS)
    if(TARGET_LD_FLAGS)
        set(TARGET_LD_FLAGS "-T${PROJECT_BINARY_DIR}/ChibiOS_Source/common/ports/ARMCMx/compilers/GCC/ld/${LINKER_FILE}.ld ${TARGET_LD_FLAGS}")
    else()
        set(TARGET_LD_FLAGS "-T${PROJECT_BINARY_DIR}/ChibiOS_Source/common/ports/ARMCMx/compilers/GCC/ld/${LINKER_FILE}.ld")
    endif()
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS ${TARGET_LD_FLAGS})

endfunction()
