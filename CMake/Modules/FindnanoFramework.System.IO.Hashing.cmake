#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.System.IO.Hashing)


# set include directories
list(APPEND nanoFramework.System.IO.Hashing_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND nanoFramework.System.IO.Hashing_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND nanoFramework.System.IO.Hashing_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND nanoFramework.System.IO.Hashing_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND nanoFramework.System.IO.Hashing_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.System.IO.Hashing_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/nanoFramework.System.IO.Hashing)

# source files
set(nanoFramework.System.IO.Hashing_SRCS

    nf_sys_io_hashing.cpp


    nf_sys_io_hashing_System_IO_Hashing_Crc32.cpp

)

foreach(SRC_FILE ${nanoFramework.System.IO.Hashing_SRCS})

    set(nanoFramework.System.IO.Hashing_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.System.IO.Hashing_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.System.IO.Hashing

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.System.IO.Hashing_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.System.IO.Hashing_SOURCES ${nanoFramework.System.IO.Hashing_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.System.IO.Hashing DEFAULT_MSG nanoFramework.System.IO.Hashing_INCLUDE_DIRS nanoFramework.System.IO.Hashing_SOURCES)
