#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(maxim_micros_sdk)

# board
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/CMSIS/Device/Maxim/MAX78000/Include)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/Boards/MAX78000/FTHR_RevA/Include)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/Boards/MAX78000/Include)

# periph drivers
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Include)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/SYS)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/ADC)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/AES)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/DMA)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/CAMERAIF)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/CRC)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/LP)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/FLC)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/GPIO)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/I2C)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/I2S)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/ICC)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/OWM)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/PT)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/RTC)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/SEMA)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/SPI)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/TRNG)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/TMR)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/UART)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/WDT)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/WUT)
list(APPEND MaximMicrosSDK_INCLUDE_DIRS ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Include/MAX78000)

set(MaximMicrosSDK_SRCS

    # BSP
    # board.c
    stdio.c
    led.c
    pb.c
    tft_fthr.c
    camera.c
    ov7692.c
    sccb.c
    max20303.c


    nano_startup_max78000.S
    system_max78000.c

    # periph drivers
    mxc_assert.c
    mxc_delay.c
    mxc_lock.c
    pins_ai85.c
    sys_ai85.c
    adc_me17.c
    adc_reva.c
    aes_me17.c
    aes_revb.c
    cameraif_ai85.c
    cameraif_reva.c
    dma_me17.c
    dma_reva.c
    crc_me17.c
    crc_reva.c
    lp_ai85.c
    flc_common.c
    flc_me17.c
    flc_reva.c
    flc_revb.c
    gpio_common.c
    gpio_ai85.c
    gpio_reva.c
    i2c_me17.c
    i2c_reva.c
    i2s_ai85.c
    i2s_reva.c
    icc_me17.c
    icc_reva.c
    owm_me17.c
    owm_reva.c
    pt_me17.c
    pt_reva.c
    rtc_me17.c
    rtc_reva.c
    sema_me17.c
    sema_reva.c
    spi_me17.c
    spi_reva.c
    trng_me17.c
    trng_revb.c
    tmr_common.c
    tmr_ai85.c
    tmr_revb.c
    uart_ai85.c
    uart_common.c
    uart_revb.c
    wdt_common.c
    wdt_me17.c
    wdt_revb.c
    wut_me17.c
    wut_reva.c

    # supplied with latest CMSIS version
    # nvic_table.c
)

foreach(SRC_FILE ${MaximMicrosSDK_SRCS})

    set(MaximMicrosSDK_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(MaximMicrosSDK_SRC_FILE ${SRC_FILE}
        PATHS 
        
            # BSP
            # ${maxim_micros_sdk_SOURCE_DIR}/Libraries/Boards/MAX78000/FTHR_RevA/Source
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/Boards/MAX78000/Source
            ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/Maxim/MAX78000_FTHR/common

            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/CMSIS/Device/Maxim/MAX78000/Source
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/CMSIS/Device/Maxim/MAX78000/Source/GCC

            # periph drivers
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/SYS
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/ADC
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/AES
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/DMA
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/CAMERAIF
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/CRC
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/LP
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/FLC
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/GPIO
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/I2C
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/I2S
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/ICC
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/OWM
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/PT
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/RTC
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/SEMA
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/SPI
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/TRNG
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/TMR
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/UART
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/WDT
            ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/WUT

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${MaximMicrosSDK_SRC_FILE}")
    endif()

    list(APPEND MaximMicrosSDK_SOURCES ${MaximMicrosSDK_SRC_FILE})

endforeach()

# unset this warning as error required for these source files
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/I2C/i2c_reva.c PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/GPIO/gpio_common.c PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/LP/lp_ai85.c PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/DMA/dma_reva.c PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/CRC/crc_reva.c PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/PT/pt_reva.c PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/UART/uart_revb.c PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-sign-compare")
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/UART/uart_ai85.c PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-sign-compare")
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/SPI/spi_reva.c PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-unused-variable")
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/WUT/wut_reva.c PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/ADC/adc_reva.c PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/SYS/mxc_assert.c PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/PeriphDrivers/Source/SYS/pins_ai85.c PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-missing-field-initializers")
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/Boards/MAX78000/Source/tft_fthr.c PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-sign-compare")
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/Boards/MAX78000/Source/stdio.c PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/Boards/MAX78000/Source/ov7692.c PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-format")
SET_SOURCE_FILES_PROPERTIES( ${maxim_micros_sdk_SOURCE_DIR}/Libraries/Boards/MAX78000/Source/camera.c PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-sign-compare;-Wno-int-conversion")
