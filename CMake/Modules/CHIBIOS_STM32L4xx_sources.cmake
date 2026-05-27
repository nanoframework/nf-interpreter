#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(chibios)

# from startup_stm32L4xx.mk
# List of the ChibiOS generic STM32L4xx startup and CMSIS files.

################################################################################################
# WHEN ADDING A NEW BOARD add the source code file for the hal/ports/STM32/STM32L4xx/platform.mk
################################################################################################

set(CHIBIOS_PORT_SRCS
   
       # RT
       chcore.c
       chcoreasm.S
)

foreach(SRC_FILE ${CHIBIOS_PORT_SRCS})

    set(CHIBIOS_L4_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(CHIBIOS_L4_SRC_FILE ${SRC_FILE}
        PATHS 

            ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M/compilers/GCC
            ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${CHIBIOS_L4_SRC_FILE}")
    endif()

    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_L4_SRC_FILE})

endforeach()

list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M)
