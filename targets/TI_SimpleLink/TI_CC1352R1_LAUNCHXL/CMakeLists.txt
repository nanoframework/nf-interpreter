#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.TI_SimpleLink)

ENABLE_LANGUAGE(ASM)

# add packages
nf_add_common_packages()
nf_add_platform_packages()

#######################################

add_subdirectory("common")
add_subdirectory("nanoCLR")

#######################################

nf_add_platform_sysconfig_steps(CC1352 CC13X2)

#######################
# nanoCLR executable

add_executable(
    # executables for project, project sources
    ${NANOCLR_PROJECT_NAME}.elf

    ${CMAKE_CURRENT_SOURCE_DIR}/CC1352R1_LAUNCHXL_fxns.c
)

nf_add_platform_dependencies(${NANOCLR_PROJECT_NAME})

nf_add_common_sources(${NANOCLR_PROJECT_NAME})
nf_add_platform_sources(${NANOCLR_PROJECT_NAME})

# include directories for nanoCLR
nf_add_common_include_directories(${NANOCLR_PROJECT_NAME})
nf_add_platform_include_directories(${NANOCLR_PROJECT_NAME})

# set compile options
nf_set_compile_options(${NANOCLR_PROJECT_NAME}.elf)

# set compile definitions
nf_set_compile_definitions(TARGET ${NANOCLR_PROJECT_NAME}.elf EXTRA_COMPILE_DEFINITIONS -DDeviceFamily_CC13X2 )

# set linker files
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/CC13x2_26x2_CLR-DEBUG.ld)
else()
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/CC13x2_26x2_CLR.ld)
endif()

# set linker options
nf_set_linker_options(TARGET ${NANOCLR_PROJECT_NAME}.elf EXTRA_LINK_FLAGS "-Wl,-T,${CMAKE_CURRENT_BINARY_DIR}/configPkg/linker.cmd")

# add other linker flags
###########################################################
# the size of CRT heap stacks is defined here
# set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--defsym=__crt_heap_size__=0x0")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--defsym=__crt_heap_size__=0x0")

# generate output files
# nf_generate_build_output_files(${NANOBOOTER_PROJECT_NAME}.elf)
nf_generate_build_output_files(${NANOCLR_PROJECT_NAME}.elf)
