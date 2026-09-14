#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Runtime.InFieldUpdate)


# set include directories
list(APPEND nanoFramework.Runtime.InFieldUpdate_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND nanoFramework.Runtime.InFieldUpdate_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND nanoFramework.Runtime.InFieldUpdate_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND nanoFramework.Runtime.InFieldUpdate_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND nanoFramework.Runtime.InFieldUpdate_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.Runtime.InFieldUpdate_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/nanoFramework.Runtime.InFieldUpdate)

# source files
set(nanoFramework.Runtime.InFieldUpdate_SRCS

    nf_runtime_ifu.cpp


    nf_runtime_ifu_nanoFramework_Runtime_InFieldUpdate_UpdateManager.cpp

)

foreach(SRC_FILE ${nanoFramework.Runtime.InFieldUpdate_SRCS})

    set(nanoFramework.Runtime.InFieldUpdate_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Runtime.InFieldUpdate_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.Runtime.InFieldUpdate

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Runtime.InFieldUpdate_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Runtime.InFieldUpdate_SOURCES ${nanoFramework.Runtime.InFieldUpdate_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Runtime.InFieldUpdate DEFAULT_MSG nanoFramework.Runtime.InFieldUpdate_INCLUDE_DIRS nanoFramework.Runtime.InFieldUpdate_SOURCES)
