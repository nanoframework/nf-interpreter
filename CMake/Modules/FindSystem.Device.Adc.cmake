#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.Device.Adc)


# set include directories
list(APPEND System.Device.Adc_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Core)
list(APPEND System.Device.Adc_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Include)
list(APPEND System.Device.Adc_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/HAL/Include)
list(APPEND System.Device.Adc_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Include)
list(APPEND System.Device.Adc_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND System.Device.Adc_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/System.Device.Adc)

# source files
# ESP32 uses legacy ADC driver for now as there is a dependency to legacy I2S due to internal DAC on ESP32
if(TARGET_SERIES_SHORT AND ${TARGET_SERIES_SHORT} STREQUAL "esp32")
set(System.Device.Adc_SRCS

    sys_dev_adc_native.cpp

    sys_dev_adc_native_System_Device_Adc_AdcChannel_legacy.cpp
    sys_dev_adc_native_System_Device_Adc_AdcController_legacy.cpp

    target_system_device_adc_config.cpp
)
else()
set(System.Device.Adc_SRCS
    sys_dev_adc_native.cpp

    sys_dev_adc_native_System_Device_Adc_AdcChannel.cpp
    sys_dev_adc_native_System_Device_Adc_AdcController.cpp

    target_system_device_adc_config.cpp
)
endif()

foreach(SRC_FILE ${System.Device.Adc_SRCS})

    set(System.Device.Adc_SRC_FILE SRC_FILE-NOTFOUND)
    
    find_file(System.Device.Adc_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${CMAKE_SOURCE_DIR}/src/System.Device.Adc

	    CMAKE_FIND_ROOT_PATH_BOTH
    )
    
    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${System.Device.Adc_SRC_FILE}")
    endif()
    
    list(APPEND System.Device.Adc_SOURCES ${System.Device.Adc_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Device.Adc DEFAULT_MSG System.Device.Adc_INCLUDE_DIRS System.Device.Adc_SOURCES)
