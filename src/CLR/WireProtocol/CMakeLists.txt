# //
# // Copyright (c) 2017 The nano Framework project contributors
# // See LICENSE file in the project root for full license information.
# //

# append Wire Protocol files to source 
list(APPEND WIRE_PROTOCOL_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol.c")
list(APPEND WIRE_PROTOCOL_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_Message.c")
list(APPEND WIRE_PROTOCOL_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_Commands.c")
list(APPEND WIRE_PROTOCOL_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c")
list(APPEND WIRE_PROTOCOL_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_ReceiverThread.c")
# make Wire Protocol files list global
set(WIRE_PROTOCOL_SOURCES ${WIRE_PROTOCOL_SOURCES_LIST} CACHE INTERNAL "Wire Protocol source files list")


# append Wire Protocol include directory
list(APPEND WIRE_PROTOCOL_INCLUDE_DIRS_LIST "${CMAKE_CURRENT_SOURCE_DIR}")
# make include dirs global
set(WIRE_PROTOCOL_INCLUDE_DIRS ${WIRE_PROTOCOL_INCLUDE_DIRS_LIST} CACHE INTERNAL "Wire Protocol include dirs list")
