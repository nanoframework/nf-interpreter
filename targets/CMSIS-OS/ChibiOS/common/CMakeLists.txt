#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# append target ChibiOS source files 
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/LaunchCLR.c")
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_ReceiverThread.c")
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/platform_heap.c")
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_STM32FlashDriver.c")

# append Target files
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage.c")

# make var global
set(TARGET_CHIBIOS_COMMON_SOURCES ${TARGET_CHIBIOS_COMMON_SOURCES} CACHE INTERNAL "make global")

# add header with target platform definitions
if(CHIBIOS_COMMUNITY_TARGET)
    configure_file("${CMAKE_SOURCE_DIR}/CMake/ChibiOS_target_os.h.in"
                    "${CMAKE_BINARY_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/target_os.h" @ONLY)
else()
    configure_file("${CMAKE_SOURCE_DIR}/CMake/ChibiOS_target_os.h.in"
                    "${CMAKE_BINARY_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/target_os.h" @ONLY)
endif()
