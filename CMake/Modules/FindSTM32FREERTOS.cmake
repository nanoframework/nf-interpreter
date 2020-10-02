#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

if(NOT STM32_CHIP_TYPE)
    STM32_GET_CHIP_TYPE(${TARGET_CHIP} STM32_CHIP_TYPE)
    if(NOT STM32_CHIP_TYPE)
        message(FATAL_ERROR "Unknown chip: ${TARGET_CHIP}.")
    endif()
    message(STATUS "${TARGET_CHIP} is ${STM32_CHIP_TYPE} device")
endif()
string(TOLOWER ${STM32_CHIP_TYPE} STM32_CHIP_TYPE_LOWER)

# find path of STM32 CMSIS FreeRTOS port for include files
find_path(STM32CMSISFREERTOS_INCLUDE_DIR cmsis_os.h
    PATH_SUFFIXES CMSIS_RTOS
    HINTS ${STM32Cube_DIR}/Middlewares/Third_Party/FreeRTOS/Source
    CMAKE_FIND_ROOT_PATH_BOTH
)

# set path search for FreeRTOS distribution
set(FREERTOS_HEADERS
    croutine.h
    deprecated_definitions.h
    event_groups.h
    FreeRTOS.h
    list.h
    mpu_prototypes.h
    mpu_wrappers.h
    portable.h
    projdefs.h
    queue.h
    semphr.h
    StackMacros.h
    task.h
    timers.h
)

# build path hints according to device series
if(STM32_SERIES STREQUAL "F0")
    set(FREERTOS_PATH_SUFFIXES ARM_CM0)
elseif(STM32_SERIES STREQUAL "F1")
    set(FREERTOS_PATH_SUFFIXES ARM_CM0)
elseif(STM32_SERIES STREQUAL "F2")
    set(FREERTOS_PATH_SUFFIXES ARM_CM0)
elseif(STM32_SERIES STREQUAL "F3")
    set(FREERTOS_PATH_SUFFIXES ARM_CM3)
elseif(STM32_SERIES STREQUAL "F4")
    set(FREERTOS_PATH_SUFFIXES ARM_CM4F)
elseif(STM32_SERIES STREQUAL "F7")
    set(FREERTOS_PATH_SUFFIXES ARM_CM7)
elseif(STM32_SERIES STREQUAL "L0")
    set(FREERTOS_PATH_SUFFIXES ARM_CM0)
elseif(STM32_SERIES STREQUAL "L1")
    set(FREERTOS_PATH_SUFFIXES ARM_CM0)
elseif(STM32_SERIES STREQUAL "L4")
    set(FREERTOS_PATH_SUFFIXES ARM_CM4F)
endif()

find_path(FREERTOS_INCLUDE_DIR ${FREERTOS_HEADERS}
    PATH_SUFFIXES FreeRTOS_Source Include
    PATHS ${PROJECT_BINARY_DIR}/FreeRTOS_Source
    CMAKE_FIND_ROOT_PATH_BOTH
)

find_path(FREERTOS_PORTABLE_INCLUDE_DIR portmacro.h
    PATHS ${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/GCC/${FREERTOS_PATH_SUFFIXES}
    CMAKE_FIND_ROOT_PATH_BOTH
)

set(STM32FREERTOS_INCLUDE_DIR
    ${STM32CMSISFREERTOS_INCLUDE_DIR}
    ${FREERTOS_INCLUDE_DIR}
    ${FREERTOS_PORTABLE_INCLUDE_DIR}
)

set(SRC_FILE_FOUND SRC_FILE-NOTFOUND)
find_file(SRC_FILE_FOUND cmsis_os.c
    #PATH_SUFFIXES CMSIS_RTOS
    PATHS ${STM32Cube_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS 
    CMAKE_FIND_ROOT_PATH_BOTH
)
list(APPEND STM32FREERTOS_SOURCES ${SRC_FILE_FOUND})

set(FREERTOS_SOURCES
    croutine.c
    event_groups.c
    list.c
    queue.c
    tasks.c
    timers.c
    port.c

    # choose here the appropriate memory management for FreeRTOS (see http://www.freertos.org/a00111.html)
    # heap_1.c
    # heap_2.c
    # heap_3.c
    heap_4.c
    #heap_5.c
)

foreach(SRC_FILE ${FREERTOS_SOURCES})
    set(FREERTOS_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(FREERTOS_SRC_FILE ${SRC_FILE}
        PATH_SUFFIXES FreeRTOS_Source Include ${FREERTOS_PATH_SUFFIXES}
        PATHS ${PROJECT_BINARY_DIR} ${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/MemMang ${PROJECT_BINARY_DIR}/FreeRTOS_Source/portable/GCC
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    list(APPEND STM32FREERTOS_SOURCES ${FREERTOS_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32FREERTOS DEFAULT_MSG STM32FREERTOS_INCLUDE_DIR STM32FREERTOS_SOURCES)
