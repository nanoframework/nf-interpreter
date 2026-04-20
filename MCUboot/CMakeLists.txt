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
# STM32F7xx — Cortex-M7, bare-metal bootloader                          #
# ---------------------------------------------------------------------- #

if(${TARGET_SERIES} STREQUAL "STM32F7xx")

    # Fetch the CMSIS source-dir variables — they are set as internal non-cache variables by
    # FetchContent_MakeAvailable in sibling subdirectories and are therefore not in scope here.
    # Calling FetchContent_GetProperties locally makes them visible in this CMakeLists.txt scope.
    include(FetchContent)
    FetchContent_GetProperties(cmsis_device_f7)
    FetchContent_GetProperties(cmsis_core)

    # Bare-metal port sources shared by all STM32F7xx boards.
    # Order matters: _mcuboot must be first so the stub nf_config.h is found
    # before any Kconfig-generated nf_config.h.
    set(MCUBOOT_SERIES_COMMON_SOURCES
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot/mcuboot_main.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot/startup_stm32f7.s
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot/system_stm32f7xx.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot/stm32f7_flash_bare.c
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot/mcuboot_serial_port.c
    )

    # Include directories shared by all STM32F7xx bootloader targets.
    set(MCUBOOT_SERIES_COMMON_INCLUDES
        # 1. Stub nf_config.h + shared port headers (stm32f7_flash_bare.h,
        #    mcuboot_board_iface.h, stm32_f7xx_flash.h).
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_mcuboot

        # 2. Global MCUboot nanoFramework headers (mcuboot_config/mcuboot_config.h,
        #    sysflash/sysflash.h, flash_map_backend/flash_map_backend.h).
        ${CMAKE_CURRENT_SOURCE_DIR}/include

        # 3. MCUboot upstream bootutil public headers.
        ${mcuboot_SOURCE_DIR}/boot/bootutil/include

        # 4. MCUboot boot_serial include — provides boot_serial.h with the
        #    boot_uart_funcs type used in boot_serial_start().
        ${mcuboot_SOURCE_DIR}/boot/boot_serial/include

        # 5. mbedTLS ASN.1 decoder headers.
        ${mcuboot_SOURCE_DIR}/ext/mbedtls-asn1/include

        # 6. Tinycrypt headers.
        ${mcuboot_SOURCE_DIR}/ext/tinycrypt/lib/include

        # 7. CMSIS Device F7 headers (stm32f769xx.h, stm32f7xx.h, system_stm32f7xx.h).
        ${cmsis_device_f7_SOURCE_DIR}/Include

        # 8. CMSIS Core headers (core_cm7.h, cmsis_gcc.h).
        ${cmsis_core_SOURCE_DIR}/Include
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
        # Suppress warnings from MCUboot upstream code that we do not own.
        -Wno-unused-parameter
        -Wno-sign-compare
        -Wno-missing-field-initializers
    )

    # Link flags shared by all STM32F7xx bootloader targets.
    # The linker script is board-specific and supplied by nf_setup_mcuboot_target_build.
    set(MCUBOOT_SERIES_COMMON_LINK_FLAGS
        "-mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mabi=aapcs \
         -Wl,--gc-sections \
         -Wl,--print-memory-usage \
         -Wl,--no-warn-rwx-segments \
         -nostartfiles \
         --specs=nano.specs \
         --specs=nosys.specs")

    # Board-specific build rules live in each board's MCUboot folder.
    include(${TARGET_BASE_LOCATION}/MCUboot/mcuboot_target.cmake)

else()

    message(FATAL_ERROR "MCUboot: add series-specific sources, includes and compile options for ${TARGET_SERIES}")

endif() # TARGET_SERIES STREQUAL "STM32F7xx"
