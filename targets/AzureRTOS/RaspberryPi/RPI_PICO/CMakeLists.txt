#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.common)
include(binutils.AzureRTOS)
include(AZURERTOS_${TARGET_SERIES}_GCC_options)

FetchContent_GetProperties(azure_rtos)

ENABLE_LANGUAGE(ASM)

# Azure RTOS settings and inclusion of build system
set(THREADX_ARCH "cortex_m0")
set(THREADX_TOOLCHAIN "gnu")
set(MCPU_FLAGS "-mthumb -mcpu=cortex-m0")
set(VFP_FLAGS "")
set(SPEC_FLAGS "--specs=nosys.specs")
add_subdirectory(${azure_rtos_SOURCE_DIR} AzureSource)

# add packages
NF_ADD_COMMON_PACKAGES()
NF_ADD_PLATFORM_PACKAGES()

#######################################

# add_subdirectory("common")
# add_subdirectory("nanoBooter")
# add_subdirectory("nanoCLR")

#######################
# nanoBooter executable

# add_executable(
#     # executables for project, project sources
#     ${NANOBOOTER_PROJECT_NAME}.elf

#     # need to add configuration manager to allow get/store configuration blocks
#     ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager.c
# )

# NF_ADD_PLATFORM_DEPENDENCIES(${NANOBOOTER_PROJECT_NAME})

# NF_ADD_COMMON_SOURCES(${NANOBOOTER_PROJECT_NAME})
# NF_ADD_PLATFORM_SOURCES(${NANOBOOTER_PROJECT_NAME})

# # include directories for nanoBooter
# NF_ADD_COMMON_INCLUDE_DIRECTORIES(${NANOBOOTER_PROJECT_NAME})
# NF_ADD_PLATFORM_INCLUDE_DIRECTORIES(${NANOBOOTER_PROJECT_NAME})

#######################
# nanoCLR executable

add_executable(
    # executables for project, project sources
    ${NANOCLR_PROJECT_NAME}.elf

    hello_world.c
)

NF_ADD_PLATFORM_DEPENDENCIES(${NANOCLR_PROJECT_NAME})

NF_ADD_COMMON_SOURCES(${NANOCLR_PROJECT_NAME})
NF_ADD_PLATFORM_SOURCES(${NANOCLR_PROJECT_NAME})

# include directories for nanoCLR
NF_ADD_COMMON_INCLUDE_DIRECTORIES(${NANOCLR_PROJECT_NAME})
NF_ADD_PLATFORM_INCLUDE_DIRECTORIES(${NANOCLR_PROJECT_NAME})

# set compiler options
# nf_set_compiler_options(${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_compiler_options(${NANOCLR_PROJECT_NAME}.elf)

# set compiler definitions
# nf_set_compiler_definitions(${NANOBOOTER_PROJECT_NAME}.elf )
nf_set_compiler_definitions(${NANOCLR_PROJECT_NAME}.elf )

# set linker files
# if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
#     nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/STM32L475VGTx_CLR-DEBUG.ld)
#     nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/STM32L475VGTx_CLR-DEBUG.ld)
# else()
#     nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/STM32L475VGTx_CLR.ld)
#     nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/STM32L475VGTx_CLR.ld)
# endif()

# set linker options
# nf_set_link_options(${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_link_options(${NANOCLR_PROJECT_NAME}.elf)

# add other linker flags
###########################################################
# the size of CRT heap is defined here
# set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--defsym=__main_stack_size__=0x400,--defsym=__crt_heap_size__=0x2000")
# set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--defsym=__main_stack_size__=0x400,--defsym=__crt_heap_size__=0x2000")

# generate output files
# nf_generate_build_output_files(${NANOBOOTER_PROJECT_NAME}.elf)
# nf_generate_build_output_files(${NANOCLR_PROJECT_NAME}.elf)


# Add pico_stdlib library which aggregates commonly used features
# target_link_libraries(${NANOCLR_PROJECT_NAME}.elf pico_stdlib)

# create map/bin/hex/uf2 file in addition to ELF.
# pico_add_extra_outputs(hello_world)
