#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_MODULES}/Windows.Devices.Adc")


# set include directories
list(APPEND Windows.Devices.Adc_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Core")
list(APPEND Windows.Devices.Adc_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Include")
list(APPEND Windows.Devices.Adc_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/HAL/Include")
list(APPEND Windows.Devices.Adc_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/PAL/Include")
list(APPEND Windows.Devices.Adc_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/${BASE_PATH_FOR_THIS_MODULE}")


# source files
set(Windows.Devices.Adc_SRCS

    win_dev_adc_native.cpp
    win_dev_adc_native.h
    win_dev_adc_native_Windows_Devices_ADC_AdcDevice.cpp
)

foreach(SRC_FILE ${Windows.Devices.Adc_SRCS})
    set(Windows.Devices.Adc_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(Windows.Devices.Adc_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_SOURCE_DIR}/${BASE_PATH_FOR_THIS_MODULE}

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${Windows.Devices.Adc_SRC_FILE}") # debug helper
    list(APPEND Windows.Devices.Adc_SOURCES ${Windows.Devices.Adc_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Debugger DEFAULT_MSG Windows.Devices.Adc_INCLUDE_DIRS Windows.Devices.Adc_SOURCES)
