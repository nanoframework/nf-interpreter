#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(simplelinkcc13xx_26xxsdk)
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


function(nf_set_linker_file target linker_file_name)

    # set linker file name
    set_target_properties(${target} PROPERTIES LINK_FLAGS "-Wl,-T${linker_file_name}")

endfunction()

# setting compile definitions for a target based on general build options
# TARGET parameter to set the target that's setting them for
# optional BUILD_TARGET when target it's a library pass here the name ot the target that's building for (either nanoBooter or nanoCLR)
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
macro(nf_set_compile_definitions)

    # parse arguments
    cmake_parse_arguments(NFSCD "" "TARGET" "EXTRA_COMPILE_DEFINITIONS;BUILD_TARGET" ${ARGN})

    if(NOT NFSCD_TARGET OR "${NFSCD_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_definitions()")
    endif()

    # definitions required for SimpleLink SDK
    target_compile_definitions(${NFSCD_TARGET} PUBLIC -Dgcc)

    nf_common_compiler_definitions(TARGET ${NFSCD_TARGET} BUILD_TARGET ${NFSCD_BUILD_TARGET})

    # include extra compiler definitions
    target_compile_definitions(${NFSCD_TARGET} PUBLIC ${NFSCD_EXTRA_COMPILE_DEFINITIONS})

endmacro()

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
# To be called from target CMakeList.txt
# optional TARGET argument with target name
macro(nf_add_platform_packages)

    # parse arguments
    cmake_parse_arguments(NFAPP "" "TARGET" "" ${ARGN})

    find_package(TI_SimpleLink REQUIRED QUIET)

    # packages specific for nanoBooter
    if("${NFAPP_TARGET}" STREQUAL "${NANOBOOTER_PROJECT_NAME}")
        # no packages for booter
    endif()

    # packages specific for nanoCLR
    if("${NFAPP_TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")
        # no packages for nanoCLR
    endif()

endmacro()

# Add TI SimpleLink platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_dependencies target)
    
    nf_add_common_dependencies(${target})

    # dependencies specific to nanoCLR
    if("${target}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        nf_add_lib_coreclr(
            EXTRA_COMPILE_DEFINITIONS
                ${NFSTBC_CLR_EXTRA_COMPILE_DEFINITIONS}

            EXTRA_INCLUDES
                ${TI_SimpleLink_INCLUDE_DIRS}
                ${TI_XDCTools_INCLUDE_DIR}
                ${TARGET_TI_SimpleLink_COMMON_INCLUDE_DIRS}
                ${TARGET_TI_SimpleLink_NANOCLR_INCLUDE_DIRS})
        
        add_dependencies(${target}.elf nano::NF_CoreCLR)

        nf_add_lib_wireprotocol(
            EXTRA_INCLUDES
                ${TI_SimpleLink_INCLUDE_DIRS}
                ${TI_XDCTools_INCLUDE_DIR}
                ${TARGET_TI_SimpleLink_COMMON_INCLUDE_DIRS}
                ${TARGET_TI_SimpleLink_NANOCLR_INCLUDE_DIRS})

        add_dependencies(${target}.elf nano::WireProtocol)

        if(NF_FEATURE_DEBUGGER)

            nf_add_lib_debugger(
                EXTRA_COMPILE_DEFINITIONS
                    ${NFSTBC_CLR_EXTRA_COMPILE_DEFINITIONS}

                EXTRA_INCLUDES
                   ${TI_SimpleLink_INCLUDE_DIRS}
                    ${TI_XDCTools_INCLUDE_DIR}
                    ${TARGET_TI_SimpleLink_COMMON_INCLUDE_DIRS}
                    ${TARGET_TI_SimpleLink_NANOCLR_INCLUDE_DIRS})
            
            add_dependencies(${target}.elf nano::NF_Debugger)

        endif()

        nf_add_lib_native_assemblies(
                EXTRA_INCLUDES
                    ${CMAKE_CURRENT_BINARY_DIR}/syscfg
                    ${TI_SimpleLink_INCLUDE_DIRS}
                    ${TI_XDCTools_INCLUDE_DIR}
                    ${TARGET_TI_SimpleLink_COMMON_INCLUDE_DIRS}
                    ${TARGET_TI_SimpleLink_NANOCLR_INCLUDE_DIRS})

        # add dependency from SysConfig and TI RTOS configs (this is required to make sure that the intermediate artifacts are generated in the proper order)
        add_dependencies(COPY_TIRTOS_CONFIG SYSCONFIG_TASKS)
        add_dependencies(TIRTOS_CONFIG COPY_TIRTOS_CONFIG)
        add_dependencies(NF_NativeAssemblies TIRTOS_CONFIG)
        
        add_dependencies(${target}.elf nano::NF_NativeAssemblies)

        # nF feature: networking
        if(USE_NETWORKING_OPTION)

            nf_add_lib_network(
                BUILD_TARGET
                    ${target}
                EXTRA_INCLUDES 
                    ${TI_SimpleLink_INCLUDE_DIRS}
                    ${TI_XDCTools_INCLUDE_DIR}
                    ${TARGET_TI_SimpleLink_COMMON_INCLUDE_DIRS}
                    ${TARGET_TI_SimpleLink_NANOCLR_INCLUDE_DIRS})

        add_dependencies(${target}.elf nano::NF_Network)

        endif()

    endif()
 
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
    
