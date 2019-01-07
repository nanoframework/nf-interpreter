#########################################################################
#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#
#########################################################################


function( Graphics PrimaryGraphicsController SecondaryGraphicsController)
   If(NOT EXISTS ${PrimaryGraphicsController})
        Message(FATAL_ERROR, "Graphics feature selected but no primary graphics controller found")
   else()
       add_subdirectory(src/CLR/GUI/Graphics)
       add_subdirectory(${PrimaryGraphicsController})
   endif()
   if("${SecondaryGraphicsController}" STREQUAL "")
        Message(STATUS, "No secondary graphics controller selected")
   else()
       If(NOT EXISTS ${SecondaryGraphicsController})
            Message(FATAL_ERROR ,"Secondary graphics controller not found")
       else()
           add_subdirectory(${SecondaryGraphicsController})
       endif()
   endif()
endfunction()

function( TouchAndInk PrimaryTouchController SecondaryTouchController)
   If(NOT EXISTS ${PrimaryTouchController})
        Message(FATAL_ERROR, "Touch feature selected but no primary touch controller found")
   else()
       add_subdirectory(src/CLR/GUI/TOUCH)
       add_subdirectory(${PrimaryTouchController})
   endif()

   if("${SecondaryTouchController}" STREQUAL "")
        Message(STATUS, "No secondary touch controller selected")
   else()
       If(NOT EXISTS ${SecondaryTouchController})
            Message(FATAL_ERROR, "Secondary touch controller not found")
       else()
           add_subdirectory(${SecondaryTouchController})
       endif()
   endif()
endfunction()


#########################################
## Support for alternateCMakeLists.cmake ##
#########################################

function(Watch_Dog Watchdog)
    if( ${Watchdog})
        set(HAL_USE_WDG_OPTION TRUE CACHE INTERNAL "NF feature watchdog")
    else()    
        set(HAL_USE_WDG_OPTION FALSE CACHE INTERNAL "NF feature watchdog")
    endif()
endfunction()

function(Realtime_Clock RealtimeClock)
    if( ${RealtimeClock})
        set(HAL_USE_RTC_OPTION TRUE CACHE INTERNAL "NF feature RTC")
    else()
        set(HAL_USE_RTC_OPTION FALSE CACHE INTERNAL "NF feature RTC")
    endif()
endfunction()

function(Double_Floating_Point  DoubleFloatingPoint)
    if(${DoubleFloatingPoint})
        set(TARGET_DP_FLOATINGPOINT TRUE CACHE INTERNAL "DP FP support")
    else()
        set(TARGET_DP_FLOATINGPOINT FALSE CACHE INTERNAL "DP FP support")
    endif()
endfunction()

function(Any_Base_Conversion  AnyBaseConversion)
    if(${AnyBaseConversion})
        set(TARGET_SUPPORT_ANY_BASE_CONVERSION TRUE CACHE INTERNAL "Option for string conversion to value from any base")
    else()
        set(TARGET_SUPPORT_ANY_BASE_CONVERSION FALSE CACHE INTERNAL "Option for string conversion to value from base 10 and partial for 16")
    endif()
endfunction()

function(Hardware_Random_Number_Generation Hrng)
  # Random number generation option supported by hardware
  # option to enable use of true random generator hardware block
    if(${Hrng})
      set(USE_RNG ON)
    else()
      set(USE_RNG OFF)
    endif()
endfunction()

function(App_Domains AppDomains)
    if(${AppDomains})
        set(NF_FEATURE_USE_APPDOMAINS TRUE CACHE INTERNAL "")
    else()
        set(NF_FEATURE_USE_APPDOMAINS FALSE CACHE INTERNAL "")
    endif()
endfunction()

function(Config_Block ConfigBlock)
    if(${ConfigBlock})
        set(TARGET_HAS_CONFIG_BLOCK TRUE CACHE INTERNAL "Configuration block storage is included")
    else()
        set(TARGET_HAS_CONFIG_BLOCK FALSE CACHE INTERNAL "Configuration block storage IS NOT included")
    endif()
endfunction()

