#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.15)

include(CMakeToolsHelpers OPTIONAL)
include(ExternalProject)

if (BUILD_VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE) # debug helper
endif()

# the following prevents launchin a build in the source tree
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# fatal error and message explaining this
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(STATUS "\n-- ###############################################\n")
    message(STATUS "Please run the build outside of the source tree.\n\n")
    message(STATUS "Hint: create a 'build' folder and run CMake from there..")
    message(STATUS "###############################################\n\n")
    message(FATAL_ERROR "Build launched in the source tree.")
endif()
#########################################


########################################################
# path to local CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
########################################################

include(nf_utils)

######################################################
# set build type to release if not specified otherwise
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
######################################################

# add compiler define for DEBUG
add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")

######################################################
# set time stamp of build
string(TIMESTAMP BUILD_TIMESTAMP UTC)
######################################################

#######################
# handle RTOSes choice

if(${RTOS} STREQUAL "")
    # no RTOS selected, defaulting to ChibiOS
    set(RTOS "CHIBIOS")
endif()

# list of supported RTOSes
# when adding a new RTOS add the corresponding string below AND...
# ... also on the compare block below
set(RTOS_SUPPORTED "CHIBIOS" "FREERTOS" "FREERTOS_ESP32" "TI_SIMPLELINK" CACHE INTERNAL "supported RTOS options")

list(FIND RTOS_SUPPORTED ${RTOS} RTOS_INDEX)
if(RTOS_INDEX EQUAL -1)
    message(FATAL_ERROR "\n\n'${RTOS}' is an invalid option for RTOS. Please choose a valid RTOS.\n\n")
else()
    message(STATUS "\nSetting RTOS to ${RTOS}\n")
endif()

string(COMPARE EQUAL "CHIBIOS" ${RTOS}  RTOS_CHIBIOS_CHECK)
string(COMPARE EQUAL "FREERTOS" ${RTOS} RTOS_FREERTOS_CHECK)
string(COMPARE EQUAL "FREERTOS_ESP32" ${RTOS} RTOS_FREERTOS_ESP32_CHECK)
string(COMPARE EQUAL "TI_SIMPLELINK" ${RTOS} RTOS_TI_SIMPLELINK_CHECK)

##########################################################################

if(RTOS_FREERTOS_CHECK)
    set(RTOS_FREERTOS_CHECK TRUE)
    set(RTOS_FREERTOS_ESP32_CHECK FALSE)
    set(RTOS_TI_SIMPLELINK_CHECK FALSE)
    set(RTOS_CHIBIOS_CHECK FALSE)
endif()

if(RTOS_FREERTOS_ESP32_CHECK)
    set(RTOS_FREERTOS_CHECK FALSE)
    set(RTOS_FREERTOS_ESP32_CHECK TRUE)
    set(RTOS_TI_SIMPLELINK_CHECK FALSE)
    set(RTOS_CHIBIOS_CHECK FALSE)
endif()

if(RTOS_CHIBIOS_CHECK)
    set(RTOS_CHIBIOS_CHECK TRUE)
    set(RTOS_FREERTOS_CHECK FALSE)
    set(RTOS_FREERTOS_ESP32_CHECK FALSE)
    set(RTOS_TI_SIMPLELINK_CHECK FALSE)
endif()

if(RTOS_TI_SIMPLELINK_CHECK)
    set(RTOS_TI_SIMPLELINK_CHECK TRUE)
    set(RTOS_FREERTOS_CHECK FALSE)
    set(RTOS_FREERTOS_ESP32_CHECK FALSE)
    set(RTOS_CHIBIOS_CHECK FALSE)
endif()

#######################

# check for toolchain path (this is required for Windows builds and we are only building on Windows)
if(NOT DEFINED TOOLCHAIN_PREFIX)
        message(STATUS "\n-- ########################################\nNo TOOLCHAIN_PREFIX specified, need one!\nCall CMake with -DTOOLCHAIN_PREFIX=\"<path_to_your_gcc_toolchain>\"\n specifing the path to your GCC toolchain (ex: E:/GNU_Tools_ARM_Embedded/5_4_2016q3)")
        message(STATUS "\nNOTE: mind the forward slash in the path, without trailing slash.)")
        message(STATUS "########################################\n\n")
        message(FATAL_ERROR "No TOOLCHAIN_PREFIX specified")
