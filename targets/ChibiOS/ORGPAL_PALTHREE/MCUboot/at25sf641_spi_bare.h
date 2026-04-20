//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Bare-metal AT25SF641 SPI flash driver for the standalone MCUboot bootloader.
//
// This driver configures SPI1 directly via CMSIS registers.
// It is intended exclusively for the MCUboot
// bootloader binary context on ORGPAL_PALTHREE (STM32F769ZI).
//
// Pin assignments (from ORGPAL_PALTHREE board.h):
//   SPI1 SCK  — GPIOA pin 5, AF5
//   SPI1 MISO — GPIOA pin 6, AF5
//   SPI1 MOSI — GPIOB pin 5, AF5
//   SPI1 CS   — GPIOI pin 15, GPIO output (active low, software controlled)
//
// Also configures:
//   GPIOJ pin 3 (SPI_FLASH_WP)   — set high (WP# = not write-protected)
//   GPIOJ pin 4 (SPI_FLASH_HOLD) — set high (HOLD# = not held)
//
// SPI1 configuration:
//   Master mode, CPOL=0, CPHA=0 (SPI mode 0), 8-bit frame, MSB-first.
//   APB2 clock = 16 MHz (HSI, no PLL in bootloader).
//   BR = 0b000 → fPCLK/2 = 8 MHz (well within AT25SF641's 100 MHz limit).
//
// Note: All transfers use polling (no DMA).  No cache management is needed
// because D-cache is not enabled in the MCUboot bootloader.

#ifndef AT25SF641_SPI_BARE_H
#define AT25SF641_SPI_BARE_H

#include <stdbool.h>
#include <stdint.h>

// AT25SF641 geometry constants (matches target_ext_flash.h from the ChibiOS driver).
#define AT25SF641_FLASH_SIZE     0x800000U // 8 MB
#define AT25SF641_SECTOR_SIZE    0x10000U  // 64 kB block (MCUboot erase unit)
#define AT25SF641_SUBSECTOR_SIZE 0x1000U   // 4 kB sub-sector (littlefs erase unit)
#define AT25SF641_PAGE_SIZE      0x100U    // 256 bytes (page-program unit)

// JEDEC ID bytes (AT25SF641 datasheet, ID Definitions table).
#define AT25SF641_MANUFACTURER_ID ((uint8_t)0x1F)
#define AT25SF641_DEVICE_ID1      ((uint8_t)0x32)
#define AT25SF641_DEVICE_ID2      ((uint8_t)0x17)

// Status Register 1 busy flag.
#define AT25SF641_SR_BUSY ((uint8_t)0x01)

#ifdef __cplusplus
extern "C"
{
#endif

    // Initialise SPI1 GPIO and peripheral, wake the AT25SF641 from deep power
    // down, and verify the JEDEC ID.
    // Returns true if the device responds with the expected JEDEC ID.
    // Must be called once from main() before any read/write/erase.
    bool at25sf641_bare_init(void);

    // Poll the AT25SF641 BUSY flag until the device is idle.
    // Returns true when idle, false on timeout (> ~4 s at 16 MHz HSI).
    bool at25sf641_bare_wait_ready(void);

    // Read len bytes from SPI flash at byte offset addr into buf.
    // Supports arbitrary length (not limited to one page).
    bool at25sf641_bare_read(uint8_t *buf, uint32_t addr, uint32_t len);

    // Write len bytes from buf to SPI flash starting at byte offset addr.
    // Handles page-boundary crossing transparently via page-program loops.
    // The destination region must already be erased.
    bool at25sf641_bare_write(const uint8_t *buf, uint32_t addr, uint32_t len);

    // Erase a block starting at byte offset addr.
    // large_block=true  → 64 kB block erase (0xD8), used by MCUboot.
    // large_block=false → 4 kB sub-sector erase (0x20), reserved for future use.
    // addr must be aligned to the chosen erase unit.
    bool at25sf641_bare_erase(uint32_t addr, bool large_block);

#ifdef __cplusplus
}
#endif

#endif // AT25SF641_SPI_BARE_H
