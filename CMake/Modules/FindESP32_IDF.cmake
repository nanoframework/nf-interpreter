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
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/xtensa/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/${TARGET_SERIES_SHORT}/include/driver)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/hal/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/soc/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/soc/${TARGET_SERIES_SHORT}/include/soc)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_hw_support/include/soc)
                                                           
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/driver/include/driver)
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
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/fatfs/diskio)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/fatfs/vfs)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/fatfs/src)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/port/esp32/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/lwip/src/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/lwip/lwip/src/include/lwip)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/log/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/hal/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/heap)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/heap/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/nvs_flash/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/newlib/platform_include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/wpa_supplicant/include/esp_supplicant)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/sdmmc)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/spiffs/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/spi_flash/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/soc/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/xtensa/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/vfs/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/wear_levelling/include)

list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_rom/include/${TARGET_SERIES_SHORT}/rom)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_rom/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/esp_rom/${TARGET_SERIES_SHORT})

# includes specific to ESP32S2
if(${TARGET_SERIES_SHORT} STREQUAL "esp32s2")
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/tinyusb/additions/include)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/tinyusb/tinyusb/src)
    list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/include/freertos)
endif()

# including here the CMake files for the source files specific to the target series
include(${TARGET_SERIES}_sources)
# and here the GCC options tuned for the target series 
include(${TARGET_SERIES}_GCC_options)

############
# freertos #
############

list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/include/esp_additions)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/include/esp_additions/freertos)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/port/xtensa/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/port/xtensa/include/freertos)


include(FindPackageHandleStandardArgs)

# FIND_PACKAGE_HANDLE_STANDARD_ARGS(ESP32_IDF DEFAULT_MSG ESP32_IDF_INCLUDE_DIRS ESP32_IDF_SOURCES)
