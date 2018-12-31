#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.0)
ENABLE_LANGUAGE(ASM)

# add header files with common OS definitions and board definitions specific for each image
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/target_board.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/nanoBooter/target_board.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/target_common.h" @ONLY)

set(NANOBOOTER_PROJECT_NAME "nanoBooter")
set(NANOCLR_PROJECT_NAME "nanoCLR")

find_package(BuildUtils REQUIRED)
find_package(CHIBIOS REQUIRED)
find_package(ChibiOSnfOverlay REQUIRED)
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

# nF feature: networking
if(USE_NETWORKING_OPTION)
    find_package(CHIBIOS_LWIP REQUIRED)
endif()

# nF feature: filesystem
if(USE_FILESYSTEM_OPTION)
    find_package(CHIBIOS_FATFS REQUIRED)
endif()

#######################################

add_subdirectory("common")
add_subdirectory("nanoBooter")
add_subdirectory("nanoCLR")

#######################
# nanoBooter executable

add_executable(
    # executables for project, project sources
    ${NANOBOOTER_PROJECT_NAME}.elf

    "${CMAKE_CURRENT_SOURCE_DIR}/target_common.c"

    # need to add configuration manager to allow get/store configuration blocks
    "${PROJECT_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager_stubs.c"

    ${COMMON_PROJECT_SOURCES}
    ${NANOBOOTER_PROJECT_SOURCES}

    ${TARGET_CMSIS_COMMON_SOURCES}
    ${TARGET_CMSIS_NANOBOOTER_SOURCES}
    
    ${TARGET_CHIBIOS_COMMON_SOURCES}
    ${TARGET_CHIBIOS_NANOBOOTER_SOURCES}

    ${CHIBIOS_SOURCES}
    ${ChibiOSnfOverlay_SOURCES}

    ${WireProtocol_SOURCES}
)

#######################
# nanoCLR executable

add_executable(
    # executables for project, project sources
    ${NANOCLR_PROJECT_NAME}.elf

    "${CMAKE_CURRENT_SOURCE_DIR}/target_common.c"

    ${COMMON_PROJECT_SOURCES}
    ${NANOCLR_PROJECT_SOURCES}

    ${TARGET_CMSIS_COMMON_SOURCES}
    ${TARGET_CMSIS_NANOCLR_SOURCES}
    
    ${TARGET_CHIBIOS_COMMON_SOURCES}
    ${TARGET_CHIBIOS_NANOCLR_SOURCES}

    ${CHIBIOS_SOURCES}
    ${ChibiOSnfOverlay_SOURCES}

    ${WireProtocol_SOURCES}

    # sources for nanoFramework libraries
    "${NF_CoreCLR_SOURCES}"
    "${NF_Debugger_SOURCES}"
    "${NF_Diagnostics_SOURCES}"

    # sources for nanoFramework APIs
    "${TARGET_NANO_APIS_SOURCES}"
)

# add dependency from ChibiOS (this is required to make sure the ChibiOS repo is downloaded before the build starts)
add_dependencies(${NANOBOOTER_PROJECT_NAME}.elf ChibiOS)
add_dependencies(${NANOCLR_PROJECT_NAME}.elf ChibiOS)

# include common directories
include_directories(
    "${CMAKE_CURRENT_BINARY_DIR}"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${PROJECT_SOURCE_DIR}/src/CLR/Core
    ${PROJECT_SOURCE_DIR}/src/CLR/Include
    ${PROJECT_SOURCE_DIR}/src/HAL/Include
    ${PROJECT_SOURCE_DIR}/src/PAL/Include

    ${WireProtocol_INCLUDE_DIRS}
    ${CHIBIOS_INCLUDE_DIRS}
    ${ChibiOSnfOverlay_INCLUDE_DIRS}

    ${TARGET_CMSIS_COMMON_INCLUDE_DIRS}
    ${TARGET_CHIBIOS_COMMON_INCLUDE_DIRS}
)

# include directories for nanoBooter
target_include_directories(${NANOBOOTER_PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/nanoBooter"
     ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter

     ${TARGET_CMSIS_NANOBOOTER_INCLUDE_DIRS}
     ${TARGET_CHIBIOS_NANOBOOTER_INCLUDE_DIRS}
)

# include directories for nanoCLR
target_include_directories(${NANOCLR_PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/nanoCLR"
    ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR

    ${TARGET_CMSIS_NANOCLR_INCLUDE_DIRS}
    ${TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS}

    # directories for nanoFramework libraries
    "${NF_CoreCLR_INCLUDE_DIRS}"
    "${NF_Debugger_INCLUDE_DIRS}"
    "${NF_Diagnostics_INCLUDE_DIRS}"

    # includes for nanoFramework APIs
    "${TARGET_NANO_APIS_INCLUDES}"

    # includes for ChibiOS LwIP
    "${CHIBIOS_LWIP_INCLUDE_DIRS}"
)

# set compiler options
nf_set_compiler_options(${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_compiler_options(${NANOCLR_PROJECT_NAME}.elf)

# set compiler definitions
nf_set_compiler_definitions(${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_compiler_definitions(${NANOCLR_PROJECT_NAME}.elf)

# set linker files
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/STM32F412xG_booter-DEBUG.ld)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/STM32F412xG_CLR-DEBUG.ld)
else()
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/STM32F412xG_booter.ld)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/STM32F412xG_CLR.ld)
endif()

# set linker options
nf_set_linker_options(${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_linker_options(${NANOCLR_PROJECT_NAME}.elf)

# add other linker flags
###########################################################
# the sizes of CRT heap and ChibiOS stacks are defined here
############################################################################
# CRT heap is assigned to RAM4 region @ CCM SRAM so we are OK to use it all
############################################################################
set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x400,--defsym=__crt_heap_size__=0x10000")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x800,--defsym=__crt_heap_size__=0x10000")

# generate output files
nf_generate_build_output_files(${NANOBOOTER_PROJECT_NAME}.elf)
nf_generate_build_output_files(${NANOCLR_PROJECT_NAME}.elf)
