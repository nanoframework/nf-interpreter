#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.common)
include(binutils.arm-none-eabi)
include(binutils.ThreadX)

###############
# build options
option(THREADX_USBX_REQUIRED "option to include ThreadX USBX")
option(THREADX_FILEX_REQUIRED "option to include ThreadX FileX")
option(THREADX_NETXDUO_REQUIRED "option to include ThreadX NetX Duo")
option(USBX_FEATURE_CDC "option to use USBX CDC class")
option(USBX_FEATURE_HID "option to use USBX HID class")

if(USBX_FEATURE_CDC)

    set(USBX_FEATURE_CDC_OPTION TRUE CACHE INTERNAL "USBX_FEATURE_CDC_OPTION for USBX_FEATURE_CDC")
    # requires:
    # THREADX_USBX_REQUIRED
    set(THREADX_USBX_REQUIRED ON CACHE INTERNAL "THREADX_USBX_REQUIRED for USBX_FEATURE_CDC")
    # THREADX_FILEX_REQUIRED
    set(THREADX_FILEX_REQUIRED ON CACHE INTERNAL "THREADX_FILEX_REQUIRED for USBX_FEATURE_CDC")
    # THREADX_NETXDUO_REQUIRED
    set(THREADX_NETXDUO_REQUIRED ON CACHE INTERNAL "THREADX_NETXDUO_REQUIRED for USBX_FEATURE_CDC")

else()
    set(USBX_FEATURE_CDC_OPTION FALSE CACHE INTERNAL "USBX_FEATURE_CDC_OPTION for USBX_FEATURE_CDC")
endif()

if(USBX_FEATURE_HID)

    set(USBX_FEATURE_HID_OPTION TRUE CACHE INTERNAL "USBX_FEATURE_HID_OPTION for USBX_FEATURE_HID")
    # requires:
    # THREADX_USBX_REQUIRED
    set(THREADX_USBX_REQUIRED ON CACHE INTERNAL "THREADX_USBX_REQUIRED for USBX_FEATURE_HID")
    # THREADX_FILEX_REQUIRED
    set(THREADX_FILEX_REQUIRED ON CACHE INTERNAL "THREADX_FILEX_REQUIRED for USBX_FEATURE_HID")
    # THREADX_NETXDUO_REQUIRED
    set(THREADX_NETXDUO_REQUIRED ON CACHE INTERNAL "THREADX_NETXDUO_REQUIRED for USBX_FEATURE_HID")

else()
    set(USBX_FEATURE_HID_OPTION FALSE CACHE INTERNAL "USBX_FEATURE_HID_OPTION for USBX_FEATURE_HID")
endif()

if(USE_NETWORKING_OPTION)
    set(THREADX_NETXDUO_REQUIRED ON CACHE INTERNAL "THREADX_NETXDUO_REQUIRED for Networking")
endif()

