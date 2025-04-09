#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.Device.Wifi)


# set include directories
list(APPEND System.Device.Wifi_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND System.Device.Wifi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/System.Device.Wifi)


# source files
set(System.Device.Wifi_SRCS

    sys_dev_wifi_native.cpp
    sys_dev_wifi_native_System_Device_Wifi_WifiAdapter.cpp
)

foreach(SRC_FILE ${System.Device.Wifi_SRCS})

    set(System.Device.Wifi_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(System.Device.Wifi_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}
            ${CMAKE_SOURCE_DIR}/src/System.Device.Wifi

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${System.Device.Wifi_SRC_FILE}")
    endif()

    list(APPEND System.Device.Wifi_SOURCES ${System.Device.Wifi_SRC_FILE})
    
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Device.Wifi DEFAULT_MSG System.Device.Wifi_INCLUDE_DIRS System.Device.Wifi_SOURCES)
