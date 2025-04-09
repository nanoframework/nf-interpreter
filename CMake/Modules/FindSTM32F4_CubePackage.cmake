#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(stm32f4_hal_driver)
FetchContent_GetProperties(cmsis_device_f4)
FetchContent_GetProperties(cmsis_core)

# set include directories
list(APPEND STM32F4_CubePackage_INCLUDE_DIRS ${cmsis_device_f4_SOURCE_DIR}/Include)
list(APPEND STM32F4_CubePackage_INCLUDE_DIRS ${stm32f4_hal_driver_SOURCE_DIR}/Inc)
list(APPEND STM32F4_CubePackage_INCLUDE_DIRS ${cmsis_core_SOURCE_DIR}/Include)
list(APPEND STM32F4_CubePackage_INCLUDE_DIRS ${TARGET_BASE_LOCATION})

# source files
set(STM32F4_CubePackage_SRCS

    # add HAL files here as required
    stm32f4xx_hal.c
    stm32f4xx_hal_eth.c

    # littlefs
    stm32f4xx_hal_dma.c
    stm32f4xx_hal_qspi.c
)

# add exception to compiler warnings as errors
SET_SOURCE_FILES_PROPERTIES(${stm32f4_hal_driver_SOURCE_DIR}/Src/stm32f4xx_hal_eth.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)

foreach(SRC_FILE ${STM32F4_CubePackage_SRCS})

    set(STM32F4_CubePackage_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(STM32F4_CubePackage_SRC_FILE ${SRC_FILE}
        PATHS 

        ${stm32f4_hal_driver_SOURCE_DIR}/Src

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${STM32F4_CubePackage_SRC_FILE}")
    endif()

    list(APPEND STM32F4_CubePackage_SOURCES ${STM32F4_CubePackage_SRC_FILE})
    
endforeach()

list(REMOVE_DUPLICATES STM32F4_CubePackage_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32F4_CubePackage DEFAULT_MSG STM32F4_CubePackage_INCLUDE_DIRS STM32F4_CubePackage_SOURCES)
