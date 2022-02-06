#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

option(ESP32_ETHERNET_SUPPORT "Enable ESP32 ethernet" OFF)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/esp32_ethernet_options.h.in
               ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/esp32_ethernet_options.h @ONLY)
 
# append networking files, if enabled
list(APPEND TARGET_ESP32_IDF_NETWORK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/NF_ESP32_Ethernet.cpp)
list(APPEND TARGET_ESP32_IDF_NETWORK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/NF_ESP32_Wireless.cpp)
list(APPEND TARGET_ESP32_IDF_NETWORK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/NF_ESP32_SmartConfig.cpp)

# make var global
set(TARGET_ESP32_IDF_NETWORK_SOURCES ${TARGET_ESP32_IDF_NETWORK_SOURCES} CACHE INTERNAL "make global")

# append include directory for target 
list(APPEND TARGET_ESP32_IDF_NETWORK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

# make var global
set(TARGET_ESP32_IDF_NETWORK_INCLUDE_DIRS ${TARGET_ESP32_IDF_NETWORK_INCLUDE_DIRS} CACHE INTERNAL "make global")