endif()

# Set up short path substitution on Windows
if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
    if(EXISTS ${CMAKE_SOURCE_DIR}/SetNFRoot.bat)
        execute_process(COMMAND ${CMAKE_SOURCE_DIR}/SetNFRoot.bat OUTPUT_VARIABLE NFROOT OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(ENV{nfRoot} ${NFROOT})
    endif()
endif()

#########################################

# set default version
if(NOT BUILD_VERSION)
    set(BUILD_VERSION 0.0.0.0)
endif()
project(nanoFramework VERSION ${BUILD_VERSION})

set(NANOBOOTER_PROJECT_NAME "nanoBooter")
set(NANOCLR_PROJECT_NAME "nanoCLR")

#######################
message(STATUS "")
message(STATUS "Building nanoFramework version ${PROJECT_VERSION} using build type '${CMAKE_BUILD_TYPE}'.")
message(STATUS "Source directory is '${CMAKE_SOURCE_DIR}'.")
message(STATUS "Build  directory is '${CMAKE_BINARY_DIR}'.")
message(STATUS "")
#######################


#################################################################
# ouput RTM build option
# Build RTM version of firmware (default is OFF so the build is not RTM and the CLR outputs some debug informations)
option(NF_BUILD_RTM "option to build with RTM definition")

if(NF_BUILD_RTM)
    message(STATUS "***************************")
    message(STATUS "** Building RTM firmware **")
    message(STATUS "***************************")
    message(STATUS "")
endif()
#################################################################

if(RTOS_CHIBIOS_CHECK OR RTOS_TI_SIMPLELINK_CHECK)

    #################################################################
    # clear some CMake flavor flags that are being set as default
    # in the GNU compiler init
    # we want to control and fine tune these
    set(CMAKE_C_FLAGS_DEBUG "" CACHE INTERNAL "clear c compiler flags")
    set(CMAKE_C_FLAGS_MINSIZEREL "" CACHE INTERNAL "clear c compiler flags")
    set(CMAKE_C_FLAGS_RELEASE "" CACHE INTERNAL "clear c compiler flags")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "" CACHE INTERNAL "clear c compiler flags")
    set(CMAKE_CXX_FLAGS_DEBUG "" CACHE INTERNAL "clear c++ compiler flags")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "" CACHE INTERNAL "clear c++ compiler flags")
    set(CMAKE_CXX_FLAGS_RELEASE "" CACHE INTERNAL "clear c++ compiler flags")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "" CACHE INTERNAL "clear c++ compiler flags")
    #################################################################


    #################################################################
    # clear default libraries that are set by CMake
    # we want to control
    set(CMAKE_C_IMPLICIT_LINK_LIBRARIES "" CACHE INTERNAL "clear c linker default libs")
    set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "" CACHE INTERNAL "clear c++ linker default libs")
    #################################################################

endif()

#################################################################################
# chooses if double-point float point support is provided by the platform
# (default is OFF which means that single-precision floating point is effective)
option(DP_FLOATINGPOINT "option to enable support for double-precision floating point")

if(DP_FLOATINGPOINT)
    set(TARGET_DP_FLOATINGPOINT TRUE CACHE INTERNAL "DP FP support")
    message(STATUS "Double-precision floating point is effective")
else()
    set(TARGET_DP_FLOATINGPOINT FALSE CACHE INTERNAL "DP FP support")
    message(STATUS "Single-precision floating point is effective")
endif()

#################################################################################
# chooses if build without complex math functions
# (default is OFF which means that all floating point math functions are available)
option(NF_FEATURE_LIGHT_MATH "option to build without complex math functions")

if(NF_FEATURE_LIGHT_MATH)
    set(TARGET_LIGHT_MATH TRUE CACHE INTERNAL "Complex Math functions disabled")
    message(STATUS "Complex math functions not available")
else()
    set(TARGET_LIGHT_MATH FALSE CACHE INTERNAL "Complex Math functions enabled")
    message(STATUS "Complex math functions available")
endif()

###################################################################################
# chooses if platform provides support for converting string to values on any base
# (default is OFF which means that conversion to value from base 10 and 16 (partial) is effective)
option(SUPPORT_ANY_BASE_CONVERSION "option to enable support for converting strings to value on any base")

