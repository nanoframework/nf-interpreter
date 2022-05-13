#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append target ChibiOS source files
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/LaunchCLR.c)
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/nanoSupport_CRC32.c)
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_ReceiverThread.c)
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c)
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_heap.c)
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_STM32FlashDriver.c)

# append Target files
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_BlockStorage.c)

# include configuration manager file, if feature is enabled
if(NF_FEATURE_HAS_CONFIG_BLOCK)
    list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigurationManager.cpp)
endif()

# append nanoHAL
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL.c)
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Time.cpp)

# append hard fault handler if the build type is to include debug info
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hard_fault_handler.c)
endif()

# if SWO opting add source file
if(SWO_OUTPUT_OPTION)
    list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/swo.cpp)
endif()

# include syscalls dummy calls, if needed
if(NOT RTOS_CHIBIOS_CHECK)
    list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/syscalls.c)
endif()

# make var global
set(TARGET_CHIBIOS_COMMON_SOURCES ${TARGET_CHIBIOS_COMMON_SOURCES} CACHE INTERNAL "make global")
