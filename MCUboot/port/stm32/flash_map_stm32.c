//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// STM32-specific MCUboot flash area implementation (porting layer stub).
//
// This file implements the flash_area_* API from flash_map_backend.h on top of
// the STM32 HAL flash driver already present in nanoFramework.
//
// PRIMARY SLOT (nanoCLR / deployment primary):
//   Uses the STM32 internal flash driver.  The existing Wire Protocol commands
//   call stm32FlashWrite() / stm32FlashErase() directly; this stub wraps the
//   same driver calls for the MCUboot path.
//   Reference: targets/ChibiOS/<BOARD>/common/WireProtocol_MonitorCommands.c
//
// SECONDARY SLOT (nanoCLR / deployment secondary — OTA staging):
//   Uses external storage.  Three options are available per board:
//     - ORGPAL_PALTHREE / ORGPAL_PALX: (Q)SPI flash via hal_lfs_read/prog/erase
//     - ST_STM32F769I_DISCOVERY:       SD card via FatFs (virtual-sector abstraction)
//   Selection is driven by compile-time defines set from the board's CMakeLists.
//
// Alignment notes (per-SoC, from STM32 reference manuals):
//   STM32F76xx: write granularity = 4 bytes (byte/halfword/word/doubleword supported)
//   STM32F429:  write granularity = 1/2/4 bytes (sector-dependent)
//
// All functions are stubs that return success.  Each contains a TODO comment
// identifying the exact driver call to substitute in Phase 1.
//
// Phase 1 implementation tasks are tagged: // TODO(ifu-phase1): <description>

#include <stdint.h>
#include <stddef.h>

#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"

//
// Static flash area table for STM32 targets.
// Addresses and sizes are placeholders; they are overridden per board by the
// board-specific mcuboot_stm32_config.h which is included by the CMake build.
//
// Layout (example: ORGPAL_PALTHREE / ST_STM32F769I_DISCOVERY, IMAGE_NUMBER=2):
//   FLASH_AREA_BOOTLOADER        (0): 0x08000000, 32 kB  (MCUboot replaces nanoBooter)
//   FLASH_AREA_IMAGE_0_PRIMARY   (1): 0x08008000, 960 kB (nanoCLR primary, internal)
//   FLASH_AREA_IMAGE_0_SECONDARY (2): external,   960 kB (nanoCLR secondary, ext. storage)
//   FLASH_AREA_IMAGE_1_PRIMARY   (4): 0x080F8000, 1024 kB (deploy_0 primary, internal)
//   FLASH_AREA_IMAGE_1_SECONDARY (5): external,   1024 kB (deploy_1 secondary, ext. storage)
//
// TODO(ifu-phase1): Populate exact addresses from board linker script / MCUboot-flash-layout.md.
//
static const struct flash_area s_stm32_flash_areas[] = {
    // clang-format off
    { .fa_id = FLASH_AREA_BOOTLOADER,        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH, .fa_off = 0x00000000, .fa_size = 0 },
    { .fa_id = FLASH_AREA_IMAGE_0_PRIMARY,   .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH, .fa_off = 0x00000000, .fa_size = 0 },
    { .fa_id = FLASH_AREA_IMAGE_0_SECONDARY, .fa_device_id = FLASH_DEVICE_EXTERNAL_FLASH, .fa_off = 0x00000000, .fa_size = 0 },
    { .fa_id = FLASH_AREA_IMAGE_1_PRIMARY,   .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH, .fa_off = 0x00000000, .fa_size = 0 },
    { .fa_id = FLASH_AREA_IMAGE_1_SECONDARY, .fa_device_id = FLASH_DEVICE_EXTERNAL_FLASH, .fa_off = 0x00000000, .fa_size = 0 },
    // clang-format on
};

#define FLASH_AREA_TABLE_COUNT (sizeof(s_stm32_flash_areas) / sizeof(s_stm32_flash_areas[0]))

int flash_area_open(uint8_t id, const struct flash_area **area_outp)
{
    for (uint32_t i = 0; i < FLASH_AREA_TABLE_COUNT; i++)
    {
        if (s_stm32_flash_areas[i].fa_id == id)
        {
            *area_outp = &s_stm32_flash_areas[i];
            return 0;
        }
    }
    return -1;
}

void flash_area_close(const struct flash_area *area)
{
    (void)area;
    // No resource to release for memory-mapped internal flash.
    // TODO(ifu-phase1): If using FatFs SD card for external slot, close the file handle here.
}

