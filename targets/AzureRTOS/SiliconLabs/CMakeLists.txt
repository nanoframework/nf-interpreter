#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(Gecko_SDK)

set(SILABS_GECKO_SDK_REQUIRED TRUE PARENT_SCOPE)

option(GECKO_FEATURE_USBD_HID "option to use Gecko SDK HID class")
option(GECKO_FEATURE_USBD_WINUSB "option to use Gecko SDK USB vendor class for WinUSB")
option(GECKO_DEVICE_CLASS_VENDOR_DESCRIPTION_LENGTH "option to override Device Class Vendor description")

#################################################################
# Options for Wire Protocol channel
option(HAL_WP_USE_SERIAL "option to use serial port (UART) to route Wire Protocol")
option(HAL_WP_USE_USB_CDC "option to use USB CDC to route Wire Protocol")

if(HAL_WP_USE_SERIAL)
    message(STATUS "Wire Protocol will use serial port (UART).")
    set(HAL_WP_USE_SERIAL_OPTION TRUE CACHE INTERNAL "set HAL_WP_USE_SERIAL")
    set(HAL_WP_USE_USB_CDC_OPTION FALSE CACHE INTERNAL "set HAL_WP_USE_USB_CDC")
elseif(HAL_WP_USE_USB_CDC)
    message(STATUS "Wire Protocol will use USB CDC.")
    set(HAL_WP_USE_USB_CDC_OPTION TRUE CACHE INTERNAL "set HAL_WP_USE_USB_CDC")
    set(HAL_WP_USE_SERIAL_OPTION FALSE CACHE INTERNAL "set HAL_WP_USE_SERIAL")
else()
    message(FATAL_ERROR "\n\nERROR: need to choose a channel for Wire Protocol. One of the build options HAL_WP_USE_SERIAL_OPTION or HAL_WP_USE_USB_CDC_OPTION have to be 'ON'.")
endif()

ProcessGSDKPackage()

if(GECKO_FEATURE_USBD_HID)

    set(GECKO_FEATURE_USBD_HID_OPTION TRUE CACHE INTERNAL "GECKO_FEATURE_USBD_HID for USBD HID")

else()
    set(GECKO_FEATURE_USBD_HID_OPTION FALSE CACHE INTERNAL "GECKO_FEATURE_USBD_HID for USBD HID")
endif()

if(API_System.Device.UsbStream)

    set(GECKO_FEATURE_USBD_WINUSB_OPTION TRUE CACHE INTERNAL "GECKO_FEATURE_USBD_WINUSB for WinUSB")
    set(GECKO_FEATURE_USBD_WINUSB ON PARENT_SCOPE)

else()
    set(GECKO_FEATURE_USBD_WINUSB_OPTION FALSE CACHE INTERNAL "GECKO_FEATURE_USBD_WINUSB for WinUSB")
endif()


# check if GECKO_SDK_SOURCE was specified or if it's empty (default is empty)
set(NO_GECKO_SDK_SOURCE TRUE)
if(GECKO_SDK_SOURCE)
    if(NOT ${GECKO_SDK_SOURCE} STREQUAL "")
        set(NO_GECKO_SDK_SOURCE FALSE)
    endif()
endif()

# Gecko SDK version
set(GECKO_SDK_VERSION_EMPTY TRUE)

# check if build was requested with a specifc Gecko SDK version
if(DEFINED GECKO_SDK_VERSION)
    if(NOT "${GECKO_SDK_VERSION}" STREQUAL "")
        set(GECKO_SDK_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc Gecko SDK version
if(GECKO_SDK_VERSION_EMPTY)
    # no Gecko SDK version actualy specified, must be empty which is fine, we'll default to a known good version
    # WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
    set(GECKO_SDK_VERSION "v4.4.6")
endif()

if(NO_GECKO_SDK_SOURCE)
    # no Gecko SDK source specified, download it from it's repo
    message(STATUS "Gecko SDK is: ${GECKO_SDK_VERSION} from official repo")

    FetchContent_Declare(
        gecko_sdk
        GIT_REPOSITORY https://github.com/SiliconLabs/gecko_sdk.git
        GIT_TAG ${GECKO_SDK_VERSION}
        GIT_SHALLOW 1
    )

else()
    # Gecko SDK source was specified

    # sanity check is source path exists
    if(EXISTS ${GECKO_SDK_SOURCE}/)
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)

            message(STATUS "Gecko SDK is: ${GECKO_SDK_VERSION} (source from: ${GECKO_SDK_SOURCE})")
            FetchContent_Declare(
                gecko_sdk
                SOURCE_DIR ${GECKO_SDK_SOURCE}
            )

        else()
            message(STATUS "Gecko SDK is: ${GECKO_SDK_VERSION} (source from: ${GECKO_SDK_SOURCE})")

            FetchContent_Declare(
                gecko_sdk
                SOURCE_DIR ${GECKO_SDK_SOURCE}
            )

        endif()
    else()
        message(FATAL_ERROR "Couldn't find Gecko SDK source at ${GECKO_SDK_SOURCE}/")
    endif()

endif()

FetchContent_GetProperties(gecko_sdk)
FetchContent_MakeAvailable(gecko_sdk)

# including here the CMake files for the source files specific to the target series
include(AzureRTOS_${TARGET_SERIES}_sources)

list(APPEND TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/_include)

# make vars global
set(TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")

# add platform dirs
add_subdirectory(_include)
add_subdirectory(_common)
add_subdirectory(_nanoBooter)
add_subdirectory(_nanoCLR)
