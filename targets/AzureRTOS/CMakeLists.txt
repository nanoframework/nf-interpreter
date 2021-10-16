#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

unset(TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS CACHE)
unset(TARGET_AZURERTOS_NANOBOOTER_INCLUDE_DIRS CACHE)
unset(TARGET_AZURERTOS_COMMON_INCLUDE_DIRS CACHE)
unset(EXTRA_COMPILE_DEFINITIONS CACHE)

include(FetchContent)
include(binutils.common)
include(binutils.arm-none-eabi)
  
# add path to local cmake
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(utilities)

# Sanity checks for cmake-variants.json
# Define PLATFORM base path (usually set in top level CMAkeLists.txt)
if(NOT BASE_PATH_FOR_PLATFORM)
    set(BASE_PATH_FOR_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
endif()

# try to find board in the targets folder - ${CMAKE_CURRENT_SOURCE_DIR}/ST/${TARGET_BOARD})
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ST/${TARGET_BOARD})
    list(APPEND PLATFORM_CHECK_FAILED "${CMAKE_CURRENT_SOURCE_DIR}/ST/${TARGET_BOARD} is not currently a supported option.")
endif()          

# this platform REQUIRES support from ChibiOS HAL
if(NOT CHIBIOS_HAL_REQUIRED)
    list(APPEND PLATFORM_CHECK_FAILED "CHIBIOS_HAL_REQUIRED=${CHIBIOS_HAL_REQUIRED} must be ON.")
else()
    set(RTOS_AZURERTOS_CHIBIOS_HAL_CHECK TRUE)
endif()          
          
if(PLATFORM_CHECK_FAILED)
    print_list(PLATFORM_CHECK_FAILED)
    message(STATUS "\n\n")
    message(FATAL_ERROR "\nFor '${RTOS}' TARGET_BOARD '${TARGET_BOARD}' the following checks failed. Please review cmake-variants.json")
endif()          


# Passed sanity checks, continue CMakeLists.txt for this platform

include(binutils.AzureRTOS)

# check if RTOS_SOURCE_FOLDER was specified or if it's empty (default is empty)
set(NO_RTOS_SOURCE_FOLDER TRUE)
if(RTOS_SOURCE_FOLDER)
    if(NOT "${RTOS_SOURCE_FOLDER}" STREQUAL "")
        set(NO_RTOS_SOURCE_FOLDER FALSE)
    endif()
endif()

# AzureRTOS version
set(RTOS_VERSION_EMPTY TRUE)

