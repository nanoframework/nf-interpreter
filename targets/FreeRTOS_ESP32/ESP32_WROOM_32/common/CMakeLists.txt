#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# append target ESP32 source files 
list(APPEND TARGET_ESP32_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/platform_heap.c")

list(APPEND TARGET_ESP32_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_ReceiverThread.c")
list(APPEND TARGET_ESP32_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c")

list(APPEND TARGET_ESP32_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_ESP32FlashDriver.c")
list(APPEND TARGET_ESP32_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage.c")
list(APPEND TARGET_ESP32_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Device_BlockStorage.c")

list(APPEND TARGET_ESP32_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-openocd.c")

list(APPEND TARGET_ESP32_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Esp32_DeviceMapping.cpp")

list(APPEND TARGET_ESP32_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/GenericPort.cpp")


# append networking files, if enabled
if(USE_NETWORKING_OPTION)
    list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Network.cpp")
endif()

# make var global
set(TARGET_ESP32_COMMON_SOURCES ${TARGET_ESP32_COMMON_SOURCES} CACHE INTERNAL "make global")
