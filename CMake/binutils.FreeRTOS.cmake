#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

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
    set_property(TARGET ${TARGET_SHORT}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${CMAKE_SOURCE_DIR}/build/${TARGET_SHORT}.map")

endfunction()


function(NF_SET_LINKER_FILE TARGET LINKER_FILE_NAME)

    # set linker file name
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-T${LINKER_FILE_NAME}")

endfunction()


function(NF_SET_COMPILER_DEFINITIONS TARGET)

    # definition for platform 
    # (always ARM here)
    target_compile_definitions(${TARGET} PUBLIC "-DPLATFORM_ARM")

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

    # include any extra compiler definitions comming from extra args
    target_compile_definitions(${TARGET} PUBLIC ${ARGN})

endfunction()

# Add packages that are common to FreeRTOS platform builds
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_PACKAGES)

    find_package(FreeRTOS REQUIRED)
    find_package(CMSIS REQUIRED)

    # nF feature: networking
    if(USE_NETWORKING_OPTION)
        find_package(LWIP REQUIRED)
    endif()

    if(USE_FILESYSTEM_OPTION)
        find_package(FATFS REQUIRED)
    endif()

endmacro()

# Add FreeRTOS platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_DEPENDENCIES TARGET)

endmacro()

# Add FreeRTOS platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_INCLUDE_DIRECTORIES TARGET)

    target_include_directories(${TARGET}.elf PUBLIC

        ${TARGET_FREERTOS_COMMON_INCLUDE_DIRS}
        ${TARGET_NXP_COMMON_INCLUDE_DIRS}
        ${FreeRTOS_INCLUDE_DIRS}
        ${CMSIS_INCLUDE_DIRS}
    )
    
    # includes specific to nanoBooter
    if(${TARGET} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        target_include_directories(${TARGET}.elf PUBLIC
            
            ${TARGET_NXP_NANOBOOTER_INCLUDE_DIRS}
            ${TARGET_FREERTOS_NANOBOOTER_INCLUDE_DIRS}
        )

    endif()

    # includes specific to nanoCRL
    if(${TARGET} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${TARGET}.elf PUBLIC

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
macro(NF_ADD_PLATFORM_SOURCES TARGET)

    # add header files with common OS definitions and board definitions 
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/target_common.h @ONLY)

    # sources common to both builds
    target_sources(${TARGET}.elf PUBLIC
    
        ${TARGET_FREERTOS_COMMON_SOURCES}
        ${TARGET_NXP_COMMON_SOURCES}
        
        ${FreeRTOS_SOURCES}
    )

    # sources specific to nanoBooter
    if(${TARGET} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        # add header files with common OS definitions and board definitions 
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/target_board.h.in
                    ${CMAKE_CURRENT_BINARY_DIR}/nanoBooter/target_board.h @ONLY)

        target_sources(${TARGET}.elf PUBLIC
            
            ${TARGET_NXP_NANOBOOTER_SOURCES}
            ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c
           
            # need to add configuration manager to allow get/store configuration blocks
            ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager_stubs.c
        )

    endif()

    # sources specific to nanoCRL
    if(${TARGET} STREQUAL ${NANOCLR_PROJECT_NAME})

        # add header files with common OS definitions and board definitions 
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                    ${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h @ONLY)

        target_sources(${TARGET}.elf PUBLIC

            ${TARGET_FREERTOS_COMMON_SOURCES}
            ${TARGET_FREERTOS_NANOCLR_SOURCES}
            ${TARGET_NXP_COMMON_SOURCES}
            ${TARGET_NXP_NANOCLR_SOURCES}

            ${FATFS_SOURCES}
            ${LWIP_SOURCES}

        )

    endif()

endmacro()
