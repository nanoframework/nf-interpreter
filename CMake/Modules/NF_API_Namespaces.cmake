#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#


############################################################################################
# WHEN ADDING A NEW API add the name that corresponds to the CMake option here
# e.g.: for namespace Windows.Devices.Gpio, the CMake option is 'API_Windows.Devices.Gpio' 
# and the namespace designation is 'Windows.Devices.Gpio'
###########################################################################################

option(API_Windows.Devices.Gpio "option for Windows.Devices.Gpio API")
option(API_Windows.Devices.Spi "option for Windows.Devices.Spi API")


#################################################################
# macro to perform individual settings to add an API to the build
macro(PerformSettingsForApiEntry apiNamespace)
    
    # namespace with '_' replacing '.'
    string(REPLACE "." "_" apiNamespaceWithoutDots "${apiNamespace}")

    # list this option
    list(APPEND apiListing "${apiNamespace}")

    # append to list of declaration for Interop Assemblies table
    list(APPEND CLR_RT_NativeAssemblyDataList "extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_${apiNamespaceWithoutDots};")

    # append to list of entries for Interop Assemblies table
    list(APPEND CLR_RT_NativeAssemblyDataTableEntriesList "&g_CLR_AssemblyNative_${apiNamespaceWithoutDots},")

    # find the module
    find_package(${apiNamespace} REQUIRED)

    # append include directories to list with includes for all the APIs
    list(APPEND TARGET_NANO_APIS_INCLUDES "${${apiNamespace}_INCLUDE_DIRS}")
    list(REMOVE_DUPLICATES TARGET_NANO_APIS_INCLUDES)

    # append source files to list wiht source files for all the APIs
    list(APPEND TARGET_NANO_APIS_SOURCES "${${apiNamespace}_SOURCES}")
    list(REMOVE_DUPLICATES TARGET_NANO_APIS_SOURCES)

endmacro()
#################################################################


############################################################################################
# WHEN ADDING A NEW API add the corresponding block bellow 
# required changes:
# 1. change the 'IF' to match the CMake option 
# 2. change the call to PerformSettingsForApiEntry() macro with the API name (doted naming)
############################################################################################

macro(ParseApiOptions)
    
    # Windows.Devices.Gpio
    if(API_Windows.Devices.Gpio)
        ##### API name here (doted name)
        PerformSettingsForApiEntry("Windows.Devices.Gpio")
    endif()
	# Windows.Devices.Spi
    if(API_Windows.Devices.Spi)
        ##### API name here (doted name)
        PerformSettingsForApiEntry("Windows.Devices.Spi")
    endif()


    # parse the declarations to have new lines and ';'
    string(REPLACE ";;" ";\n" CLR_RT_NativeAssemblyDataDeclarations "${CLR_RT_NativeAssemblyDataList}")
    # parse the list to have new lines, ',' and identation
    string(REPLACE ";" "\n    " CLR_RT_NativeAssemblyDataTableEntries "${CLR_RT_NativeAssemblyDataTableEntriesList}")


    # make the vars global
    set(CLR_RT_NativeAssemblyData_declarations CACHE INTERNAL "make global")
    set(CLR_RT_NativeAssemblyData_table_entries CACHE INTERNAL "make global")
    set(TARGET_NANO_APIS_INCLUDES ${TARGET_NANO_APIS_INCLUDES} CACHE INTERNAL "make global")
    set(TARGET_NANO_APIS_SOURCES ${TARGET_NANO_APIS_SOURCES} CACHE INTERNAL "make global")


    # output the list of APIs included
    list(LENGTH apiListing apiListingLenght)

    if(apiListingLenght GREATER 0)
        
        # APIs included
        message(STATUS "")
        message(STATUS " *** APIs included ***")
        message(STATUS "")

        foreach(entry ${apiListing})
            message(STATUS " ${entry}")
            message(STATUS "")
        endforeach(entry ${})
        
        message(STATUS " ***  end of APIs  ***")
        message(STATUS "")

    else()
        # no APIs were included
        message(STATUS " *** NO APIs included ***")    
    endif()

endmacro()
