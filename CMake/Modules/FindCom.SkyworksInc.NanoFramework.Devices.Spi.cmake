#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/Com.SkyworksInc.NanoFramework.Devices.Spi)


# set include directories
list(APPEND Com.SkyworksInc.NanoFramework.Devices.Spi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Core)
list(APPEND Com.SkyworksInc.NanoFramework.Devices.Spi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Include)
list(APPEND Com.SkyworksInc.NanoFramework.Devices.Spi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/HAL/Include)
list(APPEND Com.SkyworksInc.NanoFramework.Devices.Spi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Include)
list(APPEND Com.SkyworksInc.NanoFramework.Devices.Spi_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND Com.SkyworksInc.NanoFramework.Devices.Spi_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Com.SkyworksInc.NanoFramework.Devices.Spi)

# source files
set(Com.SkyworksInc.NanoFramework.Devices.Spi_SRCS

    com_sky_nf_dev_spi_native.cpp
    com_sky_nf_dev_spi_native_Com_SkyworksInc_NanoFramework_Devices_Spi_SpiBus.cpp

    target_com_sky_nf_dev_spi_config.cpp
)

foreach(SRC_FILE ${Com.SkyworksInc.NanoFramework.Devices.Spi_SRCS})

    set(Com.SkyworksInc.NanoFramework.Devices.Spi_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(Com.SkyworksInc.NanoFramework.Devices.Spi_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${CMAKE_SOURCE_DIR}/src/PAL/Com.SkyworksInc.NanoFramework.Devices.Spi

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    #if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${Com.SkyworksInc.NanoFramework.Devices.Spi_SRC_FILE}")
    #endif()

    list(APPEND Com.SkyworksInc.NanoFramework.Devices.Spi_SOURCES ${Com.SkyworksInc.NanoFramework.Devices.Spi_SRC_FILE})

endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Com.SkyworksInc.NanoFramework.Devices.Spi DEFAULT_MSG Com.SkyworksInc.NanoFramework.Devices.Spi_INCLUDE_DIRS Com.SkyworksInc.NanoFramework.Devices.Spi_SOURCES)
