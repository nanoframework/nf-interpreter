#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.Device.I2c.Slave)


# set include directories
list(APPEND System.Device.I2c.Slave_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND System.Device.I2c.Slave_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND System.Device.I2c.Slave_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND System.Device.I2c.Slave_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND System.Device.I2c.Slave_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND System.Device.I2c.Slave_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/System.Device.I2c.Slave)

# source files
set(System.Device.I2c.Slave_SRCS

    sys_dev_i2c_slave_native.cpp


    sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice.cpp
)

foreach(SRC_FILE ${System.Device.I2c.Slave_SRCS})

    set(System.Device.I2c.Slave_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(System.Device.I2c.Slave_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/System.Device.I2c.Slave

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${System.Device.I2c.Slave_SRC_FILE}")
    endif()

    list(APPEND System.Device.I2c.Slave_SOURCES ${System.Device.I2c.Slave_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Device.I2c.Slave DEFAULT_MSG System.Device.I2c.Slave_INCLUDE_DIRS System.Device.I2c.Slave_SOURCES)
