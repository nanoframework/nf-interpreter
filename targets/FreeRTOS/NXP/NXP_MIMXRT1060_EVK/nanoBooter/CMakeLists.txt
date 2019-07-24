#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# PAL source files 
list(APPEND NANOBOOTER_PROJECT_SOURCES "${PROJECT_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c")

# append nanoBooter source files
list(APPEND NANOBOOTER_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.c")

# append include directory for debugger
list(APPEND TARGET_FREERTOS_NANOBOOTER_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Debugger")

# make var global
set(NANOBOOTER_PROJECT_SOURCES ${NANOBOOTER_PROJECT_SOURCES} CACHE INTERNAL "make global")
set(TARGET_FREERTOS_NANOBOOTER_INCLUDE_DIRS ${TARGET_FREERTOS_NANOBOOTER_INCLUDE_DIRS} CACHE INTERNAL "make global")
