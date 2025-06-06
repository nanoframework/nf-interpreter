#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

############################################################################################
# WHEN ADDING A NEW API add the name that corresponds to the CMake option here
# e.g.: for namespace System.Device.Gpio, the CMake option is 'API_System.Device.Gpio' 
# and the namespace designation is 'System.Device.Gpio'
###########################################################################################

option(API_nanoFramework.Device.Can                     "option for nanoFramework.Device.Can")
option(API_nanoFramework.Device.OneWire                 "option for nanoFramework.Device.OneWire")
option(API_nanoFramework.Networking.Sntp                "option for nanoFramework.Networking.Sntp")
option(API_nanoFramework.Runtime.Events                 "option for nanoFramework.Runtime.Events API")
option(API_nanoFramework.ResourceManager                "option for nanoFramework.ResourceManager")
option(API_nanoFramework.System.Collections             "option for nanoFramework.System.Collections")
option(API_nanoFramework.System.Text                    "option for nanoFramework.System.Text")
option(API_System.IO.FileSystem                         "option for System.IO.FileSystem")
option(API_System.Math                                  "option for System.Math")
option(API_System.Net                                   "option for System.Net")
option(API_System.Device.Adc                            "option for System.Device.Adc API")
option(API_System.Device.Dac                            "option for System.Device.Dac API")
option(API_System.Device.Gpio                           "option for System.Device.Gpio API")
option(API_System.Device.I2c                            "option for System.Device.I2c API")
option(API_System.Device.I2c.Slave                      "option for System.Device.I2c.Slave API")
option(API_System.Device.I2s                            "option for System.Device.I2s API")
option(API_System.Device.Pwm                            "option for System.Device.Pwm API")
option(API_System.IO.Ports                              "option for System.IO.Ports API")
option(API_System.Device.Spi                            "option for System.Device.Spi API")
option(API_System.Runtime.Serialization                 "option for System.Runtime.Serialization API")
option(API_nanoFramework.Graphics                       "option for nanoFramework.Graphics")
option(API_nanoFramework.Device.Bluetooth               "option for nanoFramework.Device.Bluetooth")
option(API_System.Device.UsbStream                      "option for System.Device.UsbStream API")
option(API_nanoFramework.System.IO.Hashing              "option for nanoFramework.System.IO.Hashing API")
option(API_nanoFramework.System.Security.Cryptography   "option for nanoFramework.System.Security.Cryptography API")

# Esp32 only
option(API_Hardware.Esp32                               "option for Hardware.Esp32")
option(API_nanoFramework.Hardware.Esp32.Rmt             "option for nanoFramework.Hardware.Esp32.Rmt")
option(API_nanoFramework.Networking.Thread              "option for nanoFramework.Networking.Thread API")

# Stm32 only
option(API_Hardware.Stm32                               "option for Hardware.Stm32")

# TI CC13xxCC26xx
option(API_nanoFramework.TI.EasyLink                    "option for nanoFramework.TI.EasyLink API")
option(API_nanoFramework.Hardware.TI                    "option for nanoFramework.Hardware.TI API")

# Silabs Giant Gecko only
option(API_nanoFramework.GiantGecko.Adc                 "option for nanoFramework.GiantGecko.Adc")
option(API_Hardware.GiantGecko                          "option for Hardware.GiantGecko")

###################################
# add options for private APIs here

###################################

###################################
# add options for private APIs here

###################################

#################################################################
# macro to find the version of an API or Interop assembly
macro(AddNativeAssemblyVersion apiNamespace apiNamespaceWithoutDots nativeAssemblySources)

    # find the source file that contains the value of CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_${apiNamespaceWithoutDots}
    foreach(apiSourceFile ${nativeAssemblySources})

        file(READ "${apiSourceFile}" sourceCode)

        string(REGEX MATCH "[ \t\r\n]+CLR_RT_NativeAssemblyData[ \t\r\n]+g_CLR_AssemblyNative_${apiNamespaceWithoutDots}[ \t\r\n]*=[^{]+{[^0]+0x([0-9A-Za-z]+)[^{]+{[ \t\r\n]*([0-9]+)[ \t\r\n]*,[ \t\r\n]*([0-9]+)[ \t\r\n]*,[ \t\r\n]*([0-9]+)[ \t\r\n]*,[ \t\r\n]*([0-9]+)" _ "${sourceCode}")

        if(NOT "${CMAKE_MATCH_1}" STREQUAL "")
            list(APPEND NF_NativeAssemblies_VERSIONS "${apiNamespace},${CMAKE_MATCH_2}.${CMAKE_MATCH_3}.${CMAKE_MATCH_4}.${CMAKE_MATCH_5},0x${CMAKE_MATCH_1}")
            break()
        endif()

    endforeach()

