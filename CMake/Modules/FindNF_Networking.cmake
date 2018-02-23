#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoFramework network
list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets)
list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Lwip)

# source files for nanoFramework Networking
set(NF_Networking_SRCS

    #pal Socket
    sockets_lwip.cpp

    #Lwip 
    LwIP_Sockets.cpp
    LwIP_Sockets_functions.cpp 
)

if(NF_FEATURE_DEBUGGER)
    list(APPEND NF_Networking_SRCS sockets_debugger.cpp)
endif()

foreach(SRC_FILE ${NF_Networking_SRCS})
    set(NF_Networking_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(NF_Networking_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets
            ${PROJECT_SOURCE_DIR}/src/PAL/Lwip
            ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}
 
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${NF_Networking_SRC_FILE}") # debug helper
    list(APPEND NF_Networking_SOURCES ${NF_Networking_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Networking DEFAULT_MSG NF_Networking_INCLUDE_DIRS NF_Networking_SOURCES)
