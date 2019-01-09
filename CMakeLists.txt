#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.11)
include(CMakeToolsHelpers OPTIONAL)
include(ExternalProject)

 include(${CMAKE_SOURCE_DIR}/CMakeUserFunctions.cmake)

 set(CMAKE_VERBOSE_MAKEFILE 1)  # debug helper

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
########################################################


######################################################
# set build type to release if not specified otherwise
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
######################################################

######################################################
# set time stamp of build
string(TIMESTAMP BUILD_TIMESTAMP UTC)
######################################################

#######################
# handle RTOSes choice

if("${RTOS}" STREQUAL "")
    # no RTOS selected, defaulting to ChibiOS
    set(RTOS "CHIBIOS")
endif()

# list of supported RTOSes
# when adding a new RTOS add the corresponding string bellow AND...
# ... also on the compare block bellow
set(RTOS_SUPPORTED "CHIBIOS" "FREERTOS" CACHE INTERNAL "supported RTOS options")

list(FIND RTOS_SUPPORTED ${RTOS} RTOS_INDEX)
if(RTOS_INDEX EQUAL -1)
    message(FATAL_ERROR "\n\n'${RTOS}' is an invalid option for RTOS. Please choose a valid RTOS.\n\n")
else()
    message(STATUS "\nSetting RTOS to ${RTOS}\n")
endif()

string(COMPARE EQUAL "CHIBIOS" "${RTOS}"  RTOS_CHIBIOS_CHECK)
string(COMPARE EQUAL "FREERTOS" "${RTOS}" RTOS_FREERTOS_CHECK)

##########################################################################

if(RTOS_FREERTOS_CHECK)
    set(RTOS_FREERTOS_CHECK TRUE)
    set(RTOS_CHIBIOS_CHECK FALSE)
endif()

if(RTOS_CHIBIOS_CHECK)
    set(RTOS_CHIBIOS_CHECK TRUE)
    set(RTOS_FREERTOS_CHECK FALSE)
endif()

#######################


##########################################
# set default toolchain to GCC
set(TOOLCHAIN GCC)
# have it lower case too for file names
string(TOLOWER TOOLCHAIN_LOWER ${TOOLCHAIN})
##########################################

if(RTOS_CHIBIOS_CHECK)

    # check for toolchain path
    if(NOT TOOLCHAIN_PREFIX)
         message(STATUS "\n-- ########################################\nNo TOOLCHAIN_PREFIX specified, need one!\nCall CMake with -DTOOLCHAIN_PREFIX=\"<path_to_your_gcc_toolchain>\"\n specifing the path to your GCC toolchain (ex: E:/GNU_Tools_ARM_Embedded/5_4_2016q3)")
         message(STATUS "\nNOTE: mind the forward slash in the path, without trailing slash.)")
         message(STATUS "########################################\n\n")
         message(FATAL_ERROR "No TOOLCHAIN_PREFIX specified")
    endif()

    message(STATUS "\nSetting Toolchain file for ChibiOS \n")
    # set toolchain file
    set(CMAKE_TOOLCHAIN_FILE CMake/toolchain.ChibiOS.${TOOLCHAIN}.cmake)

elseif(RTOS_FREERTOS_CHECK)
    message(STATUS "\nSetting Toolchain file for FreeRTOS ESP32 \n")
    set(CMAKE_TOOLCHAIN_FILE CMake/toolchain.FreeRtos.ESP32.${TOOLCHAIN}.cmake)

# else()

#     # find out the chip vendor in order to move on with the appropriate configuration
#     string(REGEX MATCH "^[S][T][M]32" CHIP_VENDOR_STM32 "${TARGET_CHIP}")
#     string(COMPARE EQUAL "STM32" "${CHIP_VENDOR_STM32}" CHIP_VENDOR_STM32_CHECK)

