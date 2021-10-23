#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)

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
    cmake_parse_arguments(NFSCD "" "TARGET" "EXTRA_COMPILE_DEFINITIONS;BUILD_TARGET" ${ARGN})

    if(NOT NFSCD_TARGET OR "${NFSCD_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_definitions()")
    endif()

    # definition for platform 
    if(CHIBIOS_HAL_REQUIRED)
        # ChibiOS HAL community always include (nanoFramework overlay and official community contributions optionally)
        target_compile_definitions(${NFSCD_TARGET} PUBLIC -DHAL_USE_COMMUNITY)
    endif()

    nf_common_compiler_definitions(TARGET ${NFSCD_TARGET} BUILD_TARGET ${NFSCD_BUILD_TARGET})

    # include extra compiler definitions
    target_compile_definitions(${NFSCD_TARGET} PUBLIC ${NFSCD_EXTRA_COMPILE_DEFINITIONS})

endmacro()

function(nf_set_stm32_target_series)
    # process target series, which is in the format "STM32F4xx"
    string(REPLACE "STM32" "" TARGET_SERIES_SHORT_1 "${TARGET_SERIES}")
    string(REPLACE "xx" "" TARGET_SERIES_SHORT "${TARGET_SERIES_SHORT_1}")
    
    # store the series short name for later use
    set(TARGET_SERIES_SHORT ${TARGET_SERIES_SHORT} CACHE INTERNAL "STM32 target series short name")

endfunction()

# Add packages that are common to Azure RTOS platform builds
# To be called from target CMakeList.txt
# optional TARGET argument with target name
macro(nf_add_platform_packages)

    # parse arguments
    cmake_parse_arguments(NFAPP "" "TARGET" "" ${ARGN})

    # if(USE_FILESYSTEM_OPTION)
    #     find_package(CHIBIOS_FATFS REQUIRED QUIET)
    # endif()

    # # SPIFFS
    # if(NF_FEATURE_USE_SPIFFS)
    #     find_package(STM32F7_CubePackage REQUIRED QUIET)
    #     find_package(SPIFFS REQUIRED QUIET)
    # endif()
    
    if(CHIBIOS_HAL_REQUIRED)
        find_package(ChibiOS_${TARGET_SERIES_SHORT}_HAL REQUIRED QUIET)
        find_package(ChibiOSnfOverlay REQUIRED QUIET)
    endif()

    if(STM32_CUBE_PACKAGE_REQUIRED)
        find_package(${TARGET_STM32_CUBE_PACKAGE}_CubePackage REQUIRED QUIET)
    endif()

    if(MAXIM_MICROS_SDK_REQUIRED)
        find_package(MaximMicrosSDK REQUIRED QUIET)
    endif()    
    
    # packages specific for nanoBooter
    if("${NFAPP_TARGET}" STREQUAL "${NANOBOOTER_PROJECT_NAME}")
        # no packages for booter
    endif()

    # packages specific for nanoCRL
    if("${NFAPP_TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        if(USE_NETWORKING_OPTION)

            # find_package(NF_Network REQUIRED QUIET)
            # find_package(CHIBIOS_LWIP REQUIRED QUIET)

            # # security provider is mbedTLS
            # if(USE_SECURITY_MBEDTLS_OPTION)
            #     find_package(mbedTLS REQUIRED QUIET)
            # endif()

        endif()

    endif()

endmacro()

# Add Azure RTOS platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_dependencies target)

    nf_add_common_dependencies(${target})

    if(CHIBIOS_HAL_REQUIRED)
        # add_dependencies(${TARGET}.elf CHIBIOS_STARTUP_HAL)
    endif()

    # add_dependencies(threadx SETUP_AZURERTOS)
    add_dependencies(${target}.elf azrtos::threadx)

    # specific to nanoCRL
    if("${target}" STREQUAL "${NANOCLR_PROJECT_NAME}")
            
        FetchContent_GetProperties(azure_rtos)

        get_target_property(AZRTOS_INCLUDES azrtos::threadx INCLUDE_DIRECTORIES)

        nf_add_lib_coreclr(
            EXTRA_INCLUDES
                ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
                ${ChibiOSnfOverlay_INCLUDE_DIRS}
                ${azure_rtos_SOURCE_DIR}/common/inc
                ${AZRTOS_INCLUDES})

        add_dependencies(NF_CoreCLR azrtos::threadx)
        add_dependencies(${target}.elf nano::NF_CoreCLR)

        nf_add_lib_wireprotocol(
            EXTRA_INCLUDES
                ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
                ${ChibiOSnfOverlay_INCLUDE_DIRS}
                ${azure_rtos_SOURCE_DIR}/common/inc
                ${AZRTOS_INCLUDES})

        add_dependencies(WireProtocol azrtos::threadx)
        add_dependencies(${target}.elf nano::WireProtocol)

        if(NF_FEATURE_DEBUGGER)

            nf_add_lib_debugger(
                EXTRA_INCLUDES
                    ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
                    ${ChibiOSnfOverlay_INCLUDE_DIRS}
                    ${azure_rtos_SOURCE_DIR}/common/inc
                    ${AZRTOS_INCLUDES})

            add_dependencies(NF_Debugger azrtos::threadx)
            add_dependencies(${target}.elf nano::NF_Debugger)

        endif()

        nf_add_lib_native_assemblies(
            EXTRA_INCLUDES
                ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
                ${CHIBIOS_HAL_INCLUDE_DIRS}
                ${ChibiOSnfOverlay_INCLUDE_DIRS}
                ${azure_rtos_SOURCE_DIR}/common/inc
                ${CMAKE_SOURCE_DIR}/targets/AzureChibiOS/${TARGET_BOARD}
                ${AZRTOS_INCLUDES})
        
        add_dependencies(NF_NativeAssemblies azrtos::threadx)
        add_dependencies(${target}.elf nano::NF_NativeAssemblies)
        
        # nF feature: networking
        if(USE_NETWORKING_OPTION)

            # nf_add_lib_network(
            #     BUILD_TARGET
            #         ${target}
            #     EXTRA_SOURCES 
            #         ${TARGET_LWIP_SOURCES}
            #         ${CHIBIOS_LWIP_SOURCES}
            #     EXTRA_INCLUDES 
            #         ${CHIBIOS_INCLUDE_DIRS}
            #         ${CHIBIOS_HAL_INCLUDE_DIRS}
            #         ${TARGET_CHIBIOS_COMMON_INCLUDE_DIRS}
            #         ${TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS}
            #         ${CHIBIOS_LWIP_INCLUDE_DIRS}
            #         ${ChibiOSnfOverlay_INCLUDE_DIRS}
            #         ${CHIBIOS_CONTRIB_INCLUDE_DIRS}
            #         ${${TARGET_STM32_CUBE_PACKAGE}_CubePackage_INCLUDE_DIRS}
            #     EXTRA_COMPILE_DEFINITIONS -DHAL_USE_MAC=TRUE)

            # add_dependencies(${target}.elf nano::NF_Network)

        endif()

    endif()

endmacro()

# Add Azure RTOS platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_include_directories target)

    target_include_directories(${target}.elf PUBLIC

        ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
        # ${CHIBIOS_LWIP_INCLUDE_DIRS}
        # ${SPIFFS_INCLUDE_DIRS}
    )

    if(CHIBIOS_HAL_REQUIRED)
        target_include_directories(${target}.elf PUBLIC
            ${CHIBIOS_HAL_INCLUDE_DIRS}
            ${ChibiOSnfOverlay_INCLUDE_DIRS}
        )
    endif()

    if(STM32_CUBE_PACKAGE_REQUIRED)
        target_include_directories(${target}.elf PUBLIC
            ${${TARGET_STM32_CUBE_PACKAGE}_CubePackage_INCLUDE_DIRS}
        )
    endif()
    
    if(MAXIM_MICROS_SDK_REQUIRED)
        target_include_directories(${target}.elf PUBLIC
            ${MaximMicrosSDK_INCLUDE_DIRS}
        )
    endif()
    
    # includes specific to nanoBooter
    if(${target} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC

            ${TARGET_AZURERTOS_NANOBOOTER_INCLUDE_DIRS}
    
        )

    endif()

    # includes specific to nanoCRL
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC

            ${TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS}
            # ${CHIBIOS_FATFS_INCLUDE_DIRS}
        )

    endif()


