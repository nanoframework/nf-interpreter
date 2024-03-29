#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append target Azure RTOS nanoCLR source files 
list(APPEND TARGET_AZURERTOS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL.cpp)
list(APPEND TARGET_AZURERTOS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetPAL.c)
list(APPEND TARGET_AZURERTOS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Time.cpp)
list(APPEND TARGET_AZURERTOS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Power.c)

# append watchdog only if option is ON
if(HAL_USE_WDG_OPTION)
    list(APPEND TARGET_AZURERTOS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Watchdog.c)
endif()

# for the moment, assume all Gecko MCUs we support have a TRNG
list(APPEND TARGET_AZURERTOS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetRandom.cpp)  

# make var global
set(TARGET_AZURERTOS_NANOCLR_SOURCES ${TARGET_AZURERTOS_NANOCLR_SOURCES} CACHE INTERNAL "make global")

# append target nanoCLR include directories
list(APPEND TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Core)

# append PAL include directory
list(APPEND TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Include)

# append Runtime.Native include directory
list(APPEND TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Runtime.Native)

# append nano API includes
list(APPEND TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src)

# append includes
list(APPEND TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src)

# make var global
set(TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS ${TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS} CACHE INTERNAL "make global")
