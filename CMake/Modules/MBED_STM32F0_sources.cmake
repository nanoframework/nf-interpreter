###########################################################################
# WHEN ADDING A NEW SERIES add the include to the vendor source code bellow
###########################################################################
include(MBEDOS_STM_sources)

#######################################################################################
# WHEN ADDING A NEW TARGET add the source code file for the series\device folder bellow
#######################################################################################

# sources in series\device folder
set(MBEDOS_VENDOR_SERIES_DEVICE_SRCS
    stm32f0xx_hal.c
    stm32f0xx_hal_adc.c
    stm32f0xx_hal_adc_ex.c
    stm32f0xx_hal_can.c
    stm32f0xx_hal_cec.c
    stm32f0xx_hal_comp.c
    stm32f0xx_hal_cortex.c
    stm32f0xx_hal_crc.c
    stm32f0xx_hal_crc_ex.c
    stm32f0xx_hal_dac.c
    stm32f0xx_hal_dac_ex.c
    stm32f0xx_hal_dma.c
    stm32f0xx_hal_flash.c
    stm32f0xx_hal_flash_ex.c
    stm32f0xx_hal_gpio.c
    stm32f0xx_hal_i2c.c
    stm32f0xx_hal_i2c_ex.c
    stm32f0xx_hal_i2s.c
    stm32f0xx_hal_irda.c
    stm32f0xx_hal_iwdg.c
    stm32f0xx_hal_pcd.c
    stm32f0xx_hal_pcd_ex.c
    stm32f0xx_hal_pwr.c
    stm32f0xx_hal_pwr_ex.c
    stm32f0xx_hal_rcc.c
    stm32f0xx_hal_rcc_ex.c
    stm32f0xx_hal_rtc.c
    stm32f0xx_hal_rtc_ex.c
    stm32f0xx_hal_smartcard.c
    stm32f0xx_hal_smartcard_ex.c
    stm32f0xx_hal_smbus.c
    stm32f0xx_hal_spi.c
    stm32f0xx_hal_spi_ex.c
    stm32f0xx_hal_tim.c
    stm32f0xx_hal_tim_ex.c
    stm32f0xx_hal_tsc.c
    stm32f0xx_hal_uart.c
    stm32f0xx_hal_uart_ex.c
    stm32f0xx_hal_usart.c
    stm32f0xx_hal_wwdg.c
)
foreach(SRC_FILE ${MBEDOS_VENDOR_SERIES_DEVICE_SRCS})
    set(MBEDOS_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(MBEDOS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/mbedOS_Source/targets/TARGET_${MBEDOS_TARGET_VENDOR}/TARGET_${MBEDOS_TARGET_SERIES}/device
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${MBEDOS_SRC_FILE}") # debug helper
    list(APPEND MBEDOS_SOURCES ${MBEDOS_SRC_FILE})
endforeach()

##########################################################################
# WHEN ADDING A NEW TARGET add the source code file for the series bellow
##########################################################################

set(MBED_VENDOR_SERIES_AND_TARGET_SRCS
    analogin_api.c
    analogout_api.c
    can_api.c
    gpio_api.c
    gpio_irq_api.c
    lp_ticker.c
    mbed_overrides.c
    pinmap.c
    port_api.c
    pwmout_api.c
    rtc_api.c
    serial_api.c
    sleep.c
    spi_api.c

    PeripheralPins.c
    cmsis_nvic.c
    system_stm32f0xx.c
)
foreach(SRC_FILE ${MBEDOS_VENDOR_SERIES_AND_TARGET_SRCS})
    set(MBEDOS_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(MBEDOS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/mbedOS_Source/targets/TARGET_${MBEDOS_TARGET_VENDOR}/TARGET_${MBEDOS_TARGET_SERIES} 
            ${PROJECT_BINARY_DIR}/mbedOS_Source/targets/TARGET_${MBEDOS_TARGET_VENDOR}/TARGET_${MBEDOS_TARGET_SERIES}/TARGET_${MBEDOS_TARGET} 
            ${PROJECT_BINARY_DIR}/mbedOS_Source/targets/TARGET_${MBEDOS_TARGET_VENDOR}/TARGET_${MBEDOS_TARGET_SERIES}/TARGET_${MBEDOS_TARGET}/device 
            ${PROJECT_BINARY_DIR}/mbedOS_Source/targets/TARGET_${MBEDOS_TARGET_VENDOR}/TARGET_${MBEDOS_TARGET_SERIES}/TARGET_${MBEDOS_TARGET}/device/TOOLCHAIN_GCC_ARM
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${MBEDOS_SRC_FILE}") # debug helper
    list(APPEND MBEDOS_SOURCES ${MBEDOS_SRC_FILE})
endforeach()

#######################################################################################################################################
# WHEN ADDING A NEW TARGET add the respective if clause bellow along with the startup file and the linker file name (without extension)
#######################################################################################################################################

# startup file
if(${MBEDOS_TARGET} STREQUAL "NUCLEO_F091RC")
    list(APPEND MBEDOS_SOURCES ${PROJECT_BINARY_DIR}/mbedOS_Source/targets/TARGET_${MBEDOS_TARGET_VENDOR}/TARGET_${MBEDOS_TARGET_SERIES}/TARGET_NUCLEO_F091RC/device/TOOLCHAIN_GCC_ARM/startup_stm32f091xc.S)
    set(LINKER_FILE "STM32F091XC")
endif()

#######################################################################################################################################


################################################################
# WHEN ADDING A NEW TARGET check if the linker file bellow works
################################################################

function(MBEDOS_SET_LINKER_OPTIONS TARGET)

    get_target_property(TARGET_LD_FLAGS ${TARGET} LINK_FLAGS)
    if(TARGET_LD_FLAGS)
        set(TARGET_LD_FLAGS "-T${PROJECT_BINARY_DIR}/mbedOS_Source/targets/TARGET_${MBEDOS_TARGET_VENDOR}/TARGET_${MBEDOS_TARGET_SERIES}/TARGET_${MBEDOS_TARGET}/device/TOOLCHAIN_GCC_ARM/${LINKER_FILE}.LD ${TARGET_LD_FLAGS}")
    else()
        set(TARGET_LD_FLAGS "-T${PROJECT_BINARY_DIR}/mbedOS_Source/targets/TARGET_${MBEDOS_TARGET_VENDOR}/TARGET_${MBEDOS_TARGET_SERIES}/TARGET_${MBEDOS_TARGET}/device/TOOLCHAIN_GCC_ARM/${LINKER_FILE}.LD")
    endif()
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS ${TARGET_LD_FLAGS})

endfunction()
