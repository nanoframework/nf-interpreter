#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(simplelinkcc13x2_26x2sdk)
FetchContent_GetProperties(ti_sysconfig)
FetchContent_GetProperties(ti_xdctools)

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
    set_property(TARGET ${TARGET_SHORT}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${CMAKE_SOURCE_DIR}/build/${TARGET_SHORT}.map,--library-path=${CMAKE_SOURCE_DIR}/targets/TI_SimpleLink/common")

endfunction()


function(nf_set_linker_file target linker_file_name)

    # set linker file name
    set_target_properties(${target} PROPERTIES LINK_FLAGS "-Wl,-T${linker_file_name}")

endfunction()


function(nf_set_compiler_definitions target)

    # definitions required for SimpleLink SDK
    target_compile_definitions(${target} PUBLIC -Dgcc)

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

    # include any extra compiler definitions coming from extra args
    target_compile_definitions(${target} PUBLIC ${ARGN})

endfunction()

# check valid frequency and if configuration file exists 
function(nf_check_radio_frequency)

    if(NOT DEFINED RADIO_FREQUENCY)
        message(FATAL_ERROR "Radio frequncy NOT defined. Please set build option 'RADIO_FREQUENCY'. Valid values are 868 and 915.")
    endif()

    find_file(
        SYS-CONFIG-FILE 
        *_${RADIO_FREQUENCY}.syscfg
        
        PATHS 
            ${TARGET_BASE_LOCATION}
        )

    # check if file was found
    if(SYS-CONFIG-FILE-NOTFOUND)
        message(FATAL_ERROR "Couldn't find a sysconfig file for radio frequency ${RADIO_FREQUENCY}. Valid values are 868 and 915.")
    endif()

endfunction()


# Add packages that are common to TI SimpleLink platform builds
# To be called from target CMakeList.txt
macro(nf_add_platform_packages)

    find_package(TI_SimpleLink REQUIRED)

endmacro()

# Add TI SimpleLink platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_dependencies target)
    
    nf_add_common_dependencies(${target})

    # add dependency from SysConfig and TI RTOS configs (this is required to make sure that the intermediate artifacts are generated in the proper order)
    add_dependencies(${NANOCLR_PROJECT_NAME}.elf COPY_TIRTOS_CONFIG)
    add_dependencies(${NANOCLR_PROJECT_NAME}.elf TIRTOS_CONFIG)
    add_dependencies(TIRTOS_CONFIG COPY_TIRTOS_CONFIG)
 
endmacro()

# Add TI SimpleLink platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_include_directories target)

    target_include_directories(${target}.elf PUBLIC
        ${TI_SimpleLink_INCLUDE_DIRS}
        ${TI_XDCTools_INCLUDE_DIR}

        ${CMAKE_CURRENT_BINARY_DIR}/syscfg

        ${TARGET_TI_SimpleLink_COMMON_INCLUDE_DIRS}
        ${TARGET_TI_SimpleLink_NANOCLR_INCLUDE_DIRS}

    )

    target_link_libraries(
        ${NANOCLR_PROJECT_NAME}.elf
    
        ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/display/lib/gcc/m4f/display.a
        ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/grlib/lib/gcc/m4f/grlib.a
        ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/drivers/rf/lib/rf_multiMode_cc13x2.am4fg
        ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/drivers/lib/gcc/m4f/drivers_cc13x2.a
        ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source/ti/devices/cc13x2_cc26x2/driverlib/bin/gcc/driverlib.lib
        ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/kernel/tirtos/packages/ti/dpl/lib/gcc/m4f/dpl_cc13x2.a
        
        ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/hard/libm.a
        ${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/hard/libnosys.a
    )
    
endmacro()

# Add TI SimpleLink platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_sources target)

    # add header files with common OS definitions and board definitions specific for each image
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoCLR/target_board.h @ONLY)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_common.h @ONLY)

    # sources specific to nanoCRL
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_sources(${target}.elf PUBLIC

            ${CMAKE_CURRENT_SOURCE_DIR}/target_Power.c

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

# Add TI SimpleLink sys config steps
# To be called from target CMakeList.txt
macro(nf_add_platform_sysconfig_steps ti_device ti_device_family)

    set(TI_DEVICE_FAMILIES_WITH_RADIO_FREQUENCY "CC13x2_26x2")

    list(FIND TI_DEVICE_FAMILIES_WITH_RADIO_FREQUENCY ${TARGET_SERIES} TI_DEVICE_FAMILY_NAME_INDEX)

    if(TI_DEVICE_FAMILY_NAME_INDEX EQUAL -1)
        # this target series doesn't have/support/care radio frequency option
        # compose sys config file name 
        set(SYS_CONFIG_FILENAME ${TARGET_BOARD}.syscfg)
    else()
        # check for valid frequency setting
        nf_check_radio_frequency()

        # compose sys config file name 
        set(SYS_CONFIG_FILENAME ${TARGET_BOARD}_${RADIO_FREQUENCY}.syscfg)
    endif()

    # copy Sys Config file to build directory
    add_custom_command(
        OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/${SYS_CONFIG_FILENAME}
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/${SYS_CONFIG_FILENAME}
                ${CMAKE_CURRENT_BINARY_DIR}/${SYS_CONFIG_FILENAME}
        COMMENT "Copy TI-RTOS configuration file to build directory" 
    )

    # execute Sys Config with configuration file
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
        add_custom_command(
            OUTPUT 
            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_devices_config.c 
            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_drivers_config.c
            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_easylink_config.c
            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_radio_config.c

            DEPENDS
            ${CMAKE_CURRENT_BINARY_DIR}/${SYS_CONFIG_FILENAME}

            COMMAND ${ti_sysconfig_SOURCE_DIR}/sysconfig_cli.bat --product "${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/.metadata/product.json" --script ${SYS_CONFIG_FILENAME} -o "syscfg" --compiler gcc 
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generate configuration files" 
        )
    else()
        add_custom_command(
            OUTPUT 
            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_devices_config.c 
            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_drivers_config.c
            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_easylink_config.c
            ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_radio_config.c

            DEPENDS
            ${CMAKE_CURRENT_BINARY_DIR}/${SYS_CONFIG_FILENAME}

            COMMAND ${ti_sysconfig_SOURCE_DIR}/sysconfig_cli.sh --product "${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/.metadata/product.json" --script ${SYS_CONFIG_FILENAME} -o "syscfg" --compiler gcc
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
            COMMENT "Generate configuration files" 
        )
    endif()
    ######################################

    # need to copy the configuration file to the build directory
    # because the obj files resulting from TI-RTOS build are placed in the same directory as the configuration file
    if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        set(TI_RTOS_CONFIG_FILE ti-rtos-debug.cfg)
    else()
        set(TI_RTOS_CONFIG_FILE ti-rtos-release.cfg)
    endif()

    # need to use a specific target because target dependency PRE_BUILT doesn't work on NINJA build files

    add_custom_target(
        COPY_TIRTOS_CONFIG
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/${TI_RTOS_CONFIG_FILE}
                ${CMAKE_CURRENT_BINARY_DIR}/${TI_RTOS_CONFIG_FILE}
        COMMENT "Copy TI-RTOS configuration file to build directory" 
    )

    if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
        add_custom_target(
            TIRTOS_CONFIG        
            COMMAND ${ti_xdctools_SOURCE_DIR}/xs.exe --xdcpath="${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source\;${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/kernel/tirtos/packages" xdc.tools.configuro -o configPkg -t gnu.targets.arm.M4F -p ti.platforms.simplelink:${ti_device} -r release -c "${TOOLCHAIN_PREFIX}" --compileOptions " -DDeviceFamily_${ti_device_family} " "${CMAKE_CURRENT_BINARY_DIR}/${TI_RTOS_CONFIG_FILE}"    
            COMMENT "Generate TI-RTOS configuration" 
        )
    else()
        add_custom_target(
            TIRTOS_CONFIG
            COMMAND ${ti_xdctools_SOURCE_DIR}/xs --xdcpath="${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/source\;${simplelinkcc13x2_26x2sdk_SOURCE_DIR}/kernel/tirtos/packages" xdc.tools.configuro -o configPkg -t gnu.targets.arm.M4F -p ti.platforms.simplelink:${ti_device} -r release -c "${TOOLCHAIN_PREFIX}" --compileOptions " -DDeviceFamily_${ti_device_family} " "${CMAKE_CURRENT_BINARY_DIR}/${TI_RTOS_CONFIG_FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}    
            COMMENT "Generate TI-RTOS configuration" 
        )
    endif()
endmacro()
