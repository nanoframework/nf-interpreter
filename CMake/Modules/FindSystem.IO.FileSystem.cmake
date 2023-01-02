#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.IO.FileSystem)

if(RTOS_FREERTOS_CHECK)
    set(PROJECT_COMMON_PATH ${PROJECT_SOURCE_DIR}/targets/FreeRTOS/NXP/_common)
else()
    set(PROJECT_COMMON_PATH ${PROJECT_SOURCE_DIR}/targets/${RTOS}/_common)
endif()

# set include directories
list(APPEND System.IO.FileSystem_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND System.IO.FileSystem_INCLUDE_DIRS ${TARGET_BASE_LOCATION}/Include)
list(APPEND System.IO.FileSystem_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem)

# source files
set(System.IO.FileSystem_SRCS

    nf_sys_io_filesystem.cpp

    nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard.cpp
    nf_sys_io_filesystem_System_IO_Directory.cpp
    nf_sys_io_filesystem_System_IO_File.cpp
    nf_sys_io_filesystem_System_IO_FileStream.cpp

    Target_Windows_Storage.c
)

foreach(SRC_FILE ${System.IO.FileSystem_SRCS})

    set(System.IO.FileSystem_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(System.IO.FileSystem_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_COMMON_PATH}
            ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${System.IO.FileSystem_SRC_FILE}")
    endif()

    list(APPEND System.IO.FileSystem_SOURCES ${System.IO.FileSystem_SRC_FILE})
    
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.IO.FileSystem DEFAULT_MSG System.IO.FileSystem_INCLUDE_DIRS System.IO.FileSystem_SOURCES)
