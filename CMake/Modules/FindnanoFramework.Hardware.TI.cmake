#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Hardware.TI)


# set include directories
list(APPEND nanoFramework.Hardware.TI_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND nanoFramework.Hardware.TI_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND nanoFramework.Hardware.TI_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND nanoFramework.Hardware.TI_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND nanoFramework.Hardware.TI_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.Hardware.TI_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/nanoFramework.Hardware.TI)

# source files
set(nanoFramework.Hardware.TI_SRCS

    nf_hardware_ti_native.cpp


    nf_hardware_ti_native_nanoFramework_Hardware_TI_Power.cpp
    nf_hardware_ti_native_nanoFramework_Hardware_TI_Utilities.cpp

)

foreach(SRC_FILE ${nanoFramework.Hardware.TI_SRCS})
    set(nanoFramework.Hardware.TI_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(nanoFramework.Hardware.TI_SRC_FILE ${SRC_FILE}
        PATHS
	        "${BASE_PATH_FOR_THIS_MODULE}"
	        "${TARGET_BASE_LOCATION}"

	    CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoFramework.Hardware.TI_SRC_FILE}") # debug helper
    list(APPEND nanoFramework.Hardware.TI_SOURCES ${nanoFramework.Hardware.TI_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Hardware.TI DEFAULT_MSG nanoFramework.Hardware.TI_INCLUDE_DIRS nanoFramework.Hardware.TI_SOURCES)