endmacro()
#################################################################

#################################################################
# macro to find the version of the CorLib assembly
macro(AddCorLibAssemblyVersion apiNamespace apiNamespaceWithoutDots nativeAssemblySources)

    # find the source file that contains the value of CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_${apiNamespaceWithoutDots}
    foreach(apiSourceFile ${nativeAssemblySources})

        file(READ "${apiSourceFile}" sourceCode)

        string(REGEX MATCH "[ \t\r\n]+CLR_RT_NativeAssemblyData[ \t\r\n]+g_CLR_AssemblyNative_${apiNamespaceWithoutDots}[ \t\r\n]*=[^{]+{[^0]+0x([0-9A-Za-z]+)[^0]+0x([0-9A-Za-z]+)[^{]+{[ \t\r\n]*([0-9]+)[ \t\r\n]*,[ \t\r\n]*([0-9]+)[ \t\r\n]*,[ \t\r\n]*([0-9]+)[ \t\r\n]*,[ \t\r\n]*([0-9]+)" _ "${sourceCode}")

        if(NOT "${CMAKE_MATCH_1}" STREQUAL "")
            if (NF_FEATURE_SUPPORT_REFLECTION)
                list(APPEND NF_NativeAssemblies_VERSIONS "${apiNamespace},${CMAKE_MATCH_3}.${CMAKE_MATCH_4}.${CMAKE_MATCH_5}.${CMAKE_MATCH_6},0x${CMAKE_MATCH_1}")
            else()
                list(APPEND NF_NativeAssemblies_VERSIONS "${apiNamespace},${CMAKE_MATCH_3}.${CMAKE_MATCH_4}.${CMAKE_MATCH_5}.${CMAKE_MATCH_6},0x${CMAKE_MATCH_2}")
            endif()

            break()
        endif()

    endforeach()

endmacro()
#################################################################

#################################################################
# macro to perform individual settings to add an API to the build
macro(PerformSettingsForApiEntry apiNamespace)
    
    # namespace with '_' replacing '.'
    string(REPLACE "." "_" apiNamespaceWithoutDots "${apiNamespace}")

    # list this option
    list(APPEND apiListing "${apiNamespace}")

    # append to list of defines for Interop Assemblies include file
    set(CLR_RT_NativeAssemblyPreprocessorDefines "${CLR_RT_NativeAssemblyPreprocessorDefines}\n#define API_${apiNamespaceWithoutDots}")

    # append to list of declaration for Interop Assemblies table
    list(APPEND CLR_RT_NativeAssemblyDataList "extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_${apiNamespaceWithoutDots};")

    # append to list of entries for Interop Assemblies table
    list(APPEND CLR_RT_NativeAssemblyDataTableEntriesList "&g_CLR_AssemblyNative_${apiNamespaceWithoutDots},")

    # find the module
    find_package(${apiNamespace} REQUIRED QUIET)

    # append include directories to list with includes for all the APIs
    list(APPEND NF_NativeAssemblies_INCLUDE_DIRS "${${apiNamespace}_INCLUDE_DIRS}")
    list(REMOVE_DUPLICATES NF_NativeAssemblies_INCLUDE_DIRS)

    # append source files to list with source files for all the APIs
    list(APPEND NF_NativeAssemblies_SOURCES "${${apiNamespace}_SOURCES}")
    list(REMOVE_DUPLICATES NF_NativeAssemblies_SOURCES)

    # add the assembly version to the list
    AddNativeAssemblyVersion("${apiNamespace}" "${apiNamespaceWithoutDots}" "${${apiNamespace}_SOURCES}")
endmacro()
#################################################################

