#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoFramework Debugger
list(APPEND NF_WireProtocol_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Messaging)
list(APPEND NF_WireProtocol_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/WireProtocol)
list(APPEND NF_WireProtocol_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)

# source files for nanoFramework Debugger
set(NF_WireProtocol_SRCS

    Messaging.cpp
    WireProtocol.cpp
    nanoSupport_CRC32.c
)

foreach(SRC_FILE ${NF_WireProtocol_SRCS})
    set(NF_WireProtocol_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(NF_WireProtocol_SRC_FILE ${SRC_FILE}
        PATHS 
            
            ${PROJECT_SOURCE_DIR}/src/CLR/Messaging
            ${PROJECT_SOURCE_DIR}/src/CLR/WireProtocol
            ${PROJECT_SOURCE_DIR}/src/CLR/Core

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${NF_WireProtocol_SRC_FILE}") # debug helper
    list(APPEND NF_WireProtocol_SOURCES ${NF_WireProtocol_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_WireProtocol DEFAULT_MSG NF_WireProtocol_INCLUDE_DIRS NF_WireProtocol_SOURCES)
