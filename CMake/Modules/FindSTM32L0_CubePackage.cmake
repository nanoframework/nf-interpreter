#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(stm32l0_cubepackage)

# set include directories
list(APPEND STM32L0_CubePackage_INCLUDE_DIRS ${stm32l0_cubepackage_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32L0xx/Include)
list(APPEND STM32L0_CubePackage_INCLUDE_DIRS ${stm32l0_cubepackage_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Inc)

# source files
set(STM32L0_CubePackage_SRCS

    # add HAL files here as required
)

foreach(SRC_FILE ${STM32L0_CubePackage_SRCS})

    set(STM32L0_CubePackage_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(STM32L0_CubePackage_SRC_FILE ${SRC_FILE}
        PATHS 

        ${stm32l0_cubepackage_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${STM32L0_CubePackage_SRC_FILE}")
    endif()

    list(APPEND STM32L0_CubePackage_SOURCES ${STM32L0_CubePackage_SRC_FILE})

endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32L0_CubePackage DEFAULT_MSG STM32L0_CubePackage_INCLUDE_DIRS STM32L0_CubePackage_SOURCES)
