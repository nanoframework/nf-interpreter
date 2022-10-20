#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.Nordic)

setup_west()

######################################
# handle Nordic nRF Connect SDK installation

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

    ##########################################
    # WHEN CHANGING THIS MAKE SURE TO UPDATE:
    # - THE DEV CONTAINERS
    # - azure-pipelines-templates\download-install-ncs-zephyr-toolchain.yml
    set(NRF_CONNECT_SDK_VERSION "v2.0.0")

endif()

if(NO_NRF_CONNECT_SDK_SOURCE_FOLDER)
    # no nRF Connect SDK source specified, use west to init and download everything

    message(STATUS "Downloading Nordic nRF Connect SDK and friends. This can take a while...")

    # make NCS folder
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/_deps/ncs)

    # 1. get nRF Connect SDK code
    execute_process(
        COMMAND ${WEST_EXE} init -m https://github.com/nrfconnect/sdk-nrf --mr ${NRF_CONNECT_SDK_VERSION}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/_deps/ncs
        COMMAND_ECHO STDOUT
    )

    # 2. clone the project repositories
    execute_process(
        COMMAND ${WEST_EXE} update
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/_deps/ncs
        COMMAND_ECHO STDOUT
    )

    # 3. export Zephyr CMake package
    execute_process(
        COMMAND ${WEST_EXE} zephyr-export
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/_deps/ncs
        COMMAND_ECHO STDOUT
    )

    # save path for later
    set(NCS_BASE_LOCATION ${CMAKE_BINARY_DIR}/_deps/ncs CACHE INTERNAL "NCS base location")

else()
    # nRF Connect SDK source was specified

    # sanity check is source path exists
    if(EXISTS "${NRF_CONNECT_SDK_SOURCE_FOLDER}/")
        message(STATUS "nRF Connect SDK (source from: ${NRF_CONNECT_SDK_SOURCE_FOLDER})")

        # perform sanity check if nRF SDK version is the expected one
        set (NRF_CONNECT_SDK_FOUND_VERSION "")
        
        # Fetch the version from the installed directory.
        if (EXISTS  ${NRF_CONNECT_SDK_SOURCE_FOLDER}/nrf/VERSION)
            file(READ ${NCS_DIR}/nrf/VERSION NRF_CONNECT_SDK_FOUND_VERSION)
            string(STRIP ${NRF_CONNECT_SDK_FOUND_VERSION} NRF_CONNECT_SDK_FOUND_VERSION)
        endif()

        if (NOT ${NRF_CONNECT_SDK_FOUND_VERSION} STREQUAL ${NRF_CONNECT_SDK_VERSION})
            message(FATAL_ERROR "\n\nFound nRF Connect SDK v${NRF_CONNECT_SDK_FOUND_VERSION}.\nExpecting v${NRF_CONNECT_SDK_VERSION}.\n\n") 
        endif()

        # save path for later
        set(NCS_BASE_LOCATION ${CMAKE_BINARY_DIR}/_deps/ncs CACHE INTERNAL "NCS base location")

    else()
        message(FATAL_ERROR "Couldn't find nRF Connect SDK source at ${NRF_CONNECT_SDK_SOURCE_FOLDER}/")
    endif()

endif()

# add Zephyr package and unittest to CMake module path
list(APPEND CMAKE_MODULE_PATH ${NCS_BASE_LOCATION}/zephyr/share/zephyr-package/cmake)
list(APPEND CMAKE_MODULE_PATH ${NCS_BASE_LOCATION}/zephyr/share/zephyrunittest-package/cmake)
# add Zephyr pacakge to CMake module path
list(APPEND CMAKE_MODULE_PATH ${NCS_BASE_LOCATION}/nrf/cmake)

# need to set this so that Zephyr is configure for cross compilation using it's own GCC
set(ZEPHYR_TOOLCHAIN_VARIANT "zephyr" CACHE STRING "Zephyr toolchain variant")

# need to set this to let Zephyr build system know the location we're using
set(ENV{ZEPHYR_BASE} ${NCS_BASE_LOCATION}/zephyr)

##########################################
# WHEN CHANGING THIS MAKE SURE TO UPDATE:
# - THE DEV CONTAINERS
# - azure-pipelines-templates\download-install-ncs-zephyr-toolchain.yml
set(ZEPHYR_SDK_VERSION 0.13.1 CACHE STRING "Zephyr SDK version")

# need to set this as environment variable to be available for Zephyr modules
set(ENV{PLATFORM_NORDIC_ROOT} ${CMAKE_CURRENT_SOURCE_DIR})
