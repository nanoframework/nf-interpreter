#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "targets/CMSIS-OS/ChibiOS/nanoCLR/Windows.Devices.Gpio.Provider")


# set include directories
list(APPEND Windows.Devices.Gpio.Provider_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Core")
list(APPEND Windows.Devices.Gpio.Provider_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Include")
list(APPEND Windows.Devices.Gpio.Provider_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/HAL/Include")
list(APPEND Windows.Devices.Gpio.Provider_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/PAL/Include")
list(APPEND Windows.Devices.Gpio.Provider_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/${BASE_PATH_FOR_THIS_MODULE}")


# source files
set(Windows.Devices.Gpio.Provider_SRCS

    windows_devices_gpio_provider_native_Windows_Devices_Gpio_Provider_DefaultGpioPinProvider.cpp
    windows_devices_gpio_provider_native_Windows_Devices_Gpio_Provider_DefaultGpioControllerProvider.cpp
    windows_devices_gpio_provider_native_Windows_Devices_Gpio_Provider_DefaultGpioPinProvider_mshl.cpp
    windows_devices_gpio_provider_native_Windows_Devices_Gpio_Provider_DefaultGpioControllerProvider_mshl.cpp
    windows_devices_gpio_provider_native.cpp
)

foreach(SRC_FILE ${Windows.Devices.Gpio.Provider_SRCS})
    set(Windows.Devices.Gpio.Provider_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(Windows.Devices.Gpio.Provider_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_SOURCE_DIR}/${BASE_PATH_FOR_THIS_MODULE}

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${Windows.Devices.Gpio.Provider_SRC_FILE}") # debug helper
    list(APPEND Windows.Devices.Gpio.Provider_SOURCES ${Windows.Devices.Gpio.Provider_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Debugger DEFAULT_MSG Windows.Devices.Gpio.Provider_INCLUDE_DIRS Windows.Devices.Gpio.Provider_SOURCES)
