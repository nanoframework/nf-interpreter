#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)

# find a set of files on a list of possible locations
macro(NF_FIND_ESP32_FILES_AT_LOCATION files locations)

    foreach(SRC_FILE ${files})

        set(IDF_SRC_FILE SRC_FILE -NOTFOUND)

        find_file(IDF_SRC_FILE ${SRC_FILE}
            PATHS 

            ${locations}

            CMAKE_FIND_ROOT_PATH_BOTH
        )

        if (BUILD_VERBOSE)
            message("${SRC_FILE} >> ${IDF_SRC_FILE}")
        endif()

        list(APPEND ESP32_IDF_SOURCES ${IDF_SRC_FILE})

    endforeach()

endmacro()

function(NF_SET_OPTIMIZATION_OPTIONS TARGET) 

    # target_compile_options(${TARGET} PRIVATE
    #     $<$<CONFIG:Debug>:-Og -femit-class-debug-always -g3 -ggdb>
    #     $<$<CONFIG:Release>:-O3 -flto -fuse-linker-plugin -fno-fat-lto-objects>
    #     $<$<CONFIG:MinSizeRel>:-Os -flto>
    #     $<$<CONFIG:RelWithDebInfo>:-Os -femit-class-debug-always -g3 -ggdb>
    # )

endfunction()


function(NF_SET_LINK_MAP TARGET) 

    # # need to remove the .elf suffix from target name
    # string(FIND ${TARGET} "." TARGET_EXTENSION_DOT_INDEX)
    # string(SUBSTRING ${TARGET} 0 ${TARGET_EXTENSION_DOT_INDEX} TARGET_SHORT)
    
    # # add linker flags to generate map file
    # set_property(TARGET ${TARGET_SHORT}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET_SHORT}.map,--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common")

endfunction()

function(NF_SET_LINKER_FILE TARGET LINKER_FILE_NAME)

    # # set linker file name
    # set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-T${LINKER_FILE_NAME}")

endfunction()


function(NF_SET_COMPILER_DEFINITIONS TARGET)

    # definition for platform 

    # build types that have debugging capabilities AND are NOT RTM have to have the define 'NANOCLR_ENABLE_SOURCELEVELDEBUGGING'
    if((NOT NF_BUILD_RTM) OR NF_FEATURE_DEBUGGER)
        target_compile_definitions(${TARGET} PUBLIC "-DNANOCLR_ENABLE_SOURCELEVELDEBUGGING ")
    endif()

    # set compiler definition for RTM build option
    if(NF_BUILD_RTM)
        target_compile_definitions(${TARGET} PUBLIC -DBUILD_RTM)
    endif()

    # set compiler definition for using Application Domains feature
    if(NF_FEATURE_USE_APPDOMAINS)
        target_compile_definitions(${TARGET} PUBLIC -DNANOCLR_USE_APPDOMAINS)
    endif()

    # set compiler definition for implementing (or not) CRC32 in Wire Protocol
    if(NF_WP_IMPLEMENTS_CRC32)
        target_compile_definitions(${TARGET} PUBLIC -DWP_IMPLEMENTS_CRC32)
    endif()

    # set definition for Wire Protocol trace mask
    target_compile_definitions(${TARGET} PUBLIC -DTRACE_MASK=${WP_TRACE_MASK})

    # set compiler definition regarding inclusion of trace messages and checks on CLR
    if(NF_PLATFORM_NO_CLR_TRACE)
        target_compile_definitions(${TARGET} PUBLIC -DPLATFORM_NO_CLR_TRACE=1)
    endif()

    # set compiler definition regarding CLR IL inlining
    if(NF_CLR_NO_IL_INLINE)
        target_compile_definitions(${TARGET} PUBLIC -DNANOCLR_NO_IL_INLINE=1)
    endif()

    NF_COMMON_COMPILER_DEFINITIONS(${TARGET})

    # include any extra compiler definitions coming from extra args
    target_compile_definitions(${TARGET} PUBLIC ${ARGN})

