#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append Wire Protocol files to source 
# list(APPEND DEBUGGER_WIRE_PROTOCOL_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_Commands.c")
# list(APPEND DEBUGGER_WIRE_PROTOCOL_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_App_Interface.c")
# make Wire Protocol files list global
# set(DEBUGGER_WIRE_PROTOCOL_SOURCES ${DEBUGGER_WIRE_PROTOCOL_SOURCES_LIST} CACHE INTERNAL "CLR debugger Wire Protocol source files list")


# append Wire Protocol include directory
list(APPEND DEBUGGER_WIRE_PROTOCOL_INCLUDE_DIRS_LIST "${CMAKE_CURRENT_SOURCE_DIR}")
# make include dirs global
set(DEBUGGER_WIRE_PROTOCOL_INCLUDE_DIRS ${DEBUGGER_WIRE_PROTOCOL_INCLUDE_DIRS_LIST} CACHE INTERNAL "CLR debugger Wire Protocol include dirs list")