if(SUPPORT_ANY_BASE_CONVERSION)
    set(TARGET_SUPPORT_ANY_BASE_CONVERSION TRUE CACHE INTERNAL "Option for string conversion to value from any base")
    message(STATUS "String conversion to value on any base is effective")
else()
    set(TARGET_SUPPORT_ANY_BASE_CONVERSION FALSE CACHE INTERNAL "Option for string conversion to value from base 10 and partial for 16")
    message(STATUS "String conversion to value from base 10 and 16 (partial) is effective")
endif()

#################################################################
# true random number generation option supported by hardware
option(USE_RNG "option to enable use of true random generator hardware block" ON)

if(USE_RNG)
    message(STATUS "Random Number Generation by target MCU is activated")
else()
    message(STATUS "Random Number Generation by target MCU is NOT activated")
endif()

#################################################################
# nanoFramework features
#################################################################

###################################################################
# debugger (default is OFF so no support for debugging is included
option(NF_FEATURE_DEBUGGER "option to enable support for debugging")

if(NF_FEATURE_DEBUGGER)
    message(STATUS "Support for debugging managed applications is included.")
else()
    message(STATUS "Support for debugging managed applications IS NOT included.")
endif()

#################################################################
# enables Application Domains support in nanoCLR
# (default is OFF so Application Domains is NOT supported)
option(NF_FEATURE_USE_APPDOMAINS "option to enable Application Domains")

if(NF_FEATURE_USE_APPDOMAINS)
    message(STATUS "Application Domains support is included")
else()
    message(STATUS "Application Domains support IS NOT included")
endif()

#################################################################
# disable all trace messages and checks on CLR
# (default is OFF so all traces and checks are enabled)
option(NF_PLATFORM_NO_CLR_TRACE "option to disable all trace stuff in CLR")

if(NF_PLATFORM_NO_CLR_TRACE)
    message(STATUS "CLR has all trace messages and checks DISABLED")
else()
    message(STATUS "CLR has all trace messages and checks enabled")
endif()

#################################################################
# disable CLR IL inlining
# (default is OFF so CLR IL inline is enabled)
option(NF_CLR_NO_IL_INLINE "option to disable CLR IL inlining")

if(NF_CLR_NO_IL_INLINE)
    message(STATUS "CLR IL inlining is DISABLED")
else()
    message(STATUS "CLR IL inlining is enabled")
endif()

#################################################################
# enables configuration block storage support
# (default is OFF so Configuration block storage is NOT supported)
option(NF_FEATURE_HAS_CONFIG_BLOCK "option to enable configuration block storage")

if(NF_FEATURE_HAS_CONFIG_BLOCK)
    set(TARGET_HAS_CONFIG_BLOCK TRUE CACHE INTERNAL "Option for config block")
    message(STATUS "Configuration block storage is included")
else()
    set(TARGET_HAS_CONFIG_BLOCK FALSE CACHE INTERNAL "Option for config block")
    message(STATUS "Configuration block storage IS NOT included")
endif()

#################################################################
# enables support for SD Card
# (default is OFF so SD Card is NOT supported)
option(NF_FEATURE_HAS_SDCARD "option to enable support for SD Card")

if(NF_FEATURE_HAS_SDCARD)

    # this feature currently is supported only on ChibiOS, iMX.RT FreeRTOS and ESP32 FreeRTOS
    if(NOT RTOS_CHIBIOS_CHECK AND NOT RTOS_FREERTOS_CHECK AND NOT RTOS_FREERTOS_ESP32_CHECK)
        message(FATAL_ERROR "Support for SD Card is only available for ChibiOS Cortex-M targets, iMX.RT FreeRTOS and ESP32 targets..")
    endif()

    # force inclusion of Windows.Storage API
    set(API_Windows.Storage ON CACHE INTERNAL "Forcing Windows.Storage API option to ON")

    # force inclusion of System.IO.FileSystem API
    set(API_System.IO.FileSystem ON CACHE INTERNAL "Forcing System.IO.FileSystem API option to ON")

    message(STATUS "Support for SD Card is included")
else()
    message(STATUS "Support for SD Card IS NOT included")
endif()

#################################################################

