//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// mcuboot_flash_layout.h — Flash slot layout constants
//
// Single source of truth for all MCUboot slot addresses and sizes.
// Included by both:
//   - common/mcuboot_flash_map.c      (ChibiOS / nanoCLR context)
//   - common/mcuboot_flash_map_boot.c (bare-metal bootloader context)
//
// Layout (STM32F769NI 2 MB + W25Q512 64 MB QSPI):
//
//   FLASH_AREA_BOOTLOADER        (0): 0x08000000  32 kB   internal sector 0
//   FLASH_AREA_IMAGE_0_PRIMARY   (1): 0x08010000  704 kB  internal sectors 2-6 (bank 1)
//   FLASH_AREA_IMAGE_0_SECONDARY (2): W25Q512 @ 0x000000  704 kB  (11 × 64 kB)
//   FLASH_AREA_IMAGE_1_PRIMARY   (4): 0x080C0000  1280 kB internal sector 7 (bank 1) + bank 2
//   FLASH_AREA_IMAGE_1_SECONDARY (5): W25Q512 @ 0x0B0000  1280 kB (20 × 64 kB)
//
// Config block: 0x08008000, 32 kB (sector 1) — not a flash_area, managed by HAL.

#ifndef MCUBOOT_FLASH_LAYOUT_ORGPAL_PALX_H
#define MCUBOOT_FLASH_LAYOUT_ORGPAL_PALX_H

#include <stdint.h>
#include <assert.h>

// clang-format off

// MCUboot bootloader slot (sector 0, 32 kB)
#define NF_MCUBOOT_SLOT_BOOTLOADER_OFF      0x08000000U
#define NF_MCUBOOT_SLOT_BOOTLOADER_SIZE     (32U * 1024U)

// Image 0 primary — nanoCLR (sectors 2-6, bank 1, 704 kB)
#define NF_MCUBOOT_SLOT_IMG0_PRI_OFF        0x08010000U
#define NF_MCUBOOT_SLOT_IMG0_PRI_SIZE       (704U * 1024U)

// Image 0 secondary — CLR upgrade candidate on W25Q512 (11 × 64 kB = 704 kB)
#define NF_MCUBOOT_SLOT_IMG0_SEC_OFF        0x000000U
#define NF_MCUBOOT_SLOT_IMG0_SEC_SIZE       (704U * 1024U)

// Image 1 primary — deployment (sector 7 bank 1 + bank 2, 1280 kB)
#define NF_MCUBOOT_SLOT_IMG1_PRI_OFF        0x080C0000U
#define NF_MCUBOOT_SLOT_IMG1_PRI_SIZE       (1280U * 1024U)

// Image 1 secondary — deployment upgrade candidate on W25Q512 (20 × 64 kB = 1280 kB)
#define NF_MCUBOOT_SLOT_IMG1_SEC_OFF        0x0B0000U
#define NF_MCUBOOT_SLOT_IMG1_SEC_SIZE       (1280U * 1024U)

// clang-format on

// Boundary assertions — evaluated in every translation unit that includes this header.
static_assert(
    NF_MCUBOOT_SLOT_IMG0_PRI_OFF + NF_MCUBOOT_SLOT_IMG0_PRI_SIZE <= NF_MCUBOOT_SLOT_IMG1_PRI_OFF,
    "PALX: CLR primary overflows into deploy primary");
static_assert(
    NF_MCUBOOT_SLOT_IMG0_SEC_OFF + NF_MCUBOOT_SLOT_IMG0_SEC_SIZE <= NF_MCUBOOT_SLOT_IMG1_SEC_OFF,
    "PALX: CLR secondary overflows into deploy secondary");

#endif // MCUBOOT_FLASH_LAYOUT_ORGPAL_PALX_H
