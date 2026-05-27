#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(stm32f0_hal_driver)
FetchContent_GetProperties(cmsis_device_f0)
FetchContent_GetProperties(cmsis_core)

# set include directories
list(APPEND STM32F0_CubePackage_INCLUDE_DIRS ${cmsis_device_f0_SOURCE_DIR}/Include)
list(APPEND STM32F0_CubePackage_INCLUDE_DIRS ${stm32f0_hal_driver_SOURCE_DIR}/Inc)
list(APPEND STM32F0_CubePackage_INCLUDE_DIRS ${cmsis_core_SOURCE_DIR}/Include)
list(APPEND STM32F0_CubePackage_INCLUDE_DIRS ${TARGET_BASE_LOCATION})

# source files
set(STM32F0_CubePackage_SRCS

    # add HAL files here as required
)

foreach(SRC_FILE ${STM32F0_CubePackage_SRCS})

    set(STM32F0_CubePackage_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(STM32F0_CubePackage_SRC_FILE ${SRC_FILE}
        PATHS 

        ${stm32f0_hal_driver_SOURCE_DIR}/Drivers/STM32F0xx_HAL_Driver/Src

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${STM32F0_CubePackage_SRC_FILE}")
    endif()

    list(APPEND STM32F0_CubePackage_SOURCES ${STM32F0_CubePackage_SRC_FILE})

endforeach()

list(REMOVE_DUPLICATES STM32F0_CubePackage_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32F0_CubePackage DEFAULT_MSG STM32F0_CubePackage_INCLUDE_DIRS STM32F0_CubePackage_SOURCES)
