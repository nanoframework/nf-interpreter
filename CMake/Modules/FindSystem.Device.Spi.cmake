#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.Device.Spi)


# set include directories
list(APPEND System.Device.Spi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Core)
list(APPEND System.Device.Spi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Include)
list(APPEND System.Device.Spi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/HAL/Include)
list(APPEND System.Device.Spi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Include)
list(APPEND System.Device.Spi_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND System.Device.Spi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/System.Device.Spi)

# source files
set(System.Device.Spi_SRCS
    cpu_spi.cpp   
    nanoHAL_Spi.cpp
    sys_dev_spi_native_System_Device_Spi_SpiBusInfo.cpp
    sys_dev_spi_native_System_Device_Spi_SpiDevice.cpp
    sys_dev_spi_native.cpp
    target_system_device_spi_config.cpp
)

foreach(SRC_FILE ${System.Device.Spi_SRCS})
    set(System.Device.Spi_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(System.Device.Spi_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${CMAKE_SOURCE_DIR}/src/System.Device.Spi

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${System.Device.Spi_SRC_FILE}")
    endif()

    list(APPEND System.Device.Spi_SOURCES ${System.Device.Spi_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Device.Spi DEFAULT_MSG System.Device.Spi_INCLUDE_DIRS System.Device.Spi_SOURCES)
