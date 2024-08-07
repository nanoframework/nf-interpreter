#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)

# include the STM32 HAL and CMSIS for the appropriate series, along with general CMSIS, if requested

# check if cube package source was specified or if it's empty (default is empty)
macro(ProcessSTM32CubePackage)

    # process target series, which is in the format "STM32F4xx"
    string(REPLACE "STM32" "" TARGET_SERIES_SHORT_1 "${TARGET_SERIES}")
    string(REPLACE "xx" "" TARGET_SERIES_SHORT "${TARGET_SERIES_SHORT_1}")
    string(TOLOWER "${TARGET_SERIES_SHORT}" TARGET_SERIES_SHORT_LOWER)

    # store the series name for later use
    set(TARGET_SERIES_SHORT_LOWER ${TARGET_SERIES_SHORT_LOWER} CACHE INTERNAL "STM32 series short name lower canse")

    # store the package name for later use
    set(TARGET_STM32_CUBE_PACKAGE STM32${TARGET_SERIES_SHORT} CACHE INTERNAL "name for STM32 Cube package")

    if("${TARGET_SERIES}" STREQUAL "STM32F0xx")
        # WHEN CHANGING THESE MAKE SURE TO UPDATE THE DEV CONTAINERS
        set(HAL_DRIVER_GIT_TAG v1.7.8)
        set(CMSIS_DEVICE_GIT_TAG v2.3.7)
    elseif("${TARGET_SERIES}" STREQUAL "STM32F4xx")
        # WHEN CHANGING THESE MAKE SURE TO UPDATE THE DEV CONTAINERS
        set(HAL_DRIVER_GIT_TAG v1.8.2)
        set(CMSIS_DEVICE_GIT_TAG v2.6.9)
    elseif("${TARGET_SERIES}" STREQUAL "STM32F7xx")
        # WHEN CHANGING THESE MAKE SURE TO UPDATE THE DEV CONTAINERS
        set(HAL_DRIVER_GIT_TAG v1.3.1)
        set(CMSIS_DEVICE_GIT_TAG v1.2.9)
    elseif("${TARGET_SERIES}" STREQUAL "STM32H7xx")
        # WHEN CHANGING THESE MAKE SURE TO UPDATE THE DEV CONTAINERS
        set(HAL_DRIVER_GIT_TAG v1.11.3)
        set(CMSIS_DEVICE_GIT_TAG v1.10.4)
    elseif("${TARGET_SERIES}" STREQUAL "STM32L0xx")
        # WHEN CHANGING THESE MAKE SURE TO UPDATE THE DEV CONTAINERS
        set(HAL_DRIVER_GIT_TAG v1.10.6)
        set(CMSIS_DEVICE_GIT_TAG v1.9.3)
    elseif("${TARGET_SERIES}" STREQUAL "STM32L4xx")
        # WHEN CHANGING THESE MAKE SURE TO UPDATE THE DEV CONTAINERS
        set(HAL_DRIVER_GIT_TAG v1.13.4)
        set(CMSIS_DEVICE_GIT_TAG v1.7.3)
    endif()

    set(NO_STM32_HAL_DRIVER_SOURCE TRUE)
    if(NO_STM32_HAL_DRIVER_SOURCE)
        if(NOT "${STM32_HAL_DRIVER_SOURCE}" STREQUAL "")
            set(NO_STM32_HAL_DRIVER_SOURCE FALSE)
        endif()
    endif()

    if(NO_STM32_HAL_DRIVER_SOURCE)
        # no STM HAL driver source specified, download it from nanoFramework fork
        message(STATUS "STM32${TARGET_SERIES_SHORT} HAL driver from GitHub repo")

        FetchContent_Declare(
            stm32${TARGET_SERIES_SHORT_LOWER}_hal_driver
            GIT_REPOSITORY https://github.com/STMicroelectronics/stm32${TARGET_SERIES_SHORT_LOWER}xx_hal_driver
            GIT_TAG ${HAL_DRIVER_GIT_TAG}
        )

    else()
        # STM32  HAL driver source was specified

        # sanity check is source path exists
        if(EXISTS "${STM32_HAL_DRIVER_SOURCE}/")
            message(STATUS "STM32 HAL driver source from: ${STM32_HAL_DRIVER_SOURCE}")

            FetchContent_Declare(
                stm32${TARGET_SERIES_SHORT_LOWER}_hal_driver
                SOURCE_DIR ${STM32_HAL_DRIVER_SOURCE}
            )
    
        else()
            message(FATAL_ERROR "Couldn't find STM32 HAL driver source at ${STM32_HAL_DRIVER_SOURCE}/")
        endif()

    endif()

    set(NO_STM32_CMSIS_DEVICE_SOURCE TRUE)
    if(NO_STM32_CMSIS_DEVICE_SOURCE)
        if(NOT "${STM32_CMSIS_DEVICE_SOURCE}" STREQUAL "")
            set(NO_STM32_CMSIS_DEVICE_SOURCE FALSE)
        endif()
    endif()

    if(NO_STM32_CMSIS_DEVICE_SOURCE)
        # no STM Device CMSIS source specified, download it from nanoFramework fork
        message(STATUS "STM32${TARGET_SERIES_SHORT} Device CMSIS from GitHub repo")

        FetchContent_Declare(
            cmsis_device_${TARGET_SERIES_SHORT_LOWER}
            GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_device_${TARGET_SERIES_SHORT_LOWER}
            GIT_TAG ${CMSIS_DEVICE_GIT_TAG}
        )

    else()
        # STM32 HAL driver source was specified

        # sanity check is source path exists
        if(EXISTS "${STM32_CMSIS_DEVICE_SOURCE}/")
            message(STATUS "STM32 Device CMSIS source from: ${STM32_CMSIS_DEVICE_SOURCE}")

            FetchContent_Declare(
                cmsis_device_${TARGET_SERIES_SHORT_LOWER}
                SOURCE_DIR ${STM32_CMSIS_DEVICE_SOURCE}
            )

        else()
            message(FATAL_ERROR "Couldn't find STM32 HAL driver source at ${STM32_CMSIS_DEVICE_SOURCE}/")
        endif()

    endif()

    set(NO_STM32_CMSIS_CORE_SOURCE TRUE)
    if(NO_STM32_CMSIS_CORE_SOURCE)
        if(NOT "${STM32_CMSIS_CORE_SOURCE}" STREQUAL "")
            set(NO_STM32_CMSIS_CORE_SOURCE FALSE)
        endif()
    endif()

    if(NO_STM32_CMSIS_CORE_SOURCE)
        # no STM CMSIS Core source specified, download it from nanoFramework fork
        message(STATUS "STM32${TARGET_SERIES_SHORT} CMSIS Core from GitHub repo")
                
        FetchContent_Declare(
            cmsis_core
            GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_core
            GIT_TAG v5.6.0
        )       

    else()
        # STM32 CMSIS Core source was specified

        # sanity check is source path exists
        if(EXISTS "${STM32_CMSIS_CORE_SOURCE}/")
            message(STATUS "STM32 CMSIS Core source from: ${STM32_CMSIS_CORE_SOURCE}")

            FetchContent_Declare(
                cmsis_core
                SOURCE_DIR ${STM32_CMSIS_CORE_SOURCE}
            )

        else()
            message(FATAL_ERROR "Couldn't find STM32 CMSIS Core source at ${STM32_CMSIS_CORE_SOURCE}/")
        endif()

    endif()

    FetchContent_GetProperties(stm32${TARGET_SERIES_SHORT_LOWER}_hal_driver)
    FetchContent_MakeAvailable(stm32${TARGET_SERIES_SHORT_LOWER}_hal_driver)

    FetchContent_GetProperties(cmsis_device_${TARGET_SERIES_SHORT_LOWER})
    FetchContent_MakeAvailable(cmsis_device_${TARGET_SERIES_SHORT_LOWER})
    
    FetchContent_GetProperties(cmsis_core)
    FetchContent_MakeAvailable(cmsis_core)

