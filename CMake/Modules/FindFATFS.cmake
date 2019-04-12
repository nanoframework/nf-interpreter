#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#


# List of the required FatFs include files.
list(APPEND FATFS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/FatFS_Source/source)

set(FATFS_SRCS

    # fatfs
    ff.c
    ffunicode.c
)

foreach(SRC_FILE ${FATFS_SRCS})
    set(FATFS_SRC_FILE SRC_FILE -NOTFOUND)
    find_file(FATFS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/FatFS_Source/source

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${FATFS_SRC_FILE}") # debug helper
    list(APPEND FATFS_SOURCES ${FATFS_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(FATFS DEFAULT_MSG FATFS_INCLUDE_DIRS FATFS_SOURCES)
