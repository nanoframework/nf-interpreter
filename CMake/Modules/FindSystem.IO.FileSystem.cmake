#
# Copyright(c) 2020 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.IO.FileSystem)


# set include directories
list(APPEND System.IO.FileSystem_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND System.IO.FileSystem_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND System.IO.FileSystem_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND System.IO.FileSystem_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND System.IO.FileSystem_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND System.IO.FileSystem_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/System.IO.FileSystem)

# source files
set(System.IO.FileSystem_SRCS

    nf_sys_io_filesystem.cpp


    nf_sys_io_filesystem_System_IO_File.cpp
    nf_sys_io_filesystem_System_IO_FileStream.cpp

)

foreach(SRC_FILE ${System.IO.FileSystem_SRCS})

    set(System.IO.FileSystem_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(System.IO.FileSystem_SRC_FILE ${SRC_FILE}
        PATHS
	        "${BASE_PATH_FOR_THIS_MODULE}"
	        "${TARGET_BASE_LOCATION}"

	    CMAKE_FIND_ROOT_PATH_BOTH
    )
# message("${SRC_FILE} >> ${System.IO.FileSystem_SRC_FILE}") # debug helper
list(APPEND System.IO.FileSystem_SOURCES ${System.IO.FileSystem_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.IO.FileSystem DEFAULT_MSG System.IO.FileSystem_INCLUDE_DIRS System.IO.FileSystem_SOURCES)
