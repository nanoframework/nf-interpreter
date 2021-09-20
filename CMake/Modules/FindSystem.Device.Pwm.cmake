#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.Device.Pwm)


# set include directories
list(APPEND System.Device.Pwm_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND System.Device.Pwm_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND System.Device.Pwm_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND System.Device.Pwm_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND System.Device.Pwm_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND System.Device.Pwm_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/System.Device.Pwm)

# source files
set(System.Device.Pwm_SRCS

    sys_dev_pwm_native.cpp


    sys_dev_pwm_native_System_Device_Pwm_PwmChannel.cpp

)

foreach(SRC_FILE ${System.Device.Pwm_SRCS})
    set(System.Device.Pwm_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(System.Device.Pwm_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/System.Device.Pwm

	    CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${System.Device.Pwm_SRC_FILE}") # debug helper
    list(APPEND System.Device.Pwm_SOURCES ${System.Device.Pwm_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Device.Pwm DEFAULT_MSG System.Device.Pwm_INCLUDE_DIRS System.Device.Pwm_SOURCES)
