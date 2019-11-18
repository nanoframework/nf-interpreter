#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# set include directories
list(APPEND LoRaMacNode_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards)
list(APPEND LoRaMacNode_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/B-L072Z-LRWAN1)
list(APPEND LoRaMacNode_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/B-L072Z-LRWAN1/cmsis)
list(APPEND LoRaMacNode_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/mcu/stm32)
list(APPEND LoRaMacNode_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/mcu/stm32/cmsis)
list(APPEND LoRaMacNode_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/mcu/stm32/STM32L0xx_HAL_Driver/Inc)
list(APPEND LoRaMacNode_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/radio)
list(APPEND LoRaMacNode_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/radio/sx1276)
list(APPEND LoRaMacNode_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/system)

# add here the GCC options tuned for the target series 
include(FREERTOS_${TARGET_SERIES}_GCC_options)

# source files
set(LORAMAC_NODE_SRCS

    # board
    adc-board.c
    board.c
    delay-board.c
    eeprom-board.c
    gpio-board.c
    lpm-board.c
    rtc-board.c
    spi-board.c
    sx1276-board.c
    uart-board.c

    # STM32
    startup_stm32l072xx.s
    system_stm32l0xx.c
    sysIrqHandlers.c
    utilities.c
    stm32l0xx_hal.c
    stm32l0xx_hal_adc.c
    stm32l0xx_hal_adc_ex.c
    stm32l0xx_hal_cortex.c
    stm32l0xx_hal_dma.c
    stm32l0xx_hal_flash.c
    stm32l0xx_hal_flash_ex.c
    stm32l0xx_hal_gpio.c
    stm32l0xx_hal_i2c.c
    stm32l0xx_hal_i2c_ex.c
    stm32l0xx_hal_pwr.c
    stm32l0xx_hal_pwr_ex.c
    stm32l0xx_hal_rcc.c
    stm32l0xx_hal_rcc_ex.c
    stm32l0xx_hal_rtc.c
    stm32l0xx_hal_rtc_ex.c
    stm32l0xx_hal_spi.c
    stm32l0xx_hal_uart.c
    stm32l0xx_hal_uart_ex.c

    # radio
    sx1276.c

    # system
    adc.c
    delay.c
    eeprom.c
    fifo.c
    gpio.c
    gps.c
    i2c.c
    nvmm.c
    systime.c
    timer.c
    uart.c
)

foreach(SRC_FILE ${LORAMAC_NODE_SRCS})
    set(LORAMAC_NODE_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(LORAMAC_NODE_SRC_FILE ${SRC_FILE}
        PATHS 

        # board
        ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards
        
        # STM32
        ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/B-L072Z-LRWAN1
        ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/B-L072Z-LRWAN1/cmsis
        ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/B-L072Z-LRWAN1/cmsis/arm-gcc
        ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/mcu
        ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/mcu/stm32
        ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/mcu/stm32/STM32L0xx_HAL_Driver/Src

        # radio
        ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/radio/sx1276

        # system
        ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/system

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${LORAMAC_NODE_SRC_FILE}") # debug helper
    list(APPEND LoRaMacNode_SOURCES ${LORAMAC_NODE_SRC_FILE})
endforeach()

# unset these warnings as errors because we don't want to have to patch these source files
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/B-L072Z-LRWAN1/eeprom-board.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/B-L072Z-LRWAN1/adc-board.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/B-L072Z-LRWAN1/board.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/B-L072Z-LRWAN1/uart-board.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/B-L072Z-LRWAN1/sx1276-board.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/B-L072Z-LRWAN1/rtc-board.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_adc_ex.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/boards/mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc_ex.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/radio/sx1276/sx1276.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/LoRaMacNode_Source/src/system/adc.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LoRaMacNode DEFAULT_MSG LoRaMacNode_INCLUDE_DIRS LoRaMacNode_SOURCES)
