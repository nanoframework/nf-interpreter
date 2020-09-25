#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#


# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Devices.OneWire")


# set include directories
list(APPEND nanoFramework.Devices.OneWire_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.Devices.OneWire_INCLUDE_DIRS ${TARGET_BASE_LOCATION})
list(APPEND nanoFramework.Devices.OneWire_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/nanoFramework.Devices.OneWire)

# source files
set(nanoFramework.Devices.OneWire_SRCS

    nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController.cpp
    nf_devices_onewire_native.cpp

    target_nf_devices_onewire_config.cpp
)

foreach(SRC_FILE ${nanoFramework.Devices.OneWire_SRCS})
    set(nanoFramework.Devices.OneWire_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(nanoFramework.Devices.OneWire_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.Devices.OneWire

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoFramework.Devices.OneWire_SRC_FILE}") # debug helper
    list(APPEND nanoFramework.Devices.OneWire_SOURCES ${nanoFramework.Devices.OneWire_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Devices.OneWire DEFAULT_MSG nanoFramework.Devices.OneWire_INCLUDE_DIRS nanoFramework.Devices.OneWire_SOURCES)
