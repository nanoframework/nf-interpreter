#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(freertos)

list(APPEND FreeRTOS_INCLUDE_DIRS ${TARGET_BASE_LOCATION})

# List of the FreeRTOS generic iMXRTxx startup and CMSIS files.

set(FreeRTOS_PORT_SRCS
    heap_4.c

    port.c
)

foreach(SRC_FILE ${FreeRTOS_PORT_SRCS})

    set(FreeRTOS_IMXRT_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(FreeRTOS_IMXRT_SRC_FILE ${SRC_FILE}
        PATHS 

            ${freertos_SOURCE_DIR}/portable/MemMang
            ${freertos_SOURCE_DIR}/portable/GCC/ARM_CM7/r0p1
 

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${FreeRTOS_IMXRT_SRC_FILE}")
    endif()

    list(APPEND FreeRTOS_SOURCES ${FreeRTOS_IMXRT_SRC_FILE})
    
endforeach()

