//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// mcuboot_flash_layout.h — Flash slot layout constants.
//
// Single source of truth for all MCUboot slot addresses and sizes.
// Included by the single flash map porting layer MCUboot/mcuboot_flash_map_boot.c.
//
// Layout (STM32F769ZI 2 MB + AT25SF641 8 MB SPI1):
//
//   FLASH_AREA_BOOTLOADER        (0): 0x08000000  96 kB    internal sectors 0-2
//   [config block]                  : 0x08018000  32 kB    internal sector 3  (HAL-managed, not a MCUboot slot)
//   [unused]                        : 0x08020000  128 kB   internal sector 4  (see note below)
//   FLASH_AREA_IMAGE_0_PRIMARY   (1): 0x08040000  1280 kB  internal (5 x 256 kB logical sectors)
//   FLASH_AREA_IMAGE_0_SECONDARY (2): AT25SF641 @ 0x000000  1536 kB  (384 x 4 kB sub-sectors)
//   FLASH_AREA_IMAGE_1_PRIMARY   (3): 0x08180000  512 kB   internal (2 x 256 kB logical sectors)
//   FLASH_AREA_IMAGE_1_SECONDARY (4): AT25SF641 @ 0x180000  768 kB   (192 x 4 kB sub-sectors)
//
// USABLE IMAGE SIZE (swap-using-offset):
//   MCUboot rounds the swap trailer up to one whole logical sector, so the largest
//   image a slot can actually hold is (logical_sectors - 1) * 256 kB:
//     image 0 primary: 5 LS -> 1024 kB usable  (nanoCLR is ~825 kB)
//     image 1 primary: 2 LS ->  256 kB usable  (deploy payload)
//   imgtool's --slot-size (NF_MCUBOOT_SLOT_SIZE in CMakePresets.json) is set to that
//   usable value, not the physical slot size, so an oversized image is rejected at
//   sign time rather than at swap time.
//
// LOGICAL SECTOR SIZE:
//   CONFIG_NF_MCUBOOT_LOGICAL_SECTOR_SIZE is 256 kB (0x40000). MCUBOOT_SWAP_USING_OFFSET
//   treats one logical sector as its erase/copy unit. Every logical-sector boundary in a
//   primary slot MUST coincide with a real STM32F7 erase-page boundary
//   The primary slots below start at 0x08040000 and 0x08140000 and are whole multiples of 256 kB, so each logical sector maps onto
//   one 256 kB physical sector — or, at the start of image 1 primary and the last logical
//   sector of image 0 primary, onto a run of smaller physical sectors that erase to
//   exactly 256 kB.
//
// SECONDARY SLOT SIZING:
//   Each secondary is one logical sector (256 kB) larger than its primary, the optimal
//   distribution for MCUBOOT_SWAP_USING_OFFSET's boot_slots_compatible().

#ifndef MCUBOOT_FLASH_LAYOUT_ORGPAL_PALTHREE_H
#define MCUBOOT_FLASH_LAYOUT_ORGPAL_PALTHREE_H

#include <stdint.h>
#include <assert.h>

// clang-format off

// MCUboot bootloader slot (sectors 0-2, 96 kB)
#define NF_MCUBOOT_SLOT_BOOTLOADER_OFF      0x08000000U
#define NF_MCUBOOT_SLOT_BOOTLOADER_SIZE     (96U * 1024U)

// Config block (sector 3, 32 kB) — HAL-managed; NOT a MCUboot flash area.
// It sits between the bootloader and the CLR primary slot and is never
// enumerated, erased, or written by MCUboot during an upgrade.
#define NF_MCUBOOT_CONFIG_OFF               0x08018000U
#define NF_MCUBOOT_CONFIG_SIZE              (32U * 1024U)

// Internal sector 4 (0x08020000, 128 kB) — unused. Not 256 kB-aligned as a logical
// sector start, so it cannot belong to a swap-using-offset primary slot.
#define NF_MCUBOOT_UNUSED_S4_OFF            0x08020000U
#define NF_MCUBOOT_UNUSED_S4_SIZE          (128U * 1024U)

// Image 0 primary — nanoCLR (0x08040000, 1280 kB = 5 x 256 kB logical sectors; 1024 kB usable)
#define NF_MCUBOOT_SLOT_IMG0_PRI_OFF        0x08040000U
#define NF_MCUBOOT_SLOT_IMG0_PRI_SIZE       (1280U * 1024U)

// Image 0 secondary — CLR upgrade candidate on AT25SF641 (384 x 4 kB = 1536 kB;
// primary size + one 256 kB logical sector)
#define NF_MCUBOOT_SLOT_IMG0_SEC_OFF        0x000000U
#define NF_MCUBOOT_SLOT_IMG0_SEC_SIZE       (1536U * 1024U)

// Image 1 primary — deployment (0x08180000, 512 kB = 2 x 256 kB logical sectors; 256 kB usable)
#define NF_MCUBOOT_SLOT_IMG1_PRI_OFF        0x08180000U
#define NF_MCUBOOT_SLOT_IMG1_PRI_SIZE       (512U * 1024U)

// Image 1 secondary — deployment upgrade candidate on AT25SF641 (192 x 4 kB = 768 kB;
// primary size + one 256 kB logical sector)
#define NF_MCUBOOT_SLOT_IMG1_SEC_OFF        0x180000U
#define NF_MCUBOOT_SLOT_IMG1_SEC_SIZE       (768U * 1024U)

// clang-format on

// Boundary assertions — evaluated in every translation unit that includes this header.
static_assert(
    NF_MCUBOOT_SLOT_BOOTLOADER_OFF + NF_MCUBOOT_SLOT_BOOTLOADER_SIZE <= NF_MCUBOOT_CONFIG_OFF,
    "PALTHREE: bootloader overflows into config block");
static_assert(
    NF_MCUBOOT_CONFIG_OFF + NF_MCUBOOT_CONFIG_SIZE <= NF_MCUBOOT_UNUSED_S4_OFF,
    "PALTHREE: config block overflows into unused sector 4");
static_assert(
    NF_MCUBOOT_UNUSED_S4_OFF + NF_MCUBOOT_UNUSED_S4_SIZE <= NF_MCUBOOT_SLOT_IMG0_PRI_OFF,
    "PALTHREE: unused sector 4 overflows into CLR primary");
static_assert(
    NF_MCUBOOT_SLOT_IMG0_PRI_OFF + NF_MCUBOOT_SLOT_IMG0_PRI_SIZE <= NF_MCUBOOT_SLOT_IMG1_PRI_OFF,
    "PALTHREE: CLR primary overflows into deploy primary");
static_assert(
    NF_MCUBOOT_SLOT_IMG1_PRI_OFF + NF_MCUBOOT_SLOT_IMG1_PRI_SIZE <= 0x08200000U,
    "PALTHREE: deploy primary overflows the end of internal flash (2 MB)");
static_assert(
    NF_MCUBOOT_SLOT_IMG0_SEC_OFF + NF_MCUBOOT_SLOT_IMG0_SEC_SIZE <= NF_MCUBOOT_SLOT_IMG1_SEC_OFF,
    "PALTHREE: CLR secondary overflows into deploy secondary");

#endif // MCUBOOT_FLASH_LAYOUT_ORGPAL_PALTHREE_H
