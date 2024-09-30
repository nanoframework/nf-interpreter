#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(esp32_idf)

include(binutils.ESP32)

list(APPEND ESP32_IDF_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/config)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/${TARGET_SERIES_SHORT})
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/${TARGET_SERIES_SHORT}/esp_rom/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/${ESP32_CPU_TYPE}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/${ESP32_CPU_TYPE}/${TARGET_SERIES_SHORT}/include)

list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/${TARGET_SERIES_SHORT}/include/driver)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/hal/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/hal/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/soc/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/soc/${TARGET_SERIES_SHORT}/include/soc)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_hw_support/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_hw_support/include/soc)
                                                           
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/gptimer/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/uart/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/gpio/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/spi/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/i2c/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/i2s/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/dac/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/ledc/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/pcnt/include)
#list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/rmt/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/sdmmc/include)

# Use depecated drivers for RMT, I2S etc
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/deprecated)

# includes specific to ESP32 or ESP32S2 or ESP32S3
if(${TARGET_SERIES_SHORT} STREQUAL "esp32" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32s2" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32s3")
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/touch_sensor/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/touch_sensor/${TARGET_SERIES_SHORT}/include)
endif()

list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_adc/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_adc/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_common/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_event/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_eth/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_netif/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_netif/lwip)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_hw_support/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_ringbuf/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_timer/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_system/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_wifi/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_partition/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_pm/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/fatfs/diskio)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/fatfs/vfs)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/fatfs/src)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/include/apps)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/port/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/port/esp32xx/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/port/freertos/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/port/freertos/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/lwip/src/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/lwip/src/include/lwip)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/log/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/hal/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/heap)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/heap/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/mbedtls/mbedtls/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/nvs_flash/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/newlib/platform_include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/wpa_supplicant/include/esp_supplicant)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/spiffs/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/spi_flash/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/sdmmc/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/soc/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/vfs/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/wear_levelling/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/usb_serial_jtag/include)

list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_rom/include/${TARGET_SERIES_SHORT}/rom)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_rom/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_rom/${TARGET_SERIES_SHORT})

# includes specific to ESP32S2 and ESP32S3
if(${TARGET_SERIES_SHORT} STREQUAL "esp32s2" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32s3")
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_tinyusb/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/tinyusb/src)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/FreeRTOS-Kernel/include/freertos)
endif()

# includes specific to Thread, native only for ESP32C6 and ESP32H2. Other mcu as attached network chip (EPS32-H2 etc)
if(HAL_USE_THREAD_OPTION)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/openthread/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/openthread/openthread/include)
endif()

# including here the CMake files for the source files specific to the target series
include(${TARGET_SERIES}_sources)
# and here the GCC options tuned for the target series 
include(${TARGET_SERIES}_GCC_options)

############
# freertos #
############

list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/config/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/config/include/freertos)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/config/${ESP32_CPU_TYPE}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/FreeRTOS-Kernel/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/esp_additions/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/esp_additions/include/freertos)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/esp_additions/arch/${ESP32_CPU_TYPE}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/FreeRTOS-Kernel/portable/${ESP32_CPU_TYPE}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/FreeRTOS-Kernel/portable/${ESP32_CPU_TYPE}/include/freertos)
