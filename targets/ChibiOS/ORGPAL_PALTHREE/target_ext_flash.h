//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// AT25SF641 SPI flash driver API for ORGPAL_PALTHREE (STM32F769ZI, SPI1).
//
// This header declares the shared AT25SF641 API used by both the littlefs
// driver (target_littlefs.c) and the MCUboot flash map porting layer
// (common/mcuboot_flash_map.c).
//
// The implementation (common/target_ext_flash.c) uses ChibiOS SPI.  This
// header carries no ChibiOS dependencies and is safe to include from plain C
// translation units.

#ifndef TARGET_EXT_FLASH_H
#define TARGET_EXT_FLASH_H

#include <stdbool.h>
#include <stdint.h>

// Timeout for flash read / write / erase operations (4 s).
// Covers the worst-case timing for write and erase (except chip erase).
#define HAL_SPI_TIMEOUT_DEFAULT_VALUE ((uint32_t)4000)

// AT25SF641 geometry
// 64 Mbits => 8 MByte
#define AT25SF641_FLASH_SIZE     0x800000U
// 128 sectors of 64 kBytes (block-erase unit used by MCUboot)
#define AT25SF641_SECTOR_SIZE    0x10000U
// 2048 subsectors of 4 kBytes (minimum erase size, used by littlefs)
#define AT25SF641_SUBSECTOR_SIZE 0x1000U
// 32768 pages of 256 bytes (page-program unit)
#define AT25SF641_PAGE_SIZE      0x100U

// AT25SF641 device IDs (from AT25SF641 datasheet, ID Definitions table)
#define AT25SF641_MANUFACTURER_ID ((uint8_t)0x1F)
#define AT25SF641_DEVICE_ID1      ((uint8_t)0x32)
#define AT25SF641_DEVICE_ID2      ((uint8_t)0x17)

// AT25SF641 Status Register 1 bits
#define AT25SF641_SR_BUSY ((uint8_t)0x01)

#ifdef __cplusplus
extern "C"
{
#endif

    // Wake device from deep power down and verify JEDEC ID.
    // Must be called once after spiStart() before any other operation.
    // Returns true on success (JEDEC ID matched), false on timeout or ID mismatch.
    bool AT25SF641_Init(void);

    // Poll the BUSY flag until the device is idle or the timeout expires.
    // Returns true when the device becomes ready, false on timeout.
    bool AT25SF641_WaitReady(void);

    // Erase a block starting at byte offset addr.
    // large_block=true  => 64 kB block erase   (0xD8, used by MCUboot).
    // large_block=false => 4 kB sub-sector erase (0x20, used by littlefs).
    // addr must be aligned to the chosen erase unit.
    bool AT25SF641_Erase(uint32_t addr, bool large_block);

    // Read size bytes from flash at byte offset addr into buf.
    // Handles reads larger than one page by looping through the DMA buffer.
    bool AT25SF641_Read(uint8_t *buf, uint32_t addr, uint32_t size);

    // Write size bytes from buf to flash at byte offset addr.
    // Handles page-boundary crossing transparently via page-program loops.
    // The destination region must already be erased.
    bool AT25SF641_Write(const uint8_t *buf, uint32_t addr, uint32_t size);

    // Erase the entire AT25SF641 chip in 64 kB block increments.
    // Resets the watchdog between blocks to prevent WDT expiry during long erases.
    bool AT25SF641_EraseChip(void);

#ifdef __cplusplus
}
#endif

#endif // TARGET_EXT_FLASH_H