#     if(CHIP_VENDOR_STM32_CHECK)
#         # vendor is ST and toolchain is GCC
#         message(STATUS "Chip vendor is ST. Chip is STM32.")

#         # set CMSIS include directories
#         include_directories(STM32CMSIS_INCLUDE_DIRS)

#         # set toolchain file for cross-compiling with CMake
#         # for this vendor/chip it will be
#     # elseif(CHIP_VENDOR_??_CHECK)
#     #
#     #     # vendor is ?? and toolchain is GCC
#     #     message("Chip vendor is ??. Chip is ???.")
#     #     set(CMAKE_TOOLCHAIN_FILE CMake/???.cmake)
#     #
#     else()
#         message(STATUS "\n-- ###############################################\n")
#         message(STATUS "Unknow vendor or chip. Supported vendors/chips:\n-- ST's STM32 (e.g. STM32F407VG)\n")
#         message(STATUS "###############################################\n\n")
#         message(FATAL_ERROR "Unknow vendor or chip in TARGET_CHIP")
#     endif()

#     # set toolchain file
#     set(CMAKE_TOOLCHAIN_FILE CMake/toolchain.${CHIP_VENDOR_STM32}.${TOOLCHAIN}.cmake)
endif()

#########################################

########################################################
# check availability of hex2dfu tool if specified
# only relevant if this is running on a Windows machine
if(WIN32)
    if(NOT "${TOOL_HEX2DFU_PREFIX}" STREQUAL "")
        if(NOT EXISTS ${TOOL_HEX2DFU_PREFIX}/hex2dfu.exe)
            message(STATUS "")
            message(STATUS "Couldn't find the hex2dfu tool at the specified path: ${TOOL_HEX2DFU_PREFIX}/hex2dfu.exe")
            message(STATUS "Make sure that the CMake option TOOL_HEX2DFU_PREFIX has the correct path.")
            message(STATUS "If you don't have this tool download it from https://github.com/nanoframework/nf-tools/releases")
            message(STATUS "")
            message(FATAL_ERROR "hex2dfu tool not found")
        else()
            set(HEX2DFU_TOOL_AVAILABLE TRUE CACHE INTERNAL "hex2dfu tool available")
        endif()
    endif()
endif()


# set default version
if(NOT BUILD_VERSION)
    set(BUILD_VERSION 0.0.0.0)
endif()
project(nanoFramework VERSION ${BUILD_VERSION})

#######################
message(STATUS "")
message(STATUS "Building nanoFramework version ${PROJECT_VERSION} using build type '${CMAKE_BUILD_TYPE}'.")
message(STATUS "Source directory is '${PROJECT_SOURCE_DIR}'.")
message(STATUS "Build  directory is '${PROJECT_BINARY_DIR}'.")
message(STATUS "Toolchain is '${TOOLCHAIN}'.")
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

if(RTOS_CHIBIOS_CHECK)

    #################################################################
    # clear CMAKE_C_FLAGS_INIT and CMAKE_CXX_FLAGS_INIT
    # (this needs to be here and not before because by now
    # the compiler detection has already occurred)
    set(CMAKE_C_FLAGS "" CACHE INTERNAL "clear c compiler flags")
    set(CMAKE_CXX_FLAGS "" CACHE INTERNAL "clear cxx compiler flags")
    #################################################################


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

endif()

#################################################################
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

# (default is OFF so OpenSSL is NOT used)
option(NF_SECURITY_OPENSSL "option to use OpenSSL as the network security provider")

# (default is OFF so mbed TLS is NOT used)
option(NF_SECURITY_MBEDTLS "option to use mbed TLS as the network security provider")

# sanity check for both providers set
if(NF_SECURITY_OPENSSL AND NF_SECURITY_MBEDTLS)
    message(FATAL_ERROR "Can't have both OpenSSL and mbedTLS providers selected. Please choose one or none.")
endif()

# set default option for SNTP to ON
option(NF_NETWORKING_SNTP "option to use add SNTP support, requires networking otherwise has no effect" ON)

