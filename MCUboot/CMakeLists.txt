#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.31)

# MCUboot porting layer static library (nanoCLR side).
add_library(nf_mcuboot_port STATIC ${MCUboot_SOURCES})

target_include_directories(nf_mcuboot_port PUBLIC
    ${MCUboot_INCLUDE_DIRS}
    ${TARGET_BASE_LOCATION}
    ${TARGET_BASE_LOCATION}/MCUboot
)

##################################################################
# WHEN ADDING A NEW SERIES add the appropriate section below
##################################################################

# ---------------------------------------------------------------------- #
# Shared MCUboot upstream sources (series-agnostic)                      #
# ---------------------------------------------------------------------- #

# bootutil sources required for SWAP_USING_OFFSET + SIGN_EC256.
set(MCUBOOT_BOOTUTIL_SOURCES
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/loader.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/bootutil_misc.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/bootutil_public.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/bootutil_loader.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/bootutil_area.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/bootutil_img_hash.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/bootutil_find_key.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/bootutil_img_security_cnt.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/image_validate.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/image_ecdsa.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/swap_misc.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/swap_offset.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/fault_injection_hardening.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/caps.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/tlv.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/encrypted.c
    ${mcuboot_SOURCE_DIR}/boot/bootutil/src/boot_record.c
)

# Tinycrypt — bundled in MCUboot; provides SHA-256 and ECDSA P-256 verification.
set(MCUBOOT_TINYCRYPT_SOURCES
    ${mcuboot_SOURCE_DIR}/ext/tinycrypt/lib/source/ecc.c
    ${mcuboot_SOURCE_DIR}/ext/tinycrypt/lib/source/ecc_dsa.c
    ${mcuboot_SOURCE_DIR}/ext/tinycrypt/lib/source/sha256.c
    ${mcuboot_SOURCE_DIR}/ext/tinycrypt/lib/source/utils.c
)

# mbedTLS ASN.1 decoder — bundled in MCUboot; used by image_ecdsa.c.
set(MCUBOOT_MBEDTLS_ASN1_SOURCES
    ${mcuboot_SOURCE_DIR}/ext/mbedtls-asn1/src/asn1parse.c
    ${mcuboot_SOURCE_DIR}/ext/mbedtls-asn1/src/platform_util.c
)

# ---------------------------------------------------------------------- #
# nf_setup_mcuboot_target_build — generic bootloader executable macro    #
#                                                                         #
# Called from each board's MCUboot/mcuboot_target.cmake.                 #
# The series-specific sets (MCUBOOT_SERIES_COMMON_*) must be populated   #
# inside the appropriate series guard below before the include().         #
# The output binary is always named ${NANOMCUBOOTER_PROJECT_NAME}.elf.   #
#                                                                         #
# Parameters:                                                             #
#   LINKER_FILE           — full path to the board linker script         #
#   EXTRA_SOURCES         — board-specific source files                  #
#   EXTRA_INCLUDES        — board-specific include directories           #
#   COMPILE_DEFINITIONS   — board-specific compile definitions           #
#   EXTRA_COMPILE_OPTIONS — board-specific optimisation flags            #
# ---------------------------------------------------------------------- #

