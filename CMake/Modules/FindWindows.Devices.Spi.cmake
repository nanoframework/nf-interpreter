#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/Windows.Devices.Spi")


# set include directories
list(APPEND Windows.Devices.Spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND Windows.Devices.Spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND Windows.Devices.Spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND Windows.Devices.Spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND Windows.Devices.Spi_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND Windows.Devices.Spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/Windows.Devices.Spi)


# source files
set(Windows.Devices.Spi_SRCS
    cpu_spi.cpp
    nanoHAL_spi.cpp
    win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo.cpp
    win_dev_spi_native_Windows_Devices_Spi_SpiController.cpp
    win_dev_spi_native_Windows_Devices_Spi_SpiDevice.cpp
    win_dev_spi_native.cpp

    target_windows_devices_spi_config.cpp
)

foreach(SRC_FILE ${Windows.Devices.Spi_SRCS})
    set(Windows.Devices.Spi_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(Windows.Devices.Spi_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/Windows.Devices.Spi

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${Windows.Devices.Spi_SRC_FILE}") # debug helper
    list(APPEND Windows.Devices.Spi_SOURCES ${Windows.Devices.Spi_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Windows.Devices.Spi DEFAULT_MSG Windows.Devices.Spi_INCLUDE_DIRS Windows.Devices.Spi_SOURCES)
