#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.arm-none-eabi)
include(binutils.AzureRTOS)
include(nf_utils)

# Define PLATFORM base path
set(BASE_PATH_FOR_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

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
    set(RTOS_VERSION "v6.1.5_rel")
endif()

if(NO_RTOS_SOURCE_FOLDER)
    # no AzureRTOS source specified, download it from it's repo

    message(STATUS "RTOS is: AzureRTOS ${RTOS_VERSION} from GitHub repo")

    FetchContent_Declare(
        azure_rtos
        GIT_REPOSITORY https://github.com/azure-rtos/threadx.git
        GIT_TAG ${RTOS_VERSION}
    )

else()
    # AzureRTOS source was specified

    # sanity check is source path exists
    if(EXISTS "${RTOS_SOURCE_FOLDER}/")
        message(STATUS "RTOS is: AzureRTOS ${RTOS_VERSION} (source from: ${RTOS_SOURCE_FOLDER})")

        FetchContent_Declare(
            azure_rtos
            GIT_REPOSITORY $(RTOS_SOURCE_FOLDER)
            GIT_TAG ${RTOS_VERSION}
        )

    else()
        message(FATAL_ERROR "Couldn't find AzureRTOS source at ${RTOS_SOURCE_FOLDER}/")
    endif()

endif()

FetchContent_Populate(azure_rtos)

# AzureRTOS common directories
add_subdirectory(_common)

# try to find STM board in the targets folder
# and set target base location and base path for class libs
# this has to be set before the class library modules are pulled in
if(EXISTS ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    SET_BASE_PATH_FOR_LIBRARIES_MODULES(${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/${TARGET_BOARD})

    add_subdirectory(ST)

    # add TARGET board directory
    add_subdirectory(ST/${TARGET_BOARD})

# try to find Nordic board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/Nordic/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    SET_BASE_PATH_FOR_LIBRARIES_MODULES(${PROJECT_SOURCE_DIR}/targets/AzureRTOS/Nordic/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/Nordic/${TARGET_BOARD})

    add_subdirectory(Nordic)

    # add TARGET board directory
    add_subdirectory(Nordic/${TARGET_BOARD})

# try to find MicroBit board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/MicroBit/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    SET_BASE_PATH_FOR_LIBRARIES_MODULES(${PROJECT_SOURCE_DIR}/targets/AzureRTOS/MicroBit/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/MicroBit/${TARGET_BOARD})

    add_subdirectory(MicroBit)

    # add TARGET board directory
    add_subdirectory(MicroBit/${TARGET_BOARD})

# try to find Raspberry Pi Pico board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/RaspberryPi/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    SET_BASE_PATH_FOR_LIBRARIES_MODULES(${PROJECT_SOURCE_DIR}/targets/AzureRTOS/RaspberryPi/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/RaspberryPi/${TARGET_BOARD})

    add_subdirectory(RaspberryPi)

    # add TARGET board directory
    add_subdirectory(RaspberryPi/${TARGET_BOARD})

# try to find Maxim board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/Maxim/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    SET_BASE_PATH_FOR_LIBRARIES_MODULES(${PROJECT_SOURCE_DIR}/targets/AzureRTOS/Maxim/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/AzureRTOS/Maxim/${TARGET_BOARD})

    add_subdirectory(Maxim)

    # add TARGET board directory
    add_subdirectory(Maxim/${TARGET_BOARD})

else()

    # try to find STM board in the Community targets folder
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        SET_BASE_PATH_FOR_LIBRARIES_MODULES(${PROJECT_SOURCE_DIR}/targets/AzureRTOS/ST/_nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${TARGET_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

        # add TARGET board directory from Community
        add_subdirectory(${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/ST/${TARGET_BOARD})

    # try to find Nordic board in the Community targets folder
    elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/Nordic/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        SET_BASE_PATH_FOR_LIBRARIES_MODULES(${PROJECT_SOURCE_DIR}/targets/AzureRTOS/Nordic/_nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/Nordic/${TARGET_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Nordic)

        # add TARGET board directory from Community
        add_subdirectory(${PROJECT_SOURCE_DIR}/targets-community/AzureRTOS/Nordic/${TARGET_BOARD})

    else()

        # board NOT found in targets folder
        message(FATAL_ERROR "\n\nSorry but support for ${TARGET_BOARD} target is not available...\n\nYou can wait for it to be added or you might want to contribute and start working on a PR for it.\n\n")

    endif()

endif()
