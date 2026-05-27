#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(chibios)

# List of the ChibiOS generic RP2040 startup source files.
# RP2040 uses ARM Cortex-M0+ (ARMv6-M architecture)

set(CHIBIOS_PORT_SRCS

    # RT port for ARMv6-M
    chcore.c
    chcoreasm.S
)

foreach(SRC_FILE ${CHIBIOS_PORT_SRCS})

    set(CHIBIOS_RP_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(CHIBIOS_RP_SRC_FILE ${SRC_FILE}
        PATHS 
            ${chibios_SOURCE_DIR}/os/common/ports/ARMv6-M/compilers/GCC
            ${chibios_SOURCE_DIR}/os/common/ports/ARMv6-M
            ${chibios_SOURCE_DIR}/os/hal/ports/common/ARMCMx

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${CHIBIOS_RP_SRC_FILE}")
    endif()

    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_RP_SRC_FILE})

endforeach()

list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv6-M)