endmacro()

# Add TI SimpleLink platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_sources target)

    # add header files with common OS definitions and board definitions specific for each image
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoCLR/target_board.h @ONLY)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_common.h @ONLY)

    # sources specific to nanoCLR
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

    # setup target to take care of generating SimpleLink SysConfig files
    add_custom_target(SYSCONFIG_TASKS ALL)

    set(TI_DEVICE_FAMILIES_WITH_RADIO_FREQUENCY "CC13X2")

    list(FIND TI_DEVICE_FAMILIES_WITH_RADIO_FREQUENCY ${TARGET_SERIES} TI_DEVICE_FAMILY_NAME_INDEX)

    string(TOLOWER ${ti_device_family} TI_DEVICE_FAMILY_LOWER_CASE)

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
        TARGET
            SYSCONFIG_TASKS PRE_BUILD
        
        BYPRODUCTS
            ${CMAKE_CURRENT_BINARY_DIR}/${SYS_CONFIG_FILENAME}

        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/${SYS_CONFIG_FILENAME}
                ${CMAKE_CURRENT_BINARY_DIR}/${SYS_CONFIG_FILENAME}
        COMMENT "Copy TI-RTOS configuration file to build directory" 
    )

    # execute Sys Config with configuration file
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
        add_custom_command(
            TARGET
                SYSCONFIG_TASKS PRE_BUILD

            BYPRODUCTS
                ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_devices_config.c 
                ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_drivers_config.c
                ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_easylink_config.c
                ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_radio_config.c

            COMMAND ${ti_sysconfig_SOURCE_DIR}/sysconfig_cli.bat -s "${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/.metadata/product.json" --script ${SYS_CONFIG_FILENAME} -o "syscfg" --compiler gcc 
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generate configuration files" 
        )
    else()
        add_custom_command(
            TARGET
                SYSCONFIG_TASKS PRE_BUILD

            BYPRODUCTS
                ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_devices_config.c 
                ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_drivers_config.c
                ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_easylink_config.c
                ${CMAKE_CURRENT_BINARY_DIR}/syscfg/ti_radio_config.c

            COMMAND ${ti_sysconfig_SOURCE_DIR}/sysconfig_cli.sh -s "${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/.metadata/product.json" --script ${SYS_CONFIG_FILENAME} -o "syscfg" --compiler gcc
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
            COMMAND ${ti_xdctools_SOURCE_DIR}/xs.exe --xdcpath="${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/source\;${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/kernel/tirtos/packages" xdc.tools.configuro -o configPkg -t gnu.targets.arm.M4F -p ti.platforms.simplelink:${ti_device} -r release -c "${TOOLCHAIN_PREFIX}" --compileOptions " -DDeviceFamily_${ti_device_family} " "${CMAKE_CURRENT_BINARY_DIR}/${TI_RTOS_CONFIG_FILE}"    
            COMMENT "Generate TI-RTOS configuration" 
        )
    else()
        add_custom_target(
            TIRTOS_CONFIG
            COMMAND ${ti_xdctools_SOURCE_DIR}/xs --xdcpath="${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/source\;${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/kernel/tirtos/packages" xdc.tools.configuro -o configPkg -t gnu.targets.arm.M4F -p ti.platforms.simplelink:${ti_device} -r release -c "${TOOLCHAIN_PREFIX}" --compileOptions " -DDeviceFamily_${ti_device_family} " "${CMAKE_CURRENT_BINARY_DIR}/${TI_RTOS_CONFIG_FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}    
            COMMENT "Generate TI-RTOS configuration" 
        )
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

    # add extra libraries for SimpleLink
    set(CLR_EXTRA_LIBRARIES
        ${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/source/ti/display/lib/gcc/m4f/display_${TI_DEVICE_FAMILY_LOWER_CASE}.a
        ${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/source/ti/grlib/lib/gcc/m4f/grlib.a
        ${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/source/ti/drivers/rf/lib/gcc/m4f/rf_multiMode_${TI_DEVICE_FAMILY_LOWER_CASE}.a
        ${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/source/ti/drivers/lib/gcc/m4f/drivers_${TI_DEVICE_FAMILY_LOWER_CASE}.a
        ${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/source/ti/devices/cc13x2_cc26x2/driverlib/bin/gcc/driverlib.lib
        ${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/kernel/tirtos/packages/ti/dpl/lib/gcc/m4f/dpl_${TI_DEVICE_FAMILY_LOWER_CASE}.a
        
        ${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/hard/libm.a
        ${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/hard/libnosys.a
    )

    # add these to the ARGN list
    list(APPEND ARGN CLR_EXTRA_LIBRARIES ${CLR_EXTRA_LIBRARIES})

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
