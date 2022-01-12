#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.ESP32)

# Set target series in lower case
nf_set_esp32_target_series()

# Define PLATFORM base path
set(BASE_PATH_FOR_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# ESP32 targets use Espressif IDF and ESP32 FreeRTOS

# check if ESP32_IDF_PATH was specified or if it's empty (default is empty)
set(NO_ESP32_IDF_PATH TRUE)
if(ESP32_IDF_PATH)
    if(NOT "${ESP32_IDF_PATH}" STREQUAL "")
        set(NO_ESP32_IDF_PATH FALSE)
    endif()
endif()

# WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
set(ESP32_IDF_TAG "4.3.2" CACHE INTERNAL "ESP32 IDF tag")

if(NO_ESP32_IDF_PATH)
    # no ESP32 IDF source specified, download it from official repo

    message(STATUS "ESP32 IDF v${ESP32_IDF_TAG} from official repo")

    FetchContent_Declare(
        esp32_idf
        GIT_REPOSITORY https://github.com/espressif/esp-idf
        GIT_TAG v${ESP32_IDF_TAG}
    )

else()
    # ESP32 IDF source was specified

    message(STATUS "ESP32 IDF v${ESP32_IDF_TAG} source from: ${ESP32_IDF_PATH}")

    FetchContent_Declare(
        esp32_idf
        SOURCE_DIR ${ESP32_IDF_PATH}
    )

endif()

FetchContent_GetProperties(esp32_idf)
if(NOT esp32_idf_POPULATED)
    # Fetch the content using previously declared details
    FetchContent_Populate(esp32_idf)
endif()

# add IDF CMake dir to modules path
list(APPEND CMAKE_MODULE_PATH ${esp32_idf_SOURCE_DIR}/CMake)

# parse IDF path to allow proper comparison with environment variable
string(REPLACE "\\" "/" IDF_SOURCE_DIR_PATH "$ENV{IDF_PATH}")

# check IDF_PATH environment variable
if(NOT "${IDF_SOURCE_DIR_PATH}" STREQUAL "${esp32_idf_SOURCE_DIR}")
    
    # variable is set and it's different from the current location
    # can't continue

    message(FATAL_ERROR "\n-- ########################################\nFound environment variable IDF_PATH different from the current specified IDF location.\nIt's currently set to '$ENV{IDF_PATH}'.\nIf you have a local install and want to use it for the build:\n1) Set the build option 'ESP32_IDF_PATH' to that location\n2) Make sure it's checked out at v${ESP32_IDF_TAG}\n########################################\n\n")

else()
    message(STATUS "\n-- IDF_PATH is '${IDF_SOURCE_DIR_PATH}'\r")

    set(IDF_PATH_CMAKED ${IDF_SOURCE_DIR_PATH} CACHE INTERNAL "CMake formated IDF path")
endif()

# if using FatFS need to remove IDF ffconfig.h so it can pick ours
if(NF_FEATURE_HAS_SDCARD)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E remove ${esp32_idf_SOURCE_DIR}/components/fatfs/src/ffconf.h
    )
endif()

# target folder was added in main CMakeList

# if mbed TLS is enabled add it to the build
if(NF_SECURITY_MBEDTLS)

    # check if MBEDTLS_SOURCE was specified or if it's empty (default is empty)
    set(NO_MBEDTLS_SOURCE TRUE)

    if(MBEDTLS_SOURCE)
        if(NOT ${MBEDTLS_SOURCE} STREQUAL "")
            set(NO_MBEDTLS_SOURCE FALSE)
        endif()
    endif()

    # set tag for currently supported version
    # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
    set(MBEDTLS_GIT_TAG "mbedtls-2.26.0")

    # set options for mbed TLS
    option(ENABLE_TESTING "no testing when building mbed TLS." OFF)

    if(NO_MBEDTLS_SOURCE)
        # no mbed TLS source specified, download it from it's repo
        message(STATUS "mbedTLS ${MBEDTLS_GIT_TAG} from GitHub repo")

        FetchContent_Declare(
            mbedtls
            GIT_REPOSITORY https://github.com/ARMmbed/mbedtls
            GIT_TAG ${MBEDTLS_GIT_TAG}
        )

    else()
        # mbedTLS source was specified

        message(STATUS "mbedTLS ${MBEDTLS_GIT_TAG} (source from: ${MBEDTLS_SOURCE})")
            
        FetchContent_Declare(
            mbedtls
            SOURCE_DIR ${MBEDTLS_SOURCE}
        )

    endif()

    FetchContent_GetProperties(mbedtls)
    if(NOT mbedtls_POPULATED)
        # Fetch the content using previously declared details
        FetchContent_Populate(mbedtls)
    endif()

    set(mbedTLS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/3rdparty/humblelogging/include)
    set(mbedTLS_LIBRARIES ${CMAKE_SHARED_LIBRARY_PREFIX}mbedTLS${CMAKE_SHARED_LIBRARY_SUFFIX})

endif()

# add platform dirs
add_subdirectory(_common)
add_subdirectory(_Include)
add_subdirectory(_nanoCLR)

if(USE_NETWORKING_OPTION)
    add_subdirectory(_Network)
endif()
