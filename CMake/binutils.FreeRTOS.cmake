#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

function(nf_set_optimization_options target) 

    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Debug>:-Og -femit-class-debug-always -g3 -ggdb>
        $<$<CONFIG:Release>:-O3 -flto -fuse-linker-plugin -fno-fat-lto-objects>
        $<$<CONFIG:MinSizeRel>:-Os -flto -fuse-linker-plugin -fno-fat-lto-objects>
        $<$<CONFIG:RelWithDebInfo>:-Os -femit-class-debug-always -g3 -ggdb>
    )

endfunction()


function(nf_set_link_map target) 

    # need to remove the .elf suffix from target name
    string(FIND ${target} "." TARGET_EXTENSION_DOT_INDEX)
    string(SUBSTRING ${target} 0 ${TARGET_EXTENSION_DOT_INDEX} TARGET_SHORT)
    
    # add linker flags to generate map file
    set_property(TARGET ${TARGET_SHORT}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${CMAKE_SOURCE_DIR}/build/${TARGET_SHORT}.map")

endfunction()


function(nf_set_linker_file target linker_file_name)

    # set linker file name
    set_target_properties(${target} PROPERTIES LINK_FLAGS "-T${linker_file_name}")

endfunction()


function(nf_set_compiler_definitions target)

    # build types that have debugging capabilities AND are NOT RTM have to have the define 'NANOCLR_ENABLE_SOURCELEVELDEBUGGING'
    if((NOT NF_BUILD_RTM) OR NF_FEATURE_DEBUGGER)
        target_compile_definitions(${target} PUBLIC -DNANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    endif()

    # set compiler definition for RTM build option
    if(NF_BUILD_RTM)
        target_compile_definitions(${target} PUBLIC -DBUILD_RTM)
    endif()

    # set compiler definition for using Application Domains feature
    if(NF_FEATURE_USE_APPDOMAINS)
        target_compile_definitions(${target} PUBLIC -DNANOCLR_USE_APPDOMAINS)
    endif()

    # set compiler definition for implementing (or not) CRC32 in Wire Protocol
    if(NF_WP_IMPLEMENTS_CRC32)
        target_compile_definitions(${target} PUBLIC -DWP_IMPLEMENTS_CRC32)
    endif()

    # set definition for Wire Protocol trace mask
    target_compile_definitions(${target} PUBLIC -DTRACE_MASK=${WP_TRACE_MASK})

    # set compiler definition regarding inclusion of trace messages and checks on CLR
    if(NF_PLATFORM_NO_CLR_TRACE)
        target_compile_definitions(${target} PUBLIC -DPLATFORM_NO_CLR_TRACE=1)
    endif()

    # set compiler definition regarding CLR IL inlining
    if(NF_CLR_NO_IL_INLINE)
        target_compile_definitions(${target} PUBLIC -DNANOCLR_NO_IL_INLINE=1)
    endif()

    nf_common_compiler_definitions(${target})

    # include any extra compiler definitions comming from extra args
    target_compile_definitions(${target} PUBLIC ${ARGN})

endfunction()

# Add packages that are common to FreeRTOS platform builds
# To be called from target CMakeList.txt
# optional TARGET argument with target name
macro(nf_add_platform_packages)

    # parse arguments
    cmake_parse_arguments(_ "" "TARGET" "" ${ARGN})

    find_package(FreeRTOS REQUIRED)
    find_package(CMSIS REQUIRED)
    
    if(USE_FILESYSTEM_OPTION)
        find_package(FATFS REQUIRED)
    endif()

    # packages specific for nanoBooter
    if("${__TARGET}" STREQUAL "${NANOBOOTER_PROJECT_NAME}")
        # no packages for booter
    endif()

    # packages specific for nanoCRL
    if("${__TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        if(USE_NETWORKING_OPTION)

            find_package(NF_Network REQUIRED)
            find_package(LWIP REQUIRED)

            # security provider is mbedTLS
            if(USE_SECURITY_MBEDTLS_OPTION)
                find_package(mbedTLS REQUIRED)
            endif()

        endif()

    endif()

endmacro()

# Add FreeRTOS platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_dependencies target)

    # dependencies specific to nanoCRL
    if("${target}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        if(USE_NETWORKING_OPTION)

            nf_add_lib_network(
                EXTRA_SOURCES 
                    ${LWIP_SOURCES}
                EXTRA_INCLUDES 
                    ${FreeRTOS_INCLUDE_DIRS}
                    ${TARGET_NXP_COMMON_INCLUDE_DIRS}
                    ${TARGET_FREERTOS_COMMON_INCLUDE_DIRS}
                    ${LWIP_INCLUDE_DIRS}
                    ${CMSIS_INCLUDE_DIRS})

            add_dependencies(${target}.elf nano::NF_Network)

        endif()

    endif()

endmacro()

# Add FreeRTOS platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_include_directories target)

    target_include_directories(${target}.elf PUBLIC

        ${TARGET_FREERTOS_COMMON_INCLUDE_DIRS}
        ${TARGET_NXP_COMMON_INCLUDE_DIRS}
        ${FreeRTOS_INCLUDE_DIRS}
        ${CMSIS_INCLUDE_DIRS}
    )
    
    # includes specific to nanoBooter
    if(${target} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC
            
            ${TARGET_NXP_NANOBOOTER_INCLUDE_DIRS}
            ${TARGET_FREERTOS_NANOBOOTER_INCLUDE_DIRS}
        )

    endif()

    # includes specific to nanoCRL
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC

            ${TARGET_NXP_NANOCLR_INCLUDE_DIRS}
            ${NANOCLR_PROJECT_INCLUDE_DIRS}
            ${TARGET_FREERTOS_NANOCLR_INCLUDE_DIRS}
            ${LWIP_INCLUDE_DIRS}
            ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/_fatfs
            ${FATFS_INCLUDE_DIRS}

        )

    endif()


endmacro()

# Add FreeRTOS platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_sources target)

    # add header files with common OS definitions and board definitions 
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_common.h @ONLY)

    # sources common to both builds
    target_sources(${target}.elf PUBLIC
    
        ${TARGET_FREERTOS_COMMON_SOURCES}
        ${TARGET_NXP_COMMON_SOURCES}
        
        ${FreeRTOS_SOURCES}
    )

    # sources specific to nanoBooter
    if(${target} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        # add header files with common OS definitions and board definitions 
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/target_board.h.in
                       ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoBooter/target_board.h @ONLY)

        target_sources(${target}.elf PUBLIC
            
            ${TARGET_NXP_NANOBOOTER_SOURCES}
            ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c
           
            # need to add configuration manager to allow get/store configuration blocks
            ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager_stubs.c
        )

    endif()

    # sources specific to nanoCRL
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        # add header files with common OS definitions and board definitions 
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                       ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoCLR/target_board.h @ONLY)

        target_sources(${target}.elf PUBLIC

            ${TARGET_FREERTOS_COMMON_SOURCES}
            ${TARGET_FREERTOS_NANOCLR_SOURCES}
            ${TARGET_NXP_COMMON_SOURCES}
            ${TARGET_NXP_NANOCLR_SOURCES}

            ${FATFS_SOURCES}
        )

        if(USE_NETWORKING_OPTION)
            target_link_libraries(${target}.elf
                nano::NF_Network
            )
        endif()

    endif()

endmacro()