##############################################################################
# macro to perform individual settings to add an Interop assembly to the build
macro(PerformSettingsForInteropEntry interopAssemblyName)
    
    # namespace with '_' replacing '.'
    string(REPLACE "." "_" interopAssemblyNameWithoutDots "${interopAssemblyName}")

    # list this option
    list(APPEND interopAssemblyList "${interopAssemblyName}")

    # append to list of declaration for Interop Assemblies table
    list(APPEND CLR_RT_NativeAssemblyDataList "extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_${interopAssemblyNameWithoutDots};")

    # append to list of entries for Interop Assemblies table
    list(APPEND CLR_RT_NativeAssemblyDataTableEntriesList "&g_CLR_AssemblyNative_${interopAssemblyNameWithoutDots},")

    # find the module
    find_package("INTEROP-${interopAssemblyName}" REQUIRED QUIET)

    #########
    # because Interop assemblies are considered and treated as like any CLR assembly we add them to the same lists
    #########

    # append include directories to list with includes for all the APIs
    list(APPEND NF_NativeAssemblies_INCLUDE_DIRS "${${interopAssemblyName}_INCLUDE_DIRS}")
    list(REMOVE_DUPLICATES NF_NativeAssemblies_INCLUDE_DIRS)

    # append source files to list wiht source files for all the APIs
    list(APPEND NF_NativeAssemblies_SOURCES "${${interopAssemblyName}_SOURCES}")
    list(REMOVE_DUPLICATES NF_NativeAssemblies_SOURCES)

    # add the assembly version to the list
    AddNativeAssemblyVersion("${interopAssemblyName}" "${interopAssemblyNameWithoutDots}" "${${interopAssemblyName}_SOURCES}")
endmacro()

#################################################################
# macro that adds the requested Interop assemblies to the build
# requiremens to add an Interop assemble:
# 1) add it's namespace to the NF_INTEROP_ASSEMBLIES CMake options
# 2) have the corresponding CMake module in the Modules folder (mind the correct naming)
macro(ParseInteropAssemblies)

    # check if there are any Interop assemblies to be added
    if(NF_INTEROP_ASSEMBLIES)

        # need to split define containing assembly namespaces
        # for Windows buids this is a string with the namespaces separated by an whitespace
        # e.g.: "NF_INTEROP_ASSEMBLIES": "Assembly1_Namespace Assembly2_Namespace"
        separate_arguments(INTEROP_ASSEMBLIES_LIST NATIVE_COMMAND ${NF_INTEROP_ASSEMBLIES})

        # loop through each Interop assembly and add it to the build
        foreach(assembly ${INTEROP_ASSEMBLIES_LIST})
            PerformSettingsForInteropEntry(${assembly})
        endforeach()
       
    endif()

    # output the list of Interop assemblies included
    list(LENGTH interopAssemblyList interopAssemblyListLenght)

    if(interopAssemblyListLenght GREATER 0)
        
        # APIs included
        message(STATUS "")
        message(STATUS " *** Interop assemblies included ***")
        message(STATUS "")

        foreach(entry ${interopAssemblyList})
            message(STATUS " ${entry}")
        endforeach(entry ${})
        
        message(STATUS "")
        message(STATUS " ***  end of Interop assemblies  ***")
        message(STATUS "")

    else()
        # no Interop assemblies were included
        message(STATUS " *** NO Interop assemblies included ***")    
    endif()

endmacro()

############################################################################################
#  Add versions of native assemblies that are always included
############################################################################################

AddCorLibAssemblyVersion("mscorlib" "mscorlib" "${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native.cpp")

AddNativeAssemblyVersion("nanoFramework.Runtime.Native" "nanoFramework_Runtime_Native" "${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Native/nf_rt_native.cpp")

############################################################################################
# WHEN ADDING A NEW API add the corresponding block below 
# required changes:
# 1. change the 'IF' to match the CMake option 
# 2. change the call to PerformSettingsForApiEntry() macro with the API name (doted naming)
############################################################################################

# Hardware.Esp32
if(API_Hardware.Esp32)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Hardware.Esp32")
endif()

# nanoFramework.Hardware.Esp32.Rmt
if(API_nanoFramework.Hardware.Esp32.Rmt)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Hardware.Esp32.Rmt")
endif()

# nanoFramework.Networking.Thread
if(API_nanoFramework.Networking.Thread)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Networking.Thread")
endif()

