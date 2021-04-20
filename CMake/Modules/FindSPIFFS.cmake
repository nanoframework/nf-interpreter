#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(spiffs)

# List of the required include paths
list(APPEND SPIFFS_INCLUDE_DIRS ${spiffs_SOURCE_DIR}/src)

set(src_spiffs

    spiffs_nucleus.c
    spiffs_gc.c
    spiffs_hydrogen.c
    spiffs_cache.c
    spiffs_check.c

    # HAL implementation specific to a platform
    # has to be included at platform level
    # hal_spiffs.c

    target_spiffs.c
)

foreach(SRC_FILE ${src_spiffs})

    set(SPIFFS_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(SPIFFS_SRC_FILE ${SRC_FILE}
        PATHS

            ${spiffs_SOURCE_DIR}/src
            ${TARGET_BASE_LOCATION}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${SPIFFS_SRC_FILE}")
    endif()

    list(APPEND SPIFFS_SOURCES ${SPIFFS_SRC_FILE})
    
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SPIFFS DEFAULT_MSG SPIFFS_INCLUDE_DIRS SPIFFS_SOURCES)
