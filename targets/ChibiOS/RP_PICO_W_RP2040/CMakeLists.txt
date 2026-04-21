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

# SMP support source (Pico W uses dual-core SMP)
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${chibios_SOURCE_DIR}/os/common/ports/ARMv6-M/smp/rp2/chcoresmp.c")
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${RP2040_COMMON}/target_stubs.cpp")

# RP2040 RNG stub (ROSC-based, replaces STM32 RNG)
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/target_rng.c")

# target include directories
list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/targets/ChibiOS/_WiFi")
list(APPEND TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/targets/ChibiOS/_WiFi/cyw43")

# CYW43 driver includes (from FetchContent)
if(USE_NETWORKING_OPTION AND TARGET_HAS_WIFI)
    find_package(CYW43_Driver REQUIRED QUIET)
    list(APPEND TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS ${CYW43_DRIVER_INCLUDE_DIRS})
    list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES ${CYW43_DRIVER_SOURCES})
endif()

# PIO LLD sources for CYW43 SPI bus
list(APPEND TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS "${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/PIOv1")
list(APPEND TARGET_CHIBIOS_NANOCLR_SOURCES "${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/PIOv1/rp_pio.c")

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE
        RP2040_booter

    CLR_LINKER_FILE
        RP2040_CLR

    CLR_EXTRA_COMPILE_DEFINITIONS
        -DRUNTIME_MEMORY_PROFILE__small=1
        -DTARGET_HAS_WIFI=1
        -DRP_PIO_REQUIRED=1
        LFS_CONFIG=lfs_config.h

    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--library-path=${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC/ld,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x400,--defsym=__crt_heap_size__=0x1000"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--library-path=${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC/ld,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x800,--defsym=__crt_heap_size__=0x14000"
)

# Relax warnings for external CYW43 driver sources (we cannot modify them)
if(USE_NETWORKING_OPTION AND TARGET_HAS_WIFI)
    foreach(CYW43_SRC ${CYW43_DRIVER_SOURCES})
        get_filename_component(CYW43_SRC_NAME ${CYW43_SRC} NAME)
        if(CYW43_SRC_NAME MATCHES "^cyw43_")
            set_source_files_properties(${CYW43_SRC} PROPERTIES COMPILE_FLAGS
                "-Wno-error=shadow -Wno-error=return-type -Wno-error=implicit-function-declaration -Wno-error=type-limits -Wno-error=unused-parameter -Wno-error=missing-prototypes -Wno-error=missing-declarations -Wno-error=old-style-definition -Wno-error=undef")
        endif()
    endforeach()
endif()

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