# nanoFramework.Device.Bluetooth
if(API_nanoFramework.Device.Bluetooth)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Device.Bluetooth")
endif()

# Hardware.Stm32
if(API_Hardware.Stm32)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Hardware.Stm32")
endif()

# nanoFramework.Device.Can
if(API_nanoFramework.Device.Can)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Device.Can")
endif()

# nanoFramework.Graphics
if(API_nanoFramework.Graphics)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Graphics")
endif()

# nanoFramework.Device.OneWire
if(API_nanoFramework.Device.OneWire)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Device.OneWire")
endif()

# nanoFramework.Networking.Sntp
if(API_nanoFramework.Networking.Sntp)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Networking.Sntp")
endif()

# nanoFramework.ResourceManager
if(API_nanoFramework.ResourceManager)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.ResourceManager")
endif()

# nanoFramework.System.Collections
if(API_nanoFramework.System.Collections)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.System.Collections")
endif()

# nanoFramework.System.Text
if(API_nanoFramework.System.Text)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.System.Text")
endif()

# nanoFramework.TI.EasyLink
if(API_nanoFramework.TI.EasyLink)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.TI.EasyLink")
endif()

# nanoFramework.Hardware.TI
if(API_nanoFramework.Hardware.TI)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Hardware.TI")
endif()

# nanoFramework.Hardware.TI
if(API_nanoFramework.GiantGecko.Adc)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.GiantGecko.Adc")
endif()

# nanoFramework.Hardware.GiantGecko
if(API_Hardware.GiantGecko)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Hardware.GiantGecko")
endif()

# nanoFramework.System.IO.Hashing
if(API_nanoFramework.System.IO.Hashing)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.System.IO.Hashing")
endif()

# nanoFramework.System.Security.Cryptography
if(API_nanoFramework.System.Security.Cryptography)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.System.Security.Cryptography")

    # enable adding Mbed TLS to the build
    set(NF_REQUIRES_MBEDTLS TRUE CACHE BOOL "Enable Mbed TLS for nanoFramework.System.Security.Cryptography" FORCE)
endif()

# nanoFramework.Runtime.Events
if(API_nanoFramework.Runtime.Events)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("nanoFramework.Runtime.Events")

    # this one is special because it requires also another assembly for events that is internal (doens't have a managed end)

    # append to list of declaration for Interop Assemblies table
    list(APPEND CLR_RT_NativeAssemblyDataList "extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink_DriverProcs;")
    # append to list of entries for Interop Assemblies table
    list(APPEND CLR_RT_NativeAssemblyDataTableEntriesList "&g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink_DriverProcs,")

endif()

# System.IO.FileSystem
if(API_System.IO.FileSystem)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.IO.FileSystem")
endif()

# System.Math
if(API_System.Math)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Math")
endif()

# System.Net
if(API_System.Net)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Net")
endif()

# System.Device.Adc
if(API_System.Device.Adc)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Device.Adc")
endif()

# System.Device.Dac
if(API_System.Device.Dac)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Device.Dac")
endif()

# System.Device.Gpio
if(API_System.Device.Gpio)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Device.Gpio")
endif()

# System.Device.I2c
if(API_System.Device.I2c)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Device.I2c")
endif()

# System.Device.I2c.Slave
if(API_System.Device.I2c.Slave)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Device.I2c.Slave")
endif()

# System.Device.I2s
if(API_System.Device.I2s)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Device.I2s")
endif()

# System.Device.Pwm
if(API_System.Device.Pwm)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Device.Pwm")
endif()

# API_System.IO.Ports
if(API_System.IO.Ports)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.IO.Ports")
endif()

# System.Device.Spi
if(API_System.Device.Spi)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Device.Spi")
endif()

# System.Runtime.Serialization
if(API_System.Runtime.Serialization)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Runtime.Serialization")
endif()

# System.Device.Wifi
if(API_System.Device.Wifi)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Device.Wifi")
endif()

# System.Device.UsbStream
if(API_System.Device.UsbStream)
    ##### API name here (doted name)
    PerformSettingsForApiEntry("System.Device.UsbStream")
endif()

# Interop assemblies
ParseInteropAssemblies()

##################################
# add parsing of private APIs here

##################################

