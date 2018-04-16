#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE  "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.Net")

# set include directories
list(APPEND System.Net_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Core")
list(APPEND System.Net_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Include")
list(APPEND System.Net_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/HAL/Include")
list(APPEND System.Net_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/PAL/Include")
list(APPEND System.Net_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")

# source files
set(System.Net_SRCS

    # System.Net
    sys_net_native.cpp  
    sys_net_native_System_Net_Sockets_NativeSocket.cpp

    # System.Net.NetworkInformation
    sys_net_native_System_Net_NetworkInformation_NetworkInterface.cpp
    sys_net_native_System_Net_NetworkInformation_Wireless80211NetworkInterface.cpp

)

foreach(SRC_FILE ${System.Net_SRCS})
    set(System.Net_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(System.Net_SRC_FILE ${SRC_FILE}
        PATHS

            "${PROJECT_SOURCE_DIR}/src/HAL/System.Net"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${System.Net_SRC_FILE}") # debug helper
    list(APPEND System.Net_SOURCES ${System.Net_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Net DEFAULT_MSG System.Net_INCLUDE_DIRS System.Net_SOURCES)
