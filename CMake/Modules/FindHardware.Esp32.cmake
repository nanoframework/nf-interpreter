#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/Hardware.Esp32")


# set include directories
list(APPEND Hardware.Esp32_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Core")
list(APPEND Hardware.Esp32_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Include")
list(APPEND Hardware.Esp32_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/HAL/Include")
list(APPEND Hardware.Esp32_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/PAL/Include")
list(APPEND Hardware.Esp32_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")

# source files
set(Hardware.Esp32_SRCS
    hardware_esp32_native.cpp
    hardware_esp32_native.h
    hardware_esp32_native_Hardware_Esp32_sleep.cpp
    hardware_esp32_native_Hardware_Esp32_Logging.cpp
)

foreach(SRC_FILE ${Hardware.Esp32_SRCS})
    set(Hardware.Esp32_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(Hardware.Esp32_SRC_FILE ${SRC_FILE}
        PATHS 
            "${BASE_PATH_FOR_THIS_MODULE}"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${Hardware.Esp32_SRC_FILE}") # debug helper
    list(APPEND Hardware.Esp32_SOURCES ${Hardware.Esp32_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Hardware.Esp32 DEFAULT_MSG Hardware.Esp32_INCLUDE_DIRS Hardware.Esp32_SOURCES)
