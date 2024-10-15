#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#


# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Device.OneWire")


# set include directories
list(APPEND nanoFramework.Device.OneWire_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.Device.OneWire_INCLUDE_DIRS ${TARGET_BASE_LOCATION})
list(APPEND nanoFramework.Device.OneWire_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/nanoFramework.Device.OneWire)

# source files
set(nanoFramework.Device.OneWire_SRCS

    nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost.cpp
    nf_dev_onewire.cpp

    target_nf_dev_onewire_config.cpp
)

foreach(SRC_FILE ${nanoFramework.Device.OneWire_SRCS})

    set(nanoFramework.Device.OneWire_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Device.OneWire_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${CMAKE_SOURCE_DIR}/src/nanoFramework.Device.OneWire

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Device.OneWire_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Device.OneWire_SOURCES ${nanoFramework.Device.OneWire_SRC_FILE})

endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Device.OneWire DEFAULT_MSG nanoFramework.Device.OneWire_INCLUDE_DIRS nanoFramework.Device.OneWire_SOURCES)
