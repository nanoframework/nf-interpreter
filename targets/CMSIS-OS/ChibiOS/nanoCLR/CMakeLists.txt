#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(NF_NativeAssemblies)

# add header file with common target definitions
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/target_platform.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/target_platform.h" @ONLY)

# append nanoHAL
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL.cpp")

# append target ChibiOS nanoCLR source files
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_App_Interface.c)
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_MonitorCommands.c)
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/CLR_Startup_Thread.c") 
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetPAL.c") 

# append target HAL source files
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Power.c")
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Time.cpp")

# append watchdog only if option is ON
if(HAL_USE_WDG_OPTION)
    list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Watchdog.c")
endif()

# append target PAL source files
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetPAL_Events.cpp")
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetPAL_Time.cpp")

# append other CLR core files
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Memory.cpp")

# append nanoCRT
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nanoCRT.cpp")

# append files from Runtime.Native
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_Rtc.cpp")

# append TRNG only if option is ON
if(USE_RNG)
    
    # append random implementation for all series, except F0
    if( ${TARGET_SERIES} STREQUAL "STM32L0xx" OR
        ${TARGET_SERIES} STREQUAL "STM32F4xx" OR 
        ${TARGET_SERIES} STREQUAL "STM32F7xx" OR
        ${TARGET_SERIES} STREQUAL "STM32H7xx")

        list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetRandom.cpp")  

    else()
        # this series doesn't feature hardware TRNG, report
        message(ERROR "This CPU doesn't have a Random Number Generation. Please change USE_RNG option to OFF.")
    endif()

endif()

# add native assemblies
ParseNativeAssemblies()

# configure code file with Interop Assemblies table and...
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CLR_RT_InteropAssembliesTable.cpp.in"
                "${CMAKE_CURRENT_BINARY_DIR}/CLR_RT_InteropAssembliesTable.cpp" @ONLY)
# ... now add Interop Assemblies table to ChibiOS nanoCLR sources list
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/CLR_RT_InteropAssembliesTable.cpp")

# make var global
set(TARGET_CHIBIOS_NANOCLR_SOURCES ${TARGET_CHIBIOS_NANOCLR_SOURCES} CACHE INTERNAL "make global")

# append target ChibiOS nanoCLR include directory
list(APPEND TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)

# append PAL include directory
list(APPEND TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)

# append Runtime.Native include directory
list(APPEND TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Runtime.Native)

# make var global
set(TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS ${TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS} CACHE INTERNAL "make global")
