#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append target Azure RTOS nanoBooter source files 
list(APPEND TARGET_AZURERTOS_NANOBOOTER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_App_Interface.c)
# make var global
set(TARGET_AZURERTOS_NANOBOOTER_SOURCES ${TARGET_AZURERTOS_NANOBOOTER_SOURCES} CACHE INTERNAL "make global")

# append include directory for debugger
list(APPEND TARGET_AZURERTOS_NANOBOOTER_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Debugger)
    
# append target AZURERTOS nanoBooter include directory
list(APPEND TARGET_AZURERTOS_NANOBOOTER_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
# make var global
set(TARGET_AZURERTOS_NANOBOOTER_INCLUDE_DIRS ${TARGET_AZURERTOS_NANOBOOTER_INCLUDE_DIRS} CACHE INTERNAL "make global")
