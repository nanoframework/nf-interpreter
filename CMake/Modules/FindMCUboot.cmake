#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# FindMCUboot.cmake — FetchContent declaration for MCUboot upstream.
#
# This module downloads the MCUboot source tree at configure time using CMake
# FetchContent.  Only the porting layer header include paths and the nanoFramework
# port sources are compiled; MCUboot's own bootloader application is built as an
# independent CMake target (MCUboot/CMakeLists.txt), not as part of nanoCLR.
#
# Usage in a target CMakeLists.txt (when NF_FEATURE_USE_MCUBOOT is ON):
#   find_package(MCUboot REQUIRED)
#
# To use a local checkout instead of downloading (e.g. for offline builds or
# local development), pass FETCHCONTENT_SOURCE_DIR_MCUBOOT at configure time:
#   cmake -DFETCHCONTENT_SOURCE_DIR_MCUBOOT=/path/to/local/mcuboot ...
#
# This follows the existing nf-interpreter FetchContent conventions
# (see FindChibiOS.cmake, FindNF_LittleFS.cmake, FindFATFS.cmake).

include(FetchContent)

FetchContent_Declare(
    mcuboot
    GIT_REPOSITORY https://github.com/mcu-tools/mcuboot.git
    GIT_TAG        v2.1.0
    GIT_SHALLOW    ON
)

FetchContent_GetProperties(mcuboot)

if(NOT mcuboot_POPULATED)
    FetchContent_MakeAvailable(mcuboot)
endif()

# MCUboot upstream include paths required by the porting layer.
# These are the standard MCUboot header locations within the upstream tree.
list(APPEND MCUboot_INCLUDE_DIRS
    ${mcuboot_SOURCE_DIR}/boot/bootutil/include
    ${mcuboot_SOURCE_DIR}/ext/mbedtls-asn1/include
    ${CMAKE_SOURCE_DIR}/MCUboot/include
)

# Platform port source files — discovered using find_file() so that:
#   a) BUILD_VERBOSE prints the resolved path for debugging (same pattern as FindLITTLEFS.cmake).
#   b) A missing file produces a clear CMake error rather than a silent linker failure.
#
# Convention: each MCUboot-enabled board must provide
#   <board>/common/mcuboot_flash_map.c
# implementing the flash_area_* porting layer declared in flash_map_backend/flash_map_backend.h.
# The file is located under ${TARGET_BASE_LOCATION}/common/ regardless of RTOS.

set(src_MCUBOOT_PORT
    mcuboot_flash_map.c
)

foreach(SRC_FILE ${src_MCUBOOT_PORT})

    set(MCUBOOT_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(MCUBOOT_SRC_FILE ${SRC_FILE}
        PATHS
            ${TARGET_BASE_LOCATION}/common

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if(BUILD_VERBOSE)
        message("${SRC_FILE} >> ${MCUBOOT_SRC_FILE}")
    endif()

    list(APPEND MCUboot_SOURCES ${MCUBOOT_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(MCUboot DEFAULT_MSG MCUboot_INCLUDE_DIRS MCUboot_SOURCES)
