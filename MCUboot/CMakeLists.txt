#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# MCUboot/CMakeLists.txt — Build rules for the MCUboot bootloader integration.
#
# MCUboot is built as an INDEPENDENT binary that replaces nanoBooter.
# It is NOT linked into nanoCLR.  The nanoCLR CMake build produces a signed image
# (via imgtool post-build step) that MCUboot can validate and boot.
#
# This file is only processed when NF_FEATURE_USE_MCUBOOT is ON.
# It is included from the top-level CMakeLists.txt or target CMakeLists.txt
# via: if(NF_FEATURE_USE_MCUBOOT) add_subdirectory(MCUboot) endif()
#
# MCUboot upstream source is fetched via FindMCUboot.cmake (FetchContent).
# The nanoFramework porting layer (port/stm32/ or port/esp32/) is compiled
# together with MCUboot's bootutil library.
#
# TODO(ifu-phase1): Wire up the full MCUboot bootloader build here.
#   Step 1: add_library(mcuboot_port ...) with platform port sources.
#   Step 2: target_include_directories for MCUboot upstream + nf port headers.
#   Step 3: add_executable(mcuboot.elf) linking bootutil + mcuboot_port + BSP startup.
#   Step 4: add post-build objcopy to produce mcuboot.bin / mcuboot.hex.
#
# For now this is a skeleton CMakeLists that documents the intended structure.

cmake_minimum_required(VERSION 3.31)

# MCUboot porting layer interface library.
# Provides the include paths and port sources to any target that links MCUboot.
add_library(nf_mcuboot_port INTERFACE)

target_include_directories(nf_mcuboot_port INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# TODO(ifu-phase1): Replace INTERFACE with STATIC and add platform port sources:
#
# if(RTOS STREQUAL "CHIBIOS")
#     target_sources(nf_mcuboot_port PRIVATE
#         port/stm32/flash_map_stm32.c
#     )
#     target_include_directories(nf_mcuboot_port PUBLIC
#         port/stm32
#     )
# elseif(RTOS STREQUAL "ESP32")
#     target_sources(nf_mcuboot_port PRIVATE
#         port/esp32/flash_map_esp32.c
#     )
#     target_include_directories(nf_mcuboot_port PUBLIC
#         port/esp32
#     )
# endif()
