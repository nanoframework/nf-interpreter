#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/Windows.Storage")


# set include directories
list(APPEND Windows.Storage_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/fatfs/src")
list(APPEND Windows.Storage_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/Include")
list(APPEND Windows.Storage_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")

# source files
set(Windows.Storage_SRCS

    win_storage_native_Windows_Storage_StorageFolder.cpp  
    win_storage_native.cpp

    Target_Windows_Storage.c
)

foreach(SRC_FILE ${Windows.Storage_SRCS})
    set(Windows.Storage_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(Windows.Storage_SRC_FILE ${SRC_FILE}
        PATHS

            "${BASE_PATH_FOR_THIS_MODULE}"
            "${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/common"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${Windows.Storage_SRC_FILE}") # debug helper
    list(APPEND Windows.Storage_SOURCES ${Windows.Storage_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Windows.Storage DEFAULT_MSG Windows.Storage_INCLUDE_DIRS Windows.Storage_SOURCES)
