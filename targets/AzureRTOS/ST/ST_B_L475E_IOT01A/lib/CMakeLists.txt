# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

# Define ThreadX user configuration
set(TX_USER_FILE "${CMAKE_CURRENT_LIST_DIR}/threadx/tx_user.h" CACHE STRING "Enable TX user configuration")

# Define NetXDuo user configuration
set(NX_USER_FILE "${CMAKE_CURRENT_LIST_DIR}/netxduo/nx_user.h" CACHE STRING "Enable NX user configuration")
#set(NXD_ENABLE_AZURE_IOT ON CACHE BOOL "Enable Azure IoT")
set(NXD_ENABLE_AZURE_IOT OFF CACHE BOOL "Enable Azure IoT")
set(NXD_ENABLE_FILE_SERVERS OFF CACHE BOOL "Disable fileX dependency by netxduo")

# Azure Security Center
set(NX_AZURE_DISABLE_IOT_SECURITY_MODULE OFF CACHE BOOL "Security Center")

# Core libraries
# was: add_subdirectory(${CORE_LIB_DIR}/threadx threadx)
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
    # no AzureRTOS ThreadX source specified, download it from it's repo

    message(STATUS "RTOS is: ${RTOS} ThreadX ${RTOS_VERSION} from GitHub repo")

    FetchContent_Declare(
        azure_rtos
        GIT_REPOSITORY https://github.com/azure-rtos/threadx.git
        GIT_TAG ${RTOS_VERSION}
    )

else()
    # AzureRTOS ThreadX source was specified

    # sanity check is source path exists
    if(EXISTS "${RTOS_SOURCE_FOLDER}/")
        message(STATUS "RTOS is: ${RTOS} ThreadX ${RTOS_VERSION} (source from: ${RTOS_SOURCE_FOLDER})")

        FetchContent_Declare(
            azure_rtos
            GIT_REPOSITORY $(RTOS_SOURCE_FOLDER)
            GIT_TAG ${RTOS_VERSION}
        )

    else()
        message(FATAL_ERROR "Couldn't find ${RTOS} ThreadX source at ${RTOS_SOURCE_FOLDER}/")
    endif()

endif()

FetchContent_MakeAvailable(azure_rtos)

# check if RTOS_NETX_SOURCE_FOLDER was specified or if it's empty (default is empty)
set(NO_RTOS_NETX_SOURCE_FOLDER TRUE)
if(RTOS_NETX_SOURCE_FOLDER)
    if(NOT "${RTOS_NETX_SOURCE_FOLDER}" STREQUAL "")
        set(NO_RTOS_NETX_SOURCE_FOLDER FALSE)
    endif()
endif()

if(NO_RTOS_NETX_SOURCE_FOLDER)
    # no AzureRTOS NetxDuo source specified, download it from it's repo

    message(STATUS "RTOS Net is: ${RTOS} Netxduo ${RTOS_VERSION} from GitHub repo")

    FetchContent_Declare(
        azure_rtos_netxduo
        GIT_REPOSITORY https://github.com/azure-rtos/netxduo.git
        GIT_TAG ${RTOS_VERSION}
    )

else()
    # AzureRTOS NetxDuo source was specified

    # sanity check is source path exists
    if(EXISTS "${RTOS_NETX_SOURCE_FOLDER}/")
        message(STATUS "RTOS Net is: ${RTOS} Netxduo ${RTOS_VERSION} (source from: ${RTOS_SOURCE_FOLDER})")

        FetchContent_Declare(
            azure_rtos_netxduo
            GIT_REPOSITORY $(RTOS_NETX_SOURCE_FOLDER)
            GIT_TAG ${RTOS_VERSION}
        )

    else()
        message(FATAL_ERROR "Couldn't find ${RTOS} source at ${RTOS_SOURCE_FOLDER}/")
    endif()

endif()

# Using a local copy of NetXDuo that has been customized for this boards WiFi module
# not this custom library references some files from the main netxduo repository
if(NOT azure_rtos_netxduo_POPULATED)
  FetchContent_Populate(azure_rtos_netxduo)
endif()

add_subdirectory(netxduo)
add_subdirectory(netx_driver)

