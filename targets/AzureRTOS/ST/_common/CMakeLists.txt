#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# include Cube MX directory if Cube package was included
if(STM32_CUBE_PACKAGE_REQUIRED)
    add_subdirectory(CubeMX)
endif()

# Azure RTOS ST specific file
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_STM32FlashDriver.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/nanoSupport_CRC32.c)

# append Target files
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_BlockStorage.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/LaunchCLR.c)

# include configuration manager file, if feature is enabled
if(NF_FEATURE_HAS_CONFIG_BLOCK)
    list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigurationManager.cpp)
endif()

# include internal storage, if feature is enabled
if(NF_FEATURE_HAS_ACCESSIBLE_STORAGE)
    list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_StorageOperation.cpp)
endif()

if(NF_TRACE_TO_STDIO)
    list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Target_GenericPort_stdio.c)
    list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${TARGET_BASE_LOCATION}/target_stdio_config.c)
endif()

if(USE_NETWORKING_OPTION)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/network_options.h.in
                ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/network_options.h @ONLY)

endif()

# append hard fault handler if the build type is to include debug info
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    # list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hard_fault_handler.c)
endif()

# make vars global
set(TARGET_AZURERTOS_COMMON_SOURCES ${TARGET_AZURERTOS_COMMON_SOURCES} CACHE INTERNAL "make global")
