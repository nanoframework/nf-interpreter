#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_BlockStorage.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_MXCFlashDriver.c)

# include configuration manager file, if feature is enabled
if(NF_FEATURE_HAS_CONFIG_BLOCK)
    list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigurationManager.cpp)
endif()

# include internal storage, if feature is enabled
if(NF_FEATURE_HAS_ACCESSIBLE_STORAGE)
    list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_StorageOperation.cpp)
endif()

# append hard fault handler if the build type is to include debug info
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hard_fault_handler.c)
endif()

list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_UARTDriver.c)

# make vars global
set(TARGET_AZURERTOS_COMMON_SOURCES ${TARGET_AZURERTOS_COMMON_SOURCES} CACHE INTERNAL "make global")
