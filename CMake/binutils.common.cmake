#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set compiler definitions that are common to all builds/targets
macro(NF_COMMON_COMPILER_DEFINITIONS TARGET) 

    # set define according to target
    string(FIND ${TARGET} ${NANOBOOTER_PROJECT_NAME} BOOTER_INDEX)
    string(FIND ${TARGET} ${NANOCLR_PROJECT_NAME} CLR_INDEX)
    
    if(${BOOTER_INDEX} EQUAL 0)
        target_compile_definitions(${TARGET} PUBLIC -DI_AM_NANOBOOTER)
    elseif(${CLR_INDEX} EQUAL 0)
        target_compile_definitions(${TARGET} PUBLIC -DI_AM_NANOCLR)
    else()
        message(FATAL_ERROR "\n\n Target name is not any of the expected ones: '${NANOBOOTER_PROJECT_NAME}' or '${NANOCLR_PROJECT_NAME}'")
    endif()

endmacro()

# Add packages that are common to ALL builds
# To be called from target CMakeList.txt
macro(NF_ADD_COMMON_PACKAGES)

    find_package(BuildUtils REQUIRED)
    find_package(WireProtocol REQUIRED)
    find_package(NF_NativeAssemblies)
    find_package(NF_CoreCLR REQUIRED)
    find_package(nanoHALCore REQUIRED)

    if(NF_FEATURE_DEBUGGER)
        find_package(NF_Debugger REQUIRED)
        find_package(NF_Diagnostics REQUIRED)
    endif()

    # nF feature: networking
    if(USE_NETWORKING_OPTION)
        find_package(NF_Networking REQUIRED)
    endif()

    # security provider is mbedTLS
    if(USE_SECURITY_MBEDTLS_OPTION)
        find_package(mbedTLS REQUIRED)
    endif()

endmacro()

# Add common include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_COMMON_INCLUDE_DIRECTORIES TARGET)

    target_include_directories(${TARGET}.elf PUBLIC
        
        # target path (both source and binary)
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        # target common path
        ${CMAKE_CURRENT_SOURCE_DIR}/common

        # path for CMake target (both source and binary)
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}
        ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}

        ${NF_CoreCLR_INCLUDE_DIRS}
        ${NF_NativeAssemblies_INCLUDE_DIRS}

        ${WireProtocol_INCLUDE_DIRS}
        ${nanoHALCore_INCLUDE_DIRS}
    )

    # includes specific to nanoCRL
    if(${TARGET} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${TARGET}.elf PUBLIC

            # directories for nanoFramework libraries
            ${NF_CoreCLR_INCLUDE_DIRS}
            ${NF_Debugger_INCLUDE_DIRS}
            ${NF_Diagnostics_INCLUDE_DIRS}

            ${NF_Networking_INCLUDE_DIRS}
            ${mbedTLS_INCLUDE_DIRS}
        
            ${Graphics_Includes}
        )

    endif()


endmacro()

# Add common target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_COMMON_SOURCES TARGET)

    target_sources(${TARGET}.elf PUBLIC
    
        ${CMAKE_CURRENT_SOURCE_DIR}/target_common.c

        ${COMMON_PROJECT_SOURCES}
        ${WireProtocol_SOURCES}
        ${nanoHALCore_SOURCES}    
    )

    # sources specific to nanoBooter
    if(${TARGET} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        target_sources(${TARGET}.elf PUBLIC
            
            ${CMAKE_CURRENT_SOURCE_DIR}/target_BlockStorage.c

            ${NANOBOOTER_PROJECT_SOURCES}
        )

    endif()

    # sources specific to nanoCRL
    if(${TARGET} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_sources(${TARGET}.elf PUBLIC

            ${NANOCLR_PROJECT_SOURCES}

            # sources for nanoFramework libraries
            ${NF_CoreCLR_SOURCES}
            ${NF_Debugger_SOURCES}
            ${NF_Diagnostics_SOURCES}

            ${NF_Networking_SOURCES}
            ${mbedTLS_SOURCES}

            # sources for nanoFramework APIs
            ${NF_NativeAssemblies_SOURCES}

            ${Graphics_Sources}

        )

    endif()

endmacro()
