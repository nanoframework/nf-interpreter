#
# Copyright (c) 2017 The nanoFramework project contributors
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


# build path hints according to device series
if(STM32_SERIES STREQUAL "F0")
    set(RTXRTOS_HAL_NAME HAL_CM0.S)
    set(RTXRTOS_COMPILE_OPTIONS -D__CORTEX_M0)
elseif(STM32_SERIES STREQUAL "F1")
    set(RTXRTOS_HAL_NAME HAL_CM0.S)
    set(RTXRTOS_COMPILE_OPTIONS -D__CORTEX_M0)
elseif(STM32_SERIES STREQUAL "F2")
    set(RTXRTOS_HAL_NAME HAL_CM0.S)
    set(RTXRTOS_COMPILE_OPTIONS -D__CORTEX_M0)
elseif(STM32_SERIES STREQUAL "F3")
    set(RTXRTOS_HAL_NAME HAL_CM3.S)
    set(RTXRTOS_COMPILE_OPTIONS -D__CORTEX_M3)
elseif(STM32_SERIES STREQUAL "F4")
    set(RTXRTOS_HAL_NAME HAL_CM4.S)    
    set(RTXRTOS_COMPILE_OPTIONS -D__CORTEX_M4)
elseif(STM32_SERIES STREQUAL "F7")
    set(RTXRTOS_HAL_NAME HAL_CM4.S)
    set(RTXRTOS_COMPILE_OPTIONS -D__CORTEX_M7)
elseif(STM32_SERIES STREQUAL "L0")
    set(RTXRTOS_HAL_NAME HAL_CM0.S)
    set(RTXRTOS_COMPILE_OPTIONS -D__CORTEX_M0)
elseif(STM32_SERIES STREQUAL "L1")
    set(RTXRTOS_HAL_NAME HAL_CM0.S)
    set(RTXRTOS_COMPILE_OPTIONS -D__CORTEX_M0)
elseif(STM32_SERIES STREQUAL "L4")
    set(RTXRTOS_HAL_NAME HAL_CM4.S)
    set(RTXRTOS_COMPILE_OPTIONS -D__CORTEX_M4)
endif()

# set include directories for RXT RTOS
list(APPEND RTXRTOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/RTXRTOS_Source/CMSIS/RTOS/RTX/INC)
list(APPEND RTXRTOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/RTXRTOS_Source/CMSIS/RTOS/RTX/SRC)

set(RTXRTOS_SRCS
    Mutex.cpp
    RtosTimer.cpp
    Semaphore.cpp
    Thread.cpp

    HAL_CM.c
    rt_CMSIS.c
    rt_Event.c
    rt_List.c
    rt_Mailbox.c
    rt_MemBox.c
    rt_Memory.c
    rt_Mutex.c
    rt_Robin.c
    rt_Semaphore.c
    rt_System.c
    rt_Task.c
    rt_Time.c
    rt_Timer.c
)

foreach(SRC_FILE ${RTXRTOS_SRCS})
    set(RTXRTOS_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(RTXRTOS_SRC_FILE ${SRC_FILE}
        PATHS ${PROJECT_BINARY_DIR}/RTXRTOS_Source/CMSIS/RTOS/RTX/SRC ${PROJECT_BINARY_DIR}/RTXRTOS_Source/CMSIS/RTOS/Template/CPP
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    list(APPEND RTXRTOS_SOURCES ${RTXRTOS_SRC_FILE})
endforeach()

# find path for HAL_CMx file
set(RTXRTOS_SRC_FILE SRC_FILE-NOTFOUND)
find_file(RTXRTOS_SRC_FILE ${RTXRTOS_HAL_NAME}
    PATHS PATHS ${PROJECT_BINARY_DIR}/RTXRTOS_Source/CMSIS/RTOS/RTX/SRC/GCC
    CMAKE_FIND_ROOT_PATH_BOTH
)
list(APPEND RTXRTOS_SOURCES ${RTXRTOS_SRC_FILE})

# find path for SVC_Table file
set(RTXRTOS_SRC_FILE SRC_FILE-NOTFOUND)
find_file(RTXRTOS_SRC_FILE SVC_Table.S
    PATHS PATHS ${PROJECT_BINARY_DIR}/RTXRTOS_Source/CMSIS/RTOS/RTX/SRC/GCC
    CMAKE_FIND_ROOT_PATH_BOTH
)
list(APPEND RTXRTOS_SOURCES ${RTXRTOS_SRC_FILE})

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(RTXRTOS DEFAULT_MSG RTXRTOS_INCLUDE_DIRS RTXRTOS_SOURCES)