#################################################################
# enables USB Mass Storage support
# (default is OFF so USB Mass Storage is NOT supported)
option(NF_FEATURE_HAS_USB_MSD "option to enable USB Mass Storage")

if(NF_FEATURE_HAS_USB_MSD)

    # this feature currently is supported only on ChibiOS
    if(NOT RTOS_CHIBIOS_CHECK)
        message(FATAL_ERROR "Support for USB Mass Storage is only available for ChibiOS Cortex-M targets.")
    endif()

    # this feature requires inclusion of ChibiOS contribution repository
    set(CHIBIOS_CONTRIB_REQUIRED ON CACHE INTERNAL "Forcing ChibiOS contribution repo option to ON")

    # force inclusion of Windows.Storage API
    set(API_Windows.Storage ON CACHE INTERNAL "Forcing Windows.Storage API option to ON")

    # force inclusion of System.IO.FileSystem API
    set(API_System.IO.FileSystem ON CACHE INTERNAL "Forcing System.IO.FileSystem API option to ON")

    message(STATUS "Support for USB Mass Storage is included")
else()
    message(STATUS "Support for USB Mass Storage IS NOT included")
endif()

#################################################################

#################################################################
# enables support for SPI file system
# (default is OFF so SPIFFS is NOT supported)
option(NF_FEATURE_USE_SPIFFS "option to enable support for SPI file system")

if(NF_FEATURE_USE_SPIFFS)

    # this feature currently is supported only on ChibiOS and ESP32
    if(NOT RTOS_CHIBIOS_CHECK)
        message(FATAL_ERROR "Support for SPIFFS is only available for ChibiOS Cortex-M and ESP32 targets.")
    endif()

    # force inclusion of Windows.Storage API
    set(API_Windows.Storage ON CACHE INTERNAL "Forcing Windows.Storage API option to ON")

    set(NF_FEATURE_USE_SPIFFS_OPTION TRUE CACHE INTERNAL "Set nF SPIFFS feature TRUE")

    message(STATUS "Support for SPIFFS is included")
else()
    set(NF_FEATURE_USE_SPIFFS_OPTION FALSE CACHE INTERNAL "Set nF SPIFFS feature FALSE")
    message(STATUS "Support for SPIFFS IS NOT included")
endif()

#################################################################
# target has nanoBooter 
# (default is ON so target device HAS nanoBooter)
option(NF_TARGET_HAS_NANOBOOTER "option to signal if target implements nanoBooter" ON)

if(NF_TARGET_HAS_NANOBOOTER)
    set(TARGET_HAS_NANOBOOTER TRUE CACHE INTERNAL "Option for nanoBooter implementation")
    message(STATUS "Target implements nanoBooter")
else()
    set(TARGET_HAS_NANOBOOTER FALSE CACHE INTERNAL "Option for nanoBooter implementation")
    message(STATUS "Target DOES NOT implement nanoBooter")
endif()

#################################################################

if(RTOS_CHIBIOS_CHECK)

    #################################################################
    # ARM Cortex-M Single Wire Output (SWO)
    # (default is OFF so no SWO output)

    option(SWO_OUTPUT "option to enable SWO")

    if(SWO_OUTPUT)
        set(SWO_OUTPUT_OPTION TRUE CACHE INTERNAL "Single Wire Output Option")
        message(STATUS "Single Wire Output (SWO) enabled")
    else()
        set(SWO_OUTPUT_OPTION FALSE CACHE INTERNAL "Single Wire Output Option")
    endif()

    #################################################################
        
    #################################################################
    # enables use of ChibiOS Community contribution
    # (default is OFF so ChibiOS Community is NOT included)
    option(CHIBIOS_CONTRIB_REQUIRED "option to include ChibiOS Community contributions repository")
    #################################################################

endif()

#################################################################
# (default is ON so Wire Protocol implements CRC32)
option(NF_WP_IMPLEMENTS_CRC32 "option for Wire Protocol to implement CRC32" ON)

# reports Wire Protocol CRC32 implementation
if(NF_WP_IMPLEMENTS_CRC32)
    message(STATUS "Wire Protocol implements CRC32")
else()
    message(STATUS "Wire Protocol does NOT implement CRC32")
endif()

#################################################################
# enables Networking support in nanoCLR
# declares Networking option
# needs to show before the API namespaces processing because it's used there
# (default is OFF so Networking is NOT supported)
option(USE_NETWORKING_OPTION "option to use networking")

