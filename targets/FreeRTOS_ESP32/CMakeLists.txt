#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(toolchain.xtensa-esp32-elf)
include(binutils.ESP32)
include(binutils.common)

# Set target series in lower case
nf_set_esp32_target_series()

# Define PLATFORM base path
set(BASE_PATH_FOR_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# ESP32 targets use Espressif IDF and ESP32 FreeRTOS

# check if ESP32_IDF_SOURCE was specified or if it's empty (default is empty)
set(NO_ESP32_IDF_SOURCE TRUE)
if(ESP32_IDF_SOURCE)
    if(NOT "${ESP32_IDF_SOURCE}" STREQUAL "")
        set(NO_ESP32_IDF_SOURCE FALSE)
    endif()
endif()

# WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
set(ESP32_IDF_TAG "4.3" CACHE INTERNAL "ESP32 IDF tag")

if(NO_ESP32_IDF_SOURCE)
    # no ESP32 IDF source specified, download it from official repo

    message(STATUS "ESP32 IDF v${ESP32_IDF_TAG} from official repo")

    FetchContent_Declare(
        esp32_idf
        GIT_REPOSITORY https://github.com/espressif/esp-idf
        GIT_TAG release/v${ESP32_IDF_TAG}
    )

else()
    # ESP32 IDF source was specified

    # sanity check if source path exists
    if(EXISTS "${ESP32_IDF_SOURCE}/")
        message(STATUS "ESP32 IDF v${ESP32_IDF_TAG} source from: ${ESP32_IDF_SOURCE}")

        FetchContent_Declare(
            esp32_idf
            GIT_REPOSITORY ${ESP32_IDF_SOURCE}
            GIT_TAG release/v${ESP32_IDF_TAG}
        )

    else()
        message(FATAL_ERROR "Couldn't find ESP32 IDF source at ${ESP32_IDF_SOURCE}/")
    endif()

endif()

FetchContent_GetProperties(esp32_idf)
FetchContent_Populate(esp32_idf)

# add IDF CMake dir to modules path
list(APPEND CMAKE_MODULE_PATH ${esp32_idf_SOURCE_DIR}/CMake)

# set and grab includes for IDF
include(${esp32_idf_SOURCE_DIR}/tools/cmake/idf.cmake)

# check IDF_PATH environment variable
if(ENV{IDF_PATH} AND NOT (ENV{IDF_PATH} EQUAL ${esp32_idf_SOURCE_DIR}))
    # variable is set and it's different from the current location
    # warn user

    message(STATUS "\n-- ########################################\nFound environment variable IDF_PATH different from current IDF location.\n")
    message(STATUS "\nWas at '$ENV{IDF_PATH}'. Had to replace with current location.")
    message(STATUS "\nIf you have a local install and want to use it for the build, set the build option 'ESP32_IDF_SOURCE' to that location.")
    message(STATUS "########################################\n\n")

    set(ENV{IDF_PATH} ${esp32_idf_SOURCE_DIR})
endif()

# set IDF TARGET_NAME from our TARGET_SERIES
if(${TARGET_SERIES} STREQUAL "ESP32")
    set(TARGET_NAME ${TARGET_SERIES})

elseif(${TARGET_SERIES} STREQUAL "ESP32_S2")
    set(TARGET_NAME "ESP32S2")

elseif(${TARGET_SERIES} STREQUAL "ESP32_C3")
    set(TARGET_NAME "ESP32C3")

else()
    # series is NOT supported 
    message(FATAL_ERROR "\n\nSorry but the ${TARGET_SERIES} is not supported at this time...\nYou can wait for it to be added, or you might want to contribute by working on a PR for it.\n\n")
endif()

# board folder will be added from main CMakeList

# idf_build_set_property(__COMPONENT_REQUIRES_COMMON ${IDF_TARGET} APPEND)

# __component_get_requirements()

# add target TI SimpleLink dirs
add_subdirectory(_common)
add_subdirectory(_Include)
add_subdirectory(_nanoCLR)
