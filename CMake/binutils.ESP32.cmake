#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)

# Add packages that are common to ESP32 platform builds
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_PACKAGES)

endmacro()

# Add ESP32 platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_INCLUDE_DIRECTORIES TARGET)

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

    target_include_directories(${TARGET}.elf PUBLIC

        # directories for nanoFramework libraries
        ${NF_CoreCLR_INCLUDE_DIRS}
        ${NF_Debugger_INCLUDE_DIRS}
        ${NF_Diagnostics_INCLUDE_DIRS}
        
        ${NF_Networking_INCLUDE_DIRS}
        ${mbedTLS_INCLUDE_DIRS}

        ${Graphics_Includes}

        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/common

        ${TARGET_ESP32_IDF_INCLUDES}
    )

endmacro()

# Add ESP32 platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_SOURCES TARGET)

    # add header files with common OS definitions and board definitions specific for each image
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                   ${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                   ${CMAKE_CURRENT_BINARY_DIR}/target_common.h @ONLY)

    configure_file(${CMAKE_SOURCE_DIR}/CMake/ESP32_target_os.h.in
                   ${CMAKE_BINARY_DIR}/targets/FreeRTOS_ESP32/ESP32_WROOM_32/target_os.h @ONLY)

    target_sources(${TARGET}.elf PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}/target_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/target_BlockStorage.c

        ${COMMON_PROJECT_SOURCES}
        ${WireProtocol_SOURCES}
        ${nanoHALCore_SOURCES}    

        ${NANOCLR_PROJECT_SOURCES}

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
