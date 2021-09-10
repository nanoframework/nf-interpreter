#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(esp32_idf)

include(binutils.ESP32)

list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/${TARGET_SERIES_SHORT})
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/${TARGET_SERIES_SHORT}/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/${TARGET_SERIES_SHORT}/esp_rom/include)

# including here the CMake files for the source files specific to the target series
include(${TARGET_SERIES}_sources)
# and here the GCC options tuned for the target series 
include(${TARGET_SERIES}_GCC_options)

############
# freertos #
############

list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/include)
list(APPEND ESP32_IDF_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/freertos/port/xtensa/include/freertos)


include(FindPackageHandleStandardArgs)

# FIND_PACKAGE_HANDLE_STANDARD_ARGS(ESP32_IDF DEFAULT_MSG ESP32_IDF_INCLUDE_DIRS ESP32_IDF_SOURCES)