macro(nf_setup_mcuboot_target_build)

    cmake_parse_arguments(
        NFSMTB
        ""
        "LINKER_FILE"
        "EXTRA_SOURCES;EXTRA_INCLUDES;COMPILE_DEFINITIONS;EXTRA_COMPILE_OPTIONS"
        ${ARGN}
    )

    add_executable(${NANOMCUBOOTER_PROJECT_NAME}.elf
        ${MCUBOOT_SERIES_COMMON_SOURCES}
        ${NFSMTB_EXTRA_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/keys/signing_pub_key.c
        ${MCUBOOT_BOOTUTIL_SOURCES}
        ${MCUBOOT_TINYCRYPT_SOURCES}
        ${MCUBOOT_MBEDTLS_ASN1_SOURCES}
    )

    target_include_directories(${NANOMCUBOOTER_PROJECT_NAME}.elf PRIVATE
        ${MCUBOOT_SERIES_COMMON_INCLUDES}
        ${NFSMTB_EXTRA_INCLUDES}
    )

    target_compile_definitions(${NANOMCUBOOTER_PROJECT_NAME}.elf PRIVATE
        ${MCUBOOT_SERIES_COMMON_COMPILE_DEFINITIONS}
        ${NFSMTB_COMPILE_DEFINITIONS}
    )

    target_compile_options(${NANOMCUBOOTER_PROJECT_NAME}.elf PRIVATE
        ${MCUBOOT_SERIES_COMMON_COMPILE_OPTIONS}
        ${NFSMTB_EXTRA_COMPILE_OPTIONS}
    )

    set_target_properties(${NANOMCUBOOTER_PROJECT_NAME}.elf PROPERTIES
        LINK_FLAGS "-T${NFSMTB_LINKER_FILE} ${MCUBOOT_SERIES_COMMON_LINK_FLAGS}"
    )

    add_custom_command(TARGET ${NANOMCUBOOTER_PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:${NANOMCUBOOTER_PROJECT_NAME}.elf>
            ${CMAKE_BINARY_DIR}/${NANOMCUBOOTER_PROJECT_NAME}.bin
        COMMAND ${CMAKE_OBJCOPY} -O ihex
            $<TARGET_FILE:${NANOMCUBOOTER_PROJECT_NAME}.elf>
            ${CMAKE_BINARY_DIR}/${NANOMCUBOOTER_PROJECT_NAME}.hex
        COMMENT "Generating ${NANOMCUBOOTER_PROJECT_NAME}.bin and ${NANOMCUBOOTER_PROJECT_NAME}.hex"
    )
    add_custom_command(TARGET ${NANOMCUBOOTER_PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} --format=berkeley $<TARGET_FILE:${NANOMCUBOOTER_PROJECT_NAME}.elf>
        COMMENT "MCUboot memory usage:"
    )

endmacro()

# ---------------------------------------------------------------------- #
# STM32F7xx — Cortex-M7, ChibiOS RT + HAL minimal bootloader            #
# ---------------------------------------------------------------------- #

if(${TARGET_SERIES} STREQUAL "STM32F7xx")

    # Retrieve chibios_SOURCE_DIR — it was populated in targets/ChibiOS/CMakeLists.txt
    # but CMake FetchContent variables don't cross subdirectory scopes automatically.
    include(FetchContent)
    FetchContent_GetProperties(chibios)

    # Sources shared by all STM32F7xx MCUboot boards (ChibiOS RT + HAL + nf-overlay).
    set(MCUBOOT_SERIES_COMMON_SOURCES
        # MCUboot bootloader entry point.
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot/mcuboot_main.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot/mcuboot_serial_port.c

        # ChibiOS RT startup (Cortex-M7, ARMCMx).
        ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC/crt0_v7m.S
        ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC/crt1.c
        ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC/vectors.S

        # ChibiOS RT port (ARMv7-M).
        ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M/chcore.c
        ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M/compilers/GCC/chcoreasm.S

        # ChibiOS RT minimal kernel.
        ${chibios_SOURCE_DIR}/os/rt/src/chsys.c
        ${chibios_SOURCE_DIR}/os/rt/src/chvt.c
        ${chibios_SOURCE_DIR}/os/rt/src/chschd.c
        ${chibios_SOURCE_DIR}/os/rt/src/chthreads.c
        ${chibios_SOURCE_DIR}/os/rt/src/chinstances.c
        ${chibios_SOURCE_DIR}/os/rt/src/chrfcu.c
        ${chibios_SOURCE_DIR}/os/rt/src/chevents.c
        ${chibios_SOURCE_DIR}/os/rt/src/chsem.c
        ${chibios_SOURCE_DIR}/os/rt/src/chregistry.c
        ${chibios_SOURCE_DIR}/os/rt/src/chdebug.c
        ${chibios_SOURCE_DIR}/os/oslib/src/chmemcore.c

        # ChibiOS HAL core.
        ${chibios_SOURCE_DIR}/os/hal/src/hal.c
        ${chibios_SOURCE_DIR}/os/hal/src/hal_st.c
        ${chibios_SOURCE_DIR}/os/hal/src/hal_pal.c
        ${chibios_SOURCE_DIR}/os/hal/src/hal_spi.c
        ${chibios_SOURCE_DIR}/os/hal/src/hal_wspi.c
        ${chibios_SOURCE_DIR}/os/hal/src/hal_serial.c
        ${chibios_SOURCE_DIR}/os/hal/src/hal_queues.c

        # ChibiOS HAL STM32F7xx platform.
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/STM32F7xx/hal_lld.c
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/STM32F7xx/stm32_isr.c

        # ChibiOS HAL common ARMCMx support.
        ${chibios_SOURCE_DIR}/os/hal/ports/common/ARMCMx/nvic.c

        # ChibiOS HAL LLD (STM32 shared drivers).
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/GPIOv2/hal_pal_lld.c
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/DMAv2/stm32_dma.c
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/EXTIv1/stm32_exti.c
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SPIv2/hal_spi_v2_lld.c
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/QUADSPIv1/hal_wspi_lld.c
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/USARTv2/hal_serial_lld.c
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SYSTICKv1/hal_st_lld.c
        ${chibios_SOURCE_DIR}/os/hal/src/hal_mmcsd.c
        ${chibios_SOURCE_DIR}/os/hal/src/hal_sdc.c
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SDMMCv1/hal_sdc_lld.c

        # ChibiOS HAL OSAL (RT).
        ${chibios_SOURCE_DIR}/os/hal/osal/rt-nil/osal.c
        
        # nf-overlay: STM32 internal flash driver (FLASHv2 LLD).
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/src/stm32_flash/hal_stm32_flash.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FLASHv2/flash_lld.c

        # Stub: halCommunityInit() no-op — MCUboot does not use ChibiOS-Contrib.
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot/hal_community_stub.c
    )

    # Include directories shared by all STM32F7xx bootloader targets.
    set(MCUBOOT_SERIES_COMMON_INCLUDES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot
        ${CMAKE_CURRENT_SOURCE_DIR}/include

        ${mcuboot_SOURCE_DIR}/boot/bootutil/include
        ${mcuboot_SOURCE_DIR}/boot/boot_serial/include
        ${mcuboot_SOURCE_DIR}/ext/mbedtls-asn1/include
        ${mcuboot_SOURCE_DIR}/ext/tinycrypt/lib/include

        ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC
        ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/devices/STM32F7xx
        ${chibios_SOURCE_DIR}/os/rt/include
        ${chibios_SOURCE_DIR}/os/oslib/include
        ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M
        ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M/compilers/GCC
        ${chibios_SOURCE_DIR}/os/hal/include
        ${chibios_SOURCE_DIR}/os/hal/osal/rt-nil
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/STM32F7xx
        ${chibios_SOURCE_DIR}/os/hal/ports/common/ARMCMx
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/GPIOv2
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/DMAv2
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/EXTIv1
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/TIMv1
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SYSTICKv1
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SPIv2
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/QUADSPIv1
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/USART
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/USARTv2
        ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/SDMMCv1
        ${chibios_SOURCE_DIR}/os/common/ext/ARM/CMSIS/Core/Include
        ${chibios_SOURCE_DIR}/os/common/ext/ST/STM32F7xx
        ${chibios_SOURCE_DIR}/os/common/portability/GCC
        ${chibios_SOURCE_DIR}/os/common/ports/ARM-common
        ${chibios_SOURCE_DIR}/os/license

        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/include
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/include/dummy_includes
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FLASHv2
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/CRCv1
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/RNGv1
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FSMCv1
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/ONEWIREv1
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/QSPIv1
    )

    # Compile options shared by all STM32F7xx bootloader targets.
    set(MCUBOOT_SERIES_COMMON_COMPILE_OPTIONS
        -mthumb
        -mcpu=cortex-m7
        -mfpu=fpv5-sp-d16
        -mfloat-abi=hard
        -mabi=aapcs
        -Wall
        -Wextra
        -ffunction-sections
        -fdata-sections
        -fno-common
        # wchar_t must be 2 bytes (UTF-16) for USB string descriptors to be laid out
        # correctly. Without this flag the default 4-byte wchar_t adds alignment
        # padding inside the USB descriptor structs, corrupting every string descriptor.
        -fshort-wchar
        # Suppress warnings from MCUboot upstream code that we do not own.
        -Wno-unused-parameter
        -Wno-sign-compare
        -Wno-missing-field-initializers
    )

    # Compile definitions shared by all STM32F7xx bootloader targets.
    # CORTEX_USE_FPU must be passed explicitly: crt0_v7m.S is assembled (not compiled),
    # so chcore.h is never included and CRT0_INIT_FPU defaults to FALSE, leaving the
    # FPU un-enabled. With -mfloat-abi=hard the compiler emits FPU instructions,
    # causing a UsageFault (NOCP) -> HardFault on the first FPU access.
    set(MCUBOOT_SERIES_COMMON_COMPILE_DEFINITIONS
        CORTEX_USE_FPU=TRUE
        PLATFORM_ARM
        USE_FPU=TRUE
    )

    # Link flags shared by all STM32F7xx bootloader targets.
    # -nostartfiles is removed: ChibiOS provides crt0_v7m.s + vectors.c.
    # The linker script is board-specific and supplied by nf_setup_mcuboot_target_build.
    # -L adds the ChibiOS GCC ld directory so INCLUDE directives in the linker script
    # can resolve rules_stacks.ld, rules_code.ld, rules_data.ld, rules_memory.ld.
    # --defsym provides the stack/heap sizes expected by rules_stacks.ld.
    set(MCUBOOT_SERIES_COMMON_LINK_FLAGS
        "-mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mabi=aapcs \
         -Wl,--gc-sections \
         -Wl,--print-memory-usage \
         -Wl,--no-warn-rwx-segments \
         --specs=nano.specs \
         --specs=nosys.specs \
         -L${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC/ld \
         -Wl,--defsym=__main_stack_size__=0x400 \
         -Wl,--defsym=__process_stack_size__=0x800")

    # FatFs core + ChibiOS platform diskio for targets using SD card as secondary slot storage.
    # SDC HAL driver stack (hal_sdc.c, hal_mmcsd.c, hal_sdc_lld.c) and their headers are
    # included unconditionally above, gated at compile time by HAL_USE_SDC in halconf.h.
    # This block adds only the FatFs library files and the ChibiOS-platform diskio glue.
    # Must come BEFORE include(mcuboot_target.cmake) because the macro calls add_executable()
    # with ${MCUBOOT_SERIES_COMMON_SOURCES} immediately.
    if(NF_FEATURE_MCUBOOT_HAS_SDCARD)

        # Propagate the Kconfig option into C preprocessor
        list(APPEND MCUBOOT_SERIES_COMMON_COMPILE_DEFINITIONS NF_FEATURE_MCUBOOT_HAS_SDCARD)

        list(APPEND MCUBOOT_SERIES_COMMON_SOURCES
            ${MCUBOOT_FATFS_SOURCES}
            ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot/fatfs_diskio_boot.c
            ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot/mcuboot_fatfs_flash_area.c
        )

        list(APPEND MCUBOOT_SERIES_COMMON_INCLUDES ${MCUBOOT_FATFS_INCLUDE_DIRS})

    endif() # NF_FEATURE_MCUBOOT_HAS_SDCARD

    # When serial recovery is enabled, add the full SMP/mcumgr serial stack:
    if(NF_MCUBOOT_SERIAL_RECOVERY)

        list(APPEND MCUBOOT_SERIES_COMMON_COMPILE_DEFINITIONS MCUBOOT_SERIAL=1)

        list(APPEND MCUBOOT_SERIES_COMMON_SOURCES
            ${mcuboot_SOURCE_DIR}/boot/boot_serial/src/boot_serial.c
            ${mcuboot_SOURCE_DIR}/boot/zcbor/src/zcbor_common.c
            ${mcuboot_SOURCE_DIR}/boot/zcbor/src/zcbor_decode.c
            ${mcuboot_SOURCE_DIR}/boot/zcbor/src/zcbor_encode.c
            ${mcuboot_SOURCE_DIR}/boot/boot_serial/src/zcbor_bulk.c

            ${CMAKE_SOURCE_DIR}/MCUboot/common/crc16_ccitt.c
            ${CMAKE_SOURCE_DIR}/MCUboot/common/base64_impl.c
        )

        list(APPEND MCUBOOT_SERIES_COMMON_INCLUDES
            ${mcuboot_SOURCE_DIR}/boot/zcbor/include
            ${mcuboot_SOURCE_DIR}/boot/boot_serial/src

            ${chibios_SOURCE_DIR}/os/hal/ports/STM32/LLD/OTGv1
        )

    endif() # NF_MCUBOOT_SERIAL_RECOVERY

    # Board-specific build rules live in each board's MCUboot folder.
    include(${TARGET_BASE_LOCATION}/MCUboot/mcuboot_target.cmake)

else()

    message(FATAL_ERROR "MCUboot: add series-specific sources, includes and compile options for ${TARGET_SERIES}")

endif() # TARGET_SERIES STREQUAL "STM32F7xx"
