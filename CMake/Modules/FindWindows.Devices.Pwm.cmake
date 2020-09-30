#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/Windows.Devices.Pwm")


# set include directories
list(APPEND Windows.Devices.Pwm_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND Windows.Devices.Pwm_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND Windows.Devices.Pwm_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND Windows.Devices.Pwm_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND Windows.Devices.Pwm_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND Windows.Devices.Pwm_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/Windows.Devices.Pwm)


# source files
set(Windows.Devices.Pwm_SRCS

    win_dev_pwm_native.cpp
    win_dev_pwm_native_Windows_Devices_Pwm_PwmPin.cpp
	win_dev_pwm_native_Windows_Devices_Pwm_PwmController.cpp
)

foreach(SRC_FILE ${Windows.Devices.Pwm_SRCS})
    set(Windows.Devices.Pwm_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(Windows.Devices.Pwm_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}
            ${PROJECT_SOURCE_DIR}/src/Windows.Devices.Pwm

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${Windows.Devices.Pwm_SRC_FILE}") # debug helper
    list(APPEND Windows.Devices.Pwm_SOURCES ${Windows.Devices.Pwm_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Windows.Devices.Pwm DEFAULT_MSG Windows.Devices.Pwm_INCLUDE_DIRS Windows.Devices.Pwm_SOURCES)
