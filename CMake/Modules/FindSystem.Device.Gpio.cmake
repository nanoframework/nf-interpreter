#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.Device.Gpio)


# set include directories
list(APPEND System.Device.Gpio_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Core)
list(APPEND System.Device.Gpio_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Include)
list(APPEND System.Device.Gpio_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/HAL/Include)
list(APPEND System.Device.Gpio_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Include)
list(APPEND System.Device.Gpio_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND System.Device.Gpio_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/System.Device.Gpio)

# source files
set(System.Device.Gpio_SRCS

    sys_dev_gpio_native.cpp
    # TODO add this here when Windows.Devices.Gpio is removed
    # cpu_gpio.cpp

    sys_dev_gpio_native_System_Device_Gpio_GpioController.cpp
    sys_dev_gpio_native_System_Device_Gpio_GpioPin.cpp
    
    # core source files
    AsyncCompletions.cpp
    AsyncContinuations.cpp
    NativeEventDispatcher.cpp
    InterruptHandler.cpp
    Hardware.cpp
)

# TODO remove this IF() clause here when Windows.Devices.Gpio is removed
if(NOT API_Windows.Devices.Gpio)
    # need to have this here in case Windows.Devices.Gpio is not included
    list(APPEND System.Device.Gpio_SRCS cpu_gpio.cpp)
endif()

foreach(SRC_FILE ${System.Device.Gpio_SRCS})

    set(System.Device.Gpio_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(System.Device.Gpio_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${CMAKE_SOURCE_DIR}/src/System.Device.Gpio

            # core source files
            ${CMAKE_SOURCE_DIR}/src/PAL/AsyncProcCall
            ${CMAKE_SOURCE_DIR}/src/CLR/Core/NativeEventDispatcher
            ${CMAKE_SOURCE_DIR}/src/CLR/Core/InterruptHandler
            ${CMAKE_SOURCE_DIR}/src/CLR/Core/Hardware

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${System.Device.Gpio_SRC_FILE}")
    endif()

    list(APPEND System.Device.Gpio_SOURCES ${System.Device.Gpio_SRC_FILE})
    
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Device.Gpio DEFAULT_MSG System.Device.Gpio_INCLUDE_DIRS System.Device.Gpio_SOURCES)
