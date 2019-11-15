#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#


# append target nanoBooter source files 
list(APPEND TARGET_FREERTOS_NANOBOOTER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_App_Interface.c)
# list(APPEND TARGET_FREERTOS_NANOBOOTER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_MonitorCommands.c")

# append include directory for debugger
list(APPEND TARGET_FREERTOS_NANOBOOTER_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Debugger")
   

# make var global
set(TARGET_FREERTOS_NANOBOOTER_SOURCES ${TARGET_FREERTOS_NANOBOOTER_SOURCES} CACHE INTERNAL "make global")
set(TARGET_FREERTOS_NANOBOOTER_SOURCES ${TARGET_FREERTOS_NANOBOOTER_SOURCES} CACHE INTERNAL "make global")