endfunction()

function(NF_SET_ESP32_TARGET_SERIES)

    # process target series name to set it to lower case
    string(TOLOWER "${TARGET_SERIES}" TARGET_SERIES_1)
   
    # need to remove the underscore from the series name
    string(REPLACE "_" "" TARGET_SERIES_2 "${TARGET_SERIES_1}")

    # store the series name for later use
    set(TARGET_SERIES_SHORT ${TARGET_SERIES_2} CACHE INTERNAL "ESP32 target series lower case, short version")

endfunction()

# Add packages that are common to ESP32 platform builds
# To be called from target CMakeList.txt
# optional TARGET argument with target name
macro(nf_add_platform_packages)

    # parse arguments
    cmake_parse_arguments(NFAPP "" "TARGET" "" ${ARGN})

    # packages specific for nanoCRL
    if("${NFAPP_TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        if(USE_NETWORKING_OPTION)

            find_package(NF_Network REQUIRED QUIET)

            # security provider is mbedTLS
            if(USE_SECURITY_MBEDTLS_OPTION)
                find_package(mbedTLS REQUIRED QUIET)
            endif()

        endif()

    endif()
    
endmacro()

# Add ESP32  platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_dependencies target)

    nf_add_common_dependencies(${target})

    nf_add_lib_coreclr(
        EXTRA_INCLUDES
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}/${target}
            ${CMAKE_CURRENT_SOURCE_DIR}/${target}
            ${CMAKE_CURRENT_SOURCE_DIR}/Include
            ${CMAKE_CURRENT_SOURCE_DIR}/Network
            ${TARGET_ESP32_IDF_INCLUDES})

    find_package(ESP32_IDF REQUIRED)

    # if(USE_FILESYSTEM_OPTION)
    #     find_package(CHIBIOS_FATFS REQUIRED)
    # endif()

    # # SPIFFS
    # if(NF_FEATURE_USE_SPIFFS)
    #     find_package(STM32F7_CubePackage REQUIRED)
    #     find_package(SPIFFS REQUIRED)
    # endif()

endmacro()

# Add ESP32 platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_DEPENDENCIES TARGET)

    # if(USE_NETWORKING_OPTION)
    #     add_dependencies(${NANOCLR_PROJECT_NAME}.elf NetworkLib)
    # endif()

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

    # includes specific to nanoCRL
    if(${TARGET} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${TARGET}.elf PUBLIC

            ${TARGET_ESP32_IDF_NANOCLR_INCLUDE_DIRS}
        )

    endif()

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

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                    ${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h @ONLY)

        ${NF_HALCore_SOURCES}
        ${NF_CoreCLR_SOURCES}
        ${NF_Debugger_SOURCES}
        ${NF_Diagnostics_SOURCES}

            ${TARGET_ESP32_IDF_NANOCLR_SOURCES}
            ${ESP32_IDF_SOURCES}
        )

    endif()

    # mbed TLS requires a config file
    if(USE_SECURITY_MBEDTLS_OPTION)
        # this seems to be only option to properly set a compiler define through the command line that needs to be a string literal
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMBEDTLS_CONFIG_FILE=\"<${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/nf_mbedtls_config.h>\"")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMBEDTLS_CONFIG_FILE=\"<${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/nf_mbedtls_config.h>\"")
    endif()

endmacro()

# macro to clear binary files related with nanoBooter from output
# to make sure that the build file it's up to date
macro(nf_clear_output_files_nanobooter)
    nf_clear_common_output_files_nanobooter()
    # other files specific to this platform should go here
endmacro()

# macro to clear binary files related with nanoCLR from output
# to make sure that the build file it's up to date
macro(nf_clear_output_files_nanoclr)
    nf_clear_common_output_files_nanoclr()
    # other files specific to this platform should go here
endmacro()
