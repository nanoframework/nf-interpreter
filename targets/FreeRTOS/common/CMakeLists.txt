#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# FreeRTOS common includes
list(APPEND TARGET_FREERTOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
list(APPEND TARGET_FREERTOS_COMMON_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/FreeRTOS)

# add header with target platform definitions
configure_file(${CMAKE_SOURCE_DIR}/CMake/FreeRTOS_target_os.h.in
               ${CMAKE_BINARY_DIR}/targets/FreeRTOS/target_os.h @ONLY)

# append common source files
list(APPEND TARGET_FREERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hardfault.c)
list(APPEND TARGET_FREERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_heap.c)

# append nanoHAL
list(APPEND TARGET_FREERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL.c)
list(APPEND TARGET_FREERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Time.cpp)

list(APPEND TARGET_FREERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_ReceiverThread.c)

# FreeRTOS nanoCRL
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR)

# make vars global
set(TARGET_FREERTOS_COMMON_SOURCES ${TARGET_FREERTOS_COMMON_SOURCES} CACHE INTERNAL "make global")
set(TARGET_FREERTOS_COMMON_INCLUDE_DIRS ${TARGET_FREERTOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")
