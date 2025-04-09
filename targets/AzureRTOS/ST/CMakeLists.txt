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
    set(CHIBIOS_HAL_VERSION "stable_21.11.x")
endif()

if(NO_CHIBIOS_HAL_SOURCE)
    # no CHIBIOS source specified, download it from it's repo
    message(STATUS "ChibiOS HAL is: ${CHIBIOS_HAL_VERSION} from SVN official repo")

    FetchContent_Declare(
        chibios
        SVN_REPOSITORY http://svn.code.sf.net/p/chibios/code/branches/${CHIBIOS_HAL_VERSION}
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
            # with git svn clone http://svn.code.sf.net/p/chibios/code/branches/${CHIBIOS_HAL_VERSION} -rHEAD
            message(STATUS "ChibiOS HAL is: ${CHIBIOS_HAL_VERSION} (source from: ${CHIBIOS_HAL_SOURCE})")

            FetchContent_Declare(
                chibios
                SOURCE_DIR ${CHIBIOS_HAL_SOURCE}
            )

        endif()
    else()
        message(FATAL_ERROR "Couldn't find ChibiOS source at ${CHIBIOS_HAL_SOURCE}/")
    endif()

endif()

FetchContent_GetProperties(chibios)
FetchContent_MakeAvailable(chibios)

list(APPEND TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/_include)

# make vars global
set(TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")

# add platform dirs
add_subdirectory(_include)
add_subdirectory(_common)
add_subdirectory(_nanoBooter)
add_subdirectory(_nanoCLR)