# (default is OFF so mbed TLS is NOT used)
option(NF_SECURITY_MBEDTLS "option to use mbed TLS as the network security provider")

# set default option for SNTP to ON
option(NF_NETWORKING_SNTP "option to use add SNTP support, requires networking otherwise has no effect" ON)

# (default is OFF so driver is NOT used)
option(NF_NETWORKING_ENC28J60 "option to use ENC28J60 network driver")

#################################################################

#############################################
# handles inclusion of System.Reflection API
#############################################

# set default option for SNTP to ON
option(NF_FEATURE_SUPPORT_REFLECTION "option to add support for System.Reflection API" ON)

if(NF_FEATURE_SUPPORT_REFLECTION)
    set(TARGET_NANOCLR_REFLECTION TRUE CACHE INTERNAL "enable support for System.Reflection API")
    message(STATUS "Support for System.Reflection API enabled")
else()
    set(TARGET_NANOCLR_REFLECTION FALSE CACHE INTERNAL "DISABLE support for System.Reflection API")
    message(STATUS "Support for System.Reflection API **IS NOT** enabled")
endif()

#################################################################

#################################################################

#############################################
# handles inclusion of System.Collections API
#############################################

if(API_nanoFramework.System.Collections)
    set(TARGET_SYSTEM_COLLECTIONS TRUE CACHE INTERNAL "enable support for System.Collections API")
    message(STATUS "Support for System.Collections API enabled")
else()
    set(TARGET_SYSTEM_COLLECTIONS FALSE CACHE INTERNAL "DISABLE support for System.Collections API")
    message(STATUS "Support for System.Collections API **IS NOT** enabled")
endif()

#################################################################

#############################################
# handles inclusion of System.Text API
#############################################

if(API_nanoFramework.System.Text)
    set(TARGET_SYSTEM_TEXT TRUE CACHE INTERNAL "enable support for System.Text API")
    message(STATUS "Support for System.Text API enabled")
else()
    set(TARGET_SYSTEM_TEXT FALSE CACHE INTERNAL "DISABLE support for System.Text API")
    message(STATUS "Support for System.Text API **IS NOT** enabled")
endif()

#################################################################

#################################################################
# manage HAL/PAL required for API namespaces
#################################################################
# for some APIs we need to enable the device in the HAL config
# and/or manage other APIs that are required

if( API_nanoFramework.Devices.OneWire OR
    API_Windows.Devices.SerialCommunication)

    set(HAL_USE_UART_OPTION TRUE CACHE INTERNAL "HAL OneWire for nanoFramework.Devices.OneWire")

else()
    set(HAL_USE_UART_OPTION FALSE CACHE INTERNAL "HAL OneWire for nanoFramework.Devices.OneWire")
endif()

if( API_System.Net OR
    API_Windows.Networking.Sockets)

    # set NETWORKING option to true
    set(USE_NETWORKING_OPTION TRUE CACHE INTERNAL "NF feature NETWORKING")

    # set Security module to use
    if(NF_SECURITY_MBEDTLS )
        set(USE_SECURITY_MBEDTLS_OPTION TRUE CACHE INTERNAL "NF security MBEDTLS")
    endif()

    if(NF_NETWORKING_ENC28J60)
        set(USE_ENC28J60_DRIVER_OPTION TRUE CACHE INTERNAL "NF ENC28J60 network driver")
    else()
        set(USE_ENC28J60_DRIVER_OPTION FALSE CACHE INTERNAL "NF ENC28J60 network driver")
    endif()

   if(API_Windows.Devices.Wifi)
        set(USE_WIFI_OPTION TRUE CACHE INTERNAL  "Wifi for Windows.Devices.WiFi")
    else()
        set(USE_WIFI_OPTION FALSE CACHE INTERNAL "Wifi for Windows.Devices.WiFi")
    endif()
else()
    # need to force SNTP to OFF
    set(NF_NETWORKING_SNTP FALSE CACHE INTERNAL "SNTP is off as there is no network support")
endif()


if(API_Windows.Devices.Adc)
    set(HAL_USE_ADC_OPTION TRUE CACHE INTERNAL "HAL ADC for Windows.Devices.Adc")
