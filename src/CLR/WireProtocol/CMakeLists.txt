# //
# // Copyright (c) 2017 The nano Framework project contributors
# // See LICENSE file in the project root for full license information.
# //

# append Wire Protocol files to source 
list(APPEND WIREPROTOCOL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol.c")
list(APPEND WIREPROTOCOL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_Message.c")
list(APPEND WIREPROTOCOL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_MonitorCommands.c")
list(APPEND WIREPROTOCOL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c")
list(APPEND WIREPROTOCOL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_App_Interface.c")

# make var global
set(WIREPROTOCOL_SOURCES ${WIREPROTOCOL_SOURCES} CACHE INTERNAL "make global")

# append Wire Protocol include directory
list(APPEND WIREPROTOCOL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

# make var global
set(WIREPROTOCOL_INCLUDE_DIRS ${WIREPROTOCOL_INCLUDE_DIRS} CACHE INTERNAL "make global")
