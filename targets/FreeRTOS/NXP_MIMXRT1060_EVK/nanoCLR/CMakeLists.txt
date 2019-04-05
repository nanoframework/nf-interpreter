#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(NF_NativeAssemblies)

# append nanoCLR source files
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Memory.cpp")
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nanoHAL.cpp")
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/CLR_Startup_Thread.c")
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_App_Interface.c")

list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/TargetHAL.cpp")

# append target HAL source files
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/TargetHAL_Time.cpp")
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/TargetHAL_Power.c")

# append nanoCRT
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nanoCRT.cpp")

# append target PAL source files
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetPAL_Events.cpp")
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetPAL_Time.cpp")

# add native assemblies
ParseNativeAssemblies()

# configure code file with Interop Assemblies table and...
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CLR_RT_InteropAssembliesTable.cpp.in"
                "${CMAKE_CURRENT_BINARY_DIR}/CLR_RT_InteropAssembliesTable.cpp" @ONLY)
# ... now add Interop Assemblies table to Esp32 nanoCLR sources list
list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/CLR_RT_InteropAssembliesTable.cpp")

# make var global
set(NANOCLR_PROJECT_SOURCES ${NANOCLR_PROJECT_SOURCES} CACHE INTERNAL "make global")