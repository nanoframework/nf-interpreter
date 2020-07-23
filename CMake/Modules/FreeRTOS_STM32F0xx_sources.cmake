#
# Copyright (c) 2020 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

list(APPEND FreeRTOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/GCC/ARM_CM0)


# List of the FreeRTOS generic STM32F0xx startup and CMSIS files.

set(FreeRTOS_PORT_SRCS

    heap_4.c
    port.c
)

foreach(SRC_FILE ${FreeRTOS_PORT_SRCS})
    set(FreeRTOS_STM32F0xx_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(FreeRTOS_STM32F0xx_SRC_FILE ${SRC_FILE}
        PATHS 

        ${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/MemMang
        ${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/GCC/ARM_CM0
 

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${FreeRTOS_STM32F0xx_SRC_FILE}") # debug helper
    list(APPEND FreeRTOS_SOURCES ${FreeRTOS_STM32F0xx_SRC_FILE})
endforeach()
