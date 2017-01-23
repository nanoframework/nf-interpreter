# //
# // Copyright (c) 2017 The nano Framework project contributors
# // See LICENSE file in the project root for full license information.
# //

# append Wire Protocol files to source 
list(APPEND NANOBOOTER_WIRE_PROTOCOL_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_Commands.c")
list(APPEND NANOBOOTER_WIRE_PROTOCOL_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_ReceiverThread.c")
list(APPEND NANOBOOTER_WIRE_PROTOCOL_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_App_Interface.c")

# make Wire Protocol files list global
set(NANOBOOTER_WIRE_PROTOCOL_SOURCES ${NANOBOOTER_WIRE_PROTOCOL_SOURCES_LIST} CACHE INTERNAL "nanoBooter Wire Protocol source files list")


# append Wire Protocol include directory
list(APPEND NANOBOOTER_WIRE_PROTOCOL_INCLUDE_DIRS_LIST "${CMAKE_CURRENT_SOURCE_DIR}")
# make include dirs global
set(NANOBOOTER_WIRE_PROTOCOL_INCLUDE_DIRS ${NANOBOOTER_WIRE_PROTOCOL_INCLUDE_DIRS_LIST} CACHE INTERNAL "nanoBooter Wire Protocol include dirs list")