# parse the declarations to have new lines and ';'
string(REPLACE ";;" ";\n" CLR_RT_NativeAssemblyDataDeclarations "${CLR_RT_NativeAssemblyDataList}")
# parse the list to have new lines, ',' and identation
string(REPLACE ";" "\n    " CLR_RT_NativeAssemblyDataTableEntries "${CLR_RT_NativeAssemblyDataTableEntriesList}")


# get the count of interop assemblies
list(LENGTH CLR_RT_NativeAssemblyDataTableEntriesList CLR_RT_NativeAssembliesCount)


# configure header file with Interop Assemblies defines and...
configure_file("${CMAKE_SOURCE_DIR}/InteropAssemblies/CLR_IncludedAPI.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/CLR_IncludedAPI.h" @ONLY)

# configure code file with Interop Assemblies table and...
configure_file("${CMAKE_SOURCE_DIR}/InteropAssemblies/CLR_RT_InteropAssembliesTable.cpp.in"
                "${CMAKE_CURRENT_BINARY_DIR}/CLR_RT_InteropAssembliesTable.cpp" @ONLY)
# ... now add Interop Assemblies table to nanoCLR sources list
list(APPEND NF_NativeAssemblies_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/CLR_RT_InteropAssembliesTable.cpp")

# create a .csv file with native assembly versions in the output directory
string(REPLACE ";" "\r\n" NF_NativeAssemblies_CSV "${NF_NativeAssemblies_VERSIONS}")
file(WRITE "${CMAKE_BINARY_DIR}/native_assemblies.csv" "${NF_NativeAssemblies_CSV}")

# output the list of APIs included
list(LENGTH apiListing apiListingLenght)

if(apiListingLenght GREATER 0)
    
    # APIs included
    message(STATUS "")
    message(STATUS " *** APIs included ***")
    message(STATUS "")

    foreach(entry ${apiListing})
        message(STATUS " ${entry}")
    endforeach(entry ${})
    
    message(STATUS "")
    message(STATUS " ***  end of APIs  ***")
    message(STATUS "")

else()
    # no APIs were included
    message(STATUS " *** NO APIs included ***")    
endif()

list(APPEND NF_NativeAssemblies_INCLUDE_DIRS ${TARGET_BASE_LOCATION})

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_NativeAssemblies DEFAULT_MSG NF_NativeAssemblies_INCLUDE_DIRS NF_NativeAssemblies_SOURCES)

# macro to be called from binutils to add Core CLR library
# optional EXTRA_SOURCES with source files to be added to the library
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
macro(nf_add_lib_native_assemblies)

    # parse arguments
    cmake_parse_arguments(NFALNA "" "" "EXTRA_SOURCES;EXTRA_INCLUDES;EXTRA_COMPILE_DEFINITIONS" ${ARGN})

    # add this has a library
    set(LIB_NAME NF_NativeAssemblies)

    add_library(
        ${LIB_NAME} STATIC 
            ${NF_NativeAssemblies_SOURCES}
            ${NFALNA_EXTRA_SOURCES})   

    target_include_directories(
        ${LIB_NAME} 
        PUBLIC 
            ${NF_NativeAssemblies_INCLUDE_DIRS}
            ${NF_CoreCLR_INCLUDE_DIRS}

            ${NFALNA_EXTRA_INCLUDES})   

    # TODO can be removed later
    if(RTOS_ESP32_CHECK)
        # this is the only one different

        nf_set_compile_options(TARGET ${LIB_NAME})

        nf_set_compile_definitions(
            TARGET ${LIB_NAME} 
            EXTRA_COMPILE_DEFINITIONS
                -DPLATFORM_ESP32
                ${NFALNA_EXTRA_COMPILE_DEFINITIONS} 
            BUILD_TARGET ${NANOCLR_PROJECT_NAME})

        nf_set_link_options(TARGET ${LIB_NAME})

    else() 
        nf_set_compile_options(TARGET ${LIB_NAME})
        nf_set_compile_definitions(TARGET ${LIB_NAME} EXTRA_COMPILE_DEFINITIONS ${NFALNA_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NANOCLR_PROJECT_NAME})
        nf_set_link_options(TARGET ${LIB_NAME})
    endif()

    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})
    
endmacro()
