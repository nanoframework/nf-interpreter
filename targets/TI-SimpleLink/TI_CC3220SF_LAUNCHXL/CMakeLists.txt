#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

ENABLE_LANGUAGE(ASM)

# add header files with common OS definitions and board definitions specific for each image
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/target_common.h" @ONLY)
 
set(NANOCLR_PROJECT_NAME "nanoCLR")

find_package(BuildUtils REQUIRED)
find_package(TI_SimpleLink REQUIRED)
find_package(WireProtocol REQUIRED)

# packages for nanoFramework libraries
#######################################
# mandatory 
find_package(NF_CoreCLR REQUIRED)

#######################################
# optional

# nF feature: debugger
if(NF_FEATURE_DEBUGGER)
    find_package(NF_Debugger REQUIRED)
    find_package(NF_Diagnostics REQUIRED)
endif()

#######################################

add_subdirectory("common")
add_subdirectory("nanoCLR")

#######################
# nanoCLR executable

add_executable(
    # executables for project, project sources
    ${NANOCLR_PROJECT_NAME}.elf

    "${CMAKE_CURRENT_SOURCE_DIR}/target_common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/target_Power.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/CC3220SF_LAUNCHXL.c"

    ${COMMON_PROJECT_SOURCES}
    ${NANOCLR_PROJECT_SOURCES}
    
    ${TARGET_TI_SimpleLink_COMMON_SOURCES}

    ${TARGET_TI_SimpleLink_NANOCLR_SOURCES}

    ${TI_SimpleLink_SOURCES}

    ${WireProtocol_SOURCES}

    # sources for nanoFramework libraries
    ${NF_CoreCLR_SOURCES}
    ${NF_Debugger_SOURCES}
    ${NF_Diagnostics_SOURCES}

    # sources for nanoFramework APIs
    ${TARGET_NANO_APIS_SOURCES}
)

target_link_libraries(
    ${NANOCLR_PROJECT_NAME}.elf

    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/ti/drivers/lib/drivers_cc32xx.am4g
    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/ti/drivers/net/wifi/gcc/rtos/simplelink.a
    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/ti/dpl/lib/dpl_cc32xx.am4g

    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/ti/drivers/net/wifi/slnetif/gcc/Release/slnetifwifi.a
    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/ti/net/utils/gcc/Release/net_utils.a
    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/ti/net/lib/gcc/m4/slnetsock_release.a
    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/ti/net/sntp/lib/gcc/m4/sntp_release.a

    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/ti/devices/cc32xx/driverlib/gcc/Release/driverlib.a
    
    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/nofp/libc_nano.a
    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/nofp/libm.a
    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/nofp/libnosys.a

    # need to add this a second time otherwise we'll get undefined references
    ${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages/ti/dpl/lib/dpl_cc32xx.am4g

)

# add dependency from TI SimpleLink CC32xx SDK and XDCTools (this is required to make sure that those repos are downloaded before the build starts)
add_dependencies(${NANOCLR_PROJECT_NAME}.elf TI_XDCTools)
add_dependencies(${NANOCLR_PROJECT_NAME}.elf SimpleLinkCC32xxSDK)

######################################

# need to copy the configuration file to the build directory
# because the obj files resulting from TI-RTOS build are placed in the same directory as the configuration file
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    set(TI_RTOS_CONFIG_FILE ti-rtos-debug.cfg)
else()
    set(TI_RTOS_CONFIG_FILE ti-rtos-release.cfg)
endif()

add_custom_command(
    TARGET ${NANOCLR_PROJECT_NAME}.elf PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/${TI_RTOS_CONFIG_FILE}
            ${CMAKE_CURRENT_BINARY_DIR}/${TI_RTOS_CONFIG_FILE}
    COMMENT "Copy TI-RTOS configuration file to build directory" 
)

add_custom_command(
    TARGET ${NANOCLR_PROJECT_NAME}.elf PRE_BUILD
    COMMAND ${CMAKE_BINARY_DIR}/TI_XDCTools_Source/xs.exe --xdcpath="${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source\;${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSdk_Source/kernel/tirtos/packages\;" xdc.tools.configuro -o configPkg -t gnu.targets.arm.M4 -p ti.platforms.simplelink:CC3220SF -r release -c "${TOOLCHAIN_PREFIX}" --compileOptions "-DDeviceFamily_CC3220" "${CMAKE_CURRENT_BINARY_DIR}/${TI_RTOS_CONFIG_FILE}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generate TI-RTOS configuration" 
)

#############################

# include common directories
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/common"
    "${PROJECT_SOURCE_DIR}/src/CLR/Core"
    "${PROJECT_SOURCE_DIR}/src/CLR/Include"
    "${PROJECT_SOURCE_DIR}/src/HAL/Include"
    "${PROJECT_SOURCE_DIR}/src/PAL/Include"

    "${WireProtocol_INCLUDE_DIRS}"
    "${TI_SimpleLink_INCLUDE_DIRS}"

    ${TI_XDCTools_INCLUDE_DIR}

    "${TARGET_TI_SimpleLink_COMMON_INCLUDE_DIRS}"
)

# include directories for nanoCLR
target_include_directories(${NANOCLR_PROJECT_NAME}.elf PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/nanoCLR
    "${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR"

    "${TARGET_TI_SimpleLink_NANOCLR_INCLUDE_DIRS}"

    # directories for nanoFramework libraries
    ${NF_CoreCLR_INCLUDE_DIRS}
    ${NF_Debugger_INCLUDE_DIRS}
    ${NF_Diagnostics_INCLUDE_DIRS}
    
    # includes for nanoFramework APIs
    ${TARGET_NANO_APIS_INCLUDES}
)

# set compiler options
nf_set_compiler_options(${NANOCLR_PROJECT_NAME}.elf)

# set compiler definitions
###############################################
# add -DUSE_CC3220_ROM_DRV_API to use ROM API #
###############################################
nf_set_compiler_definitions(${NANOCLR_PROJECT_NAME}.elf -DUSE_CC3220_ROM_DRV_API -DDeviceFamily_CC3220 -DBOARD_DISPLAY_USE_UART_ANSI=0 -DBOARD_DISPLAY_USE_LCD=0 )

# set linker files
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/CC3220SF_CLR-DEBUG.ld)
else()
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/CC3220SF_CLR.ld)
endif()

# set linker options
nf_set_linker_options(${NANOCLR_PROJECT_NAME}.elf -Wl,-T,${CMAKE_CURRENT_BINARY_DIR}/configPkg/linker.cmd)

# add other linker flags
###########################################################
# the size of CRT heap stacks is defined here
# set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--defsym=__crt_heap_size__=0x0")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--defsym=__crt_heap_size__=0x0")

# generate output files
# nf_generate_build_output_files(${NANOBOOTER_PROJECT_NAME}.elf)
nf_generate_build_output_files(${NANOCLR_PROJECT_NAME}.elf)
