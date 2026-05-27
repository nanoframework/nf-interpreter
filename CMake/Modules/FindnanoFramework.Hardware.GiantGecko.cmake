#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Hardware.GiantGecko)


# set include directories
list(APPEND nanoFramework.Hardware.GiantGecko_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})

# source files
set(nanoFramework.Hardware.GiantGecko_SRCS

    nf_hardware_giantgecko.cpp

    nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration.cpp
    nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Power.cpp
    nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_RTC.cpp
    nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Utilities.cpp

)

foreach(SRC_FILE ${nanoFramework.Hardware.GiantGecko_SRCS})

    set(nanoFramework.Hardware.GiantGecko_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Hardware.GiantGecko_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Hardware.GiantGecko_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Hardware.GiantGecko_SOURCES ${nanoFramework.Hardware.GiantGecko_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Hardware.GiantGecko DEFAULT_MSG nanoFramework.Hardware.GiantGecko_INCLUDE_DIRS nanoFramework.Hardware.GiantGecko_SOURCES)