#################################################################
# enables filesysytem support in nanoCLR
# (default is OFF so the filesystem is NOT supported)
option(NF_FEATURE_USE_FILESYSTEM "option to use filesystem")

if(NF_FEATURE_USE_FILESYSTEM)
    set(USE_FILESYSTEM_OPTION TRUE CACHE INTERNAL "NF feature FILESYSTEM")
    set(HAL_USE_SDC_OPTION TRUE CACHE INTERNAL "HAL SDC for NF_FEATURE_USE_FILESYSTEM")
    message(STATUS "Support for filesystem enabled")
else()
    set(USE_FILESYSTEM_OPTION FALSE CACHE INTERNAL "NF feature FILESYSTEM")
    set(HAL_USE_SDC_OPTION FALSE CACHE INTERNAL "HAL SDC for NF_FEATURE_USE_FILESYSTEM")
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
    if(NF_SECURITY_OPENSSL)
        set(USE_SECURITY_OPENSSL_OPTION TRUE CACHE INTERNAL "NF security OPENSSL")
    elseif(NF_SECURITY_MBEDTLS )
        set(USE_SECURITY_MBEDTLS_OPTION TRUE CACHE INTERNAL "NF security MBEDTLS")
    endif()

    if(API_Windows.Devices.Wifi)
        set(USE_WIFI_OPTION TRUE CACHE INTERNAL  "Wifi for Windows.Devices.WiFi")
    else()
        set(USE_WIFI_OPTION FALSE CACHE INTERNAL "Wifi for Windows.Devices.WiFi")
    endif()

endif()


if(API_Windows.Devices.Adc)
    set(HAL_USE_ADC_OPTION TRUE CACHE INTERNAL "HAL ADC for Windows.Devices.Adc")
else()
    set(HAL_USE_ADC_OPTION FALSE CACHE INTERNAL "HAL ADC for Windows.Devices.Adc")
endif()


if(API_Windows.Devices.Gpio)
    set(HAL_USE_GPIO_OPTION TRUE CACHE INTERNAL "HAL GPIO for Windows.Devices.Gpio")
else()
    set(HAL_USE_GPIO_OPTION FALSE CACHE INTERNAL "HAL GPIO for Windows.Devices.Gpio")
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


#################################################################
# manage dependent APIs required for some API namespaces
#################################################################

# include nanoFramework.Runtime.Events API
if( API_nanoFramework.Devices.OneWire OR 
    API_System.Net OR
    API_Windows.Devices.Gpio OR
    API_Windows.Devices.SerialCommunication OR
    API_Windows.Networking.Sockets OR
    API_nanoFramework.Devices.Can)

    # these APIs requires nanoFramework.Runtime.Events
    set(API_nanoFramework.Runtime.Events ON CACHE INTERNAL "enable of API_nanoFramework.Runtime.Events")

endif()

if(API_System.Net)
    # manage inclusion of SNTP
    if(NF_NETWORKING_SNTP)
        set(API_nanoFramework.Networking.Sntp ON CACHE INTERNAL "enable API_nanoFramework..Networking.Sntp")
    else()
        set(API_nanoFramework.Networking.Sntp OFF CACHE INTERNAL "disable API_nanoFramework..Networking.Sntp")
    endif()
endif()


#################################################################
# handles Networking support at HAL level

if(USE_NETWORKING_OPTION)
    set(HAL_USE_MAC_OPTION TRUE CACHE INTERNAL "HAL MAC for USE_NETWORKING_OPTION")

    if(NF_SECURITY_OPENSSL)
        message(STATUS "Support for networking enabled with security from OpenSSL")
    elseif(NF_SECURITY_MBEDTLS)
        message(STATUS "Support for networking enabled with security from mbed TLS")
    else()
        message(STATUS "Support for networking enabled WITHOUT security")
    endif()

