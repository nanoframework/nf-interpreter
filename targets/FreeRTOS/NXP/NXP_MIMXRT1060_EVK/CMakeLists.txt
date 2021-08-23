#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.FreeRTOS)


# add packages
nf_add_common_packages()
nf_add_platform_packages()

#######################################

add_subdirectory("nanoBooter")
add_subdirectory("nanoCLR")

#######################
# nanoBooter executable

add_executable(
    # executables for project, project sources
    ${NANOBOOTER_PROJECT_NAME}.elf
)

nf_add_platform_packages(TARGET ${NANOBOOTER_PROJECT_NAME})
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

nf_add_platform_packages(TARGET ${NANOCLR_PROJECT_NAME})
nf_add_platform_dependencies(${NANOCLR_PROJECT_NAME})

nf_add_common_sources(${NANOCLR_PROJECT_NAME})
nf_add_platform_sources(${NANOCLR_PROJECT_NAME})

# include directories for nanoCLR
nf_add_common_include_directories(${NANOCLR_PROJECT_NAME})
nf_add_platform_include_directories(${NANOCLR_PROJECT_NAME})

# set compile options
nf_set_compile_options(${NANOBOOTER_PROJECT_NAME}.elf -DCPU_MIMXRT1062CVL5A -DCPU_MIMXRT1062CVL5A_cm7)
nf_set_compile_options(${NANOCLR_PROJECT_NAME}.elf -DCPU_MIMXRT1062CVL5A -DCPU_MIMXRT1062CVL5A_cm7)
target_compile_definitions(NF_Network PUBLIC -DCPU_MIMXRT1062CVL5A -DCPU_MIMXRT1062CVL5A_cm7)
target_compile_definitions(NF_NativeAssemblies PUBLIC -DCPU_MIMXRT1062CVL5A -DCPU_MIMXRT1062CVL5A_cm7)

# set linker files
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/MIMXRT10xx.ld)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/MIMXRT10xx.ld)
else()
    nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/MIMXRT10xx.ld)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/MIMXRT10xx.ld)
endif()

# set compile definitions
nf_set_compile_definitions(TARGET ${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_compile_definitions(TARGET ${NANOCLR_PROJECT_NAME}.elf)

# set linker options
nf_set_linker_options(TARGET ${NANOBOOTER_PROJECT_NAME}.elf)
nf_set_linker_options(
    TARGET 
        ${NANOCLR_PROJECT_NAME}.elf
    EXTRA_LINK_FLAGS
        )

# set link maps
nf_set_link_map(
    TARGET 
        ${NANOBOOTER_PROJECT_NAME}.elf
    EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/TI_SimpleLink/common,--defsym=__crt_heap_size__=0x0")
nf_set_link_map(
    TARGET 
        ${NANOCLR_PROJECT_NAME}.elf
    EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/TI_SimpleLink/common,--defsym=__crt_heap_size__=0x0")

# generate output files
nf_generate_build_output_files(${NANOBOOTER_PROJECT_NAME}.elf)
nf_generate_build_output_files(${NANOCLR_PROJECT_NAME}.elf)

if(SRECORD_TOOL_AVAILABLE)
    nf_generate_hex_package(
        ${CMAKE_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.hex
        ${CMAKE_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.hex
        ${CMAKE_SOURCE_DIR}/build/nanobooter-nanoclr.hex)
endif()
