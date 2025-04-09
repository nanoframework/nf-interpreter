#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/System.IO.FileSystem)

if(RTOS_FREERTOS_CHECK)
    set(PROJECT_COMMON_PATH ${PROJECT_SOURCE_DIR}/targets/FreeRTOS/NXP/_common)
    set(FATFS_PLATFORM ${PROJECT_SOURCE_DIR}/targets/FreeRTOS/NXP/_FatFs)
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

    nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard_stubs.cpp
    nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard.cpp
    nf_sys_io_filesystem_System_IO_Directory.cpp
    nf_sys_io_filesystem_System_IO_DriveInfo.cpp
    nf_sys_io_filesystem_System_IO_NativeFileStream.cpp
    nf_sys_io_filesystem_System_IO_NativeFindFile.cpp
    nf_sys_io_filesystem_System_IO_NativeIO.cpp

    # file system drivers
    nanoPAL_FileSystem.cpp

    Target_System_IO_FileSystem.c
    target_FileSystem.cpp
)

# add littlefs FS driver
if(NF_FEATURE_USE_LITTLEFS_OPTION)

    list(APPEND System.IO.FileSystem_SRCS
        littlefs_FS_Driver.cpp)
    
    list(APPEND System.IO.FileSystem_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_littlefs)
endif()

# add FatFs FS driver
if(NF_FEATURE_HAS_SDCARD OR NF_FEATURE_HAS_USB_MSD)

    list(APPEND System.IO.FileSystem_SRCS
        fatfs_FS_Driver.cpp)

    list(APPEND System.IO.FileSystem_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_FatFs)
endif()

foreach(SRC_FILE ${System.IO.FileSystem_SRCS})

    set(System.IO.FileSystem_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(System.IO.FileSystem_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_COMMON_PATH}
            ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}

            ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem

            ${CMAKE_SOURCE_DIR}/src/PAL/FileSystem

            # littlefs
            ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_littlefs

            # FatFs
            ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_FatFs
            ${FATFS_PLATFORM}

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${System.IO.FileSystem_SRC_FILE}")
    endif()

    list(APPEND System.IO.FileSystem_SOURCES ${System.IO.FileSystem_SRC_FILE})
    
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.IO.FileSystem DEFAULT_MSG System.IO.FileSystem_INCLUDE_DIRS System.IO.FileSystem_SOURCES)
