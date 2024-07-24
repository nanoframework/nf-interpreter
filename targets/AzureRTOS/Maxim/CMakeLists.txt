#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.common)

set(MAXIM_MICROS_SDK_REQUIRED TRUE PARENT_SCOPE)

# check if Maxim Integrated Micros SDK folder was specified or if it's empty (default is empty)
set(NO_MAXIM_MICROS_SDK_SOURCE_FOLDER TRUE)
if(MAXIM_MICROS_SDK_SOURCE_FOLDER)
    if(NOT "${MAXIM_MICROS_SDK_SOURCE_FOLDER}" STREQUAL "")
        set(NO_MAXIM_MICROS_SDK_SOURCE_FOLDER FALSE)
    endif()
endif()

# version
set(MAXIM_MICROS_SDK_VERSION_EMPTY TRUE)

# check if build was requested with a specifc Maxim Integrated Micros SDK version
if(DEFINED MAXIM_MICROS_SDK_VERSION)
    if(NOT "${MAXIM_MICROS_SDK_VERSION}" STREQUAL "")
        set(MAXIM_MICROS_SDK_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc Maxim Integrated Micros SDK version
if(MAXIM_MICROS_SDK_VERSION_EMPTY)
    # no Maxim Integrated Micros SDK version actualy specified, must be empty which is fine, we'll default to a known good version
    set(MAXIM_MICROS_SDK_VERSION "v1.0.0.2_nano")
endif()

if(NO_MAXIM_MICROS_SDK_SOURCE_FOLDER)
    # no Maxim Integrated Micros SDK source specified, download it from it's repo

    message(STATUS "Maxim Integrated Micros SDK ${MAXIM_MICROS_SDK_VERSION} from GitHub repo")
    
    FetchContent_Declare(
        maxim_micros_sdk
        GIT_REPOSITORY https://github.com/nanoframework/MaximIntegratedMicros-SDK
        GIT_TAG ${MAXIM_MICROS_SDK_VERSION}
    )

else()
    # Maxim Integrated Micros SDK source was specified

    # sanity check is source path exists
    if(EXISTS "${MAXIM_MICROS_SDK_SOURCE_FOLDER}/")

        message(STATUS "Maxim Integrated Micros SDK ${MAXIM_MICROS_SDK_VERSION} (source from: ${MAXIM_MICROS_SDK_SOURCE_FOLDER})")

        FetchContent_Declare(
            maxim_micros_sdk
            SOURCE_DIR ${MAXIM_MICROS_SDK_SOURCE_FOLDER}
        )

    else()
        message(FATAL_ERROR "Couldn't find Maxim Integrated Micros SDK source at ${MAXIM_MICROS_SDK_SOURCE_FOLDER}/")
    endif()

endif()

FetchContent_MakeAvailable(maxim_micros_sdk)

# make vars global
set(NANOCLR_PROJECT_SOURCES ${NANOCLR_PROJECT_SOURCES} CACHE INTERNAL "make global")
set(NANOCLR_PROJECT_INCLUDE_DIRS ${NANOCLR_PROJECT_INCLUDE_DIRS} CACHE INTERNAL "make global")

list(APPEND TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/_include)

# check if CMSIS_SOURCE was specified or if it's empty (default is empty)
set(NO_CMSIS_SOURCE TRUE)
if(CMSIS_SOURCE)
    if(NOT "${CMSIS_SOURCE}" STREQUAL "")
        set(NO_CMSIS_SOURCE FALSE)
    endif()
endif()

# CMSIS version
set(CMSIS_VERSION_EMPTY TRUE)

# check if build was requested with a specifc CMSIS version
if(DEFINED CMSIS_VERSION)
    if(NOT "${CMSIS_VERSION}" STREQUAL "")
        set(CMSIS_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc CMSIS version
if(CMSIS_VERSION_EMPTY)
    # no CMSIS version actualy specified, must be empty which is fine, we'll default to a known good version
    set(CMSIS_GIT_TAG "5.5.1")
else()
    # set Git tag
    set(CMSIS_GIT_TAG "${CMSIS_VERSION}")
endif()            

if(NO_CMSIS_SOURCE)

    message(STATUS "CMSIS v${CMSIS_GIT_TAG} from GitHub repo")

    FetchContent_Declare(
        cmsis
        GIT_REPOSITORY https://github.com/ARM-software/CMSIS_5.git
        GIT_TAG ${CMSIS_GIT_TAG}
    )

else()
    # CMSIS source was specified

    # sanity check is source path exists
    if(EXISTS "${CMSIS_SOURCE}/")
        message(STATUS "CMSIS v${CMSIS_GIT_TAG} (source from: ${CMSIS_SOURCE})")
       
        FetchContent_Declare(
            cmsis
            SOURCE_DIR ${CMSIS_SOURCE}
        )
    else()
        message(FATAL_ERROR "Couldn't find CMSIS source at ${CMSIS_SOURCE}/")
    endif()

endif()

FetchContent_GetProperties(cmsis)
FetchContent_MakeAvailable(cmsis)

# add platform dirs
add_subdirectory(_include)
add_subdirectory(_common)
add_subdirectory(_nanoBooter)
add_subdirectory(_nanoCLR)
