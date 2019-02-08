#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(NF_NativeAssemblies)

# add header file with common target definitions
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/target_platform.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/target_platform.h" @ONLY)

# append target Esp32 nanoCLR source files
list(APPEND TARGET_ESP32_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/app_main.c)
list(APPEND TARGET_ESP32_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_App_Interface.c)
list(APPEND TARGET_ESP32_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_MonitorCommands.c)
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/CLR_Startup_Thread.c") 
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetPAL.c") 
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nanoHAL.cpp")

# append target HAL source files
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Power.c")
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_Time.cpp")

# Either NVS or SPIFSS storage for config, NVS is unable to store large Certificate bundles(over 1864 bytes)
#list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigStorageNVS.cpp")
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigStorageSPIFFS.cpp")
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigurationManager.cpp")

# append target PAL source files
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetPAL_Events.cpp")
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetPAL_Time.cpp")

# append other CLR core files
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Memory.cpp")

# append nanoCRT
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nanoCRT.cpp")

# append nanoHAL
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL.cpp")

# append files from Runtime.Native
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_Rtc.cpp")

# append nanoHAL
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetRandom.cpp")

# add native assemblies
ParseNativeAssemblies()

# configure code file with Interop Assemblies table and...
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CLR_RT_InteropAssembliesTable.cpp.in"
                "${CMAKE_CURRENT_BINARY_DIR}/CLR_RT_InteropAssembliesTable.cpp" @ONLY)
# ... now add Interop Assemblies table to Esp32 nanoCLR sources list
list(APPEND TARGET_ESP32_NANOCLR_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/CLR_RT_InteropAssembliesTable.cpp")

# make var global
set(TARGET_ESP32_NANOCLR_SOURCES ${TARGET_ESP32_NANOCLR_SOURCES} CACHE INTERNAL "make global")

# append target nanoCLR include directory
list(APPEND TARGET_ESP32_NANOCLR_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND TARGET_ESP32_NANOCLR_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND TARGET_ESP32_NANOCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)

# append PAL include directory
list(APPEND TARGET_ESP32_NANOCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)

# append Runtime.Native include directory
list(APPEND TARGET_ESP32_NANOCLR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Runtime.Native)

# make var global
set(TARGET_ESP32_NANOCLR_INCLUDE_DIRS ${TARGET_ESP32_NANOCLR_INCLUDE_DIRS} CACHE INTERNAL "make global")
