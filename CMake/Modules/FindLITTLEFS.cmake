#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(littlefs)

# List of the required include paths
list(APPEND littlefs_INCLUDE_DIRS
    ${littlefs_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_littlefs)

set(src_LITTLEFS

    lfs.c
    lfs_util.c

    # HAL implementation specific to a platform
    # has to be included at platform level
    hal_littlefs.c

    target_littlefs.c
)

foreach(SRC_FILE ${src_LITTLEFS})

    set(LITTLEFS_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(LITTLEFS_SRC_FILE ${SRC_FILE}
        PATHS

            ${littlefs_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_littlefs
            ${TARGET_BASE_LOCATION}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${LITTLEFS_SRC_FILE}")
    endif()

    list(APPEND littlefs_SOURCES ${LITTLEFS_SRC_FILE})
    
endforeach()

set_source_files_properties(${littlefs_SOURCE_DIR}/lfs.c PROPERTIES COMPILE_FLAGS -Wno-shadow)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(littlefs DEFAULT_MSG littlefs_INCLUDE_DIRS littlefs_SOURCES)
