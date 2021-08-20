#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.ChibiOS)

ENABLE_LANGUAGE(ASM)

# add packages
nf_add_common_packages()
nf_add_platform_packages()

#######################################

add_subdirectory("common")
add_subdirectory("nanoBooter")
add_subdirectory("nanoCLR")

#######################
# nanoBooter executable

add_executable(
    # executables for project, project sources
    ${NANOBOOTER_PROJECT_NAME}.elf

    # need to add configuration manager to allow get/store configuration blocks
    ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager_stubs.c
)

nf_add_platform_dependencies(${NANOBOOTER_PROJECT_NAME})

nf_add_common_sources(${NANOBOOTER_PROJECT_NAME})
nf_add_platform_sources(${NANOBOOTER_PROJECT_NAME})

# include directories for nanoBooter
nf_add_common_include_directories(${NANOBOOTER_PROJECT_NAME})
nf_add_platform_include_directories(${NANOBOOTER_PROJECT_NAME})

#######################
# nanoCLR executable

add_executable(
    # executables for project, project sources
    ${NANOCLR_PROJECT_NAME}.elf

    # the next one is required is the target implements and it's using external memory
    ${CMAKE_CURRENT_SOURCE_DIR}/target_external_memory.c
)

nf_add_platform_dependencies(${NANOCLR_PROJECT_NAME})

nf_add_common_sources(${NANOCLR_PROJECT_NAME})
nf_add_platform_sources(${NANOCLR_PROJECT_NAME})

# include directories for nanoCLR
nf_add_common_include_directories(${NANOCLR_PROJECT_NAME})
nf_add_platform_include_directories(${NANOCLR_PROJECT_NAME})

# set compile options
nf_set_compile_options(${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_compile_options(${NANOCLR_PROJECT_NAME}.elf)

# set compile definitions
nf_set_compile_definitions(TARGET ${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_compile_definitions(TARGET ${NANOCLR_PROJECT_NAME}.elf)

# set linker files
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/STM32F429xI_booter-DEBUG.ld)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/STM32F429xI_CLR-DEBUG.ld)
else()
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/STM32F429xI_booter.ld)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/STM32F429xI_CLR.ld)
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
