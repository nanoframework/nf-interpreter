#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoFramework Debugger
list(APPEND NF_Debugger_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Debugger)
list(APPEND NF_Debugger_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Messaging)
list(APPEND NF_Debugger_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/WireProtocol)
list(APPEND NF_Debugger_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)

# source files for nanoFramework Debugger
set(NF_Debugger_SRCS

    Debugger.cpp

    Messaging.cpp
    
    WireProtocol_Message.c
    WireProtocol_MonitorCommands.c
    WireProtocol_HAL_Interface.c
    WireProtocol_App_Interface.c

    nanoSupport_CRC32.c
)

# add the debugger source file according to the build flavor
if(NF_BUILD_RTM)
    set(NF_Debugger_SRCS ${NF_Debugger_SRCS} Debugger_minimal.cpp)
else()
    set(NF_Debugger_SRCS ${NF_Debugger_SRCS} Debugger_full.cpp)
endif()


foreach(SRC_FILE ${NF_Debugger_SRCS})
    set(NF_Debugger_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(NF_Debugger_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_SOURCE_DIR}/src/CLR/Debugger
            ${PROJECT_SOURCE_DIR}/src/CLR/Messaging
            ${PROJECT_SOURCE_DIR}/src/CLR/WireProtocol
            ${PROJECT_SOURCE_DIR}/src/CLR/Core

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${NF_Debugger_SRC_FILE}") # debug helper
    list(APPEND NF_Debugger_SOURCES ${NF_Debugger_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Debugger DEFAULT_MSG NF_Debugger_INCLUDE_DIRS NF_Debugger_SOURCES)
