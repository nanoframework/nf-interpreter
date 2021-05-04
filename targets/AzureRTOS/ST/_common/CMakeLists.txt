#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Azure RTOS ST specific file
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tx_initialize_low_level.S)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_STM32FlashDriver.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/nanoSupport_CRC32.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ST_Hal_Gpio_Helpers.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_UARTDriver.c)

# append Target files
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_BlockStorage.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/LaunchCLR.c)

# include configuration manager file, if feature is enabled
if(NF_FEATURE_HAS_CONFIG_BLOCK)
    list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigurationManager.cpp)
endif()

# append hard fault handler if the build type is to include debug info
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    list(APPEND TARGET_CHIBIOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hard_fault_handler.c)
endif()

# make vars global
set(TARGET_AZURERTOS_COMMON_SOURCES ${TARGET_AZURERTOS_COMMON_SOURCES} CACHE INTERNAL "make global")