endmacro()

# macro to be called from binutils to add ST Cube package as a library
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
macro(nf_add_stm32_cube)
   
    # parse arguments
    cmake_parse_arguments(NFSTMPKG "" "BUILD_TARGET" "EXTRA_INCLUDES;EXTRA_COMPILE_DEFINITIONS" ${ARGN})
    
    if("${NFSTMPKG_BUILD_TARGET}" STREQUAL "${NANOBOOTER_PROJECT_NAME}")
        set(CONFIG_FILES_PATH "${TARGET_BASE_LOCATION}/nanoBooter")
    elseif("${NFSTMPKG_BUILD_TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")
        set(CONFIG_FILES_PATH "${TARGET_BASE_LOCATION}/nanoCLR")
    endif()

    # add THESE has a library
    set(LIB_NAME stm32${TARGET_SERIES_SHORT_LOWER}_hal_driver_${NFSTMPKG_BUILD_TARGET})

    add_library(
        ${LIB_NAME} STATIC 
            ${${TARGET_STM32_CUBE_PACKAGE}_CubePackage_SOURCES}
    )

    target_include_directories(
        ${LIB_NAME} 
        PUBLIC 
            "${${TARGET_STM32_CUBE_PACKAGE}_CubePackage_INCLUDE_DIRS}"
            ${NFSTMPKG_EXTRA_INCLUDES}
    )

    nf_set_compile_options(TARGET ${LIB_NAME})
    nf_set_compile_definitions(TARGET ${LIB_NAME} EXTRA_COMPILE_DEFINITIONS ${NFSTMPKG_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NFSTMPKG_BUILD_TARGET})
    nf_set_link_options(TARGET ${LIB_NAME})

    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})
    
endmacro()
