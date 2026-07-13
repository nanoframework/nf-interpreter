#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(chibios)

include(binutils.common)
include(binutils.ChibiOS)

# Configure-time flag used by the WiFi driver gating below.
set(TARGET_HAS_WIFI TRUE)

# RP2350 W target-local sources and include directories
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/common")

list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/common")

list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_RP2350FlashDriver.c")

list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/target_stubs.cpp")

# Keep core1 behavior single-core/stubbed on RP2350 W to match known stable target runtime.
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/target_core1_stubs.c")

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
        RP2350_booter

    CLR_LINKER_FILE
        RP2350_CLR

    CLR_EXTRA_COMPILE_DEFINITIONS
        -DRUNTIME_MEMORY_PROFILE__small=1
        -DTARGET_HAS_WIFI=1
        -DTARGET_RP2350=1
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
        if(CYW43_SRC_NAME STREQUAL "cyw43_ll.c")
            # cyw43_ll.c is a very large file. GCC 15 changed -Og to keep more variables on
            # the stack for debuggability, pushing function frames past the
            # SP-relative ldr offset limit. Force -Os -fno-inline to prevent this.
            set_source_files_properties(${CYW43_SRC} PROPERTIES COMPILE_FLAGS
                "-Os -fno-inline -Wno-undef -Wno-error=shadow -Wno-error=return-type -Wno-error=implicit-function-declaration -Wno-error=type-limits -Wno-error=unused-parameter -Wno-error=missing-prototypes -Wno-error=missing-declarations -Wno-error=old-style-definition")
        elseif(CYW43_SRC_NAME MATCHES "^cyw43_")
            set_source_files_properties(${CYW43_SRC} PROPERTIES COMPILE_FLAGS
                "-Wno-undef -Wno-error=shadow -Wno-error=return-type -Wno-error=implicit-function-declaration -Wno-error=type-limits -Wno-error=unused-parameter -Wno-error=missing-prototypes -Wno-error=missing-declarations -Wno-error=old-style-definition")
        endif()
    endforeach()
endif()

################################################################################
## RP2350 UF2 output generation                                               ##
## Addresses must match the linker scripts (RP2350_booter.ld / RP2350_CLR.ld) ##
## RP2350 UF2 family ID (ARM secure image): 0xe48bff59                       ##
################################################################################

nf_generate_uf2_package(
    ${CMAKE_BINARY_DIR}/${NANOBOOTER_PROJECT_NAME}.bin 0x10000000
    ${CMAKE_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.bin 0x10014000
    0xe48bff59
    ${CMAKE_BINARY_DIR}/nanoCLR.uf2
)
