#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Hardware.ESP32")


# set include directories
list(APPEND nanoFramework.Hardware.Esp32_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/CLR/Core")
list(APPEND nanoFramework.Hardware.Esp32_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/CLR/Include")
list(APPEND nanoFramework.Hardware.Esp32_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/HAL/Include")
list(APPEND nanoFramework.Hardware.Esp32_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/PAL/Include")
list(APPEND nanoFramework.Hardware.Esp32_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")

# source files
set(nanoFramework.Hardware.Esp32_SRCS

    nanoFramework_hardware_esp32_native.cpp
    nanoFramework_hardware_esp32_native_Hardware_Esp32_Sleep.cpp
    nanoFramework_hardware_esp32_native_Hardware_Esp32_Logging.cpp
    nanoFramework_hardware_esp32_native_Hardware_Esp32_HighResTimer.cpp
	nanoFramework_hardware_esp32_native_Hardware_Esp32_Configuration.cpp
    nanoFramework_hardware_esp32_native_Hardware_Esp32_NativeMemory.cpp
    nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad.cpp
    nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCounter.cpp
)

foreach(SRC_FILE ${nanoFramework.Hardware.Esp32_SRCS})

    set(nanoFramework.Hardware.Esp32_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Hardware.Esp32_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Hardware.Esp32_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Hardware.Esp32_SOURCES ${nanoFramework.Hardware.Esp32_SRC_FILE})

endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Hardware.Esp32 DEFAULT_MSG nanoFramework.Hardware.Esp32_INCLUDE_DIRS nanoFramework.Hardware.Esp32_SOURCES)
