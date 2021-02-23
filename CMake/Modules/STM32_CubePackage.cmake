#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)

# include the STM32 Cube package for the appropriate series, if requested

# check if cube package source was specified or if it's empty (default is empty)
macro(ProcessSTM32CubePackage)

    # process target series, which is in the format "STM32F4xx"
    string(REPLACE "STM32" "" TARGET_SERIES_SHORT_1 "${TARGET_SERIES}")
    string(REPLACE "xx" "" TARGET_SERIES_SHORT "${TARGET_SERIES_SHORT_1}")

    set(NO_STM32_CUBE_PACKAGE_SOURCE TRUE)
    if(NO_STM32_CUBE_PACKAGE_SOURCE)
        if(NOT "${STM32_CUBE_PACKAGE_SOURCE}" STREQUAL "")
            set(NO_STM32_CUBE_PACKAGE_SOURCE FALSE)
        endif()
    endif()

    # store the package name for later use
    set(TARGET_STM32_CUBE_PACKAGE STM32${TARGET_SERIES_SHORT} CACHE INTERNAL "name for STM32 Cube package")

    if(NO_STM32_CUBE_PACKAGE_SOURCE)
        # no STM Cube package source specified, download it from nanoFramework fork
        message(STATUS "STM32${TARGET_SERIES_SHORT} Cube package from GitHub repo")

        FetchContent_Declare(
            stm32${TARGET_SERIES_SHORT}_cubepackage
            GIT_REPOSITORY https://github.com/nanoframework/STM32Cube${TARGET_SERIES_SHORT}
            GIT_TAG nf-build
        )

    else()
        # STM32 Cube package source was specified

        # sanity check is source path exists
        if(EXISTS "${STM32_CUBE_PACKAGE_SOURCE}/")
            message(STATUS "STM32 Cube package source from: ${STM32_CUBE_PACKAGE_SOURCE}")

            FetchContent_Declare(
                stm32${TARGET_SERIES_SHORT}_cubepackage
                GIT_REPOSITORY ${STM32_CUBE_PACKAGE_SOURCE}
                GIT_TAG nf-build
            )
    
        else()
            message(FATAL_ERROR "Couldn't find STM32 Cube package source at ${STM32_CUBE_PACKAGE_SOURCE}/")
        endif()

    endif()

    FetchContent_GetProperties(stm32${TARGET_SERIES_SHORT}_cubepackage)
    FetchContent_Populate(stm32${TARGET_SERIES_SHORT}_cubepackage)

endmacro()