else()
    set(HAL_USE_ADC_OPTION FALSE CACHE INTERNAL "HAL ADC for Windows.Devices.Adc")
endif()


if(API_System.Device.Dac)
    set(HAL_USE_DAC_OPTION TRUE CACHE INTERNAL "HAL DAC for System.Device.Dac")
else()
    set(HAL_USE_DAC_OPTION FALSE CACHE INTERNAL "HAL DAC for System.Device.Dac")
endif()


if(API_System.Device.Gpio)
    set(HAL_USE_GPIO_OPTION TRUE CACHE INTERNAL "HAL GPIO for System.Device.Gpio")
else()
    set(HAL_USE_GPIO_OPTION FALSE CACHE INTERNAL "HAL GPIO for System.Device.Gpio")
endif()


if(API_Windows.Devices.Gpio)
    set(HAL_USE_GPIO_OPTION TRUE CACHE INTERNAL "HAL GPIO for Windows.Devices.Gpio")
else()
    set(HAL_USE_GPIO_OPTION FALSE CACHE INTERNAL "HAL GPIO for Windows.Devices.Gpio")
endif()


if(API_System.Device.I2c)
    set(HAL_USE_I2C_OPTION TRUE CACHE INTERNAL "HAL I2C for System.Device.I2c")
else()
    set(HAL_USE_I2C_OPTION FALSE CACHE INTERNAL "HAL I2C for System.Device.I2c")
endif()


if(API_Windows.Devices.I2c)
    set(HAL_USE_I2C_OPTION TRUE CACHE INTERNAL "HAL I2C for Windows.Devices.I2c")
else()
    set(HAL_USE_I2C_OPTION FALSE CACHE INTERNAL "HAL I2C for Windows.Devices.I2c")
endif()


if(API_Windows.Devices.Pwm)
    set(HAL_USE_PWM_OPTION TRUE CACHE INTERNAL "HAL PWM for Windows.Devices.Pwm")
else()
    set(HAL_USE_PWM_OPTION FALSE CACHE INTERNAL "HAL PWM for Windows.Devices.Pwm")
endif()


if(API_Windows.Devices.Spi)
    set(HAL_USE_SPI_OPTION TRUE CACHE INTERNAL "HAL SPI for Windows.Devices.Spi")
else()
    set(HAL_USE_SPI_OPTION FALSE CACHE INTERNAL "HAL SPI for Windows.Devices.Spi")
endif()


if(API_nanoFramework.Devices.Can)
    set(HAL_USE_CAN_OPTION TRUE CACHE INTERNAL "HAL CAN for nanoFramework.Devices.Can")
else()
    set(HAL_USE_CAN_OPTION FALSE CACHE INTERNAL "HAL CAN for nanoFramework.Devices.Can")
endif()


if(API_nanoFramework.Devices.OneWire)
    set(HAL_USE_STM32_ONEWIRE_OPTION TRUE CACHE INTERNAL "HAL STM32_ONEWIRE for nanoFramework.Devices.OneWire")
else()
    set(HAL_USE_STM32_ONEWIRE_OPTION FALSE CACHE INTERNAL "HAL STM32_ONEWIRE for nanoFramework.Devices.OneWire")
endif()

#################################################################################
#  Option to include the graphics bitmap primitives
#################################################################################
if(API_nanoFramework.Graphics)
    set(NANOCLR_GRAPHICS TRUE CACHE INTERNAL "Graphics Support")
else()
    set(NANOCLR_GRAPHICS FALSE CACHE INTERNAL "No Graphics Support")
endif()

if(NF_FEATURE_HAS_SDCARD)
    set(HAL_USE_SDC_OPTION TRUE CACHE INTERNAL "HAL SDC for NF_FEATURE_HAS_SDCARD")
else()
    set(HAL_USE_SDC_OPTION FALSE CACHE INTERNAL "HAL SDC for NF_FEATURE_HAS_SDCARD")
endif()

if(NF_FEATURE_HAS_USB_MSD)
    set(HAL_USBH_USE_MSD_OPTION TRUE CACHE INTERNAL "HAL USBH_USE_MSD for NF_FEATURE_HAS_USB_MSD")
else()
    set(HAL_USBH_USE_MSD_OPTION FALSE CACHE INTERNAL "HAL USBH_USE_MSD for NF_FEATURE_HAS_USB_MSD")
