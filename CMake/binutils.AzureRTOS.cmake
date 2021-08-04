#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)

function(NF_SET_OPTIMIZATION_OPTIONS TARGET) 

    target_compile_options(${TARGET} PRIVATE
        $<$<CONFIG:Debug>:-Og -femit-class-debug-always -g3 -ggdb>
        $<$<CONFIG:Release>:-O3 -flto -fuse-linker-plugin -fno-fat-lto-objects>
        $<$<CONFIG:MinSizeRel>:-Os -flto -fuse-linker-plugin -fno-fat-lto-objects>
        $<$<CONFIG:RelWithDebInfo>:-Os -femit-class-debug-always -g3 -ggdb>
    )

endfunction()


function(NF_SET_LINK_MAP TARGET) 

    # need to remove the .elf suffix from target name
    string(FIND ${TARGET} "." TARGET_EXTENSION_DOT_INDEX)
    string(SUBSTRING ${TARGET} 0 ${TARGET_EXTENSION_DOT_INDEX} TARGET_SHORT)
    
    # add linker flags to generate map file

    if(CHIBIOS_HAL_REQUIRED)
        set_property(TARGET ${TARGET_SHORT}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${TARGET_SHORT}.map,--library-path=${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common")
    else()
        set_property(TARGET ${TARGET_SHORT}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${TARGET_SHORT}.map,--library-path=${CMAKE_SOURCE_DIR}/targets/AzureRTOS/_common")
    endif()

endfunction()


function(NF_SET_LINKER_FILE TARGET LINKER_FILE_NAME)

    # set linker file name
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-T${LINKER_FILE_NAME}")

endfunction()


function(NF_SET_COMPILER_DEFINITIONS TARGET)

    # definition for platform 
    # (always ARM here)
    if(CHIBIOS_HAL_REQUIRED)
        # ChibiOS HAL community always include (nanoFramework overlay and official community contributions optionally)
        target_compile_definitions(${TARGET} PUBLIC "-DPLATFORM_ARM -DHAL_USE_COMMUNITY")
    else()
        target_compile_definitions(${TARGET} PUBLIC "-DPLATFORM_ARM")
    endif()

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

    # include any extra compiler definitions comming from extra args
    target_compile_definitions(${TARGET} PUBLIC ${ARGN})

endfunction()

function(NF_SET_STM32_TARGET_SERIES)
    # process target series, which is in the format "STM32F4xx"
    string(REPLACE "STM32" "" TARGET_SERIES_SHORT_1 "${TARGET_SERIES}")
    string(REPLACE "xx" "" TARGET_SERIES_SHORT "${TARGET_SERIES_SHORT_1}")
    
    # store the series short name for later use
    set(TARGET_SERIES_SHORT ${TARGET_SERIES_SHORT} CACHE INTERNAL "STM32 target series short name")

endfunction()

# Add packages that are common to Azure RTOS platform builds
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_PACKAGES)

    # # nF feature: networking
    # if(USE_NETWORKING_OPTION)
    #     find_package(CHIBIOS_LWIP REQUIRED)
    # endif()

    # if(USE_FILESYSTEM_OPTION)
    #     find_package(CHIBIOS_FATFS REQUIRED)
    # endif()

    # # SPIFFS
    # if(NF_FEATURE_USE_SPIFFS)
    #     find_package(STM32F7_CubePackage REQUIRED)
    #     find_package(SPIFFS REQUIRED)
    # endif()
    
    if(CHIBIOS_HAL_REQUIRED)
        find_package(ChibiOS_${TARGET_SERIES_SHORT}_HAL REQUIRED)
        find_package(ChibiOSnfOverlay REQUIRED)
    endif()

    if(STM32_CUBE_PACKAGE_REQUIRED)
        find_package(${TARGET_STM32_CUBE_PACKAGE}_CubePackage REQUIRED)
    endif()

    if(MAXIM_MICROS_SDK_REQUIRED)
        find_package(MaximMicrosSDK REQUIRED)
    endif()    
     
endmacro()

# Add Azure RTOS platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_DEPENDENCIES TARGET)

    if(CHIBIOS_HAL_REQUIRED)
        # add_dependencies(${TARGET}.elf CHIBIOS_STARTUP_HAL)
    endif()

    add_dependencies(${TARGET}.elf azrtos::threadx)

    # specific to nanoCRL
    if(${TARGET} STREQUAL ${NANOCLR_PROJECT_NAME})

        # TODO
        # # add dependencies from CHIBIOS_NETWORK_COMPONENTS (this is required to make sure that AzureRTOS network components are unzip at the proper locations before the build starts)
        # # only required if networking is ON
        # if(USE_NETWORKING_OPTION)
        #     add_dependencies(${NANOCLR_PROJECT_NAME}.elf CHIBIOS_NETWORK_COMPONENTS)

        #     # add dependency for security provider mbedTLS
        #     if(USE_SECURITY_MBEDTLS_OPTION)
        #         add_dependencies(${NANOCLR_PROJECT_NAME}.elf mbedTLS)
        #     endif()
        # endif()

        # if(NF_FEATURE_USE_SPIFFS)
        #     add_dependencies(${NANOCLR_PROJECT_NAME}.elf SPIFFS)
        # endif()

    endif()

endmacro()

# Add Azure RTOS platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_INCLUDE_DIRECTORIES TARGET)

    target_include_directories(${TARGET}.elf PUBLIC

        ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
        # ${CHIBIOS_LWIP_INCLUDE_DIRS}
        # ${SPIFFS_INCLUDE_DIRS}
    )

    if(CHIBIOS_HAL_REQUIRED)
        target_include_directories(${TARGET}.elf PUBLIC
            ${CHIBIOS_HAL_INCLUDE_DIRS}
            ${ChibiOSnfOverlay_INCLUDE_DIRS}
        )
    endif()

    if(STM32_CUBE_PACKAGE_REQUIRED)
        target_include_directories(${TARGET}.elf PUBLIC
            ${${TARGET_STM32_CUBE_PACKAGE}_CubePackage_INCLUDE_DIRS}
        )
    endif()
    
    if(MAXIM_MICROS_SDK_REQUIRED)
        target_include_directories(${TARGET}.elf PUBLIC
            ${MaximMicrosSDK_INCLUDE_DIRS}
        )
    endif()
    
    # includes specific to nanoBooter
    if(${TARGET} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        target_include_directories(${TARGET}.elf PUBLIC

            ${TARGET_AZURERTOS_NANOBOOTER_INCLUDE_DIRS}
    
        )

    endif()

    # includes specific to nanoCRL
    if(${TARGET} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${TARGET}.elf PUBLIC

            ${TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS}
            # ${CHIBIOS_FATFS_INCLUDE_DIRS}
        )

    endif()


endmacro()

# Add Azure RTOS platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_SOURCES TARGET)

    # add header files with common OS definitions and board definitions
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/target_common.h @ONLY)

    # sources common to both builds
    target_sources(${TARGET}.elf PUBLIC
        
        ${TARGET_AZURERTOS_COMMON_SOURCES}
    )

    if(CHIBIOS_HAL_REQUIRED)
        target_sources(${TARGET}.elf PUBLIC
            ${CHIBIOS_HAL_SOURCES}
            ${ChibiOSnfOverlay_SOURCES}
        )
    endif()

    if(STM32_CUBE_PACKAGE_REQUIRED)
        target_sources(${TARGET}.elf PUBLIC
            ${${TARGET_STM32_CUBE_PACKAGE}_CubePackage_SOURCES}
        )
    endif()

    if(MAXIM_MICROS_SDK_REQUIRED)
        target_sources(${TARGET}.elf PUBLIC
            ${MaximMicrosSDK_SOURCES}
        )
    endif()

    # sources specific to nanoBooter
    if(${TARGET} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        # add header file for board definition
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/target_board.h.in
                    ${CMAKE_CURRENT_BINARY_DIR}/nanoBooter/target_board.h @ONLY)

        target_sources(${TARGET}.elf PUBLIC
            
            ${TARGET_AZURERTOS_NANOBOOTER_SOURCES}
            ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c
           
            # need to add configuration manager to allow get/store configuration blocks
            ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager_stubs.c
        )

    endif()

    # sources specific to nanoCRL
    if(${TARGET} STREQUAL ${NANOCLR_PROJECT_NAME})

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                    ${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h @ONLY)

        # TODO
        target_sources(${TARGET}.elf PUBLIC

            ${TARGET_AZURERTOS_NANOCLR_SOURCES}
            #     ${CHIBIOS_FATFS_SOURCES}
            #     ${CHIBIOS_LWIP_SOURCES}
            #     ${SPIFFS_SOURCES}
            # 
        )

    endif()

    target_link_libraries(${TARGET}.elf
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