###########################
# Define PLATFORM base path
set(BASE_PATH_FOR_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# check if THREADX_SOURCE_FOLDER was specified or if it's empty (default is empty)
set(NO_THREADX_SOURCE_FOLDER TRUE)
if(THREADX_SOURCE_FOLDER)
    if(NOT "${THREADX_SOURCE_FOLDER}" STREQUAL "")
        set(NO_THREADX_SOURCE_FOLDER FALSE)
    endif()
endif()

###################
# ThreadX version
set(RTOS_VERSION_EMPTY TRUE)

# check if build was requested with a specifc ThreadX version
if(DEFINED RTOS_VERSION)
    if(NOT "${RTOS_VERSION}" STREQUAL "")
        set(RTOS_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc ThreadX version
if(RTOS_VERSION_EMPTY)
    # no ThreadX version actualy specified, must be empty which is fine, we'll default to a known good version
    # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
    set(RTOS_VERSION "v6.5.0.202601_rel")
endif()

if(NO_THREADX_SOURCE_FOLDER)
    # no ThreadX source specified, download it from it's repo

    message(STATUS "RTOS is: ThreadX ${RTOS_VERSION} from GitHub repo")

    FetchContent_Populate(
        threadx
        GIT_REPOSITORY https://github.com/eclipse-threadx/threadx.git
        GIT_TAG ${RTOS_VERSION}
    )

else()
    # ThreadX source was specified

    # sanity check is source path exists
    if(EXISTS "${THREADX_SOURCE_FOLDER}/")
        message(STATUS "RTOS is: ThreadX ${RTOS_VERSION} (source from: ${THREADX_SOURCE_FOLDER})")

        FetchContent_Populate(
            threadx
            SOURCE_DIR ${THREADX_SOURCE_FOLDER}
        )

    else()
        message(FATAL_ERROR "Couldn't find ThreadX source at ${THREADX_SOURCE_FOLDER}/")
    endif()

endif()

###########################
# ThreadX USBX
if(THREADX_USBX_REQUIRED)

    # check if USBX_SOURCE_FOLDER was specified or if it's empty (default is empty)
    set(NO_USBX_SOURCE_FOLDER TRUE)
    if(USBX_SOURCE_FOLDER)
        if(NOT "${USBX_SOURCE_FOLDER}" STREQUAL "")
            set(NO_USBX_SOURCE_FOLDER FALSE)
        endif()
    endif()

    # ThreadX USBX version
    set(USBX_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc ThreadX USBX version
    if(DEFINED USBX_VERSION)
        if(NOT "${USBX_VERSION}" STREQUAL "")
            set(USBX_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc ThreadX USBX version
    if(USBX_VERSION_EMPTY)
        # no ThreadX USBX version actualy specified, must be empty which is fine, we'll default to ThreadX version
        set(USBX_VERSION ${RTOS_VERSION})
    endif()

    if(NO_USBX_SOURCE_FOLDER)
        # no ThreadX USBX source specified, download it from it's repo

        message(STATUS "ThreadX USBX ${USBX_VERSION} from GitHub repo")

        FetchContent_Populate(
            threadx_usbx
            GIT_REPOSITORY https://github.com/eclipse-threadx/usbx.git
            GIT_TAG ${RTOS_VERSION}
        )

    else()
        # ThreadX USBX source was specified

        # sanity check is source path exists
        if(EXISTS "${USBX_SOURCE_FOLDER}/")
            message(STATUS "ThreadX USBX ${USBX_VERSION} (source from: ${USBX_SOURCE_FOLDER})")

            FetchContent_Populate(
                threadx_usbx
                SOURCE_DIR ${USBX_SOURCE_FOLDER}
            )

        else()
            message(FATAL_ERROR "Couldn't find ThreadX USBX source at ${USBX_SOURCE_FOLDER}/")
        endif()

    endif()

endif()

###########################
# ThreadX FileX
if(THREADX_FILEX_REQUIRED)

    # check if FILEX_SOURCE_FOLDER was specified or if it's empty (default is empty)
    set(NO_FILEX_SOURCE_FOLDER TRUE)
    if(FILEX_SOURCE_FOLDER)
        if(NOT "${FILEX_SOURCE_FOLDER}" STREQUAL "")
            set(NO_FILEX_SOURCE_FOLDER FALSE)
        endif()
    endif()

    # ThreadX FileX version
    set(FILEX_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc ThreadX FileX version
    if(DEFINED FILEX_VERSION)
        if(NOT "${FILEX_VERSION}" STREQUAL "")
            set(FILEX_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc ThreadX FileX version
    if(FILEX_VERSION_EMPTY)
        # no ThreadX FileX version actualy specified, must be empty which is fine, we'll default to ThreadX version
        set(FILEX_VERSION ${RTOS_VERSION})
    endif()

    if(NO_FILEX_SOURCE_FOLDER)
        # no ThreadX FileX source specified, download it from it's repo

        message(STATUS "ThreadX FileX ${FILEX_VERSION} from GitHub repo")

        FetchContent_Populate(
            threadx_filex
            GIT_REPOSITORY https://github.com/eclipse-threadx/filex.git
            GIT_TAG ${FILEX_VERSION}
        )

    else()
        # ThreadX FileX source was specified

        # sanity check is source path exists
        if(EXISTS "${FILEX_SOURCE_FOLDER}/")
            message(STATUS "ThreadX FileX ${FILEX_VERSION} (source from: ${FILEX_SOURCE_FOLDER})")

            FetchContent_Populate(
                threadx_filex
                SOURCE_DIR ${FILEX_SOURCE_FOLDER}
            )

        else()
            message(FATAL_ERROR "Couldn't find ThreadX FileX source at ${FILEX_SOURCE_FOLDER}/")
        endif()

    endif()

endif()

###########################
# ThreadX NetX Duo
if(THREADX_NETXDUO_REQUIRED)

    # check if NETXDUO_SOURCE_FOLDER was specified or if it's empty (default is empty)
    set(NO_NETXDUO_SOURCE_FOLDER TRUE)
    if(NETXDUO_SOURCE_FOLDER)
        if(NOT "${NETXDUO_SOURCE_FOLDER}" STREQUAL "")
            set(NO_NETXDUO_SOURCE_FOLDER FALSE)
        endif()
    endif()

    # ThreadX NetX Duo version
    set(NETXDUO_VERSION_EMPTY TRUE)

    # check if build was requested with a specifc ThreadX NetX Duo version
    if(DEFINED NETXDUO_VERSION)
        if(NOT "${NETXDUO_VERSION}" STREQUAL "")
            set(NETXDUO_VERSION_EMPTY FALSE)
        endif()
    endif()

    # check if build was requested with a specifc ThreadX NetX Duo version
    if(NETXDUO_VERSION_EMPTY)
        # no ThreadX NetX Duo version actualy specified, must be empty which is fine, we'll default to ThreadX version
        set(NETXDUO_VERSION ${RTOS_VERSION})
    endif()

    if(NO_NETXDUO_SOURCE_FOLDER)
        # no ThreadX NetX Duo source specified, download it from it's repo

        message(STATUS "ThreadX NetX Duo ${NETXDUO_VERSION} from GitHub repo")

        FetchContent_Populate(
            threadx_netxduo
            GIT_REPOSITORY https://github.com/eclipse-threadx/netxduo.git
            GIT_TAG ${NETXDUO_VERSION}
        )

    else()
        # ThreadX NetX Duo source was specified

        # sanity check is source path exists
        if(EXISTS "${NETXDUO_SOURCE_FOLDER}/")
            message(STATUS "ThreadX NetX Duo ${NETXDUO_VERSION} (source from: ${NETXDUO_SOURCE_FOLDER})")

            FetchContent_Populate(
                threadx_netxduo
                SOURCE_DIR ${NETXDUO_SOURCE_FOLDER}
            )

        else()
            message(FATAL_ERROR "Couldn't find ThreadX NetX Duo source at ${NETXDUO_SOURCE_FOLDER}/")
        endif()

    endif()

    # enable BSD layer in NetX Duo
    set(NXD_ENABLE_BSD TRUE)

endif()

##############################
# network connectivity options
nf_set_network_connectivity_options()

#############################################
# try to find STM board in the targets folder
# and set target base location and base path for class libs
# this has to be set before the class library modules are pulled in
if(EXISTS ${PROJECT_SOURCE_DIR}/targets/ThreadX/ST/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${PROJECT_SOURCE_DIR}/targets/ThreadX/ST/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/ThreadX/ST/${TARGET_BOARD})

    if(THREADX_CHIBIOS_HAL_REQUIRED)
        set(CHIBIOS_BOARD_DEFINITIONS_PATH ${PROJECT_SOURCE_DIR}/targets/ThreadX/ST/${TARGET_BOARD} CACHE INTERNAL "Path of board definition files")
    endif()

    set(TARGET_VENDOR ST)

# try to find Nordic board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/ThreadX/Nordic/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${PROJECT_SOURCE_DIR}/targets/ThreadX/Nordic/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/ThreadX/Nordic/${TARGET_BOARD})

    set(TARGET_VENDOR Nordic)

# try to find MicroBit board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/ThreadX/MicroBit/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${PROJECT_SOURCE_DIR}/targets/ThreadX/MicroBit/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/ThreadX/MicroBit/${TARGET_BOARD})

    set(TARGET_VENDOR MicroBit)

# try to find Raspberry Pi Pico board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/ThreadX/RaspberryPi/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${PROJECT_SOURCE_DIR}/targets/ThreadX/RaspberryPi/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/ThreadX/RaspberryPi/${TARGET_BOARD})

    set(TARGET_VENDOR RaspberryPi)

# try to find Maxim board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/ThreadX/Maxim/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${PROJECT_SOURCE_DIR}/targets/ThreadX/Maxim/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/ThreadX/Maxim/${TARGET_BOARD})

    set(TARGET_VENDOR Maxim)

# try to find SiliconLabs board in the targets folder
elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets/ThreadX/SiliconLabs/${TARGET_BOARD})

    # board found
    message(STATUS "Support for target board '${TARGET_BOARD}' found")

    # Define base path for the class libraries
    nf_set_base_path_for_libraries_modules(${PROJECT_SOURCE_DIR}/targets/ThreadX/SiliconLabs/_nanoCLR)

    # set target base location
    set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets/ThreadX/SiliconLabs/${TARGET_BOARD})

    set(TARGET_VENDOR SiliconLabs)

else()

    # try to find STM board in the Community targets folder
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/ThreadX/ST/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        nf_set_base_path_for_libraries_modules(${PROJECT_SOURCE_DIR}/targets/ThreadX/ST/_nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets-community/ThreadX/ST/${TARGET_BOARD})

        if(THREADX_CHIBIOS_HAL_REQUIRED)
            set(CHIBIOS_BOARD_DEFINITIONS_PATH ${PROJECT_SOURCE_DIR}/targets-community/ThreadX/ST/${TARGET_BOARD} CACHE INTERNAL "Path of board definition files")
        endif()

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ST)

        # add TARGET board directory from Community
        add_subdirectory(${PROJECT_SOURCE_DIR}/targets-community/ThreadX/ST/${TARGET_BOARD})

    # try to find Nordic board in the Community targets folder
    elseif(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/ThreadX/Nordic/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")

        # Define base path for the class libraries
        nf_set_base_path_for_libraries_modules(${PROJECT_SOURCE_DIR}/targets/ThreadX/Nordic/_nanoCLR)

        # set target base location
        set(TARGET_BASE_LOCATION ${PROJECT_SOURCE_DIR}/targets-community/ThreadX/Nordic/${TARGET_BOARD})

        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Nordic)

        # add TARGET board directory from Community
        add_subdirectory(${PROJECT_SOURCE_DIR}/targets-community/ThreadX/Nordic/${TARGET_BOARD})

    else()

        # board NOT found in targets folder
        message(FATAL_ERROR "\n\nSorry but support for ${TARGET_BOARD} target is not available...\n\nYou can wait for it to be added or you might want to contribute and start working on a PR for it.\n\n")

    endif()

endif()

set(TARGET_BASE_LOCATION_BINARY ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_VENDOR}/${TARGET_BOARD})
set(TX_USER_FILE ${TARGET_BASE_LOCATION}/target_tx_user.h CACHE STRING "Enable TX user configuration")

##############################
# ThreadX common directories
add_subdirectory(_common)
add_subdirectory(_include)

# add TARGET vendor directory
add_subdirectory(${TARGET_VENDOR})
# add TARGET board directory
add_subdirectory(${TARGET_VENDOR}/${TARGET_BOARD})
