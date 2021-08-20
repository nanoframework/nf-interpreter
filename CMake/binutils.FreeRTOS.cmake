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

function(nf_set_linker_file target linker_file_name)

    # set linker file name
    set_target_properties(${target} PROPERTIES LINK_FLAGS "-T${linker_file_name}")

endfunction()


# setting compile definitions for a target based on general build options
# TARGET parameter to set the target that's setting them for
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
# optional BUILD_TARGET when target it's a library pass here the name ot the target that's building for (either nanoBooter or nanoCLR)
macro(nf_set_compile_definitions)

    # parse arguments
    cmake_parse_arguments(_ "" "TARGET" "EXTRA_COMPILE_DEFINITIONS;BUILD_TARGET" ${ARGN})

    if(NOT __TARGET OR "${__TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_definitions()")
    endif()

    nf_common_compiler_definitions(TARGET ${__TARGET} BUILD_TARGET ${__BUILD_TARGET})

    # include extra compiler definitions
    target_compile_definitions(${__TARGET} PUBLIC ${__EXTRA_COMPILE_DEFINITIONS})

endmacro()

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

    nf_add_common_dependencies(${target})

    # dependencies specific to nanoCRL
    if("${target}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        nf_add_lib_coreclr(
            TARGET
                ${target}
            EXTRA_INCLUDES
                ${CMSIS_INCLUDE_DIRS}
                ${FreeRTOS_INCLUDE_DIRS}
                ${TARGET_NXP_COMMON_INCLUDE_DIRS}
                ${TARGET_FREERTOS_COMMON_INCLUDE_DIRS}
                ${TARGET_FREERTOS_NANOCLR_INCLUDE_DIRS})

        if(USE_NETWORKING_OPTION)

            nf_add_lib_network(
                TARGET
                    ${target}
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

            ${CMAKE_CURRENT_SOURCE_DIR}/target_BlockStorage.c

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
