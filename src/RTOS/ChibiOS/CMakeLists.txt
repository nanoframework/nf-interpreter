# //
# // Copyright (c) 2017 The nano Framework project contributors
# // See LICENSE file in the project root for full license information.
# //

# append source files required for ChibiOS interface  
list(APPEND CHIBIOS_NANO_INTERFACE_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c")
# make ChibiOS interface files list global
set(CHIBIOS_NANO_INTERFACE_SOURCES ${CHIBIOS_NANO_INTERFACE_SOURCES_LIST} CACHE INTERNAL "ChibiOS nanoFramework interface source files list")


# append Wire Protocol include directory
list(APPEND CHIBIOS_NANO_INTERFACE_INCLUDE_DIRS_LIST "${CMAKE_CURRENT_SOURCE_DIR}")
# make include dirs global
set(CHIBIOS_NANO_INTERFACE_INCLUDE_DIRS ${CHIBIOS_NANO_INTERFACE_INCLUDE_DIRS_LIST} CACHE INTERNAL "ChibiOS nanoFramework interface include dirs list")
