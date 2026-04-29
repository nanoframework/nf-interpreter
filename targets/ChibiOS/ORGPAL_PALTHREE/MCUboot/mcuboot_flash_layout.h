//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// mcuboot_flash_layout.h — Flash slot layout constants.
//
// Single source of truth for all MCUboot slot addresses and sizes.
// Included by both:
//   - common/mcuboot_flash_map.c      (ChibiOS / nanoCLR context)
//   - common/mcuboot_flash_map_boot.c (bare-metal bootloader context)
//
// Layout (STM32F769ZI 2 MB + AT25SF641 8 MB SPI1):
//
//   FLASH_AREA_BOOTLOADER        (0): 0x08000000  64 kB   internal sectors 0-1
//   [config block]                  : 0x08010000  32 kB   internal sector 2  (HAL-managed, not a MCUboot slot)
//   FLASH_AREA_IMAGE_0_PRIMARY   (1): 0x08018000  928 kB  internal sectors 3-7 (bank 1)
//   FLASH_AREA_IMAGE_0_SECONDARY (2): AT25SF641 @ 0x000000  928 kB  (232 × 4 kB sub-sectors)
//   FLASH_AREA_IMAGE_1_PRIMARY   (4): 0x08100000  1024 kB internal bank 2
//   FLASH_AREA_IMAGE_1_SECONDARY (5): AT25SF641 @ 0x0E8000  1024 kB (256 × 4 kB sub-sectors)

#ifndef MCUBOOT_FLASH_LAYOUT_ORGPAL_PALTHREE_H
#define MCUBOOT_FLASH_LAYOUT_ORGPAL_PALTHREE_H

#include <stdint.h>
#include <assert.h>

// clang-format off

// MCUboot bootloader slot (sectors 0-1, 64 kB)
#define NF_MCUBOOT_SLOT_BOOTLOADER_OFF      0x08000000U
#define NF_MCUBOOT_SLOT_BOOTLOADER_SIZE     (64U * 1024U)

// Config block (sector 2, 32 kB) — HAL-managed; NOT a MCUboot flash area.
// It sits between the bootloader and the CLR primary slot and is never
// enumerated, erased, or written by MCUboot during an upgrade.
#define NF_MCUBOOT_CONFIG_OFF               0x08010000U
#define NF_MCUBOOT_CONFIG_SIZE              (32U * 1024U)

// Image 0 primary — nanoCLR (sectors 3-7, bank 1, 928 kB)
#define NF_MCUBOOT_SLOT_IMG0_PRI_OFF        0x08018000U
#define NF_MCUBOOT_SLOT_IMG0_PRI_SIZE       (928U * 1024U)

// Image 0 secondary — CLR upgrade candidate on AT25SF641 (232 × 4 kB = 928 kB)
#define NF_MCUBOOT_SLOT_IMG0_SEC_OFF        0x000000U
#define NF_MCUBOOT_SLOT_IMG0_SEC_SIZE       (928U * 1024U)

// Image 1 primary — deployment (bank 2, 1024 kB)
#define NF_MCUBOOT_SLOT_IMG1_PRI_OFF        0x08100000U
#define NF_MCUBOOT_SLOT_IMG1_PRI_SIZE       (1024U * 1024U)

// Image 1 secondary — deployment upgrade candidate on AT25SF641 (256 × 4 kB = 1024 kB)
#define NF_MCUBOOT_SLOT_IMG1_SEC_OFF        0x0E8000U
#define NF_MCUBOOT_SLOT_IMG1_SEC_SIZE       (1024U * 1024U)

// clang-format on

// Boundary assertions — evaluated in every translation unit that includes this header.
static_assert(
    NF_MCUBOOT_SLOT_IMG0_PRI_OFF + NF_MCUBOOT_SLOT_IMG0_PRI_SIZE <= NF_MCUBOOT_SLOT_IMG1_PRI_OFF,
    "PALTHREE: CLR primary overflows into deploy primary");
static_assert(
    NF_MCUBOOT_SLOT_IMG0_SEC_OFF + NF_MCUBOOT_SLOT_IMG0_SEC_SIZE <= NF_MCUBOOT_SLOT_IMG1_SEC_OFF,
    "PALTHREE: CLR secondary overflows into deploy secondary");

#endif // MCUBOOT_FLASH_LAYOUT_ORGPAL_PALTHREE_H
