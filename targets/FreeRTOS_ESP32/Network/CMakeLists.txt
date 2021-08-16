#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

option(ESP32_ETHERNET_SUPPORT "Enable ESP32 ethenet" OFF)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/esp32_ethernet_options.h.in
               ${CMAKE_BINARY_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/esp32_ethernet_options.h @ONLY)
 
# append networking files, if enabled
list(APPEND TARGET_ESP32_IDF_NETWORK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Network.cpp")
list(APPEND TARGET_ESP32_IDF_NETWORK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Target_Network.cpp")
list(APPEND TARGET_ESP32_IDF_NETWORK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Esp32_Ethernet_Lan8720.cpp")
list(APPEND TARGET_ESP32_IDF_NETWORK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Esp32_Wireless.cpp")
list(APPEND TARGET_ESP32_IDF_NETWORK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Esp32_SmartConfig.cpp")

# make var global
set(TARGET_ESP32_IDF_NETWORK_SOURCES ${TARGET_ESP32_IDF_NETWORK_SOURCES} CACHE INTERNAL "make global")
