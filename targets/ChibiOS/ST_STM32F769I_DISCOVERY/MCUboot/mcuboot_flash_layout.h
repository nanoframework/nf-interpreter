//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// mcuboot_flash_layout.h — Flash slot layout constants for ST_STM32F769I_DISCOVERY.
//
// Single source of truth for all MCUboot slot addresses and sizes.
// Included by both:
//   - common/mcuboot_flash_map.c      (ChibiOS / nanoCLR context)
//   - MCUboot/mcuboot_flash_map_boot.c (bare-metal bootloader context)
//
// Layout (STM32F769NI 2 MB internal + SD card via FatFs for secondary slots):
//
//   FLASH_AREA_BOOTLOADER        (0): 0x08000000  32 kB   internal sector 0
//   FLASH_AREA_IMAGE_0_PRIMARY   (1): 0x08010000  960 kB  internal sectors 2-7 (bank 1)
//   FLASH_AREA_IMAGE_0_SECONDARY (2): SD card file /mcuboot/img0_sec.bin  960 kB
//   FLASH_AREA_IMAGE_1_PRIMARY   (4): 0x08100000  1024 kB internal bank 2
//   FLASH_AREA_IMAGE_1_SECONDARY (5): SD card file /mcuboot/img1_sec.bin  1024 kB
//
// Secondary slots are accessed as FatFs files (virtual sector = 4 kB).
// FatFs integration is deferred — mcuboot_ext_flash_init() currently returns -1
// so MCUboot boots the primary slot without attempting an upgrade.

#ifndef MCUBOOT_FLASH_LAYOUT_ST_STM32F769I_DISCOVERY_H
#define MCUBOOT_FLASH_LAYOUT_ST_STM32F769I_DISCOVERY_H

#include <stdint.h>
#include <assert.h>

// clang-format off

// MCUboot bootloader slot (sectors 0-1, 64 kB)
#define NF_MCUBOOT_SLOT_BOOTLOADER_OFF      0x08000000U
#define NF_MCUBOOT_SLOT_BOOTLOADER_SIZE     (64U * 1024U)

// Image 0 primary — nanoCLR (sectors 2-7, bank 1, 960 kB)
#define NF_MCUBOOT_SLOT_IMG0_PRI_OFF        0x08010000U
#define NF_MCUBOOT_SLOT_IMG0_PRI_SIZE       (960U * 1024U)

// Image 0 secondary — CLR upgrade candidate on SD card (virtual, 960 kB)
#define NF_MCUBOOT_SLOT_IMG0_SEC_OFF        0x000000U
#define NF_MCUBOOT_SLOT_IMG0_SEC_SIZE       (960U * 1024U)

// Image 1 primary — deployment (bank 2, 1024 kB)
#define NF_MCUBOOT_SLOT_IMG1_PRI_OFF        0x08100000U
#define NF_MCUBOOT_SLOT_IMG1_PRI_SIZE       (1024U * 1024U)

// Image 1 secondary — deployment upgrade candidate on SD card (virtual, 1024 kB)
#define NF_MCUBOOT_SLOT_IMG1_SEC_OFF        0x0F0000U
#define NF_MCUBOOT_SLOT_IMG1_SEC_SIZE       (1024U * 1024U)

// clang-format on

// Boundary assertions — evaluated in every translation unit that includes this header.
static_assert(
    NF_MCUBOOT_SLOT_IMG0_PRI_OFF + NF_MCUBOOT_SLOT_IMG0_PRI_SIZE <= NF_MCUBOOT_SLOT_IMG1_PRI_OFF,
    "DISCOVERY: CLR primary overflows into deploy primary");
static_assert(
    NF_MCUBOOT_SLOT_IMG0_SEC_OFF + NF_MCUBOOT_SLOT_IMG0_SEC_SIZE <= NF_MCUBOOT_SLOT_IMG1_SEC_OFF,
    "DISCOVERY: CLR secondary overflows into deploy secondary");

#endif // MCUBOOT_FLASH_LAYOUT_ST_STM32F769I_DISCOVERY_H