function(Arm_Cortex_SWO ArmCortexSWO)
    if(${ArmCortexSWO})
        set(SWO_OUTPUT_OPTION TRUE CACHE INTERNAL "Single Wire Output (SWO) ENABLED")
    else()
        set(SWO_OUTPUT_OPTION FALSE CACHE INTERNAL "Single Wire Output (SWO) DISABLED")
    endif()
endfunction()

function(CRC32 Crc32)
    if(${Crc32})
        set(NF_WP_IMPLEMENTS_CRC32 TRUE CACHE INTERNAL "Wire Protocol implements CRC32")
    else()
        set(NF_WP_IMPLEMENTS_CRC32 FALSE CACHE INTERNAL "Wire Protocol does NOT implement CRC32")
    endif()
endfunction()

function(Network Networking  NetworkingSecurity NetworkingSNTP NetworkWiFi NetworkMAC)
    if(${Networking})
        Enable_Runtime_Events()
        set(USE_NETWORKING_OPTION TRUE CACHE INTERNAL "Wire Protocol implements CRC32")
        if(${NetworkingSecurity} == "SSL")
            set(NF_SECURITY_OPENSSL TRUE CACHE INTERNAL "use OpenSSL as the network security provider")
        elseif(${NetworkingSecurity} == "TLS")
            set(NF_SECURITY_MBEDTLS FALSE CACHE INTERNAL "option to use mbed TLS as the network security provider")
            # Add mbed TLS is enabled add it to the build
            Download_MBED_TLS()
        else() # None ( or unknown)
            set(NF_SECURITY_OPENSSL FALSE CACHE INTERNAL "No network security provider")
            set(NF_SECURITY_MBEDTLS FALSE CACHE INTERNAL "No network security provider")
        endif()
        if(${NetworkingSNTP})
           set(NF_NETWORKING_SNTP TRUE CACHE INTERNAL "Add SNTP support")
           set(API_nanoFramework.Networking.Sntp ON CACHE INTERNAL "enable API_nanoFramework..Networking.Sntp")
        else()
            set(NF_NETWORKING_SNTP FALSE CACHE INTERNAL "No SNTP support")
            set(API_nanoFramework.Networking.Sntp OFF CACHE INTERNAL "disable API_nanoFramework..Networking.Sntp")
        endif()
        if(${NetworkWiFi})
            set(USE_WIFI_OPTION TRUE CACHE INTERNAL  "Wifi for Windows.Devices.WiFi")
        else()
            set(USE_WIFI_OPTION FALSE CACHE INTERNAL "Wifi for Windows.Devices.WiFi")
        endif()
        if(${NetworkMAC})
            set(HAL_USE_MAC_OPTION TRUE CACHE INTERNAL "HAL MAC for USE_NETWORKING_OPTION")
        else()
            set(HAL_USE_MAC_OPTION FALSE CACHE INTERNAL "No HAL MAC")
        endif()

        Enable_Runtime_Events()

    else()
        set(USE_NETWORKING_OPTION FALSE "No network")
        set(NF_SECURITY_OPENSSL FALSE "No network")
        set(NF_SECURITY_MBEDTLS FALSE "No network")
        set(HAL_USE_MAC_OPTION FALSE CACHE INTERNAL "No network")
        set(HAL_USE_MAC_OPTION FALSE CACHE INTERNAL "No HAL MAC")
        set(API_nanoFramework.Networking.Sntp OFF CACHE INTERNAL "disable API_nanoFramework..Networking.Sntp")
    endif()
endfunction()

function(File_System FileSystem)
  if(${FileSystem})
    set(USE_FILESYSTEM_OPTION TRUE CACHE INTERNAL "NF feature FILESYSTEM")
    set(HAL_USE_SDC_OPTION TRUE CACHE INTERNAL "HAL SDC for NF_FEATURE_USE_FILESYSTEM")
  else()
    set(USE_FILESYSTEM_OPTION FALSE CACHE INTERNAL "NF feature FILESYSTEM")
    set(HAL_USE_SDC_OPTION FALSE CACHE INTERNAL "HAL SDC for NF_FEATURE_USE_FILESYSTEM")
  endif()
