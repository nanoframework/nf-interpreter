//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// W25Q512 QSPI flash driver API for ORGPAL_PALX (STM32F769, QSPI).
//
// This header declares the shared W25Q512 API used by both the littlefs
// driver (target_littlefs.c) and the MCUboot flash map porting layer
// (common/mcuboot_flash_map.c).
//
// The implementation (common/target_ext_flash.c) uses ChibiOS QSPI and the
// STM32 HAL QSPI driver.  This header carries no ChibiOS dependencies and is
// safe to include from plain C translation units.

#ifndef TARGET_EXT_FLASH_H
#define TARGET_EXT_FLASH_H

#include <stdbool.h>
#include <stdint.h>

// W25Q512 geometry
// 512 Mbits => 64 MBytes
#define W25Q512_FLASH_SIZE   0x4000000U
// 1024 blocks of 64 kBytes (block-erase unit used by MCUboot)
#define W25Q512_BLOCK_SIZE   0x10000U
// 16384 sectors of 4 kBytes (minimum erase size, used by littlefs)
#define W25Q512_SECTOR_SIZE  0x1000U
// 262144 pages of 256 bytes (page-program unit)
#define W25Q512_PAGE_SIZE    0x100U

#ifdef __cplusplus
extern "C"
{
#endif

    // Configure MPU, initialise QSPI peripheral, reset flash device, enter QPI
    // mode, and verify JEDEC ID.  Must be called once before any other operation.
    // Returns true on success, false on any initialisation failure.
    bool W25Q512_Init(void);

    // Erase a region starting at byte offset addr.
    // use_64k=true  => 64 kB block erase   (0xD8, used by MCUboot).
    // use_64k=false => 4 kB sector erase   (0x20, used by littlefs).
    // addr must be aligned to the chosen erase unit.
    bool W25Q512_Erase(uint32_t addr, bool use_64k);

    // Read size bytes from flash at byte offset addr into buf.
    bool W25Q512_Read(uint8_t *buf, uint32_t addr, uint32_t size);

    // Write size bytes from buf to flash at byte offset addr.
    // Handles page-boundary crossing transparently.
    // The destination region must already be erased.
    bool W25Q512_Write(uint8_t *buf, uint32_t addr, uint32_t size);

    // Erase the entire W25Q512 chip in sector increments.
    // Resets the watchdog between sectors to prevent WDT expiry.
    bool W25Q512_EraseChip(void);

#ifdef __cplusplus
}
#endif

#endif // TARGET_EXT_FLASH_H
