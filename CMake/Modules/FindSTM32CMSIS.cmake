if(NOT STM32_CHIP_TYPE)
    STM32_GET_CHIP_TYPE(${TARGET_CHIP} STM32_CHIP_TYPE)
    if(NOT STM32_CHIP_TYPE)
        message(FATAL_ERROR "Unknown chip: ${TARGET_CHIP}.")
    endif()
    message(STATUS "${TARGET_CHIP} is ${STM32_CHIP_TYPE} device")
endif()
string(TOLOWER ${STM32_CHIP_TYPE} STM32_CHIP_TYPE_LOWER)

set(CMSIS_COMMON_HEADERS
    arm_common_tables.h
    arm_const_structs.h
    arm_math.h
    core_cmFunc.h
    core_cmInstr.h
    core_cmSimd.h
)

if(STM32_SERIES STREQUAL "F1")

    list(APPEND CMSIS_COMMON_HEADERS core_cm3.h)
    set(CMSIS_DEVICE_HEADERS stm32f1xx.h system_stm32f1xx.h)
    set(CMSIS_DEVICE_SOURCES system_stm32f1xx.c)

elseif(STM32_SERIES STREQUAL "F2")

    string(REGEX REPLACE "^(2[01]7).[BCDEFG]" "\\1" STM32_DEVICE_NUM ${STM32_CHIP_TYPE})
    set(CMSIS_STARTUP_SOURCE startup_stm32f${STM32_DEVICE_NUM}xx.s)

    list(APPEND CMSIS_COMMON_HEADERS core_cm4.h)
    set(CMSIS_DEVICE_HEADERS stm32f2xx.h system_stm32f2xx.h)
    set(CMSIS_DEVICE_SOURCES system_stm32f2xx.c)

elseif(STM32_SERIES STREQUAL "F4")

    list(APPEND CMSIS_COMMON_HEADERS core_cm4.h)
    set(CMSIS_DEVICE_HEADERS stm32f4xx.h system_stm32f4xx.h)
    set(CMSIS_DEVICE_SOURCES system_stm32f4xx.c)

elseif(STM32_SERIES STREQUAL "F7")

    list(APPEND CMSIS_COMMON_HEADERS core_cm7.h)
    set(CMSIS_DEVICE_HEADERS stm32f7xx.h system_stm32f7xx.h)
    set(CMSIS_DEVICE_SOURCES system_stm32f7xx.c)

elseif(STM32_SERIES STREQUAL "F0")

    list(APPEND CMSIS_COMMON_HEADERS core_cm0.h)
    set(CMSIS_DEVICE_HEADERS stm32f0xx.h system_stm32f0xx.h)
    set(CMSIS_DEVICE_SOURCES system_stm32f0xx.c)

elseif(STM32_SERIES STREQUAL "L0")

    list(APPEND CMSIS_COMMON_HEADERS core_cm0.h)
    set(CMSIS_DEVICE_HEADERS stm32l0xx.h system_stm32l0xx.h)
    set(CMSIS_DEVICE_SOURCES system_stm32l0xx.c)
    if(NOT CMSIS_STARTUP_SOURCE)
        set(CMSIS_STARTUP_SOURCE startup_stm32l${STM32_CHIP_TYPE_LOWER}.s)
    endif()
endif()

if(NOT CMSIS_STARTUP_SOURCE)
    set(CMSIS_STARTUP_SOURCE startup_stm32f${STM32_CHIP_TYPE_LOWER}.s)
endif()

find_path(CMSIS_COMMON_INCLUDE_DIR ${CMSIS_COMMON_HEADERS}
    PATH_SUFFIXES include stm32${STM32_SERIES_LOWER} cmsis
    HINTS ${STM32Cube_DIR}/Drivers/CMSIS/Include/
    CMAKE_FIND_ROOT_PATH_BOTH
)

find_path(CMSIS_DEVICE_INCLUDE_DIR ${CMSIS_DEVICE_HEADERS}
    PATH_SUFFIXES include stm32${STM32_SERIES_LOWER} cmsis
    HINTS ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32${STM32_SERIES}xx/Include
    CMAKE_FIND_ROOT_PATH_BOTH
)

set(STM32CMSIS_INCLUDE_DIRS
    ${CMSIS_DEVICE_INCLUDE_DIR}
    ${CMSIS_COMMON_INCLUDE_DIR}
)

foreach(SRC ${CMSIS_DEVICE_SOURCES})
    set(SRC_FILE SRC_FILE-NOTFOUND)
    find_file(SRC_FILE ${SRC}
        PATH_SUFFIXES src stm32${STM32_SERIES_LOWER} cmsis
        HINTS ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32${STM32_SERIES}xx/Source/Templates/
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    list(APPEND STM32CMSIS_SOURCES ${SRC_FILE})
endforeach()

if(STM32_CHIP_TYPE)
    set(SRC_FILE SRC_FILE-NOTFOUND)
    find_file(SRC_FILE ${CMSIS_STARTUP_SOURCE}
        PATH_SUFFIXES src stm32${STM32_SERIES_LOWER} cmsis
        HINTS ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32${STM32_SERIES}xx/Source/Templates/gcc/
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    list(APPEND STM32CMSIS_SOURCES ${SRC_FILE})
endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CMSIS DEFAULT_MSG STM32CMSIS_INCLUDE_DIRS STM32CMSIS_SOURCES)
