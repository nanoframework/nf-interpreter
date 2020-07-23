#
# Copyright (c) 2020 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#


# append target ST nanoBooter source files 
list(APPEND TARGET_FREERTOS_ST_NANOBOOTER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_MonitorCommands.c")

# make var global
set(TARGET_FREERTOS_ST_NANOBOOTER_SOURCES ${TARGET_FREERTOS_ST_NANOBOOTER_SOURCES} CACHE INTERNAL "make global")

# append include directory for debugger
list(APPEND TARGET_FREERTOS_ST_NANOBOOTER_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Debugger")
    
# append target ChibiOS nanoBooter include directory
list(APPEND TARGET_FREERTOS_ST_NANOBOOTER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
# make var global
set(TARGET_FREERTOS_ST_NANOBOOTER_INCLUDE_DIRS ${TARGET_FREERTOS_ST_NANOBOOTER_INCLUDE_DIRS} CACHE INTERNAL "make global")
