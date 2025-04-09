#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.GiantGecko.Adc)

# set include directories
list(APPEND nanoFramework.GiantGecko.Adc_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.GiantGecko.Adc_INCLUDE_DIRS ${TARGET_BASE_LOCATION})

# source files
set(nanoFramework.GiantGecko.Adc_SRCS

    nano_gg_adc_native.cpp

    nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcChannel.cpp
    nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController.cpp

    target_nano_gg_adc_config.cpp
)

foreach(SRC_FILE ${nanoFramework.GiantGecko.Adc_SRCS})

    set(nanoFramework.GiantGecko.Adc_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.GiantGecko.Adc_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.GiantGecko.Adc

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.GiantGecko.Adc_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.GiantGecko.Adc_SOURCES ${nanoFramework.GiantGecko.Adc_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.GiantGecko.Adc DEFAULT_MSG nanoFramework.GiantGecko.Adc_INCLUDE_DIRS nanoFramework.GiantGecko.Adc_SOURCES)
