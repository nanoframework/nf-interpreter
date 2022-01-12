#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.common)
include(binutils.arm-none-eabi)
include(binutils.FreeRTOS)

# Define PLATFORM base path
set(BASE_PATH_FOR_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# check if RTOS_SOURCE_FOLDER was specified or if it's empty (default is empty)
set(NO_RTOS_SOURCE_FOLDER TRUE)
if(RTOS_SOURCE_FOLDER)
    if(NOT "${RTOS_SOURCE_FOLDER}" STREQUAL "")
        set(NO_RTOS_SOURCE_FOLDER FALSE)
    endif()
endif()

if(NOT "${TOOL_SRECORD_PREFIX}" STREQUAL "")
    if(NOT ((EXISTS ${TOOL_SRECORD_PREFIX}/srec_cat.exe) OR (EXISTS ${TOOL_SRECORD_PREFIX}/srec_cat)))
        message(STATUS "")
        message(STATUS "Couldn't find the srec_cat tool at the specified path: ${TOOL_SRECORD_PREFIX}/srec_cat.exe")
        message(STATUS "Make sure that the CMake option TOOL_SRECORD_PREFIX has the correct path.")
        message(STATUS "If you don't have this tool download it from https://sourceforge.net/projects/srecord/files/srecord-win32/")
        message(STATUS "")
        message(FATAL_ERROR "srec_cat tool not found")
    else()
        set(SRECORD_TOOL_AVAILABLE TRUE CACHE INTERNAL "srec_cat tool available")
    endif()
endif()

# FreeRTOS version
set(RTOS_VERSION_EMPTY TRUE)

# check if build was requested with a specifc FreeRTOS version
if(DEFINED RTOS_VERSION)
    if(NOT "${RTOS_VERSION}" STREQUAL "")
        set(RTOS_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc FreeRTOS version
if(RTOS_VERSION_EMPTY)
    # no FreeRTOS version actualy specified, must be empty which is fine, we'll default to a known good version
    # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
    set(RTOS_VERSION "V10.4.1-kernel-only")
endif()

if(NO_RTOS_SOURCE_FOLDER)
    # no FreeRTOS source specified, download it from it's repo

    message(STATUS "RTOS is: FreeRTOS ${RTOS_VERSION} from GitHub repo")

    FetchContent_Declare(
        freertos
        GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
        GIT_TAG ${RTOS_VERSION}
    )

else()
    # FreeRTOS source was specified

    message(STATUS "RTOS is: FreeRTOS ${RTOS_VERSION} (source from: ${RTOS_SOURCE_FOLDER})")
    
    FetchContent_Declare(
        freertos
        SOURCE_DIR ${RTOS_SOURCE_FOLDER}
    )

endif()

FetchContent_GetProperties(freertos)
if(NOT freertos_POPULATED)
    # Fetch the content using previously declared details
    FetchContent_Populate(freertos)
endif()

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
    # no CMSIS version actualy specified, must be empty which is fine, we'll default to a known good version*
    # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
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

    message(STATUS "CMSIS v${CMSIS_GIT_TAG} (source from: ${CMSIS_SOURCE})")
    
    FetchContent_Declare(
        cmsis
        SOURCE_DIR ${CMSIS_SOURCE}
    )

endif()

FetchContent_GetProperties(cmsis)
FetchContent_Populate(cmsis)
if(NOT cmsis_POPULATED)
    # Fetch the content using previously declared details
    FetchContent_Populate(cmsis)
endif()

# FreeRTOS common directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/_common)

# try to find board in the targets folder
# and set target base location and base path for class libs
# this has to be set before the class library modules are pulled in
if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/NXP/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${CMAKE_SOURCE_DIR}/targets/FreeRTOS/NXP/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/NXP/${TARGET_BOARD})

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP)

    # add TARGET board directory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP/${TARGET_BOARD})

# try to find board in the targets folder
elseif(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/ST/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${CMAKE_SOURCE_DIR}/targets/FreeRTOS/ST/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/ST/${TARGET_BOARD})

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

    # add TARGET board directory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST/${TARGET_BOARD})

else()

    # try to find NXP board in the Community targets folder
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/NXP/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        nf_set_base_path_for_libraries_modules(${CMAKE_SOURCE_DIR}/targets/FreeRTOS/NXP/_nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/NXP/${TARGET_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NXP)

        # add TARGET board directory from Community
        add_subdirectory(${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/NXP/${TARGET_BOARD})

    # try to find STM board in the Community targets folder
    elseif(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/ST/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        nf_set_base_path_for_libraries_modules(${CMAKE_SOURCE_DIR}/targets/FreeRTOS/ST/_nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/ST/${TARGET_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

        # add TARGET board directory from Community
        add_subdirectory(${CMAKE_SOURCE_DIR}/targets-community/FreeRTOS/ST/${TARGET_BOARD})

    else()

        # board NOT found in targets folder
        message(FATAL_ERROR "\n\nSorry but support for ${TARGET_BOARD} target is not available...\n\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")

    endif()

endif()
