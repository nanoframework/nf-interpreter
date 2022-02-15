#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.Device.WiFi)


# set include directories
list(APPEND System.Device.WiFi_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND System.Device.WiFi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/System.Device.WiFi)


# source files
set(System.Device.WiFi_SRCS

    sys_dev_wifi_native.cpp
    sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter.cpp
)

foreach(SRC_FILE ${System.Device.WiFi_SRCS})

    set(System.Device.WiFi_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(System.Device.WiFi_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}
            ${CMAKE_SOURCE_DIR}/src/System.Device.WiFi

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${System.Device.WiFi_SRC_FILE}")
    endif()

    list(APPEND System.Device.WiFi_SOURCES ${System.Device.WiFi_SRC_FILE})
    
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Device.WiFi DEFAULT_MSG System.Device.WiFi_INCLUDE_DIRS System.Device.WiFi_SOURCES)
