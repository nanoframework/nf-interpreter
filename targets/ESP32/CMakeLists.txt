#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.ESP32)

# Set target series in lower case
nf_set_esp32_target_series()

# Options for Wire Protocol channel
option(HAL_WP_USE_SERIAL "option to use serial port (UART) to route Wire Protocol")
option(HAL_WP_USE_USB_CDC "option to use USB CDC to route Wire Protocol")

if(HAL_WP_USE_SERIAL)
    message(STATUS "Wire Protocol will use serial port (UART).")
    set(HAL_WP_USE_SERIAL_OPTION TRUE CACHE INTERNAL "set HAL_WP_USE_SERIAL")
    set(HAL_WP_USE_USB_CDC_OPTION FALSE CACHE INTERNAL "set HAL_WP_USE_USB_CDC")
elseif(HAL_WP_USE_USB_CDC)
    message(STATUS "Wire Protocol will use USB CDC.")
    set(HAL_WP_USE_SERIAL_OPTION FALSE CACHE INTERNAL "set HAL_WP_USE_SERIAL")
    set(HAL_WP_USE_USB_CDC_OPTION TRUE CACHE INTERNAL "set HAL_WP_USE_USB_CDC")
else()
    set(HAL_WP_USE_SERIAL_OPTION FALSE CACHE INTERNAL "set HAL_WP_USE_SERIAL")
    set(HAL_WP_USE_USB_CDC_OPTION FALSE CACHE INTERNAL "set HAL_WP_USE_USB_CDC")
endif()

# option to reserve RAM for IDF allocator
option(ESP32_RESERVED_RAM_FOR_IDF_ALLOCATION "Reserved RAM for IDF allocation (in kBytes)")

if(NOT ESP32_RESERVED_RAM_FOR_IDF_ALLOCATION)
    unset(ESP32_RESERVED_RAM_FOR_IDF_ALLOCATION CACHE)
    set(ESP32_RESERVED_RAM_FOR_IDF_ALLOCATION "0" CACHE STRING "Setting default value for ESP32_RESERVED_RAM_FOR_IDF_ALLOCATION")
endif()
message(STATUS "Reserved RAM for allocation by IDF: ${ESP32_RESERVED_RAM_FOR_IDF_ALLOCATION}k")

# option to reserve SPI RAM for IDF allocator
option(ESP32_SPIRAM_FOR_IDF_ALLOCATION "Reserved SPI RAM for IDF allocation (in kBytes)")

if(NOT ESP32_SPIRAM_FOR_IDF_ALLOCATION)
    unset(ESP32_SPIRAM_FOR_IDF_ALLOCATION CACHE)
    set(ESP32_SPIRAM_FOR_IDF_ALLOCATION "0" CACHE STRING "Setting default value for ESP32_SPIRAM_FOR_IDF_ALLOCATION")
endif()
message(STATUS "Reserved SPI RAM for allocation by IDF: ${ESP32_SPIRAM_FOR_IDF_ALLOCATION}k")

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

#################################################################################
# WHEN CHANGING THIS MAKE SURE TO UPDATE:                                       #
# 1. the github tags in dev containers SRC                                      #     
# 2. ref in AZDO pipeline yaml (main, nightly build and Community Targets repo) #
# 3. cache tag in build and download AZDO yaml templates                        #
#################################################################################
#################################################################################
set(ESP32_IDF_TAG "5.2.3" CACHE INTERNAL "ESP32 IDF tag")
#################################################################################
#################################################################################

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

# need to lowercase both paths to compare them
string(TOLOWER "${IDF_SOURCE_DIR_PATH}" IDF_SOURCE_DIR_PATH_LOWER )
string(TOLOWER "${esp32_idf_SOURCE_DIR}" esp32_idf_SOURCE_DIR_LOWER )

# check IDF_PATH environment variable
if(NOT "${IDF_SOURCE_DIR_PATH_LOWER}" STREQUAL "${esp32_idf_SOURCE_DIR_LOWER}")
    # variable is set and it's different from the current location
    # can't continue

    message(FATAL_ERROR "\n-- ########################################\nFound environment variable IDF_PATH different from the current specified IDF location.\nIt's currently set to '$ENV{IDF_PATH}'.\nIf you have a local install and want to use it for the build:\n1) Set the build option 'ESP32_IDF_PATH' to that location\n2) Make sure it's checked out at v${ESP32_IDF_TAG}\n########################################\n\n")

else()
    message(STATUS "\n-- IDF_PATH is '${IDF_SOURCE_DIR_PATH}'\r")

    # on Windows need to make sure the path has the same case as the actual path
    if(CMAKE_HOST_WIN32)
        if(NOT "${IDF_SOURCE_DIR_PATH}" STREQUAL "${esp32_idf_SOURCE_DIR}")
            message(FATAL_ERROR "ESP32_IDF_PATH var is '${esp32_idf_SOURCE_DIR}'. It's using a different case than the actual path. Please fix this.")
        endif()
    endif()

    set(IDF_PATH_CMAKED ${IDF_SOURCE_DIR_PATH} CACHE INTERNAL "CMake formated IDF path")

endif()

# if using FatFS need to remove IDF ffconfig.h so it can pick ours, but save it first to "ffconf.h.sav" so it can be restored
if(NF_FEATURE_HAS_SDCARD)
    if(EXISTS ${esp32_idf_SOURCE_DIR}/components/fatfs/src/ffconf.h)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${esp32_idf_SOURCE_DIR}/components/fatfs/src/ffconf.h ${esp32_idf_SOURCE_DIR}/components/fatfs/src/ffconf.h.sav
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E remove ${esp32_idf_SOURCE_DIR}/components/fatfs/src/ffconf.h
        )
    endif()
else()
    # if not using FatFS need to check if IDF ffconf.h exists
    if(EXISTS ${esp32_idf_SOURCE_DIR}/components/fatfs/src/ffconf.h)
        # all good, ffconf.h still there
    else()
        # ffconf.h doesn't exist so restore it
        if(EXISTS ${esp32_idf_SOURCE_DIR}/components/fatfs/src/ffconf.h.sav)
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E copy ${esp32_idf_SOURCE_DIR}/components/fatfs/src/ffconf.h.sav ${esp32_idf_SOURCE_DIR}/components/fatfs/src/ffconf.h
            )
        else()
            message(FATAL_ERROR "\r\n*******************************************************************\r\n'${esp32_idf_SOURCE_DIR}/components/fatfs/src/ffconf.h' file is missing from IDF\r\nIf this was deleted on a previous build using SDCard, just revert the change and restore the file.\r\n*******************************************************************\r\n")
        endif()
    endif()
endif()

# target folder was added in main CMakeList

# force this to ON
set(NF_SECURITY_MBEDTLS ON CACHE INTERNAL "MbedTLS must be ON for IDF build")

# add platform dirs
add_subdirectory(_common)
add_subdirectory(_include)
add_subdirectory(_nanoCLR)

if(USE_NETWORKING_OPTION)
    add_subdirectory(_Network)
endif()
 