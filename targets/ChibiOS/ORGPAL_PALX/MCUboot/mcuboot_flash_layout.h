//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// mcuboot_flash_layout.h — Flash slot layout constants
//
// Single source of truth for all MCUboot slot addresses and sizes.
// Included by the single flash map porting layer MCUboot/mcuboot_flash_map_boot.c,
// which is compiled both for the bootloader and (lean scope) for the nanoCLR
// nf_mcuboot_port library.
//
// Layout (STM32F769NI 2 MB + W25Q512 64 MB QSPI):
//
//   FLASH_AREA_BOOTLOADER        (0): 0x08000000  64 kB    internal sectors 0-1
//   [config block]                  : 0x08010000  32 kB    internal sector 2  (HAL-managed, not a MCUboot slot)
//   [unused]                        : 0x08018000  160 kB   internal sectors 3-4  (see note below)
//   FLASH_AREA_IMAGE_0_PRIMARY   (1): 0x08040000  1024 kB  internal (4 x 256 kB logical sectors)
//   FLASH_AREA_IMAGE_0_SECONDARY (2): W25Q512 @ 0x000000  1280 kB  (40 x 32 kB blocks)
//   FLASH_AREA_IMAGE_1_PRIMARY   (3): 0x08140000  768 kB   internal (3 x 256 kB logical sectors)
//   FLASH_AREA_IMAGE_1_SECONDARY (4): W25Q512 @ 0x140000  1024 kB  (32 x 32 kB blocks)
//
// USABLE IMAGE SIZE (swap-using-offset):
//   MCUboot rounds the swap trailer up to one whole logical sector, so the largest
//   image a slot can actually hold is (logical_sectors - 1) * 256 kB:
//     image 0 primary: 4 LS -> 768 kB usable  (PALX nanoCLR has no networking/crypto stack)
//     image 1 primary: 3 LS -> 512 kB usable  (deploy payload)
//   imgtool's --slot-size (NF_MCUBOOT_SLOT_SIZE in CMakePresets.json) is set to that
//   usable value, not the physical slot size, so an oversized image is rejected at
//   sign time rather than at swap time.
//
// LOGICAL SECTOR SIZE:
//   CONFIG_NF_MCUBOOT_LOGICAL_SECTOR_SIZE is 256 kB (0x40000). MCUBOOT_SWAP_USING_OFFSET
//   treats one logical sector as its erase/copy unit. Every logical-sector boundary in a
//   primary slot MUST coincide with a real STM32F7 erase-page boundary.
//   The primary slots below start at 0x08040000 and 0x08140000 and are whole multiples
//   of 256 kB, so each logical sector maps onto one 256 kB physical sector — or, over the
//   dual-bank small-sector run at 0x08100000-0x08140000 (4 x 32 kB + 1 x 128 kB), onto a
//   group of smaller physical sectors that erase to exactly 256 kB.
//   A 32 kB logical sector (the previous value, CLR primary at 0x08018000 / 672 kB) split
//   the 128 kB and 256 kB physical pages and corrupts image 0 on the first real
//   swap-over-populated-slot.
//
// SECONDARY SLOT SIZING:
//   Each secondary is one logical sector (256 kB) larger than its primary, the optimal
//   distribution for MCUBOOT_SWAP_USING_OFFSET's boot_slots_compatible().

#ifndef MCUBOOT_FLASH_LAYOUT_ORGPAL_PALX_H
#define MCUBOOT_FLASH_LAYOUT_ORGPAL_PALX_H

#include <stdint.h>
#include <assert.h>

// clang-format off

// MCUboot bootloader slot (sectors 0-1, 64 kB)
#define NF_MCUBOOT_SLOT_BOOTLOADER_OFF      0x08000000U
#define NF_MCUBOOT_SLOT_BOOTLOADER_SIZE     (64U * 1024U)

// Config block (sector 2, 32 kB) — HAL-managed; not part of any MCUboot flash_area.
#define NF_MCUBOOT_CONFIG_OFF               0x08010000U
#define NF_MCUBOOT_CONFIG_SIZE              (32U * 1024U)

// Internal sectors 3-4 (0x08018000, 32 kB + 128 kB) — unused. 0x08018000 is not a
// 256 kB boundary, so neither sector can start (or belong to) a swap-using-offset
// primary slot. The first 256 kB-aligned address after the config block is 0x08040000.
#define NF_MCUBOOT_UNUSED_OFF               0x08018000U
#define NF_MCUBOOT_UNUSED_SIZE             (160U * 1024U)

// Image 0 primary — nanoCLR (0x08040000, 1024 kB = 4 x 256 kB logical sectors; 768 kB usable)
#define NF_MCUBOOT_SLOT_IMG0_PRI_OFF        0x08040000U
#define NF_MCUBOOT_SLOT_IMG0_PRI_SIZE       (1024U * 1024U)

// Image 0 secondary — CLR upgrade candidate on W25Q512 (40 x 32 kB = 1280 kB;
// primary size + one 256 kB logical sector)
#define NF_MCUBOOT_SLOT_IMG0_SEC_OFF        0x000000U
#define NF_MCUBOOT_SLOT_IMG0_SEC_SIZE       (1280U * 1024U)

// Image 1 primary — deployment (0x08140000, 768 kB = 3 x 256 kB logical sectors; 512 kB usable)
#define NF_MCUBOOT_SLOT_IMG1_PRI_OFF        0x08140000U
#define NF_MCUBOOT_SLOT_IMG1_PRI_SIZE       (768U * 1024U)

// Image 1 secondary — deployment upgrade candidate on W25Q512 (32 x 32 kB = 1024 kB;
// primary size + one 256 kB logical sector)
#define NF_MCUBOOT_SLOT_IMG1_SEC_OFF        0x140000U
#define NF_MCUBOOT_SLOT_IMG1_SEC_SIZE       (1024U * 1024U)

// clang-format on

// Boundary assertions — evaluated in every translation unit that includes this header.
static_assert(
    NF_MCUBOOT_SLOT_BOOTLOADER_OFF + NF_MCUBOOT_SLOT_BOOTLOADER_SIZE <= NF_MCUBOOT_CONFIG_OFF,
    "PALX: bootloader overflows into config block");
static_assert(
    NF_MCUBOOT_CONFIG_OFF + NF_MCUBOOT_CONFIG_SIZE <= NF_MCUBOOT_UNUSED_OFF,
    "PALX: config block overflows into unused sectors 3-4");
static_assert(
    NF_MCUBOOT_UNUSED_OFF + NF_MCUBOOT_UNUSED_SIZE <= NF_MCUBOOT_SLOT_IMG0_PRI_OFF,
    "PALX: unused sectors 3-4 overflow into CLR primary");
static_assert(
    NF_MCUBOOT_SLOT_IMG0_PRI_OFF + NF_MCUBOOT_SLOT_IMG0_PRI_SIZE <= NF_MCUBOOT_SLOT_IMG1_PRI_OFF,
    "PALX: CLR primary overflows into deploy primary");
static_assert(
    NF_MCUBOOT_SLOT_IMG1_PRI_OFF + NF_MCUBOOT_SLOT_IMG1_PRI_SIZE <= 0x08200000U,
    "PALX: deploy primary overflows the end of internal flash (2 MB)");
static_assert(
    NF_MCUBOOT_SLOT_IMG0_SEC_OFF + NF_MCUBOOT_SLOT_IMG0_SEC_SIZE <= NF_MCUBOOT_SLOT_IMG1_SEC_OFF,
    "PALX: CLR secondary overflows into deploy secondary");

#endif // MCUBOOT_FLASH_LAYOUT_ORGPAL_PALX_H
