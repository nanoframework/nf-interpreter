#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.0)
ENABLE_LANGUAGE(ASM)

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

#######################################

add_subdirectory("common")
add_subdirectory("nanoBooter")
# add_subdirectory("nanoCLR")

#######################
# nanoBooter executable

add_executable(
    # executables for project, project sources
    ${NANOBOOTER_PROJECT_NAME}.elf

    ${COMMON_PROJECT_SOURCES}
    ${NANOBOOTER_PROJECT_SOURCES}

    ${FREERTOS_SOURCES}

    #${WireProtocol_SOURCES}
)

#######################
# nanoCLR executable

# add_executable(
#     # executables for project, project sources
#     ${NANOCLR_PROJECT_NAME}.elf    
# )

# add dependency from FreeRTOS (this is required to make sure the FreeRTOS repo is downloaded before the build starts)
# add_dependencies(${NANOBOOTER_PROJECT_NAME}.elf FreeRTOS)
# add_dependencies(${NANOCLR_PROJECT_NAME}.elf FreeRTOS)

# include common directories
include_directories(
    "${CMAKE_CURRENT_BINARY_DIR}"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/common/board
    ${CMAKE_CURRENT_SOURCE_DIR}/common/component/serial_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/common/component/uart
    ${CMAKE_CURRENT_SOURCE_DIR}/common/device
    ${CMAKE_CURRENT_SOURCE_DIR}/common/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/common/utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/common/xip

    ${PROJECT_SOURCE_DIR}/src/CLR/Core
    ${PROJECT_SOURCE_DIR}/src/CLR/Include
    ${PROJECT_SOURCE_DIR}/src/HAL/Include
    ${PROJECT_SOURCE_DIR}/src/PAL/Include

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

# # include directories for nanoCLR
# target_include_directories(${NANOCLR_PROJECT_NAME}.elf PUBLIC
#     "${CMAKE_CURRENT_BINARY_DIR}/nanoCLR"
#      ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR

#      ${TARGET_CMSIS_NANOCLR_INCLUDE_DIRS}
#      ${TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS}

#     # directories for nanoFramework libraries
#     "${NF_CoreCLR_INCLUDE_DIRS}"
#     "${NF_Debugger_INCLUDE_DIRS}"
#     "${NF_Diagnostics_INCLUDE_DIRS}"
    
#     # includes for nanoFramework APIs
#     "${TARGET_NANO_APIS_INCLUDES}"

#     # includes for ChibiOS FatFS
#     "${CHIBIOS_FATFS_INCLUDE_DIRS}"

#     # includes for ChibiOS LwIP
#     "${CHIBIOS_LWIP_INCLUDE_DIRS}"

#     # incudes for Networking and TLS
#     "${NF_Networking_INCLUDE_DIRS}"
#     "${mbedTLS_INCLUDE_DIRS}"
# )

# set compiler options
nf_set_compiler_options(${NANOBOOTER_PROJECT_NAME}.elf)
# nf_set_compiler_options(${NANOCLR_PROJECT_NAME}.elf)

# set compiler definitions
nf_set_compiler_definitions(${NANOBOOTER_PROJECT_NAME}.elf)
# nf_set_compiler_definitions(${NANOCLR_PROJECT_NAME}.elf)

# set linker files
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/MIMXRT10xx.ld)
    # nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/STM32F407xG_CLR-DEBUG.ld)
else()
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/MIMXRT10xx.ld)
    # nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/STM32F407xG_CLR.ld)
endif()

# set linker options
nf_set_linker_options(${NANOBOOTER_PROJECT_NAME}.elf)
# nf_set_linker_options(${NANOCLR_PROJECT_NAME}.elf)

# add other linker flags
###########################################################
# the sizes of CRT heap and ChibiOS stacks are defined here
# set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x800,--defsym=__crt_heap_size__=0x2000")
# set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x800,--defsym=__crt_heap_size__=0x3B000")

# generate output files
nf_generate_build_output_files(${NANOBOOTER_PROJECT_NAME}.elf)
# nf_generate_build_output_files(${NANOCLR_PROJECT_NAME}.elf)
