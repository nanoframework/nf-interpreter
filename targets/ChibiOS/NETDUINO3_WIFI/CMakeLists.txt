#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.ChibiOS)


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
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/netduino3wifi_booter-DEBUG.ld)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/netduino3wifi_CLR-DEBUG.ld)
else()
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/netduino3wifi_booter.ld)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/netduino3wifi_CLR.ld)
endif()

# set linker options
nf_set_linker_options(TARGET ${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_linker_options(TARGET ${NANOCLR_PROJECT_NAME}.elf)

# set link maps
###########################################################
# the sizes of CRT heap and ChibiOS stacks are defined here
############################################################################
# CRT heap is assigned to RAM4 region @ CCM SRAM so we are OK to use it all
############################################################################
nf_set_link_map(
    TARGET 
        ${NANOBOOTER_PROJECT_NAME}.elf
    EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x400,--defsym=__crt_heap_size__=0x10000")
nf_set_link_map(
    TARGET 
        ${NANOCLR_PROJECT_NAME}.elf
    EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x800,--defsym=__crt_heap_size__=0x10000")

# generate output files
nf_generate_build_output_files(${NANOBOOTER_PROJECT_NAME}.elf)
nf_generate_build_output_files(${NANOCLR_PROJECT_NAME}.elf)

# if HEX2DFU tool is available pack the binaries into a DFU package
if(HEX2DFU_TOOL_AVAILABLE)

    ####################################################################################################
    ## when changing the linker file make sure to update the new addresses for the image files below  ##
    ## DO NOT use the leading 0x notation, just the address in plain hexadecimal formating            ##
    ####################################################################################################

    if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        nf_generate_dfu_package(
            ${CMAKE_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.bin 08000000 
            ${CMAKE_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.bin 08008000 
            ${CMAKE_SOURCE_DIR}/build/nanobooter-nanoclr.dfu
        )
    else()
        nf_generate_dfu_package(
            ${CMAKE_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.bin 08000000 
            ${CMAKE_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.bin 08004000 
            ${CMAKE_SOURCE_DIR}/build/nanobooter-nanoclr.dfu
        )
    endif()

endif()
