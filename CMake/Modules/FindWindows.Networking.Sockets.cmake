#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
#set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/Windows.Networking.Sockets")
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.Net")


# set include directories
list(APPEND Windows.Networking.Sockets_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND Windows.Networking.Sockets_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND Windows.Networking.Sockets_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND Windows.Networking.Sockets_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND Windows.Networking.Sockets_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND Windows.Networking.Sockets_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/Windows.Networking.Sockets)


# source files
set(Windows.Networking.Sockets_SRCS

#    win_net_sockets_native.cpp
#    win_net_sockets_native_Windows_Networking_Sockets_SocketNative.cpp
    sys_net_native.cpp  
    sys_net_native_System_Net_Sockets_NativeSocket.cpp
)

foreach(SRC_FILE ${Windows.Networking.Sockets_SRCS})
    set(Windows.Networking.Sockets_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(Windows.Networking.Sockets_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}
            ${PROJECT_SOURCE_DIR}/src/Windows.Networking.Sockets

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${Windows.Networking.Sockets_SRC_FILE}") # debug helper
    list(APPEND Windows.Networking.Sockets_SOURCES ${Windows.Networking.Sockets_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Windows.Networking.Sockets DEFAULT_MSG Windows.Networking.Sockets_INCLUDE_DIRS Windows.Networking.Sockets_SOURCES)