# check if build was requested with a specifc AzureRTOS version
if(DEFINED RTOS_VERSION)
    if(NOT "${RTOS_VERSION}" STREQUAL "")
        set(RTOS_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc AzureRTOS version
if(RTOS_VERSION_EMPTY)
    # no AzureRTOS version actualy specified, must be empty which is fine, we'll default to a known good version
    set(RTOS_VERSION "v6.1.8_rel")
endif()

if(NO_RTOS_SOURCE_FOLDER)
    # no AzureRTOS source specified, download it from it's repo

    message(STATUS "RTOS is: ${RTOS} ${RTOS_VERSION} from GitHub repo")

    FetchContent_Declare(
        azure_rtos
        GIT_REPOSITORY https://github.com/azure-rtos/threadx.git
        GIT_TAG ${RTOS_VERSION}
    )

else()
    # AzureRTOS source was specified

    # sanity check is source path exists
    if(EXISTS "${RTOS_SOURCE_FOLDER}/")
        message(STATUS "RTOS is: ${RTOS} ${RTOS_VERSION} (source from: ${RTOS_SOURCE_FOLDER})")

        FetchContent_Declare(
            azure_rtos
            GIT_REPOSITORY $(RTOS_SOURCE_FOLDER)
            GIT_TAG ${RTOS_VERSION}
        )

    else()
        message(FATAL_ERROR "Couldn't find ${RTOS} source at ${RTOS_SOURCE_FOLDER}/")
    endif()

endif()

FetchContent_Populate(azure_rtos)

# AzureRTOS common directories
add_subdirectory(_common)
add_subdirectory(_include)
		      
message(STATUS "BASE_PATH_FOR_PLATFORM=${BASE_PATH_FOR_PLATFORM}")

message(STATUS "TARGET_PLATFORM=${BASE_PATH_FOR_PLATFORM}/ST/${TARGET_BOARD}")

# try to find STM board in the targets folder
# and set target base location and base path for class libs
# this has to be set before the class library modules are pulled in
if(EXISTS ${BASE_PATH_FOR_PLATFORM}/ST/${TARGET_BOARD})

    # board found
    message(STATUS "${RTOS} support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${BASE_PATH_FOR_PLATFORM}/ST/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${BASE_PATH_FOR_PLATFORM}/ST/${TARGET_BOARD})

    if(CHIBIOS_HAL_REQUIRED)
        set(CHIBIOS_BOARD_DEFINITIONS_PATH ${BASE_PATH_FOR_PLATFORM}/ST/${TARGET_BOARD} CACHE INTERNAL "Path of board definition files")
        set(RTOS_AZURERTOS_CHIBIOS_HAL_CHECK TRUE)
    endif()

    add_subdirectory(ST)

    # add TARGET board directory
    add_subdirectory(ST/${TARGET_BOARD})

# try to find Nordic board in the targets folder
elseif(EXISTS ${BASE_PATH_FOR_PLATFORM}/Nordic/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${BASE_PATH_FOR_PLATFORM}/Nordic/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${BASE_PATH_FOR_PLATFORM}/Nordic/${TARGET_BOARD})

    add_subdirectory(Nordic)

    # add TARGET board directory
    add_subdirectory(Nordic/${TARGET_BOARD})

# try to find MicroBit board in the targets folder
elseif(EXISTS ${BASE_PATH_FOR_PLATFORM}/MicroBit/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${BASE_PATH_FOR_PLATFORM}/MicroBit/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${BASE_PATH_FOR_PLATFORM}/MicroBit/${TARGET_BOARD})

    add_subdirectory(MicroBit)

    # add TARGET board directory
    add_subdirectory(MicroBit/${TARGET_BOARD})

# try to find Raspberry Pi Pico board in the targets folder
elseif(EXISTS ${BASE_PATH_FOR_PLATFORM}/RaspberryPi/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${BASE_PATH_FOR_PLATFORM}/RaspberryPi/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${BASE_PATH_FOR_PLATFORM}/RaspberryPi/${TARGET_BOARD})

    add_subdirectory(RaspberryPi)

    # add TARGET board directory
    add_subdirectory(RaspberryPi/${TARGET_BOARD})

# try to find Maxim board in the targets folder
elseif(EXISTS ${BASE_PATH_FOR_PLATFORM}/Maxim/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${BASE_PATH_FOR_PLATFORM}/Maxim/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${BASE_PATH_FOR_PLATFORM}/Maxim/${TARGET_BOARD})

    add_subdirectory(Maxim)

    # add TARGET board directory
    add_subdirectory(Maxim/${TARGET_BOARD})

else()

    # try to find STM board in the Community targets folder
    set(COMMUNITY_BASE_PATH_FOR_PLATFORM  ${PROJECT_SOURCE_DIR}/targets-community/${RTOS} PARENT_SCOPE)
    set(COMMUNITY_BASE_PATH_FOR_PLATFORM  ${PROJECT_SOURCE_DIR}/targets-community/${RTOS})

    if(EXISTS ${COMMUNITY_BASE_PATH_FOR_PLATFORM}/ST/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        nf_set_base_path_for_libraries_modules(${COMMUNITY_BASE_PATH_FOR_PLATFORM}/ST/_nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${COMMUNITY_BASE_PATH_FOR_PLATFORM}/ST/${TARGET_BOARD})

        if(CHIBIOS_HAL_REQUIRED)
             set(CHIBIOS_BOARD_DEFINITIONS_PATH ${COMMUNITY_BASE_PATH_FOR_PLATFORM}/ST/${TARGET_BOARD} CACHE INTERNAL "Path of board definition files")
        endif()

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

        # add TARGET board directory from Community
        add_subdirectory(${COMMUNITY_BASE_PATH_FOR_PLATFORM}/ST/${TARGET_BOARD})

    # try to find Nordic board in the Community targets folder
    elseif(EXISTS ${${COMMUNITY_BASE_PATH_FOR_PLATFORM}}/Nordic/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        nf_set_base_path_for_libraries_modules(${${COMMUNITY_BASE_PATH_FOR_PLATFORM}}/Nordic/_nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${COMMUNITY_BASE_PATH_FOR_PLATFORM}/Nordic/${TARGET_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Nordic)

        # add TARGET board directory from Community
        add_subdirectory(${COMMUNITY_BASE_PATH_FOR_PLATFORM}/Nordic/${TARGET_BOARD})

    else()

        # board NOT found in targets folder
        message(FATAL_ERROR "\n\nSorry but support for ${TARGET_BOARD} target is not available on ${RTOS} ...\n\nYou can wait for it to be added or you might want to contribute and start working on a PR for it.\n\n")

    endif()

endif()
