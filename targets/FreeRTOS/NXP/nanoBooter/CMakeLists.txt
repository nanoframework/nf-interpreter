#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#


# append target ChibiOS nanoBooter source files 
list(APPEND TARGET_NXP_NANOBOOTER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_App_Interface.c")
list(APPEND TARGET_NXP_NANOBOOTER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_MonitorCommands.c")

# make var global
set(TARGET_NXP_NANOBOOTER_SOURCES ${TARGET_NXP_NANOBOOTER_SOURCES} CACHE INTERNAL "make global")

# append include directory for debugger
list(APPEND TARGET_NXP_NANOBOOTER_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Debugger")
    
# append target ChibiOS nanoBooter include directory
list(APPEND TARGET_NXP_NANOBOOTER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
# make var global
set(TARGET_NXP_NANOBOOTER_INCLUDE_DIRS ${TARGET_NXP_NANOBOOTER_INCLUDE_DIRS} CACHE INTERNAL "make global")
