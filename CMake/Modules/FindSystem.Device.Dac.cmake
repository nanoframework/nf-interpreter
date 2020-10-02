#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.Device.Dac")


# set include directories
list(APPEND System.Device.Dac_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND System.Device.Dac_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND System.Device.Dac_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND System.Device.Dac_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND System.Device.Dac_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND System.Device.Dac_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/System.Device.Dac)


# source files
set(System.Device.Dac_SRCS

    sys_dev_dac_native.cpp
    sys_dev_dac_native_System_Device_Dac_DacController.cpp
    sys_dev_dac_native_System_Device_Dac_DacChannel.cpp

    target_system_device_dac_config.cpp
)

foreach(SRC_FILE ${System.Device.Dac_SRCS})
    set(System.Device.Dac_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(System.Device.Dac_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/System.Device.Dac
            
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${System.Device.Dac_SRC_FILE}") # debug helper
    list(APPEND System.Device.Dac_SOURCES ${System.Device.Dac_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Device.Dac DEFAULT_MSG System.Device.Dac_INCLUDE_DIRS System.Device.Dac_SOURCES)
