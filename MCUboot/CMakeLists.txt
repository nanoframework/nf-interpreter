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
# The nanoFramework porting layer is compiled as a static library (nf_mcuboot_port)
# and linked by each board target.
#
# TODO(ifu-later): Wire up the full MCUboot bootloader executable here:
#   - add_executable(mcuboot.elf) linking bootutil + nf_mcuboot_port + BSP startup
#   - post-build objcopy to produce mcuboot.bin / mcuboot.hex

cmake_minimum_required(VERSION 3.31)

# MCUboot porting layer static library.
# MCUboot_SOURCES and MCUboot_INCLUDE_DIRS are populated by FindMCUboot.cmake
# (called via find_package(MCUboot REQUIRED) before add_subdirectory(MCUboot)).
#
# mcuboot_flash_map.c is discovered by FindMCUboot.cmake using find_file() against
# ${TARGET_BASE_LOCATION}/common — no board-specific path needed here.
#
# Each board's CMakeLists.txt is responsible for adding:
#   - the external flash driver source (e.g. target_spi1_flash.c, target_qspi_flash.c)
#   - board-specific include directories (e.g. _common/mcuboot for stm32_f7xx_flash.h)
# via target_sources(nf_mcuboot_port PRIVATE ...) and
#      target_include_directories(nf_mcuboot_port PUBLIC ...)

add_library(nf_mcuboot_port STATIC ${MCUboot_SOURCES})

target_include_directories(nf_mcuboot_port PUBLIC
    ${MCUboot_INCLUDE_DIRS}
    ${TARGET_BASE_LOCATION}
)
