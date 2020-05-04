#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

list(APPEND FREERTOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/GCC/ARM_CM0)


# List of the FreeRTOS generic STM32L0xx startup and CMSIS files.

set(FREERTOS_PORT_SRCS

    heap_4.c
    port.c
)

foreach(SRC_FILE ${FREERTOS_PORT_SRCS})
    set(FREERTOS_STM32L0xx_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(FREERTOS_STM32L0xx_SRC_FILE ${SRC_FILE}
        PATHS 

        ${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/MemMang
        ${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/GCC/ARM_CM0
 

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${FREERTOS_STM32L0xx_SRC_FILE}") # debug helper
    list(APPEND FREERTOS_SOURCES ${FREERTOS_STM32L0xx_SRC_FILE})
endforeach()
