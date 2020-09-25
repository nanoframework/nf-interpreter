#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# handle Wire Protocol _TRACE_ preferences, if any
option(NF_WP_TRACE_ERRORS "option to Trace errors with Wire Protocol")
option(NF_WP_TRACE_HEADERS "option to Trace headers with Wire Protocol")
option(NF_WP_TRACE_STATE "option to Trace state with Wire Protocol")
option(NF_WP_TRACE_NODATA "option to Trace empty packets with Wire Protocol")
option(NF_WP_TRACE_ALL "option to Trace  with Wire Protocol")
option(NF_WP_IMPLEMENTS_CRC32 "option to report if target implements CRC32 in Wire Protocol")

# this one has to follow the declaration on src\CLR\Include\WireProtocol_Message.h
# #define TRACE_ERRORS 1
# #define TRACE_HEADERS 2
# #define TRACE_STATE 4
# #define TRACE_NODATA 8

# default to 0 (no Wire Protocol trace)
set(WP_TRACE_MASK 0 CACHE INTERNAL "WP trace mask")

if(NF_WP_TRACE_ERRORS)
    math(EXPR WP_TRACE_MASK "${WP_TRACE_MASK} + 1")
endif()
if(NF_WP_TRACE_HEADERS)
    math(EXPR WP_TRACE_MASK "${WP_TRACE_MASK} + 2")
endif()
if(NF_WP_TRACE_STATE)
    math(EXPR WP_TRACE_MASK "${WP_TRACE_MASK} + 4")
endif()
if(NF_WP_TRACE_NODATA)
    math(EXPR WP_TRACE_MASK "${WP_TRACE_MASK} + 8")
endif()
if(NF_WP_TRACE_ALL)
    math(EXPR WP_TRACE_MASK "8 + 4 + 2 + 1")
endif()

message(STATUS "Wire Protocol TRACE_MASK is '${WP_TRACE_MASK}'") # debug helper

# set include directories for Wire Protocol
list(APPEND WireProtocol_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)

# source files for Wire Protocol
set(WireProtocol_SRCS

    WireProtocol_Message.c
    WireProtocol_MonitorCommands.c
    WireProtocol_HAL_Interface.c
    WireProtocol_App_Interface.c

    nanoSupport_CRC32.c
)

foreach(SRC_FILE ${WireProtocol_SRCS})
    set(WireProtocol_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(WireProtocol_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_SOURCE_DIR}/src/CLR/WireProtocol
            ${PROJECT_SOURCE_DIR}/src/CLR/Core

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${WireProtocol_SRC_FILE}") # debug helper
    list(APPEND WireProtocol_SOURCES ${WireProtocol_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(WireProtocol DEFAULT_MSG WireProtocol_INCLUDE_DIRS WireProtocol_SOURCES)