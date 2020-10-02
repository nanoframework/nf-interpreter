#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/Windows.Devices.Gpio")


# set include directories
list(APPEND Windows.Devices.Gpio_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND Windows.Devices.Gpio_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND Windows.Devices.Gpio_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND Windows.Devices.Gpio_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND Windows.Devices.Gpio_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND Windows.Devices.Gpio_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/Windows.Devices.Gpio)


# source files
set(Windows.Devices.Gpio_SRCS

    # class library source files
	cpu_gpio.cpp
    win_dev_gpio_native_Windows_Devices_Gpio_GpioController.cpp
    win_dev_gpio_native_Windows_Devices_Gpio_GpioPin.cpp
	win_dev_gpio_native_Windows_Devices_Gpio_GpioChangeCounter.cpp
    win_dev_gpio_native.cpp

    # core source files
    AsyncCompletions.cpp
    AsyncContinuations.cpp
    NativeEventDispatcher.cpp
    InterruptHandler.cpp
    Hardware.cpp
)

foreach(SRC_FILE ${Windows.Devices.Gpio_SRCS})
    set(Windows.Devices.Gpio_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(Windows.Devices.Gpio_SRC_FILE ${SRC_FILE}
        PATHS 

            # class library source files
            ${BASE_PATH_FOR_THIS_MODULE}

            # core source files
            ${PROJECT_SOURCE_DIR}/src/PAL/AsyncProcCall
            ${PROJECT_SOURCE_DIR}/src/CLR/Core/NativeEventDispatcher
            ${PROJECT_SOURCE_DIR}/src/CLR/Core/InterruptHandler
            ${PROJECT_SOURCE_DIR}/src/CLR/Core/Hardware
            ${PROJECT_SOURCE_DIR}/src/Windows.Devices.Gpio

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${Windows.Devices.Gpio_SRC_FILE}") # debug helper
    list(APPEND Windows.Devices.Gpio_SOURCES ${Windows.Devices.Gpio_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Windows.Devices.Gpio DEFAULT_MSG Windows.Devices.Gpio_INCLUDE_DIRS Windows.Devices.Gpio_SOURCES)
