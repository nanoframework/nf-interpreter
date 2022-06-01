#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(fatfs)

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/Windows.Storage")


# set include directories
if(RTOS_CHIBIOS_CHECK)
    #list(APPEND Windows.Storage_INCLUDE_DIRS ${chibios_SOURCE_DIR}/ext/fatfs/src)
    list(APPEND Windows.Storage_INCLUDE_DIRS ${fatfs_SOURCE_DIR}/source)
elseif(RTOS_ESP32_CHECK)
    # TODO: this needs to be changed so it's not platform & target dependent
elseif(RTOS_FREERTOS_CHECK)
    list(APPEND Windows.Storage_INCLUDE_DIRS ${fatfs_SOURCE_DIR}/source)
    # TODO: this needs to be changed so it's not platform & target dependent
endif()

list(APPEND Windows.Storage_INCLUDE_DIRS ${TARGET_BASE_LOCATION}/Include)
list(APPEND Windows.Storage_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND Windows.Storage_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/Windows.Storage)

# source files
set(Windows.Storage_SRCS

    win_storage_native_Windows_Storage_FileIO.cpp
    win_storage_native_Windows_Storage_StorageFile.cpp  
    win_storage_native_Windows_Storage_StorageFolder.cpp  
    win_storage_native_Windows_Storage_Devices_SDCard.cpp
    win_storage_native.cpp
)

foreach(SRC_FILE ${Windows.Storage_SRCS})

    set(Windows.Storage_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(Windows.Storage_SRC_FILE ${SRC_FILE}
        PATHS

            ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${CMAKE_SOURCE_DIR}/src/Windows.Storage

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${Windows.Storage_SRC_FILE}")
    endif()

    list(APPEND Windows.Storage_SOURCES ${Windows.Storage_SRC_FILE})
    
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Windows.Storage DEFAULT_MSG Windows.Storage_INCLUDE_DIRS Windows.Storage_SOURCES)
