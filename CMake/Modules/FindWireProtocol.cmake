# set include directories for Wire Protocol
list(APPEND WireProtocol_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/WireProtocol)

# source files for Wire Protocol
set(WireProtocol_SRCS

    WireProtocol.c
    WireProtocol_Message.c
    WireProtocol_MonitorCommands.c
    WireProtocol_HAL_Interface.c
    WireProtocol_App_Interface.c

)

foreach(SRC_FILE ${WireProtocol_SRCS})
    set(WireProtocol_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(WireProtocol_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_SOURCE_DIR}/src/CLR/WireProtocol

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${WireProtocol_SRC_FILE}") # debug helper
    list(APPEND WireProtocol_SOURCES ${WireProtocol_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(WireProtocol DEFAULT_MSG WireProtocol_INCLUDE_DIRS WireProtocol_SOURCES)