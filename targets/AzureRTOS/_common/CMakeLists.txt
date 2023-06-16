#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

list(APPEND TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/AzureRTOS)

# add header with target platform definitions
if(AZURERTOS_COMMUNITY_TARGET)
    configure_file(${CMAKE_SOURCE_DIR}/CMake/AzureRTOS_target_os.h.in
                   ${CMAKE_BINARY_DIR}/targets-community/AzureRTOS/target_os.h @ONLY)
else()
    configure_file(${CMAKE_SOURCE_DIR}/CMake/AzureRTOS_target_os.h.in
                   ${CMAKE_BINARY_DIR}/targets/AzureRTOS/target_os.h @ONLY)
endif()

# append target Azure RTOS source files
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_heap.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_ReceiverThread.c)

# append nanoHAL
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Time.cpp)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Rtos.c)

# append low level initialization
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${TARGET_BASE_LOCATION}/common/tx_initialize_low_level.S)

# make vars global
set(TARGET_AZURERTOS_COMMON_SOURCES ${TARGET_AZURERTOS_COMMON_SOURCES} CACHE INTERNAL "make global")
set(TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")

add_subdirectory(include)
add_subdirectory(nanoBooter)
add_subdirectory(nanoCLR)
