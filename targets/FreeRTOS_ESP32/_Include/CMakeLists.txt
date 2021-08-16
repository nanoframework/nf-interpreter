#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append include directory for target 
list(APPEND TARGET_ESP32_IDF_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND TARGET_ESP32_IDF_COMMON_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/FreeRTOS_ESP32)

# make var global
set(TARGET_ESP32_IDF_COMMON_INCLUDE_DIRS ${TARGET_ESP32_IDF_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")

# add header with target platform definitions
configure_file(${CMAKE_SOURCE_DIR}/CMake/ESP32_target_os.h.in
               ${CMAKE_BINARY_DIR}/targets/FreeRTOS_ESP32/target_os.h @ONLY)