endfunction()

function(Adc Adc)
    if(${Adc})
        set(HAL_USE_ADC_OPTION TRUE CACHE INTERNAL "HAL ADC for Windows.Devices.Adc")
    else()
        set(HAL_USE_ADC_OPTION FALSE CACHE INTERNAL "HAL ADC for Windows.Devices.Adc")
    endif()
endfunction()

function(GPIO Gpio)
    if(${Gpio})
        Enable_Runtime_Events()
        set(HAL_USE_GPIO_OPTION TRUE CACHE INTERNAL "HAL GPIO for Windows.Devices.Gpio")
    else()
        set(HAL_USE_GPIO_OPTION FALSE CACHE INTERNAL "HAL GPIO for Windows.Devices.Gpio")
    endif()
endfunction()

function(I2C I2C)
    if(${I2C})
        set(HAL_USE_I2C_OPTION TRUE CACHE INTERNAL "HAL I2C for Windows.Devices.I2c")
    else()
        set(HAL_USE_I2C_OPTION FALSE CACHE INTERNAL "HAL I2C for Windows.Devices.I2c")
    endif()
endfunction()

function(PWM Pwm)
    if(${Pwm})
        set(HAL_USE_PWM_OPTION TRUE CACHE INTERNAL "HAL PWM for Windows.Devices.Pwm")
    else()
        set(HAL_USE_PWM_OPTION FALSE CACHE INTERNAL "HAL PWM for Windows.Devices.Pwm")
    endif()
endfunction()

function(SPI Spi)
    if(${Spi})
        set(HAL_USE_SPI_OPTION TRUE CACHE INTERNAL "HAL SPI for Windows.Devices.Spi")
    else()
        set(HAL_USE_SPI_OPTION FALSE CACHE INTERNAL "HAL SPI for Windows.Devices.Spi")
    endif()
endfunction()

function(CAN Can)
    if(${Can})
        Enable_Runtime_Events()
        set(HAL_USE_CAN_OPTION TRUE CACHE INTERNAL "HAL CAN for nanoFramework.Devices.Can")
    else()
        set(HAL_USE_CAN_OPTION FALSE CACHE INTERNAL "HAL CAN for nanoFramework.Devices.Can")
    endif()
endfunction()

function(One_Wire OneWire)
    if(${OneWire})
        Enable_Runtime_Events()
        Setup_UART(TRUE)
    else()
        Setup_UART(FALSE)
    endif()
endfunction()

function(Serial_Communication SerialCommunication)
    if(${SerialCommunication})
        Enable_Runtime_Events()
        Setup_UART(TRUE)
    else()
        Setup_UART(FALSE)
    endif()
endfunction()

function(Setup_UART Uart)
    if( ${Uart})
        set(HAL_USE_UART_OPTION TRUE CACHE INTERNAL "Uart ON")
    else()
        set(HAL_USE_UART_OPTION FALSE CACHE INTERNAL "Uart Off")
    endif()
endfunction()

function(Enable_Runtime_Events )
   set(API_nanoFramework.Runtime.Events ON CACHE INTERNAL "enable of API_nanoFramework.Runtime.Events")
endfunction()

function(Download_MBED_TLS)
 # # need to setup a separate CMake project to download the code from the GitHub repository
 # # otherwise it won't be available before the actual build step
 # configure_file("CMake/mbedTLS.CMakeLists.cmake.in"
 #             "${CMAKE_BINARY_DIR}/mbedTLS_Download/CMakeLists.txt")
 #
 # # setup CMake project for mbedTLS download
 # execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
 #                 RESULT_VARIABLE result
 #                 WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/mbedTLS_Download")
 #
 # # run build on mbedTLS download CMake project to perform the download
 # execute_process(COMMAND ${CMAKE_COMMAND} --build .
 #                 RESULT_VARIABLE result
 #                 WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/mbedTLS_Download")
 #
 # # add mbedTLS as external project
 # ExternalProject_Add(
 #     mbedTLS
 #     PREFIX mbedTLS
 #     SOURCE_DIR ${CMAKE_BINARY_DIR}/mbedTLS_Source
 #     GIT_REPOSITORY  https://github.com/nanoframework/mbedtls
 #     GIT_TAG ${MBEDTLS_GIT_TAG}  # target specified branch
 #     GIT_SHALLOW 1   # download only the tip of the branch, not the complete history
 #     TIMEOUT 10
 #     LOG_DOWNLOAD 1
 #
 #     # Disable all other steps
 #     CONFIGURE_COMMAND ""
 #     BUILD_COMMAND ""
 #     INSTALL_COMMAND ""
 # )
