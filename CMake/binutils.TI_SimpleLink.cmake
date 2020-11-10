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
    set_property(TARGET ${TARGET_SHORT}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${CMAKE_SOURCE_DIR}/build/${TARGET_SHORT}.map,--library-path=${CMAKE_SOURCE_DIR}/targets/TI-SimpleLink/common")

endfunction()


function(NF_SET_LINKER_FILE TARGET LINKER_FILE_NAME)

    # set linker file name
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-Wl,-T${LINKER_FILE_NAME}")

endfunction()


function(NF_SET_COMPILER_DEFINITIONS TARGET)

    # definition for platform 
    # (always ARM here)
    target_compile_definitions(${TARGET} PUBLIC "-DPLATFORM_ARM ")

    # definitions required for SimpleLink SDK
    target_compile_definitions(${TARGET} PUBLIC -Dgcc)

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

    # include any extra compiler definitions coming from extra args
    target_compile_definitions(${TARGET} PUBLIC ${ARGN})

endfunction()

# Add packages that are common to TI SimpleLink platform builds
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_PACKAGES)

    find_package(TI_SimpleLink REQUIRED)

endmacro()

# Add TI SimpleLink platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_INCLUDE_DIRECTORIES TARGET)

    target_include_directories(${TARGET}.elf PUBLIC
        ${TI_SimpleLink_INCLUDE_DIRS}
        ${TI_XDCTools_INCLUDE_DIR}

        ${CMAKE_CURRENT_BINARY_DIR}/syscfg

        ${TARGET_TI_SimpleLink_COMMON_INCLUDE_DIRS}
        ${TARGET_TI_SimpleLink_NANOCLR_INCLUDE_DIRS}

    )

endmacro()

# Add TI SimpleLink platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_SOURCES TARGET)

    # add header files with common OS definitions and board definitions specific for each image
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                   ${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                   ${CMAKE_CURRENT_BINARY_DIR}/target_common.h @ONLY)

    # sources specific to nanoCRL
    if(${TARGET} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_sources(${TARGET}.elf PUBLIC

            ${CMAKE_CURRENT_SOURCE_DIR}/target_Power.c
            ${CMAKE_CURRENT_SOURCE_DIR}/CC1352R1_LAUNCHXL_fxns.c

            ${TARGET_TI_SimpleLink_COMMON_SOURCES}
            ${TARGET_TI_SimpleLink_NANOCLR_SOURCES}
            ${TARGET_TI_SimpleLink_SOURCES}

            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_devices_config.c 
            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_drivers_config.c
            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_easylink_config.c
            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_radio_config.c
        
        )

    endif()

endmacro()
