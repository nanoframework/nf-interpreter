#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(chibios)

include(binutils.common)
include(binutils.ChibiOS)

# shared RP2040 sources and include directory
set(RP2040_COMMON "${CMAKE_SOURCE_DIR}/targets/ChibiOS/_RP2040")
list(APPEND CMAKE_PREFIX_PATH "${RP2040_COMMON}")
list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS "${RP2040_COMMON}")
list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS "${RP2040_COMMON}/common")

# RP2040 flash driver and common sources from shared directory
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${RP2040_COMMON}/Target_BlockStorage_RP2040FlashDriver.c")
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${RP2040_COMMON}/target_stubs.cpp")

# target include directories
list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE 
        RP2040_booter

    CLR_LINKER_FILE 
        RP2040_CLR

    CLR_EXTRA_COMPILE_DEFINITIONS
        -DRUNTIME_MEMORY_PROFILE__small=1
        -DTARGET_XIP_FLASH_BLOCKS_USB=1
        LFS_CONFIG=lfs_config.h

    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--library-path=${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC/ld,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x400,--defsym=__crt_heap_size__=0x1000"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--library-path=${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC/ld,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x800,--defsym=__crt_heap_size__=0x4000"
)

####################################################################################################
## RP2040 UF2 output generation                                                                   ##
## Addresses must match the linker scripts (RP2040_booter.ld / RP2040_CLR.ld)                     ##
## RP2040 UF2 family ID: 0xe48bff56                                                               ##
####################################################################################################

nf_generate_uf2_package(
    ${CMAKE_BINARY_DIR}/${NANOBOOTER_PROJECT_NAME}.bin 0x10000000
    ${CMAKE_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.bin 0x10014000
    0xe48bff56
    ${CMAKE_BINARY_DIR}/nanoCLR.uf2
)
