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
# optional BUILD_TARGET when target it's a library pass here the name ot the target that's building for (either nanoBooter or nanoCLR)
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
macro(nf_set_compile_definitions)

    # parse arguments
    cmake_parse_arguments(NFSCD "" "TARGET;BUILD_TARGET" "EXTRA_COMPILE_DEFINITIONS" ${ARGN})

    if(NOT NFSCD_TARGET OR "${NFSCD_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_definitions()")
    endif()

    nf_common_compiler_definitions(TARGET ${NFSCD_TARGET} BUILD_TARGET ${NFSCD_BUILD_TARGET})

    # include extra compiler definitions
    target_compile_definitions(${NFSCD_TARGET} PUBLIC ${NFSCD_EXTRA_COMPILE_DEFINITIONS})

endmacro()

# Add packages that are common to FreeRTOS platform builds
# To be called from target CMakeList.txt
# optional TARGET argument with target name
macro(nf_add_platform_packages)

    # parse arguments
    cmake_parse_arguments(NFAPP "" "TARGET" "" ${ARGN})

    find_package(FreeRTOS REQUIRED QUIET)
    find_package(CMSIS REQUIRED QUIET)
    
    if(USE_FILESYSTEM_OPTION)
        find_package(FATFS REQUIRED QUIET)
    endif()

    # packages specific for nanoBooter
    if("${NFAPP_TARGET}" STREQUAL "${NANOBOOTER_PROJECT_NAME}")
        # no packages for booter
    endif()

    # packages specific for nanoCLR
    if("${NFAPP_TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        if(USE_NETWORKING_OPTION)

            find_package(NF_Network REQUIRED QUIET)
            find_package(LWIP REQUIRED QUIET)

        endif()

    endif()

endmacro()

# Add FreeRTOS platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_dependencies target)

    nf_add_common_dependencies(${target})
    
    # packages specific for nanoBooter
    if("${target}" STREQUAL "${NANOBOOTER_PROJECT_NAME}")
        # no packages for booter
    endif()

    # dependencies specific to nanoCLR
    if("${target}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        nf_add_lib_coreclr(
            EXTRA_INCLUDES
                ${CMSIS_INCLUDE_DIRS}
                ${FreeRTOS_INCLUDE_DIRS}
                ${TARGET_NXP_COMMON_INCLUDE_DIRS}
                ${TARGET_FREERTOS_COMMON_INCLUDE_DIRS}
                ${TARGET_FREERTOS_NANOCLR_INCLUDE_DIRS})

        add_dependencies(${target}.elf nano::NF_CoreCLR)
       
        nf_add_lib_wireprotocol(
            EXTRA_INCLUDES
                ${CMSIS_INCLUDE_DIRS}
                ${FreeRTOS_INCLUDE_DIRS}
                ${TARGET_NXP_COMMON_INCLUDE_DIRS}
                ${TARGET_FREERTOS_COMMON_INCLUDE_DIRS}
                ${TARGET_FREERTOS_NANOCLR_INCLUDE_DIRS})
        
        add_dependencies(${target}.elf nano::WireProtocol)

        if(NF_FEATURE_DEBUGGER)

            nf_add_lib_debugger(
                EXTRA_INCLUDES
                    ${CMSIS_INCLUDE_DIRS}
                    ${FreeRTOS_INCLUDE_DIRS}
                    ${TARGET_NXP_COMMON_INCLUDE_DIRS}
                    ${TARGET_FREERTOS_COMMON_INCLUDE_DIRS}
                    ${TARGET_FREERTOS_NANOCLR_INCLUDE_DIRS})
            
            add_dependencies(${target}.elf nano::NF_Debugger)

        endif()

        nf_add_lib_native_assemblies(
            EXTRA_INCLUDES
                ${CMSIS_INCLUDE_DIRS}
                ${FreeRTOS_INCLUDE_DIRS}
                ${LWIP_INCLUDE_DIRS}
                ${TARGET_NXP_COMMON_INCLUDE_DIRS}
                ${TARGET_NXP_NANOCLR_INCLUDE_DIRS}
                ${TARGET_FREERTOS_COMMON_INCLUDE_DIRS}
                ${TARGET_FREERTOS_NANOCLR_INCLUDE_DIRS}
                ${FATFS_INCLUDE_DIRS}
                ${CMAKE_CURRENT_BINARY_DIR}
                ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/NXP/_fatfs
                ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD})
        
        add_dependencies(${target}.elf nano::NF_NativeAssemblies)

        if(USE_NETWORKING_OPTION)

            nf_add_lib_network(
                BUILD_TARGET
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

    # includes specific to nanoCLR
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC

            ${TARGET_NXP_NANOCLR_INCLUDE_DIRS}
            ${NANOCLR_PROJECT_INCLUDE_DIRS}
            ${TARGET_FREERTOS_NANOCLR_INCLUDE_DIRS}
            ${LWIP_INCLUDE_DIRS}
            ${CMAKE_CURRENT_BINARY_DIR}

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
        )

    endif()

    # sources specific to nanoCLR
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

# macro to setup the build for a target
# mandatory HAS_NANOBOOTER specifing if the target implements nanoBooter
# BOOTER_LINKER_FILE with the path to the linker file for nanoBooter (if the target has it)
# mandatory CLR_LINKER_FILE with the path to the linker file for nanoCLR
# optional BOOTER_EXTRA_SOURCE_FILES with paths to extra files to be added to the nanoBooter build target
# optional CLR_EXTRA_SOURCE_FILES with paths to extra files to be added to the nanoCLR build target
# optional BOOTER_EXTRA_COMPILE_DEFINITIONS extra nanoBooter compile definitions to pass to nf_set_compile_definitions() 
# optional CLR_EXTRA_COMPILE_DEFINITIONS extra nanoCLR compile definitions to pass to nf_set_compile_definitions() 
# optional BOOTER_EXTRA_LINKMAP_PROPERTIES extra nanoBooter link map properties to pass to nf_set_link_map() 
# optional CLR_EXTRA_LINKMAP_PROPERTIES extra nanoCLR link map properties to pass to nf_set_link_map() 
# optional BOOTER_EXTRA_LINK_FLAGS extra nanoBooter link flags to pass to nf_set_link_options() 
# optional CLR_EXTRA_LINK_FLAGS extra nanoCLR link flags to pass to nf_set_link_options() 
macro(nf_setup_target_build)

    # OK to pass ARGN, to have it perform it's parsings and validation 
    nf_setup_target_build_common(${ARGN})

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
