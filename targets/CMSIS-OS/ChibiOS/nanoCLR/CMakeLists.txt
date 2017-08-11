#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(NF_API_Namespaces)

# add header file with common target definitions
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/target_platform.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/target_platform.h" @ONLY)

# append target ChibiOS nanoCLR source files
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_App_Interface.c)
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_MonitorCommands.c)
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Aborts_CortexM3.cpp)
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/CLR_Startup_Thread.c") 

# append target HAL source files
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Time.cpp")

# append target PAL source files
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetPAL_Events.cpp")
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetPAL_Time.cpp")

# append other CLR core files
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/CLRStartup.cpp")
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Memory.cpp")

# append nanoCRT
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nanoCRT.cpp")

# append nanoHAL
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL.cpp")

# parse nanoFramework API options
ParseApiOptions()

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

# make var global
set(TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS ${TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS} CACHE INTERNAL "make global")