endfunction()

function(Debugger Debugger)
   # Option to enable support for debugging")
    if(${Debugger})
        set(NF_FEATURE_DEBUGGER TRUE CACHE INTERNAL "Option for string conversion to value from any base")
    else()
        set(NF_FEATURE_DEBUGGER FALSE CACHE INTERNAL "Option for string conversion to value from base 10 and partial for 16")
    endif()
endfunction()

function(CLR_Trace PlatformTrace)

    if(${PlatformTrace})
        set(NF_PLATFORM_NO_CLR_TRACE TRUE CACHE INTERNAL "CLR has all trace messages and checks DISABLED")
    else()
        set(NF_PLATFORM_NO_CLR_TRACE FALSE CACHE INTERNAL "CLR has all trace messages and checks enabled")
    endif()
endfunction()

function(CLR_IL_Inline CLRInline)
    if(${CLRInline})
        set(NF_CLR_NO_IL_INLINE FALSE CACHE INTERNAL "CLR IL inlining is ENABLED")
    else()
        set(NF_CLR_NO_IL_INLINE TRUE CACHE INTERNAL "CLR IL inlining is DISABLED")
    endif()
endfunction()

function(Trace_Errors TraceErrors)
   # Option to enable support for debugging")
    if(${TraceErrors})
        set(NF_WP_TRACE_ERRORS TRUE CACHE INTERNAL "")
    else()
        set(NF_WP_TRACE_ERRORS FALSE CACHE INTERNAL "")
    endif()
endfunction()

function(Trace_Headers TraceHeaders)
   # Option to enable support for debugging")
    if(${TraceHeaders})
        set(NF_WP_TRACE_HEADERS TRUE CACHE INTERNAL "")
    else()
        set(NF_WP_TRACE_HEADERS FALSE CACHE INTERNAL "")
    endif()
endfunction()

function(Trace_State TraceState)
   # Option to enable support for debugging")
    if(${TraceState})
        set(NF_WP_TRACE_STATE TRUE CACHE INTERNAL "")
    else()
        set(NF_WP_TRACE_STATE FALSE CACHE INTERNAL "")
    endif()
endfunction()

function(Trace_NoData TraceNoData)
   # Option to enable support for debugging")
    if(${TraceNoData})
        set(NF_WP_TRACE_NODATA TRUE CACHE INTERNAL "")
    else()
        set(NF_WP_TRACE_NODATA FALSE CACHE INTERNAL "")
    endif()
endfunction()

function(Trace_ALL TraceALL)
   # Option to enable support for debugging")
    if(${TraceALL})
        set(NF_WP_TRACE_ALL TRUE CACHE INTERNAL "")
    else()
        set(NF_WP_TRACE_ALL FALSE CACHE INTERNAL "")
    endif()
endfunction()

function(Interop_Assemblies InteropAssemblies)
   # Option to enable support for debugging")
    if(${InteropAssemblies})
        set(NF_INTEROP_ASSEMBLIES TRUE CACHE INTERNAL "")
    else()
        set(NF_INTEROP_ASSEMBLIES FALSE CACHE INTERNAL "")
    endif()
endfunction()

function(Build_RTM BuildRTM)
   # Option to enable support for debugging")
    if(${BuildRTM})
        set(NF_BUILD_RTM TRUE CACHE INTERNAL "")
    else()
        set(NF_BUILD_RTM FALSE CACHE INTERNAL "")
    endif()
endfunction()