else()
    set(HAL_USE_MAC_OPTION FALSE CACHE INTERNAL "HAL MAC for USE_NETWORKING_OPTION")
    message(STATUS "Support for networking IS NOT enabled")
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

#######################
# ChibiOS
if(RTOS_CHIBIOS_CHECK)

    # sanity check on SSL provider: ChibiOS only supports mbed TLS
    if(NF_SECURITY_OPENSSL)
        message(FATAL "Can't use OpenSSL for ChibiOS. Currently only mbed TLS is supported.")
    endif()

    # check if CHIBIOS_SOURCE was specified or if it's empty (default is empty)
    set(NO_CHIBIOS_SOURCE TRUE)
    if(NOT "${CHIBIOS_SOURCE}" STREQUAL "")
       set(NO_CHIBIOS_SOURCE FALSE)
    endif()

    if(NO_CHIBIOS_SOURCE)
        # no CHIBIOS source specified, download it from it's repo

        # hack to make the FindGit to work in Windows platforms (check the module comment for details)
        include(Hack_SetGitSearchPath)

        # check for Git (needed here for advanced warning to user if it's not installed)
        find_package(Git)

        #  check if Git was found, if not report to user and abort
        if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
        endif()

        # ChibiOS version
        set(CHIBIOS_VERSION_EMPTY TRUE)

        # check if build was requested with a specifc ChibiOS version
        if(DEFINED CHIBIOS_VERSION)
            if(NOT "${CHIBIOS_VERSION}" STREQUAL "")
                set(CHIBIOS_VERSION_EMPTY FALSE)
            endif()
        endif()

        if(CHIBIOS_VERSION_EMPTY)
            # no ChibiOS version actualy specified, must be empty which is fine, we'll grab the code from the stable_18.2.x branch
            message(STATUS "RTOS is: CHIBIOS (latest available code from stable_18.2.x)")
            set(CHIBIOS_GIT_TAG "stable_18.2.x")
        else()
            message(STATUS "RTOS is: ChibiRTOS v${CHIBIOS_VERSION}")

            # branch naming follows the pattern 'stable_18.2.x'
            # need to extract the major and minor numbers from the CHIBIOS_VERSION parameter
            # find 1st dot in version number
            string(FIND ${CHIBIOS_VERSION} "." CHIBIOS_VERSION_DOT_INDEX)
            # find 2nd dot in version number
            string(FIND ${CHIBIOS_VERSION} "." CHIBIOS_VERSION_DOT_INDEX REVERSE)
            # extract 'short' version
            string(SUBSTRING ${CHIBIOS_VERSION} 0 ${CHIBIOS_VERSION_DOT_INDEX} CHIBIOS_SHORT_VERSION)

            # set branch name
            set(CHIBIOS_GIT_TAG "stable_${CHIBIOS_SHORT_VERSION}.x")
        endif()

        # need to setup a separate CMake project to download the code from the GitHub repository
        # otherwise it won't be available before the actual build step
        configure_file("CMake/ChibiOS.CMakeLists.cmake.in"
                    "${CMAKE_BINARY_DIR}/ChibiOS_Download/CMakeLists.txt")

        # setup CMake project for ChibiOS download
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/ChibiOS_Download")

        # run build on ChibiOS download CMake project to perform the download
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                        RESULT_VARIABLE result
                        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/ChibiOS_Download")

        # add ChibiOS as external project
        ExternalProject_Add(
            ChibiOS
            PREFIX ChibiOS
            SOURCE_DIR ${CMAKE_BINARY_DIR}/ChibiOS_Source
            GIT_REPOSITORY  https://github.com/nanoframework/ChibiOS
            GIT_TAG ${CHIBIOS_GIT_TAG}  # target specified branch
            GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
            TIMEOUT 10
            LOG_DOWNLOAD 1

            INSTALL_COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip-2.0.3-patched.7z WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/

            # Disable all other steps
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )

        # get source dir for ChibiOS CMake project
        ExternalProject_Get_Property(ChibiOS SOURCE_DIR)

    else()
        # ChibiOS source was specified

        # sanity check is source path exists
        if(EXISTS "${CHIBIOS_SOURCE}/")
            message(STATUS "RTOS is: ChibiOS (source from: ${CHIBIOS_SOURCE})")

            # check if we already have the sources, no need to copy again
            if(NOT EXISTS "${CMAKE_BINARY_DIR}/ChibiOS_Source")
                message(status "File Copy: ${CMAKE_BINARY_DIR}/ChibiOS_Source")
                file(COPY "${CHIBIOS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/ChibiOS_Source")
            else()
                message(STATUS "Using local cache of ChibiOS source from ${CHIBIOS_SOURCE}")
            endif()

            set(CHIBIOS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/ChibiOS_Source/include)
        else()
            message(FATAL_ERROR "Couldn't find ChibiOS source at ${CHIBIOS_SOURCE}/")
        endif()

        # add ChibiOS as external project
        ExternalProject_Add(
            ChibiOS
            PREFIX ChibiOS
            SOURCE_DIR ${CMAKE_BINARY_DIR}/ChibiOS_Source

            INSTALL_COMMAND ${CMAKE_COMMAND} -E tar xvf ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/lwip-2.0.3-patched.7z WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/

            # Disable all other steps
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
        )

        # get source dir for ChibiOS CMake project
        ExternalProject_Get_Property(ChibiOS SOURCE_DIR)

    endif()

    # if mbed TLS is enabled add it to the build
    if(NF_SECURITY_MBEDTLS)

        # check if MBEDTLS_SOURCE was specified or if it's empty (default is empty)
        set(NO_MBEDTLS_SOURCE TRUE)

        if(MBEDTLS_SOURCE)
            if(NOT "${MBEDTLS_SOURCE}" STREQUAL "")
                set(NO_MBEDTLS_SOURCE FALSE)
            endif()
        endif()

        # set options for mbed TLS
        option(ENABLE_TESTING "no testing when building mbed TLS." OFF)

        if(NO_MBEDTLS_SOURCE)
            # no mbed TLS source specified, download it from it's repo

            # hack to make the FindGit to work in Windows platforms (check the module comment for details)
            include(Hack_SetGitSearchPath)

            # check for Git (needed here for advanced warning to user if it's not installed)
            find_package(Git)

            #  check if Git was found, if not report to user and abort
            if(NOT GIT_EXECUTABLE)
                message(FATAL_ERROR "error: could not find Git, make sure you have it installed.")
            endif()

            # set tag for currently supported version
            set(MBEDTLS_GIT_TAG "mbedtls-2.14")

            # need to setup a separate CMake project to download the code from the GitHub repository
            # otherwise it won't be available before the actual build step
            configure_file("CMake/mbedTLS.CMakeLists.cmake.in"
                        "${CMAKE_BINARY_DIR}/mbedTLS_Download/CMakeLists.txt")

            # setup CMake project for mbedTLS download
            execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                            RESULT_VARIABLE result
                            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/mbedTLS_Download")

            # run build on mbedTLS download CMake project to perform the download
            execute_process(COMMAND ${CMAKE_COMMAND} --build .
                            RESULT_VARIABLE result
                            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/mbedTLS_Download")

            # add mbedTLS as external project
            ExternalProject_Add(
                mbedTLS
                PREFIX mbedTLS
                SOURCE_DIR ${CMAKE_BINARY_DIR}/mbedTLS_Source
                GIT_REPOSITORY  https://github.com/nanoframework/mbedtls
                GIT_TAG ${MBEDTLS_GIT_TAG}  # target specified branch
                GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
                TIMEOUT 10
                LOG_DOWNLOAD 1

                # Disable all other steps
                CONFIGURE_COMMAND ""
                BUILD_COMMAND ""
                INSTALL_COMMAND ""
            )

        else()
            # mbedTLS source was specified

            # sanity check is source path exists
            if(EXISTS "${MBEDTLS_SOURCE}/")

                # check if we already have the sources, no need to copy again
                if(NOT EXISTS "${CMAKE_BINARY_DIR}/mbedTLS_Source")
                    message(STATUS "mbedTLS source from: ${MBEDTLS_SOURCE}")
                    file(COPY "${MBEDTLS_SOURCE}/" DESTINATION "${CMAKE_BINARY_DIR}/mbedTLS_Source")
                else()
                    message(STATUS "Using local cache of mbedTLS source from ${MBEDTLS_SOURCE}")
                endif()

                set(MBEDTLS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/mbedTLS_Source/include)
            else()
                message(FATAL_ERROR "Couldn't find mbedTLS source at ${MBEDTLS_SOURCE}/")
            endif()

            # add mbedTLS as external project
            ExternalProject_Add(
                mbedTLS
                PREFIX mbedTLS
                SOURCE_DIR ${CMAKE_BINARY_DIR}/mbedTLS_Source

                # Disable all other steps
                CONFIGURE_COMMAND ""
                BUILD_COMMAND ""
                INSTALL_COMMAND ""
            )

        endif()

        # get source dir for mbedTLS CMake project
        ExternalProject_Get_Property(mbedTLS SOURCE_DIR)

        set(mbedTLS_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/3rdparty/humblelogging/include")
        set(mbedTLS_LIBRARIES "${CMAKE_SHARED_LIBRARY_PREFIX}mbedTLS${CMAKE_SHARED_LIBRARY_SUFFIX}")
        include_directories(${mbedTLS_INCLUDE_DIRS})

    endif()

    # Define base path for the class libraries
    set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES "${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nanoCLR")

    # set target base location
    # this has to be set before the class library modules are pulled in
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
        # set target base location
        set(TARGET_BASE_LOCATION "${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}")
    else()
        # try to find board in the Community targets folder
        if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
            # set target base location
            set(TARGET_BASE_LOCATION "${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}")
        else()
            # board NOT found in targets folder
            # can't continue
            message(FATAL_ERROR "\n\nSorry but support for ${CHIBIOS_BOARD} target is not available...\n\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
        endif()
    endif()

    # set CMSIS RTOS include directory
    include_directories( ${CMSIS_RTOS_INCLUDE_DIR})

    # add target CMSIS OS folders
    add_subdirectory(targets/CMSIS-OS/common/Include)
    add_subdirectory(targets/CMSIS-OS/common)
    add_subdirectory(targets/CMSIS-OS/nanoBooter)
    add_subdirectory(targets/CMSIS-OS/nanoCLR)

    # need to find board definition files (board.c and board.h)

    # assume no community board... until proven otherwise
    set(CHIBIOS_COMMUNITY_TARGET FALSE CACHE INTERNAL "Community target flag")

    # start search in nanoFramework ChibiOS 'overlay' folder
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/boards/${CHIBIOS_BOARD})
        # board found
        # if it's on nF overlay board.c and board.h exist there for sure
        set(CHIBIOS_BOARD_DEFINITIONS_LOCATION "Board definition files taken from nanoFramework overlay" CACHE INTERNAL "Location of board definition files")
    else()
        # board NOT found in ChibiOS 'overlay'

        # try to find it in the target boards
        if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
            # board found
            # check if the board definition files are available at the target folder
            if( EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/board.c AND
                EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/board.h)
                # definition files found
                set(CHIBIOS_BOARD_DEFINITIONS_LOCATION "Board definition files taken from target folder" CACHE INTERNAL "Location of board definition files")

            else()
                # board.c and board.h are NOT in the target folder, try to find them in the official distribution

                if(EXISTS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/boards/${CHIBIOS_BOARD})
                    # board found
                    # if it's on the ChibiOS official distribution board.c and board.h exist here for sure
                    set(CHIBIOS_BOARD_DEFINITIONS_LOCATION "Board definition files taken from official ChibiOS distribution" CACHE INTERNAL "Location of board definition files")

                else()
                    # board NOT found in official distribution
                    # quit now as there is no were else to search for these
                    message(FATAL_ERROR "\n\nSorry but couldn't find definition files for ${CHIBIOS_BOARD} in the available list of ChibiOS supported boards...\n\n")

                endif()
            endif()

        else()
            # try to find board in the Community targets folder
            if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
                # set flag for this being a community board
                set(CHIBIOS_COMMUNITY_TARGET TRUE CACHE INTERNAL "Community target flag")
            else()
                # board NOT found in official distribution
                # quit now as there is no were else to search for these
                message(FATAL_ERROR "\n\nSorry but couldn't find definition files for ${CHIBIOS_BOARD} in the available list of ChibiOS supported boards...\n\n")
            endif()

        endif()

    endif()

    # add target ChibiOS dirs
    add_subdirectory(targets/CMSIS-OS/ChibiOS/Include)
    add_subdirectory(targets/CMSIS-OS/ChibiOS/common)
    add_subdirectory(targets/CMSIS-OS/ChibiOS/nanoBooter)
    add_subdirectory(targets/CMSIS-OS/ChibiOS/nanoCLR)

    # now add the subdirectory for the board
    # try to find board in the targets folder
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
        # board found
        message(STATUS "Support for target board '${CHIBIOS_BOARD}' found")
        message(STATUS "${CHIBIOS_BOARD_DEFINITIONS_LOCATION}")

        # add TARGET board directory
        add_subdirectory("targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}")

    else()

        # try to find board in the Community targets folder
        if(EXISTS ${PROJECT_SOURCE_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
            # board found
            message(STATUS "Support for target board '${CHIBIOS_BOARD}' found in Community targets")
            message(STATUS "${CHIBIOS_BOARD_DEFINITIONS_LOCATION}")

            # add TARGET board directory from Community
            add_subdirectory("targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}")

        else()
        # board NOT found in targets folder
            # board NOT found in targets folder
            message(FATAL_ERROR "\n\nSorry but support for ${CHIBIOS_BOARD} target is not available...\n\You can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
        endif()

    endif()

#######################
# FreeRTOS
elseif(RTOS_FREERTOS_CHECK)

    if("${TARGET_SERIES}" STREQUAL "ESP32")
        message(STATUS "Platform ESP32")

        # check watchdog feature
        # EPS32 build has watchdog enabled by default, so setting it to OFF doens't make sense 
        # becuase the build system can't honour that preference
        if(NOT NF_FEATURE_WATCHDOG)
        message(FATAL_ERROR "\n\nESP32 watchdog is enabled by default in so you can't have the NF_FEATURE_WATCHDOG option set to OFF.\n\n")
        endif()

        # Define base path for the class libraries
        set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES "${PROJECT_SOURCE_DIR}/targets/FreeRTOS/ESP32_DevKitC/nanoCLR")

        # set target base location
        set(TARGET_BASE_LOCATION "${PROJECT_SOURCE_DIR}/targets/FreeRTOS/ESP32_DevKitC")

 		add_subdirectory(targets/FreeRTOS/ESP32_DevKitC)
    endif()

endif()

    #########################################
    # Graphics - see CMakeUserFunctions.cmake
    #########################################
    if(NF_FEATURE_UI_GRAPHICS)
         Graphics(${CpuToGraphicsDisplay} ${GraphicsDisplay} )
    endif()

    ##############################################
    # Touch and Ink - see CMakeUserFunctions.cmake
    ##############################################
    if(NF_FEATURE_UI_TOUCH)
         Message(STATUS, "\nTouch feature selected\n")
         Touch(${CpuToTouchSensor} ${TouchSensor} )
    endif()

