#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)

# Add packages that are common to ESP32 platform builds
# To be called from target CMakeList.txt
# optional TARGET argument with target name
macro(nf_add_platform_packages)

    # parse arguments
    cmake_parse_arguments(NFAPP "" "TARGET" "" ${ARGN})

    # packages specific for nanoCRL
    if("${NFAPP_TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        if(USE_NETWORKING_OPTION)

            find_package(NF_Network REQUIRED)

            # security provider is mbedTLS
            if(USE_SECURITY_MBEDTLS_OPTION)
                find_package(mbedTLS REQUIRED)
            endif()

        endif()

    endif()
    
endmacro()

# Add ESP32  platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_dependencies target)

    nf_add_common_dependencies(${target})

    nf_add_lib_coreclr(
        TARGET
            ${target}
        EXTRA_INCLUDES
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}/${target}
            ${CMAKE_CURRENT_SOURCE_DIR}/${target}
            ${CMAKE_CURRENT_SOURCE_DIR}/Include
            ${CMAKE_CURRENT_SOURCE_DIR}/Network
            ${TARGET_ESP32_IDF_INCLUDES})

endmacro()

# Add ESP32 platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_include_directories target)

    target_include_directories(${target}.elf PUBLIC
                
        # target path (both source and binary)
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        # target common path
        ${CMAKE_CURRENT_SOURCE_DIR}/common

        # path for CMake target (both source and binary)
        ${CMAKE_CURRENT_BINARY_DIR}/${target}
        ${CMAKE_CURRENT_SOURCE_DIR}/${target}

        ${NF_CoreCLR_INCLUDE_DIRS}
        ${NF_NativeAssemblies_INCLUDE_DIRS}

        ${WireProtocol_INCLUDE_DIRS}
    )

    target_include_directories(${target}.elf PUBLIC

        # directories for nanoFramework libraries
        ${NF_CoreCLR_INCLUDE_DIRS}
        ${NF_Debugger_INCLUDE_DIRS}
        ${NF_Diagnostics_INCLUDE_DIRS}
        
        ${NF_Network_INCLUDE_DIRS}

        ${Graphics_Includes}

        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/common

        ${TARGET_ESP32_IDF_INCLUDES}
    )

endmacro()

# Add ESP32 platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_sources target)

    # add header files with common OS definitions and board definitions specific for each image
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoCLR/target_board.h @ONLY)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_common.h @ONLY)

    configure_file(${CMAKE_SOURCE_DIR}/CMake/ESP32_target_os.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_os.h @ONLY)

    target_sources(${target}.elf PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}/target_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/target_BlockStorage.c
        ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c

        ${COMMON_PROJECT_SOURCES}
        ${WireProtocol_SOURCES}

        ${NANOCLR_PROJECT_SOURCES}

        ${NF_HALCore_SOURCES}
        ${NF_CoreCLR_SOURCES}
        ${NF_Debugger_SOURCES}
        ${NF_Diagnostics_SOURCES}

        ${TARGET_ESP32_COMMON_SOURCES}
           
        ${TARGET_ESP32_NANOCLR_SOURCES}

        ${Graphics_Sources}
    )

    # mbed TLS requires a config file
    if(USE_SECURITY_MBEDTLS_OPTION)
        # this seems to be only option to properly set a compiler define through the command line that needs to be a string literal
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMBEDTLS_CONFIG_FILE=\"<${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/nf_mbedtls_config.h>\"")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMBEDTLS_CONFIG_FILE=\"<${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/nf_mbedtls_config.h>\"")
    endif()

endmacro()
