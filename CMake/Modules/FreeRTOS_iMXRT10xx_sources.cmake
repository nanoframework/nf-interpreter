#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# List of the FreeRTOS generic iMXRTxx startup and CMSIS files.

set(FREERTOS_PORT_SRCS
    heap_4.c

    port.c
)

foreach(SRC_FILE ${FREERTOS_PORT_SRCS})
    set(FREERTOS_IMXRT_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(FREERTOS_IMXRT_SRC_FILE ${SRC_FILE}
        PATHS 

            ${PROJECT_BINARY_DIR}/FreeRTOS_Source/FreeRTOS/Source/portable/MemMang
            ${PROJECT_BINARY_DIR}/FreeRTOS_Source/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1
 

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${FREERTOS_IMXRT_SRC_FILE}") # debug helper
    list(APPEND FREERTOS_SOURCES ${FREERTOS_IMXRT_SRC_FILE})
endforeach()

