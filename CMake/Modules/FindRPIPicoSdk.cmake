#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

list(APPEND RPI_PICO_SKD_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/RPIPicoSdk_Source/src\common\boot_picoboot\include\boot)
list(APPEND RPI_PICO_SKD_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/RPIPicoSdk_Source/src\common\boot_uf2\include\boot\uf2.h)

# source files for Raspberry Pi Pico SDK
set(RPI_PICO_SKD_SRCS

)

foreach(SRC_FILE ${RPI_PICO_SKD_SRCS})
    set(RPI_PICO_SKD_SRC_FILE SRC_FILE -NOTFOUND)
    find_file(RPI_PICO_SKD_SRC_FILE ${SRC_FILE}
        PATHS

            ${PROJECT_BINARY_DIR}/RPIPicoSdk_Source/common/src

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${RPI_PICO_SKD_SRC_FILE}")
    endif()

    list(APPEND RPI_PICO_SKD_SOURCES ${RPI_PICO_SKD_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)


FIND_PACKAGE_HANDLE_STANDARD_ARGS(RPIPicoSdk DEFAULT_MSG RPI_PICO_SKD_INCLUDE_DIRS RPI_PICO_SKD_SOURCES)
