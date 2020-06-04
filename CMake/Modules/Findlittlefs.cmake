#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#


# List of the required littlefs include files.
list(APPEND LITTLEFS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/littlefs_Source)
list(APPEND LITTLEFS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/littlefs_Source/bd)
list(APPEND LITTLEFS_INCLUDE_DIRS ${TARGET_BASE_LOCATION})

set(LITTLEFS_SRCS

    # littlefs
    lfs.c
    lfs_util.c

    lfs_filebd.c
    lfs_rambd.c
    lfs_testbd.c
)

foreach(SRC_FILE ${LITTLEFS_SRCS})
    set(LITTLEFS_SRC_FILE SRC_FILE -NOTFOUND)
    find_file(LITTLEFS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/littlefs_Source

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${LITTLEFS_SRC_FILE}") # debug helper
    list(APPEND FATFS_SOURCES ${LITTLEFS_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LITTLEFS DEFAULT_MSG LITTLEFS_INCLUDE_DIRS LITTLEFS_SOURCES)
