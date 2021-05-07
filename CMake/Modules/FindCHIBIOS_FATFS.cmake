#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(fatfs)
FetchContent_GetProperties(chibios)
FetchContent_GetProperties(chibios-contrib)

# List of the required FatFs include files.
list(APPEND CHIBIOS_FATFS_INCLUDE_DIRS ${fatfs_SOURCE_DIR}/source)


set(FATFS_SRCS
    # bindings
    fatfs_syscall.c

    # fatfs
    ff.c
    ffunicode.c
)

foreach(SRC_FILE ${FATFS_SRCS})

    set(FATFS_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(FATFS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${chibios_SOURCE_DIR}/os/various/fatfs_bindings
            ${fatfs_SOURCE_DIR}/source

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${FATFS_SRC_FILE}")
    endif()

    list(APPEND CHIBIOS_FATFS_SOURCES ${FATFS_SRC_FILE})

endforeach()

# fatfs_diskio is hacked because of USB Host, so we need to use the source from the appropriate location
if(NF_FEATURE_HAS_USB_MSD)
    # get it from ChibiOS contribution
    list(APPEND CHIBIOS_FATFS_SOURCES ${chibios-contrib_SOURCE_DIR}/os/various/fatfs_bindings/fatfs_diskio.c)
else()
    # get it from the targets directory for fixing STM32F7 series Cache issues
    list(APPEND CHIBIOS_FATFS_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_FatFS/fatfs_diskio.c)
endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CHIBIOS_FATFS DEFAULT_MSG CHIBIOS_FATFS_INCLUDE_DIRS CHIBIOS_FATFS_SOURCES)
