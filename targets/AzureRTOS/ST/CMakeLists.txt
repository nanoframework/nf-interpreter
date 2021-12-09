#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(STM32_CubePackage)

# Set target series
nf_set_stm32_target_series()

# (default is OFF so STM Cube package is NOT included)
option(STM32_CUBE_PACKAGE_REQUIRED "option to include STM Cube package in the build")
if(STM32_CUBE_PACKAGE_REQUIRED)
    ProcessSTM32CubePackage()
endif()

# check if CHIBIOS_HAL_SOURCE was specified or if it's empty (default is empty)
set(NO_CHIBIOS_HAL_SOURCE TRUE)
if(CHIBIOS_HAL_SOURCE)
    if(NOT ${CHIBIOS_HAL_SOURCE} STREQUAL "")
        set(NO_CHIBIOS_HAL_SOURCE FALSE)
    endif()
endif()

# ChibiOS HAL version
set(CHIBIOS_HAL_VERSION_EMPTY TRUE)

# check if build was requested with a specifc ChibiOS HAL version
if(DEFINED CHIBIOS_HAL_VERSION)
    if(NOT "${CHIBIOS_HAL_VERSION}" STREQUAL "")
        set(CHIBIOS_HAL_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc ChibiOS HAL version
if(CHIBIOS_HAL_VERSION_EMPTY)
    # no ChibiOS version actualy specified, must be empty which is fine, we'll default to a known good version
    # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
    set(CHIBIOS_HAL_VERSION "stable_21.6.x")
endif()

if(NO_CHIBIOS_HAL_SOURCE)
    # no CHIBIOS source specified, download it from it's repo
    message(STATUS "ChibiOS HAL is: ${CHIBIOS_HAL_VERSION} from SVN official repo")

    FetchContent_Declare(
        chibios
        SVN_REPOSITORY https://svn.osdn.net/svnroot/chibios/branches/${CHIBIOS_HAL_VERSION}
        SVN_REVISION -rHEAD
    )

else()
    # ChibiOS source was specified

    # sanity check is source path exists
    if(EXISTS ${CHIBIOS_HAL_SOURCE}/)
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)

            message(STATUS "ChibiOS HAL is: ${CHIBIOS_HAL_VERSION} (source from: ${CHIBIOS_HAL_SOURCE})")
            FetchContent_Declare(
                chibios
                SOURCE_DIR ${CHIBIOS_HAL_SOURCE}
            )

        else()
            # running on *nix requires setting up a local git repo from SVN
            # with git svn clone https://svn.osdn.net/svnroot/chibios/branches/${CHIBIOS_HAL_VERSION} -rHEAD
            message(STATUS "ChibiOS HAL is: ${CHIBIOS_HAL_VERSION} (source from: ${CHIBIOS_HAL_SOURCE})")

            FetchContent_Declare(
                chibios
                GIT_REPOSITORY ${CHIBIOS_HAL_SOURCE}
                GIT_TAG master
            )

        endif()
    else()
        message(FATAL_ERROR "Couldn't find ChibiOS source at ${CHIBIOS_HAL_SOURCE}/")
    endif()

endif()

FetchContent_GetProperties(chibios)
FetchContent_Populate(chibios)

if(CHIBIOS_CONTRIB_REQUIRED)

    # check if CHIBIOS-Contrib_SOURCE was specified or if it's empty (default is empty)
    set(NO_CHIBIOS_CONTRIB_SOURCE TRUE)
    if(CHIBIOS_CONTRIB_SOURCE)
        if(NOT ${CHIBIOS_CONTRIB_SOURCE} STREQUAL "")
            set(NO_CHIBIOS_CONTRIB_SOURCE FALSE)
        endif()
    endif()

    if(NO_CHIBIOS_CONTRIB_SOURCE)
        # no CHIBIOS_CONTRIB source specified, download it from it's repo

        FetchContent_Declare(
            chibios-contrib
            GIT_REPOSITORY https://github.com/nanoframework/ChibiOS-Contrib
            GIT_TAG nanoframework
        )

        message(STATUS "ChibiOS-Contrib from GitHub repo")

    else()
        # ChibiOS-Contrib source was specified

        # sanity check is source path exists
        if(EXISTS ${CHIBIOS_CONTRIB_SOURCE}/)
            message(STATUS "ChibiOS-Contrib (source from: ${CHIBIOS_CONTRIB_SOURCE})")

            FetchContent_Declare(
                chibios-contrib
                SOURCE_DIR ${CHIBIOS_CONTRIB_SOURCE}
            )

        else()
            message(FATAL_ERROR "Couldn't find ChibiOS-Contrib source at ${CHIBIOS_CONTRIB_SOURCE}/")
        endif()

    endif()

    FetchContent_GetProperties(chibios-contrib)
    FetchContent_Populate(chibios-contrib)

endif()

list(APPEND TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/_include)

# make vars global
set(TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")

# add platform dirs
add_subdirectory(_include)
add_subdirectory(_common)
add_subdirectory(_nanoBooter)
add_subdirectory(_nanoCLR)