endif()

#################################################################
# manage dependent APIs required for some API namespaces
#################################################################

# include nanoFramework.Runtime.Events API
if( API_nanoFramework.Devices.OneWire OR 
    API_System.Net OR
    API_System.Device.Gpio OR
    API_Windows.Devices.Gpio OR
    API_Windows.Devices.SerialCommunication OR
    API_Windows.Networking.Sockets OR
    API_nanoFramework.Devices.Can OR
    API_Windows.Storage OR
    API._System.IO.FileSystem)

    # these APIs requires nanoFramework.Runtime.Events
    set(API_nanoFramework.Runtime.Events ON CACHE INTERNAL "enable of API_nanoFramework.Runtime.Events")

endif()

if(API_System.Net)
    # manage inclusion of SNTP
    if(NF_NETWORKING_SNTP)
        set(API_nanoFramework.Networking.Sntp ON CACHE INTERNAL "enable API_nanoFramework.Networking.Sntp")
    else()
        set(API_nanoFramework.Networking.Sntp OFF CACHE INTERNAL "disable API_nanoFramework.Networking.Sntp")
    endif()
endif()


#################################################################
# handles Networking support at HAL level

if(USE_NETWORKING_OPTION)
    set(HAL_USE_MAC_OPTION TRUE CACHE INTERNAL "HAL MAC for USE_NETWORKING_OPTION")

    if(NF_SECURITY_MBEDTLS)
        message(STATUS "Support for networking enabled with security from mbedTLS")
    elseif(RTOS_TI_SIMPLELINK_CHECK)
        message(STATUS "Support for networking enabled with security")
    else()
        message(STATUS "Support for networking enabled WITHOUT security")
    endif()

    if(NF_NETWORKING_ENC28J60)
        message(STATUS "Support for ENC28j60 network driver enabled")
    endif()

    # sanity check for missing configuration block option
    # which is required for network

    if(NOT NF_FEATURE_HAS_CONFIG_BLOCK)
        message(FATAL_ERROR "\n\nERROR: network build requires NF_FEATURE_HAS_CONFIG_BLOCK build option to be 'ON'. Make sure you have that on your cmake-variants or in the build command line.")
    endif()

else()
    set(HAL_USE_MAC_OPTION FALSE CACHE INTERNAL "HAL MAC for USE_NETWORKING_OPTION")
    message(STATUS "Support for networking IS NOT enabled")
endif()

#################################################################


#################################################################
# enables filesysytem support in nanoCLR

if(NF_FEATURE_HAS_SDCARD OR NF_FEATURE_HAS_USB_MSD)
    set(USE_FILESYSTEM_OPTION TRUE CACHE INTERNAL "NF feature FILESYSTEM")
else()
    set(USE_FILESYSTEM_OPTION FALSE CACHE INTERNAL "NF feature FILESYSTEM")
endif()

#################################################################


#################################################################
# RTC (real time clock) (default is OFF so RTC is NOT included)
option(NF_FEATURE_RTC "option to use hardware RTC")

if(NF_FEATURE_RTC)
    set(HAL_USE_RTC_OPTION TRUE CACHE INTERNAL "NF feature RTC")
else()
    set(HAL_USE_RTC_OPTION FALSE CACHE INTERNAL "NF feature RTC")
endif()

#################################################################

#################################################################
# CPU watchdog (default is ON so watchdog is included)
option(NF_FEATURE_WATCHDOG "option to use hardware watchdog" ON)

if(NF_FEATURE_WATCHDOG)
    set(HAL_USE_WDG_OPTION TRUE CACHE INTERNAL "NF feature watchdog")
else()    
    set(HAL_USE_WDG_OPTION FALSE CACHE INTERNAL "NF feature watchdog")
endif()

#################################################################

#################################################################
# Baudrate for serial communication 

if(TARGET_SERIAL_BAUDRATE)
    message(STATUS "Baudrate for serial communication was set to ${TARGET_SERIAL_BAUDRATE}.")
endif()

#################################################################

#################################################################
# Override target name on device properties

if(TARGET_NAME)
    message(STATUS "Target name set to ${TARGET_NAME}.")
else()
    # use target board as the name
    set(TARGET_NAME ${TARGET_BOARD})
