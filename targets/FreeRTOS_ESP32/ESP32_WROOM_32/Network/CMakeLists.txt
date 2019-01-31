#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# append networking files, if enabled
list(APPEND TARGET_ESP32_NETWORK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Network.cpp")
list(APPEND TARGET_ESP32_NETWORK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/target_Network.cpp")
list(APPEND TARGET_ESP32_NETWORK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Esp32_Ethernet_Lan8720.cpp")
list(APPEND TARGET_ESP32_NETWORK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Esp32_Wireless.cpp")
list(APPEND TARGET_ESP32_NETWORK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Esp32_SmartConfig.cpp")

# make var global
set(TARGET_ESP32_NETWORK_SOURCES ${TARGET_ESP32_NETWORK_SOURCES} CACHE INTERNAL "make global")
