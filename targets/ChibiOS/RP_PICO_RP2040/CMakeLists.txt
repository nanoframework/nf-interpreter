#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.ChibiOS)

# RP2040 flash driver sources from target directory
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_RP2040FlashDriver.c")
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/platform_BlockStorage.c")
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/target_stubs.cpp")

# Async stubs (HAL_CONTINUATION) when nanoFramework.Runtime.Events is not enabled
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_SOURCE_DIR}/src/PAL/AsyncProcCall/Async_stubs.cpp")

# target include directories (for the RP2040 flash driver header)
list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE 
        RP2040_booter

    CLR_LINKER_FILE 
        RP2040_CLR

    CLR_EXTRA_COMPILE_DEFINITIONS
        -DRUNTIME_MEMORY_PROFILE__small=1

    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--library-path=${CHIBIOS_SOURCE_FOLDER}/os/common/startup/ARMCMx/compilers/GCC/ld,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x400,--defsym=__crt_heap_size__=0x1000"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--library-path=${CHIBIOS_SOURCE_FOLDER}/os/common/startup/ARMCMx/compilers/GCC/ld,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x400,--defsym=__crt_heap_size__=0x2000"
)

####################################################################################################
## RP2040 UF2 output generation                                                                   ##
## Addresses must match the linker scripts (RP2040_booter.ld / RP2040_CLR.ld)                     ##
## RP2040 UF2 family ID: 0xe48bff56                                                               ##
####################################################################################################

nf_generate_uf2_package(
    ${CMAKE_BINARY_DIR}/${NANOBOOTER_PROJECT_NAME}.bin 0x10000000
    ${CMAKE_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.bin 0x10018000
    0xe48bff56
    ${CMAKE_BINARY_DIR}/nanoCLR.uf2
)
