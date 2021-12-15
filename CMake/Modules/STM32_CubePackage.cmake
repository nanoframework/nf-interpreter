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
    string(TOLOWER "${TARGET_SERIES_SHORT}" TARGET_SERIES_SHORT_LOWER)

    # store the series name for later use
    set(TARGET_SERIES_SHORT_LOWER ${TARGET_SERIES_SHORT_LOWER} CACHE INTERNAL "STM32 series short name lower canse")

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
            stm32${TARGET_SERIES_SHORT_LOWER}_cubepackage
            # GIT_REPOSITORY https://github.com/nanoframework/STM32Cube${TARGET_SERIES_SHORT}
            # GIT_TAG nf-build

 GIT_REPOSITORY https://github.com/STMicroelectronics/stm32l4xx_hal_driver
 GIT_TAG v1.13.2


        )

        FetchContent_Declare(
            cmsis_device_${TARGET_SERIES_SHORT_LOWER}
            # GIT_REPOSITORY https://github.com/nanoframework/STM32Cube${TARGET_SERIES_SHORT}
            # GIT_TAG nf-build

 GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_device_l4
 GIT_TAG v1.5.1


        )
        

    else()
        # STM32 Cube package source was specified

        # sanity check is source path exists
        if(EXISTS "${STM32_CUBE_PACKAGE_SOURCE}/")
            message(STATUS "STM32 Cube package source from: ${STM32_CUBE_PACKAGE_SOURCE}")

            FetchContent_Declare(
                stm32${TARGET_SERIES_SHORT_LOWER}_cubepackage
                GIT_REPOSITORY ${STM32_CUBE_PACKAGE_SOURCE}
                GIT_TAG nf-build
            )
    
        else()
            message(FATAL_ERROR "Couldn't find STM32 Cube package source at ${STM32_CUBE_PACKAGE_SOURCE}/")
        endif()

    endif()

    FetchContent_GetProperties(stm32${TARGET_SERIES_SHORT_LOWER}_cubepackage)
    FetchContent_Populate(stm32${TARGET_SERIES_SHORT_LOWER}_cubepackage)

    FetchContent_GetProperties(cmsis_device_${TARGET_SERIES_SHORT_LOWER})
    FetchContent_Populate(cmsis_device_${TARGET_SERIES_SHORT_LOWER})

endmacro()


# macro to be called from binutils to add ST Cube package as a library
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
macro(nf_add_stm32_cube)
   
    # parse arguments
    cmake_parse_arguments(NFSTMPKG "" "" "EXTRA_INCLUDES;EXTRA_COMPILE_DEFINITIONS" ${ARGN})

    # add this has a library
    set(LIB_NAME stm32${TARGET_SERIES_SHORT_LOWER}_cube)

    add_library(
        ${LIB_NAME} STATIC 
            ${STM32${TARGET_SERIES_SHORT_LOWER}_CubePackage_SOURCES})

    target_include_directories(
        ${LIB_NAME} 
        PUBLIC 
            ${STM32${TARGET_SERIES_SHORT_LOWER}_CubePackage_INCLUDE_DIRS}
            ${NFSTMPKG_EXTRA_INCLUDES})

    nf_set_compile_options(TARGET ${LIB_NAME} BUILD_TARGET ${NANOCLR_PROJECT_NAME})
    nf_set_compile_definitions(TARGET ${LIB_NAME} EXTRA_COMPILE_DEFINITIONS ${NFSTMPKG_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NANOCLR_PROJECT_NAME})
    nf_set_link_options(TARGET ${LIB_NAME})

    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})
    
endmacro()
