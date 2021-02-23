#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(stm32l4_cubepackage)

# set include directories
list(APPEND STM32L4_CubePackage_INCLUDE_DIRS ${stm32l4_cubepackage_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32L4xx/Include)
list(APPEND STM32L4_CubePackage_INCLUDE_DIRS ${stm32l4_cubepackage_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Inc)

# source files
set(STM32L4_CubePackage_SRCS

    # add HAL files here as required
    
    # SPIFFS
    stm32l4xx_hal_qspi.c
)

foreach(SRC_FILE ${STM32L4_CubePackage_SRCS})

    set(STM32L4_CubePackage_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(STM32L4_CubePackage_SRC_FILE ${SRC_FILE}
        PATHS 

        ${stm32l4_cubepackage_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${STM32L4_CubePackage_SRC_FILE}")
    endif()

    list(APPEND STM32L4_CubePackage_SOURCES ${STM32L4_CubePackage_SRC_FILE})
    
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32L4_CubePackage DEFAULT_MSG STM32L4_CubePackage_INCLUDE_DIRS STM32L4_CubePackage_SOURCES)