endmacro()

# Add Azure RTOS platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_sources target)


    # add header files with common OS definitions and board definitions
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_common.h @ONLY)

    # sources common to both builds
    target_sources(${target}.elf PUBLIC
        
        ${TARGET_AZURERTOS_COMMON_SOURCES}
    )

    if(CHIBIOS_HAL_REQUIRED)
        target_sources(${target}.elf PUBLIC
            ${CHIBIOS_HAL_SOURCES}
            ${ChibiOSnfOverlay_SOURCES}
        )
    endif()

    if(STM32_CUBE_PACKAGE_REQUIRED)
        target_sources(${target}.elf PUBLIC
            ${${TARGET_STM32_CUBE_PACKAGE}_CubePackage_SOURCES}
        )
    endif()

    if(MAXIM_MICROS_SDK_REQUIRED)
        target_sources(${target}.elf PUBLIC
            ${MaximMicrosSDK_SOURCES}
        )
    endif()

    # sources specific to nanoBooter
    if(${target} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        # add header file for board definition
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/target_board.h.in
                       ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoBooter/target_board.h @ONLY)

        target_sources(${target}.elf PUBLIC
            
            ${TARGET_AZURERTOS_NANOBOOTER_SOURCES}
            ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c
           
            # need to add configuration manager to allow get/store configuration blocks
            ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager_stubs.c
        )

    endif()

    # sources specific to nanoCRL
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                       ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoCLR/target_board.h @ONLY)

        # TODO
        target_sources(${target}.elf PUBLIC

            ${TARGET_AZURERTOS_NANOCLR_SOURCES}
            #     ${CHIBIOS_FATFS_SOURCES}
            #     ${CHIBIOS_LWIP_SOURCES}
            #     ${SPIFFS_SOURCES}
            # 
        )

        if(USE_NETWORKING_OPTION)
            # target_link_libraries(${target}.elf
            #     nano::NF_Network
            # )
        endif()

    endif()

    target_link_libraries(${target}.elf
        azrtos::threadx
    )

    # TODO
    # # mbed TLS requires a config file
    # if(USE_SECURITY_MBEDTLS_OPTION)
    #     # this seems to be only option to properly set a compiler define through the command line that needs to be a string literal
    #     SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMBEDTLS_CONFIG_FILE=\"<${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/nf_mbedtls_config.h>\"")
    #     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMBEDTLS_CONFIG_FILE=\"<${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/nf_mbedtls_config.h>\"")
    # endif()

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
