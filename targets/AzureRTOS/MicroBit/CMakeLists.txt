#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(nf_utils)

####################
# CODAL Micro Bit V2

# check if CODAL Micro Bit V2 folder was specified or if it's empty (default is empty)
set(NO_CODAL_MICROBIT_V2_SOURCE_FOLDER TRUE)
if(CODAL_MICROBIT_V2_SOURCE_FOLDER)
    if(NOT "${CODAL_MICROBIT_V2_SOURCE_FOLDER}" STREQUAL "")
        set(NO_CODAL_MICROBIT_V2_SOURCE_FOLDER FALSE)
    endif()
endif()

# version
set(CODAL_MICROBIT_V2_VERSION_EMPTY TRUE)

# check if build was requested with a specifc CODAL Micro Bit V2 version
if(DEFINED CODAL_MICROBIT_V2_VERSION)
    if(NOT "${CODAL_MICROBIT_V2_VERSION}" STREQUAL "")
        set(CODAL_MICROBIT_V2_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc CODAL Micro Bit V2 version
if(CODAL_MICROBIT_V2_VERSION_EMPTY)
    # no CODAL Micro Bit V2 version actualy specified, must be empty which is fine, we'll default to a known good version
    set(CODAL_MICROBIT_V2_VERSION "v0.2.24")
endif()

if(NO_CODAL_MICROBIT_V2_SOURCE_FOLDER)
    # no CODAL Micro Bit V2 source specified, download it from it's repo
    
    FetchContent_Declare(
        CODALMicroBitV2
        GIT_REPOSITORY https://github.com/lancaster-university/codal-microbit-v2
        GIT_TAG ${CODAL_MICROBIT_V2_VERSION}
    )

else()
    # CODAL Micro Bit V2 source was specified

    # sanity check is source path exists
    if(EXISTS "${CODAL_MICROBIT_V2_SOURCE_FOLDER}/")
        message(STATUS "CODAL Micro Bit V2 (source from: ${CODAL_MICROBIT_V2_SOURCE_FOLDER})")

        FetchContent_Declare(
            CODALMicroBitV2
            SOURCE_DIR ${CODAL_MICROBIT_V2_SOURCE_FOLDER}
        )

    else()
        message(FATAL_ERROR "Couldn't find CODAL Micro Bit V2 source at ${CODAL_MICROBIT_V2_SOURCE_FOLDER}/")
    endif()

endif()

FetchContent_MakeAvailable(CODALMicroBitV2)

####################
# CODAL Core

# check if CODAL Core folder was specified or if it's empty (default is empty)
set(NO_CODAL_CORE_SOURCE_FOLDER TRUE)
if(CODAL_CORE_SOURCE_FOLDER)
    if(NOT "${CODAL_CORE_SOURCE_FOLDER}" STREQUAL "")
        set(NO_CODAL_CORE_SOURCE_FOLDER FALSE)
    endif()
endif()

# version
set(CODAL_CORE_VERSION_EMPTY TRUE)

# check if build was requested with a specifc CODAL Core version
if(DEFINED CODAL_CORE_VERSION)
    if(NOT "${CODAL_CORE_VERSION}" STREQUAL "")
        set(CODAL_CORE_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc CODAL Core version
if(CODAL_CORE_VERSION_EMPTY)
    # no CODAL Core version actualy specified, must be empty which is fine, we'll default to a known good version
    set(CODAL_CORE_VERSION "master")
endif()

if(NO_CODAL_CORE_SOURCE_FOLDER)
    # no CODAL Core source specified, download it from it's repo
    
    FetchContent_Declare(
        CodalCore
        GIT_REPOSITORY https://github.com/lancaster-university/codal-core
        GIT_TAG ${CODAL_CORE_VERSION}
    )

else()
    # CODAL Core source was specified

    # sanity check is source path exists
    if(EXISTS "${CODAL_CORE_SOURCE_FOLDER}/")
        message(STATUS "CODAL Core (source from: ${CODAL_CORE_SOURCE_FOLDER})")

        FetchContent_Declare(
            CodalCore
            SOURCE_DIR ${CODAL_CORE_SOURCE_FOLDER}
        )

    else()
        message(FATAL_ERROR "Couldn't find CODAL Core source at ${CODAL_CORE_SOURCE_FOLDER}/")
    endif()

endif()

FetchContent_MakeAvailable(CodalCore)

####################
# CODAL NRF52

# check if CODAL NRF52 folder was specified or if it's empty (default is empty)
set(NO_CODAL_NRF52_SOURCE_FOLDER TRUE)
if(CODAL_NRF52_SOURCE_FOLDER)
    if(NOT "${CODAL_NRF52_SOURCE_FOLDER}" STREQUAL "")
        set(NO_CODAL_NRF52_SOURCE_FOLDER FALSE)
    endif()
endif()

# version
set(CODAL_NRF52_VERSION_EMPTY TRUE)

# check if build was requested with a specifc CODAL NRF52 version
if(DEFINED CODAL_NRF52_VERSION)
    if(NOT "${CODAL_NRF52_VERSION}" STREQUAL "")
        set(CODAL_NRF52_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc CODAL NRF52 version
if(CODAL_NRF52_VERSION_EMPTY)
    # no CODAL NRF52 version actualy specified, must be empty which is fine, we'll default to a known good version
    set(CODAL_NRF52_VERSION "master")
endif()

if(NO_CODAL_NRF52_SOURCE_FOLDER)
    # no CODAL NRF52 source specified, download it from it's repo
    
    FetchContent_Declare(
        CodalNrf52
        GIT_REPOSITORY https://github.com/lancaster-university/codal-nrf52
        GIT_TAG ${CODAL_NRF52_VERSION}
    )

else()
    # CODAL NRF52 source was specified

    # sanity check is source path exists
    if(EXISTS "${CODAL_NRF52_SOURCE_FOLDER}/")
        message(STATUS "CODAL NRF52 (source from: ${CODAL_NRF52_SOURCE_FOLDER})")

        FetchContent_Declare(
            CodalNrf52
            SOURCE_DIR ${CODAL_NRF52_SOURCE_FOLDER}
        )

    else()
        message(FATAL_ERROR "Couldn't find CODAL NRF52 source at ${CODAL_NRF52_SOURCE_FOLDER}/")
    endif()

endif()

FetchContent_MakeAvailable(CodalNrf52)

########################
# CODAL Micro Bit NRF52 SDK

# check if CODAL Micro Bit NRF52 SDK folder was specified or if it's empty (default is empty)
set(NO_CODAL_MICROBIT_NRF5SDK_SOURCE_FOLDER TRUE)
if(CODAL_MICROBIT_NRF5SDK_SOURCE_FOLDER)
    if(NOT "${CODAL_MICROBIT_NRF5SDK_SOURCE_FOLDER}" STREQUAL "")
        set(NO_CODAL_MICROBIT_NRF5SDK_SOURCE_FOLDER FALSE)
    endif()
endif()

# version
set(CODAL_MICROBIT_NRF5SDK_VERSION_EMPTY TRUE)

# check if build was requested with a specifc CODAL Micro Bit NRF52 SDK version
if(DEFINED CODAL_MICROBIT_NRF5SDK_VERSION)
    if(NOT "${CODAL_MICROBIT_NRF5SDK_VERSION}" STREQUAL "")
        set(CODAL_MICROBIT_NRF5SDK_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc CODAL Micro Bit NRF52 SDK version
if(CODAL_MICROBIT_NRF5SDK_VERSION_EMPTY)
    # no CODAL Micro Bit NRF52 SDK version actualy specified, must be empty which is fine, we'll default to a known good version
    set(CODAL_MICROBIT_NRF5SDK_VERSION "master")
endif()

if(NO_CODAL_MICROBIT_NRF5SDK_SOURCE_FOLDER)
    # no CODAL Micro Bit NRF52 SDK source specified, download it from it's repo
    
    FetchContent_Declare(
        CodalMicrobitNrf5Sdk
        GIT_REPOSITORY https://github.com/microbit-foundation/codal-microbit-nrf5sdk
        GIT_TAG ${CODAL_MICROBIT_NRF5SDK_VERSION}
    )

else()
    # CODAL Micro Bit NRF52 SDK source was specified

    # sanity check is source path exists
    if(EXISTS "${CODAL_MICROBIT_NRF5SDK_SOURCE_FOLDER}/")
        message(STATUS "CODAL Micro Bit NRF52 SDK (source from: ${CODAL_MICROBIT_NRF5SDK_SOURCE_FOLDER})")

        FetchContent_Declare(
            CodalMicrobitNrf5Sdk
            SOURCE_DIR ${CODAL_MICROBIT_NRF5SDK_SOURCE_FOLDER}
        )

    else()
        message(FATAL_ERROR "Couldn't find CODAL Micro Bit NRF52 SDK source at ${CODAL_MICROBIT_NRF5SDK_SOURCE_FOLDER}/")
    endif()

endif()

FetchContent_MakeAvailable(CodalMicrobitNrf5Sdk)

# make vars global
set(NANOCLR_PROJECT_SOURCES ${NANOCLR_PROJECT_SOURCES} CACHE INTERNAL "make global")
set(NANOCLR_PROJECT_INCLUDE_DIRS ${NANOCLR_PROJECT_INCLUDE_DIRS} CACHE INTERNAL "make global")

list(APPEND TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/_include)

# add platform dirs
add_subdirectory(_include)
add_subdirectory(_common)
add_subdirectory(_nanoBooter)
add_subdirectory(_nanoCLR)
