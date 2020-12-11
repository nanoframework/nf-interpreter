#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

list(APPEND TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/AzureRTOS)
list(APPEND TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/AzureRTOS_Source/common/inc)

# add header with target platform definitions
if(AZURERTOS_COMMUNITY_TARGET)
    configure_file(${CMAKE_SOURCE_DIR}/CMake/AzureRTOS_target_os.h.in
                   ${CMAKE_BINARY_DIR}/targets-community/AzureRTOS/target_os.h @ONLY)
else()
    configure_file(${CMAKE_SOURCE_DIR}/CMake/AzureRTOS_target_os.h.in
                   ${CMAKE_BINARY_DIR}/targets/AzureRTOS/target_os.h @ONLY)
endif()

# append target Azure RTOS source files
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/LaunchCLR.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_heap.c)

# append Target files
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_BlockStorage.c)

# include configuration manager file, if feature is enabled
if(NF_FEATURE_HAS_CONFIG_BLOCK)
    list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigurationManager.cpp)
endif()

# append nanoHAL
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Time.cpp)

# make vars global
set(TARGET_AZURERTOS_COMMON_SOURCES ${TARGET_AZURERTOS_COMMON_SOURCES} CACHE INTERNAL "make global")
set(TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")

add_subdirectory(include)
add_subdirectory(nanoBooter)
add_subdirectory(nanoCLR)
