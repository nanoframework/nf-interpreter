#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(nf_utils)

# check if Raspberry Pi Pico SDK folder was specified or if it's empty (default is empty)
set(NO_RPI_PICO_SDK_SOURCE_FOLDER TRUE)
if(RPI_PICO_SDK_SOURCE_FOLDER)
    if(NOT "${RPI_PICO_SDK_SOURCE_FOLDER}" STREQUAL "")
        set(NO_RPI_PICO_SDK_SOURCE_FOLDER FALSE)
    endif()
endif()

# Raspberry Pi Pico SDK version
set(RPI_PICO_SDK_VERSION_EMPTY TRUE)

# check if build was requested with a specifc Raspberry Pi Pico SDK version
if(DEFINED RPI_PICO_SDK_VERSION)
    if(NOT "${RPI_PICO_SDK_VERSION}" STREQUAL "")
        set(RPI_PICO_SDK_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc Raspberry Pi Pico SDK version
if(RPI_PICO_SDK_VERSION_EMPTY)
    # no Raspberry Pi Pico SDK version actualy specified, must be empty which is fine, we'll default to a known good version
    set(RPI_PICO_SDK_VERSION "1.0.1")
endif()

# now up to grabbing the sources
if(NO_RPI_PICO_SDK_SOURCE_FOLDER)
    # no Raspberry Pi Pico SDK source specified, download it from it's repo

    message(STATUS "Raspberry Pi Pico SDK ${RPI_PICO_SDK_VERSION} from GitHub repo")

    FetchContent_Declare(
        pico_sdk
        GIT_REPOSITORY https://github.com/raspberrypi/pico-sdk
        GIT_TAG ${RPI_PICO_SDK_VERSION}
    )

else()
    # Raspberry Pi Pico SDK source was specified

    # sanity check is source path exists
    if(EXISTS "${RPI_PICO_SDK_SOURCE_FOLDER}/")
        message(STATUS "Raspberry Pi Pico SDK  ${RPI_PICO_SDK_VERSION} (source from: ${RPI_PICO_SDK_SOURCE_FOLDER})")

        FetchContent_Declare(
            pico_sdk
            SOURCE_DIR ${RPI_PICO_SDK_SOURCE_FOLDER}
        )

    else()
        message(FATAL_ERROR "Couldn't find Raspberry Pi Pico SDK source at ${RPI_PICO_SDK_SOURCE_FOLDER}/")
    endif()

endif()

FetchContent_MakeAvailable(pico_sdk)

# set these to help Pico SDK
set(PICO_TOOLCHAIN_PATH ${TOOLCHAIN_PREFIX})
set(PICO_COMPILER "pico_arm_gcc")

# taken from pico_sdk_import.cmake
set(PICO_SDK_PATH ${pico_sdk_SOURCE_DIR})
set(PICO_SDK_PATH ${PICO_SDK_PATH} CACHE PATH "Path to the Raspberry Pi Pico SDK" FORCE)

set(PICO_PLATFORM "rp2040" CACHE INTERNAL "Setting Pico platform to hardware")

# taken from pico_sdk_init.cmake
list(APPEND CMAKE_MODULE_PATH ${PICO_SDK_PATH}/cmake)
include(${PICO_SDK_PATH}/pico_sdk_version.cmake)
include(pico_utils)
message("PICO_SDK_PATH is ${PICO_SDK_PATH}")
include(pico_pre_load_platform)
add_subdirectory(${PICO_SDK_PATH} pico-sdk)

# add platform dirs
add_subdirectory(_include)
add_subdirectory(_common)
add_subdirectory(_nanoBooter)
add_subdirectory(_nanoCLR)
