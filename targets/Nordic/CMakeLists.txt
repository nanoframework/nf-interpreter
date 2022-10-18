#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.Nordic)
include(FetchContent)

# Define tested version of Nordic Connect SDK
set (NCS_SUPPPORTED_VERSION 2.0.0)

#
# Check for a preexisting copy of the Nordic
# connect SDK. We do this in steps.
#   1. If RTOS_SOURCE_FOLDER exists, check there
#   2. If not, and the ZEPHYR_BASE env var exists, check there
#   3. Otherwise, pull down the NCS from the web. (not supported)
#
set(NO_RTOS_SOURCE_FOLDER TRUE)

# check if RTOS_SOURCE_FOLDER was specified or if it's empty (default is empty)
if(RTOS_SOURCE_FOLDER)
    if(NOT "${RTOS_SOURCE_FOLDER}" STREQUAL "")
        set (ZBASE ${RTOS_SOURCE_FOLDER}/Zephyr) 
        set (NRF_CONNECT_SDK_SOURCE_FOLDER ${RTOS_SOURCE_FOLDER})
        set (ENV{ZEPHYR_BASE} ${ZBASE}) 
        set(NO_RTOS_SOURCE_FOLDER FALSE)
    endif()
endif()   

#if RTOS_SOURCE_FOLDER not set or null, check for ZEPHYR_BASE environment var
if(NO_RTOS_SOURCE_FOLDER)
    set (ZBASE $ENV{ZEPHYR_BASE})
    if (ZBASE)
        set(NRF_CONNECT_SDK_SOURCE_FOLDER ${ZBASE}/..) 
        set(NO_RTOS_SOURCE_FOLDER FALSE)
    endif()
endif()

# Clean up the NRF string 
string(REPLACE "\\" "/" NRF_CONNECT_SDK_SOURCE_FOLDER ${NRF_CONNECT_SDK_SOURCE_FOLDER})
set(NCS_DIR ${NRF_CONNECT_SDK_SOURCE_FOLDER})

# sanity check is source path exists
if(EXISTS "${NRF_CONNECT_SDK_SOURCE_FOLDER}/")
    message(STATUS "nRF Connect SDK (source from: ${NRF_CONNECT_SDK_SOURCE_FOLDER})")
else()
    set(NO_RTOS_SOURCE_FOLDER TRUE)
endif()

if(NO_RTOS_SOURCE_FOLDER)
    # Auto-download of NCS not currently supported.
    if (0)
        # no nRF Connect SDK source specified, download it from it's repo
        message(STATUS "Downloading NRF Connect SDK...")

        FetchContent_Declare(
            nRFConnectSdk
            GIT_REPOSITORY https://github.com/nrfconnect/sdk-nrf
            GIT_TAG ${NCS_SUPPPORTED_VERSION}
        )

        FetchContent_Declare(
            zephyr
            GIT_REPOSITORY https://github.com/nrfconnect/sdk-zephyr
            GIT_TAG 8e883581d322f9212f50cfb15f686873a96d6a29
        )

        FetchContent_Populate(nRFConnectSdk)
        FetchContent_Populate(zephyr)

        message(STATUS "Downloading NRF Connect SDK...")
    else()
        message(STATUS "\n-- ################################################################")
        message(STATUS "The NRF Connect SDK is not installed or ZEPHYR_BASE")
        message(STATUS "is not set.Auto-download of the NCS is not supported")
        message(STATUS "at this time. Please download the NCS from")
        message(STATUS "www.nordicsemi.com/Products/Development-software/nrf-connect-sdk")
        message(STATUS "################################################################\n")
        message(FATAL_ERROR "Nordic Connect SDK not found.")
    endif()
endif()

#
# Check for a supported version
#
set (NRF_CONNECT_SDK_VERSION "")
# Fetch the version from the installed directory.
if (EXISTS  ${NCS_DIR}/nrf/VERSION)
    message(STATUS "version file found<")
    file(READ ${NCS_DIR}/nrf/VERSION NRF_CONNECT_SDK_VERSION)
    string(STRIP ${NRF_CONNECT_SDK_VERSION} NRF_CONNECT_SDK_VERSION)
endif()

if (NOT ${NRF_CONNECT_SDK_VERSION} STREQUAL ${NCS_SUPPPORTED_VERSION})
    message(WARNING "***************************************************************\n"
    "    NRF Connecct SDK version ${NRF_CONNECT_SDK_VERSION} is not supported.\n"
    "    Version ${NCS_SUPPPORTED_VERSION} is the only supported version.\n"
    "***************************************************************") 
endif()

# 
# Set RTOS root so module cmakelists.txt will know how to find things.
#
set(TARGET_RTOS_ROOT  ${CMAKE_CURRENT_SOURCE_DIR})
set(TARGET_RTOS_ROOT ${TARGET_RTOS_ROOT} CACHE INTERNAL "make global")

set(TARGET_RTOS_BINDIR  ${CMAKE_CURRENT_BINARY_DIR})
set(TARGET_RTOS_BINDIR ${TARGET_RTOS_BINDIR} CACHE INTERNAL "make global")

#
# Add to the places where the board directories are searched
#
list(APPEND BOARD_ROOT "${TARGET_RTOS_ROOT}/boards")


