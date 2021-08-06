#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(chibios)

# from os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32h7xx.mk
# List of the ChibiOS generic STM32H7xx startup and CMSIS files.

################################################################################################
# WHEN ADDING A NEW BOARD add the source code file for the hal/ports/STM32/STM32H7xx/platform.mk
################################################################################################

set(CHIBIOS_PORT_SRCS

    # RT
    chcore.c
    chcoreasm.S
)

foreach(SRC_FILE ${CHIBIOS_PORT_SRCS})

    set(CHIBIOS_H7_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(CHIBIOS_H7_SRC_FILE ${SRC_FILE}
        PATHS 

            ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M/compilers/GCC
            ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${CHIBIOS_H7_SRC_FILE}")
    endif()

    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_H7_SRC_FILE})

endforeach()

list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M)
