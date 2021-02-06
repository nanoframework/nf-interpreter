#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# List of the FreeRTOS generic iMXRTxx startup and CMSIS files.

set(FreeRTOS_PORT_SRCS
    heap_4.c

    port.c
)

foreach(SRC_FILE ${FreeRTOS_PORT_SRCS})
    set(FreeRTOS_IMXRT_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(FreeRTOS_IMXRT_SRC_FILE ${SRC_FILE}
        PATHS 

            ${CMAKE_BINARY_DIR}/FreeRTOS_Source/lib/FreeRTOS/portable/MemMang
            ${CMAKE_BINARY_DIR}/FreeRTOS_Source/lib/FreeRTOS/portable/GCC/ARM_CM7/r0p1
 

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${FreeRTOS_IMXRT_SRC_FILE}") # debug helper
    list(APPEND FreeRTOS_SOURCES ${FreeRTOS_IMXRT_SRC_FILE})
endforeach()

