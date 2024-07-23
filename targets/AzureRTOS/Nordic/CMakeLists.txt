#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(nf_utils)

# check if nRF Connect SDK folder was specified or if it's empty (default is empty)
set(NO_NRF_CONNECT_SDK_SOURCE_FOLDER TRUE)
if(NRF_CONNECT_SDK_SOURCE_FOLDER)
    if(NOT "${NRF_CONNECT_SDK_SOURCE_FOLDER}" STREQUAL "")
        set(NO_NRF_CONNECT_SDK_SOURCE_FOLDER FALSE)
    endif()
endif()

# version
set(NRF_CONNECT_SDK_VERSION_EMPTY TRUE)

# check if build was requested with a specifc nRF Connect SDK version
if(DEFINED NRF_CONNECT_SDK_VERSION)
    if(NOT "${NRF_CONNECT_SDK_VERSION}" STREQUAL "")
        set(NRF_CONNECT_SDK_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc nRF Connect SDK version
if(NRF_CONNECT_SDK_VERSION_EMPTY)
    # no nRF Connect SDK version actualy specified, must be empty which is fine, we'll default to a known good version
    set(NRF_CONNECT_SDK_VERSION "v1.4.2")
endif()

if(NO_NRF_CONNECT_SDK_SOURCE_FOLDER)
    # no nRF Connect SDK source specified, download it from it's repo
    
    FetchContent_Declare(
        nRFConnectSdk
        GIT_REPOSITORY https://github.com/nrfconnect/sdk-nrf
        GIT_TAG ${NRF_CONNECT_SDK_VERSION}
    )

else()
    # nRF Connect SDK source was specified

    # sanity check is source path exists
    if(EXISTS "${NRF_CONNECT_SDK_SOURCE_FOLDER}/")
        message(STATUS "nRF Connect SDK (source from: ${NRF_CONNECT_SDK_SOURCE_FOLDER})")

        FetchContent_Declare(
            nRFConnectSdk
            SOURCE_DIR ${NRF_CONNECT_SDK_SOURCE_FOLDER}
        )

    else()
        message(FATAL_ERROR "Couldn't find nRF Connect SDK source at ${NRF_CONNECT_SDK_SOURCE_FOLDER}/")
    endif()

endif()

FetchContent_MakeAvailable(nRFConnectSdk)

# Point to NCS root directory
set(NRF_DIR ${nRFConnectSdk_SOURCE_DIR} CACHE PATH "NCS root directory")
list(APPEND CMAKE_MODULE_PATH ${NRF_DIR}/cmake)
include(extensions.cmake)
include(version.cmake)
include(multi_image.cmake)
include(reports.cmake)


# make vars global
set(NANOCLR_PROJECT_SOURCES ${NANOCLR_PROJECT_SOURCES} CACHE INTERNAL "make global")
set(NANOCLR_PROJECT_INCLUDE_DIRS ${NANOCLR_PROJECT_INCLUDE_DIRS} CACHE INTERNAL "make global")

list(APPEND TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/_include)

# add platform dirs
add_subdirectory(_include)
add_subdirectory(_common)
add_subdirectory(_nanoBooter)
add_subdirectory(_nanoCLR)
