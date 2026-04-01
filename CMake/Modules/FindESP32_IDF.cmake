#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)

include(binutils.ESP32)

list(APPEND ESP32_IDF_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/config)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_rom/${TARGET_SERIES_SHORT}/include/${TARGET_SERIES_SHORT})
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/${ESP32_CPU_TYPE}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/${ESP32_CPU_TYPE}/${TARGET_SERIES_SHORT}/include)

list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/driver/${TARGET_SERIES_SHORT}/include/driver)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/driver/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/hal/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/hal/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/soc/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/soc/${TARGET_SERIES_SHORT}/include/soc)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/soc/${TARGET_SERIES_SHORT}/register)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_hw_support/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_hw_support/dma/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_hw_support/include/soc)
                                                           
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_driver_gptimer/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_driver_uart/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_driver_gpio/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_driver_spi/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/driver/i2c/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_driver_i2s/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_driver_dac/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_driver_ledc/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_driver_pcnt/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_driver_sdmmc/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_driver_sdspi/include)

# Use depecated drivers for RMT, I2S etc
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/driver/deprecated)

# includes specific to ESP32 or ESP32S2 or ESP32S3
if(${TARGET_SERIES_SHORT} STREQUAL "esp32" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32s2" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32s3")
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/driver/touch_sensor/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/driver/touch_sensor/${TARGET_SERIES_SHORT}/include)
endif()

list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_adc/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_adc/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_common/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_event/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_eth/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_netif/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_netif/lwip)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_hw_support/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_ringbuf/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_timer/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_system/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_wifi/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_wifi/include/local)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_partition/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_pm/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/fatfs/diskio)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/fatfs/vfs)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/fatfs/src)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/lwip/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/lwip/include/apps)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/lwip/port/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/lwip/port/esp32xx/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/lwip/port/freertos/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/lwip/lwip/src/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/lwip/lwip/src/include/lwip)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/log/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/hal/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/heap)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/heap/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/mbedtls/port/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/mbedtls/mbedtls/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/nvs_flash/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/newlib/platform_include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/wpa_supplicant/include/esp_supplicant)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/spiffs/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/spi_flash/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/sdmmc/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/soc/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/vfs/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/wear_levelling/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_driver_usb_serial_jtag/include)

list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_rom/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_rom/${TARGET_SERIES_SHORT})
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_rom/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_rom/${TARGET_SERIES_SHORT}/include/${TARGET_SERIES_SHORT}/rom)

# includes specific to ESP32S2 and ESP32S3
if(${TARGET_SERIES_SHORT} STREQUAL "esp32s2" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32s3")
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_tinyusb/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/tinyusb/src)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/freertos/FreeRTOS-Kernel/include/freertos)
endif()

# includes specific to ESP32P4
if(${TARGET_SERIES_SHORT} STREQUAL "esp32p4")
    # defaulting to hardware v3, if v1 version is needed, need to use hw_ver1 instead of hw_ver3 in the path below
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/soc/esp32p4/register/hw_ver3)
endif()

# includes specific to Thread, native only for ESP32C6 and ESP32H2. Other mcu as attached network chip (EPS32-H2 etc)
if(HAL_USE_THREAD_OPTION)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/openthread/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/openthread/openthread/include)
endif()

# including here the CMake files for the source files specific to the target series
include(${TARGET_SERIES}_sources)
# and here the GCC options tuned for the target series 
include(${TARGET_SERIES}_GCC_options)

############
# freertos #
############

list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/freertos/config/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/freertos/config/include/freertos)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/freertos/config/${ESP32_CPU_TYPE}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/freertos/FreeRTOS-Kernel/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/freertos/esp_additions/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/freertos/esp_additions/include/freertos)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/freertos/esp_additions/arch/${ESP32_CPU_TYPE}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/freertos/FreeRTOS-Kernel/portable/${ESP32_CPU_TYPE}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/freertos/FreeRTOS-Kernel/portable/${ESP32_CPU_TYPE}/include/freertos)