endif()

#################################################################

#######################
# ChibiOS
if(RTOS_CHIBIOS_CHECK)

    # Define base path for the class libraries
    SET_BASE_PATH_FOR_LIBRARIES_MODULES(${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nanoCLR)
    
    add_subdirectory(targets/CMSIS-OS/ChibiOS)
    add_subdirectory(targets/CMSIS-OS/common)
    add_subdirectory(targets/CMSIS-OS/common/Include)

    # now add the subdirectory for the board
    # try to find board in the targets folder
    if(EXISTS ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found")
        message(STATUS "${CHIBIOS_BOARD_DEFINITIONS_LOCATION}")

        # add TARGET board directory
        add_subdirectory(${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${TARGET_BOARD})

    else()

        # try to find board in the Community targets folder
        if(EXISTS ${CMAKE_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${TARGET_BOARD})
            # board found
            message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")
            message(STATUS "${CHIBIOS_BOARD_DEFINITIONS_LOCATION}")

            # add TARGET board directory from Community
            add_subdirectory(targets-community/CMSIS-OS/ChibiOS/${TARGET_BOARD})
        else()
        # board NOT found in targets folder
            # board NOT found in targets folder
            message(FATAL_ERROR "\n\nSorry but support for ${TARGET_BOARD} target is not available...\n\You can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
        endif()

    endif()

#######################
# FreeRTOS
elseif(RTOS_FREERTOS_CHECK)

    add_subdirectory(targets/FreeRTOS)

#######################
# FreeRTOS ESP32
elseif(RTOS_FREERTOS_ESP32_CHECK)

    if(${TARGET_SERIES} STREQUAL "ESP32")
        message(STATUS "Platform ESP32 target board: ${TARGET_BOARD}")

        # check watchdog feature
        # EPS32 build has watchdog enabled by default, so setting it to OFF doesn't make sense 
        # becuase the build system can't honour that preference
        if(NOT NF_FEATURE_WATCHDOG)
        message(FATAL_ERROR "\n\nESP32 watchdog is enabled by default in so you can't have the NF_FEATURE_WATCHDOG option set to OFF.\n\n")
        endif()

        # Define base path for the class libraries
        SET_BASE_PATH_FOR_LIBRARIES_MODULES(${CMAKE_SOURCE_DIR}/targets/FreeRTOS_ESP32/${TARGET_BOARD}/nanoCLR)

        add_subdirectory(targets/FreeRTOS_ESP32)

        # set target base location
        set(TARGET_BASE_LOCATION ${CMAKE_SOURCE_DIR}/targets/FreeRTOS_ESP32/${TARGET_BOARD})
 		add_subdirectory(targets/FreeRTOS_ESP32/${TARGET_BOARD})

    endif()

#######################
# TI_SIMPLELINK
elseif(RTOS_TI_SIMPLELINK_CHECK)

    # Define base path for the class libraries
    SET_BASE_PATH_FOR_LIBRARIES_MODULES(${CMAKE_SOURCE_DIR}/targets/TI-SimpleLink/nanoCLR)

    # now add the subdirectory for the board
    # try to find board in the targets folder
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/targets/TI-SimpleLink/${TARGET_BOARD})

        # set target base location
        set(TARGET_BASE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/targets/TI-SimpleLink/${TARGET_BOARD})

        # board found
        message(STATUS "Support for target board '${TARGET_BOARD}' found")
        message(STATUS "${TARGET_BASE_LOCATION}")

    else()

        # try to find board in the Community targets folder
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/targets-community/TI-SimpleLink/${TARGET_BOARD})

            # set target base location
            set(TARGET_BASE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/targets-community/TI-SimpleLink/${TARGET_BOARD})
        
            # board found
            message(STATUS "Support for target board '${TARGET_BOARD}' found in Community targets")
            message(STATUS "${TARGET_BASE_LOCATION}")

        else()
        # board NOT found in targets folder
            # board NOT found in targets folder
            message(FATAL_ERROR "\n\nSorry but support for ${TARGET_BOARD} target is not available...\n\You can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
        endif()

    endif()
    
    # add common directory for SimpleLink
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/targets/TI-SimpleLink)
    # add TARGET board directory
    add_subdirectory(${TARGET_BASE_LOCATION})

endif()
