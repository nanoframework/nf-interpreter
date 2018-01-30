#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_MODULES}/Windows.Devices.Wifi")


# set include directories
list(APPEND Windows.Devices.Wifi_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Core")
list(APPEND Windows.Devices.Wifi_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Include")
list(APPEND Windows.Devices.Wifi_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/HAL/Include")
list(APPEND Windows.Devices.Wifi_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/PAL/Include")
list(APPEND Windows.Devices.Wifi_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/${BASE_PATH_FOR_THIS_MODULE}")


# source files
set(Windows.Devices.Wifi_SRCS

    win_dev_wifi_native.cpp
    win_dev_wifi_native.h
    win_dev_wifi_native_Windows_Devices_WIFI_WifiAdapter.cpp
    win_dev_wifi_native_Windows_Devices_WiFi_WiFiNetworkReport.cpp
)

foreach(SRC_FILE ${Windows.Devices.Wifi_SRCS})
    set(Windows.Devices.Wifi_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(Windows.Devices.Wifi_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_SOURCE_DIR}/${BASE_PATH_FOR_THIS_MODULE}

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${Windows.Devices.Wifi_SRC_FILE}") # debug helper
    list(APPEND Windows.Devices.Wifi_SOURCES ${Windows.Devices.Wifi_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Debugger DEFAULT_MSG Windows.Devices.Wifi_INCLUDE_DIRS Windows.Devices.Wifi_SOURCES)