int flash_area_read(const struct flash_area *area, uint32_t off, void *dst, uint32_t len)
{
    (void)area;
    (void)off;
    (void)dst;
    (void)len;

    if (area->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        // TODO(ifu-phase1): Replace with memcpy from (area->fa_off + off) for memory-mapped internal flash.
        //   Internal STM32 flash is memory-mapped and readable via direct pointer dereference.
        //   Example: memcpy(dst, (void *)(area->fa_off + off), len);
    }
    else
    {
        // TODO(ifu-phase1): ORGPAL_PALTHREE/PALX: call hal_lfs_read_0() / hal_lfs_read_1().
        //   F769I: call f_read() via FatFs virtual-sector abstraction for the SD card slot file.
    }

    return 0; // Stub: success
}

int flash_area_write(const struct flash_area *area, uint32_t off, const void *src, uint32_t len)
{
    (void)area;
    (void)off;
    (void)src;
    (void)len;

    if (area->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        // TODO(ifu-phase1): Call stm32FlashWrite(area->fa_off + off, src, len).
        //   stm32FlashWrite() wraps the STM32 HAL FLASH_Program_* calls with unlock/lock.
        //   Reference: targets/ChibiOS/<BOARD>/common/WireProtocol_MonitorCommands.c
    }
    else
    {
        // TODO(ifu-phase1): ORGPAL_PALTHREE/PALX: call hal_lfs_prog_0() / hal_lfs_prog_1().
        //   F769I: call f_write() via FatFs for the SD card slot file.
    }

    return 0; // Stub: success
}

int flash_area_erase(const struct flash_area *area, uint32_t off, uint32_t len)
{
    (void)area;
    (void)off;
    (void)len;

    if (area->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        // TODO(ifu-phase1): Call stm32FlashErase(area->fa_off + off, len).
        //   Must align erase requests to sector boundaries per flash_area_get_sectors().
        //   Reference: targets/ChibiOS/<BOARD>/common/WireProtocol_MonitorCommands.c
    }
    else
    {
        // TODO(ifu-phase1): ORGPAL_PALTHREE/PALX: call hal_lfs_erase_0() / hal_lfs_erase_1().
        //   F769I: erase range via FatFs (overwrite with 0xFF blocks) for the SD card file.
    }

    return 0; // Stub: success
}

uint32_t flash_area_align(const struct flash_area *area)
{
    if (area->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        // TODO(ifu-phase1): Return SoC-specific write granularity.
        //   STM32F76xx: 4 (word-aligned writes via FLASH_CR_PSIZE_WORD)
        //   STM32F429:  4 (same; F429 also supports byte/halfword but word is safest)
        return 4;
    }
    else
    {
        // TODO(ifu-phase1): Return write granularity for the external storage device.
        //   AT25SF641 SPI flash (PALTHREE): 1 byte minimum page-program granularity.
        //   W25Q128 QSPI flash (PALX): 1 byte minimum page-program granularity.
        //   SD card FatFs (F769I): 512 bytes (block size); implement virtual-sector shim.
        return 1;
    }
}

uint8_t flash_area_erased_val(const struct flash_area *area)
{
    (void)area;
    // NOR flash (both internal STM32 and all external SPI/QSPI devices used by nanoFramework)
    // reads as 0xFF after erase.
    return 0xFF;
}

int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors)
{
    (void)fa_id;
    (void)count;
    (void)sectors;

    // TODO(ifu-phase1): Populate sectors[] with the actual erase sector geometry.
    //   Internal STM32F76xx: non-uniform sectors (16k/16k/16k/16k/64k/128k*11).
    //     The CLR primary slot (0x08008000, 960kB) spans: partial 64kB sector + eleven 128kB sectors.
    //   External AT25SF641 (PALTHREE): uniform 64kB erase blocks → 960kB/64kB = 15 blocks.
    //   External W25Q128 (PALX): uniform 64kB erase blocks → slot-size/64kB blocks.
    //   SD card (F769I): implement 4kB virtual sectors over FatFs file I/O.
    //
    // Example skeleton for uniform sectors:
    //   uint32_t sector_size = MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE;
    //   uint32_t area_size   = /* fa_size for fa_id */;
    //   uint32_t n           = area_size / sector_size;
    //   for (uint32_t i = 0; i < n && i < *count; i++) {
    //       sectors[i].fs_off  = i * sector_size;
    //       sectors[i].fs_size = sector_size;
    //   }
    //   *count = n;
    //   return 0;

    *count = 0;
    return 0; // Stub: success
}

int flash_area_id_from_multi_image_slot(int image_index, int slot)
{
    if (image_index == 0)
    {
        return (slot == 0) ? FLASH_AREA_IMAGE_0_PRIMARY : FLASH_AREA_IMAGE_0_SECONDARY;
    }
    else if (image_index == 1)
    {
        return (slot == 0) ? FLASH_AREA_IMAGE_1_PRIMARY : FLASH_AREA_IMAGE_1_SECONDARY;
    }
    return -1;
}

int flash_area_id_from_image_slot(int slot)
{
    return flash_area_id_from_multi_image_slot(0, slot);
}
