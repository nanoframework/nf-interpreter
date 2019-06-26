#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.0)
ENABLE_LANGUAGE(ASM)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/target_board.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/nanoBooter/target_board.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/target_common.h" @ONLY)

set(NANOBOOTER_PROJECT_NAME "nanoBooter")
set(NANOCLR_PROJECT_NAME "nanoCLR")

find_package(BuildUtils REQUIRED)
find_package(FreeRTOS REQUIRED)
find_package(CMSIS REQUIRED)
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
    find_package(LWIP REQUIRED)
    find_package(NF_NETWORKING REQUIRED)
endif()

# nF feature: filesystem
if(USE_FILESYSTEM_OPTION)
    find_package(FATFS REQUIRED)
endif()

#######################################

add_subdirectory("nanoBooter")
add_subdirectory("nanoCLR")

#######################
# nanoBooter executable

add_executable(
    # executables for project, project sources
    ${NANOBOOTER_PROJECT_NAME}.elf

    "${CMAKE_CURRENT_SOURCE_DIR}/target_common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/target_BlockStorage.c"

    # need to add configuration manager to allow get/store configuration blocks
    "${PROJECT_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager.c"

    ${TARGET_NXP_COMMON_SOURCES}
    ${NANOBOOTER_PROJECT_SOURCES}

    ${FREERTOS_SOURCES}

    ${WireProtocol_SOURCES}
)

#######################
# nanoCLR executable

add_executable(
    # executables for project, project sources
    ${NANOCLR_PROJECT_NAME}.elf  

    "${CMAKE_CURRENT_SOURCE_DIR}/target_common.c"
    
    ${TARGET_NXP_COMMON_SOURCES}
    ${TARGET_NXP_NANOCLR_SOURCES}
    ${NANOCLR_PROJECT_SOURCES}

    ${FREERTOS_SOURCES}

    ${WireProtocol_SOURCES}

    # sources for nanoFramework libraries
    "${NF_CoreCLR_SOURCES}"
    "${NF_Debugger_SOURCES}"
    "${NF_Diagnostics_SOURCES}"

    # sources for FatFS
    "${FATFS_SOURCES}"

    # sources for nanoFramework Network LWIP, Sockets and TLS
    "${LWIP_SOURCES}"
    "${NF_Networking_SOURCES}"
    #"${mbedTLS_SOURCES}"

    # sources for nanoFramework APIs
    "${TARGET_NANO_APIS_SOURCES}"
)

# add dependency from FreeRTOS (this is required to make sure the FreeRTOS repo is downloaded before the build starts)
add_dependencies(${NANOBOOTER_PROJECT_NAME}.elf FreeRTOS CMSIS)
add_dependencies(${NANOCLR_PROJECT_NAME}.elf FreeRTOS CMSIS)

# include common directories
include_directories(
    "${CMAKE_CURRENT_BINARY_DIR}"

    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TARGET_NXP_COMMON_INCLUDE_DIRS}
    ${TARGET_NXP_NANOCLR_INCLUDE_DIRS}

    ${PROJECT_SOURCE_DIR}/src/CLR/Core
    ${PROJECT_SOURCE_DIR}/src/CLR/Include
    ${PROJECT_SOURCE_DIR}/src/HAL/Include
    ${PROJECT_SOURCE_DIR}/src/PAL/Include
    ${PROJECT_SOURCE_DIR}/src/CLR/Runtime.Native

    ${WireProtocol_INCLUDE_DIRS}
    ${FREERTOS_INCLUDE_DIRS}
    ${CMSIS_INCLUDE_DIRS}
)

# include directories for nanoBooter
target_include_directories(${NANOBOOTER_PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/nanoBooter"
     ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter

     ${TARGET_CMSIS_NANOBOOTER_INCLUDE_DIRS}
     ${TARGET_FREERTOS_NANOBOOTER_INCLUDE_DIRS}
)

# include directories for nanoCLR
target_include_directories(${NANOCLR_PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/nanoCLR"
     ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR

    # directories for nanoFramework libraries
    "${NF_CoreCLR_INCLUDE_DIRS}"
    "${NF_Debugger_INCLUDE_DIRS}"
    "${NF_Diagnostics_INCLUDE_DIRS}"

    # includes for nanoFramework APIs
    "${TARGET_NANO_APIS_INCLUDES}"

    # includes for FatFS
    ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/fatfs
    "${FATFS_INCLUDE_DIRS}"

    # includes for LwIP
    "${LWIP_INCLUDE_DIRS}"
    "${NANOCLR_PROJECT_INCLUDE_DIRS}"

    # incudes for Networking and TLS
    "${NF_Networking_INCLUDE_DIRS}"
    #"${mbedTLS_INCLUDE_DIRS}"
    
)

# set compiler options
nf_set_compiler_options(${NANOBOOTER_PROJECT_NAME}.elf -DCPU_MIMXRT1062CVL5A -DCPU_MIMXRT1062CVL5A_cm7)
nf_set_compiler_options(${NANOCLR_PROJECT_NAME}.elf -DCPU_MIMXRT1062CVL5A -DCPU_MIMXRT1062CVL5A_cm7)

# set linker files
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/MIMXRT10xx.ld)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/MIMXRT10xx.ld)
else()
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/MIMXRT10xx.ld)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/MIMXRT10xx.ld)
endif()

# set compiler definitions
nf_set_compiler_definitions(${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_compiler_definitions(${NANOCLR_PROJECT_NAME}.elf)

# set linker options
nf_set_linker_options(${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_linker_options(${NANOCLR_PROJECT_NAME}.elf)

# generate output files
nf_generate_build_output_files(${NANOBOOTER_PROJECT_NAME}.elf)
nf_generate_build_output_files(${NANOCLR_PROJECT_NAME}.elf)
