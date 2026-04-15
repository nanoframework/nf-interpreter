//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Bare-metal W25Q512 QSPI flash driver for the standalone MCUboot bootloader
//
// This driver is intended exclusively for the MCUboot bootloader binary
//
// Hardware: W25Q512 64 MB QSPI flash on the STM32F769 QUADSPI peripheral.
// Pin mapping:
//   NCS  = PB6  AF10   (chip select, active low)
//   CLK  = PF10 AF9    (SCLK)
//   D0   = PD11 AF9    (SIO0 / MOSI)
//   D1   = PD12 AF9    (SIO1 / MISO)
//   D2   = PE2  AF9    (SIO2 / /WP)
//   D3   = PD13 AF9    (SIO3 / /HOLD)
//
// All flash operations use 1-line (SPI-compatible) indirect mode.
// This avoids the quad-enable sequence and simplifies initialisation while
// remaining sufficient for the MCUboot bootloader (no throughput requirement).
//
// Write unit: 1 byte (byte-granular page-program); MCUboot write alignment = 4.
// Erase unit: 64 kB block (BLOCK_ERASE_CMD 0xD8) — matches MCUboot erase granularity.
//
// Clock: HCLK/2 = 8 MHz at 16 MHz HSI (prescaler=1 in QUADSPI CR).
// Addressing: 24-bit (3-byte) — all MCUboot slot offsets are well below 16 MB.

#ifndef W25Q512_QSPI_BARE_H
#define W25Q512_QSPI_BARE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // Initialise the QUADSPI peripheral and the W25Q512 device.
    // Configures GPIO alternate functions, enables RCC clocks, resets the
    // device and verifies the JEDEC ID.
    // Returns true on success (JEDEC ID verified), false if device not found.
    bool w25q512_bare_init(void);

    // Read len bytes from flash address addr into dst.
    // addr is the flash offset (0 = first byte of the device).
    // Returns true on success.
    bool w25q512_bare_read(uint8_t *dst, uint32_t addr, uint32_t len);

    // Write len bytes from src to flash address addr.
    // The target region must be erased before calling.
    // Handles page boundaries automatically.
    // Returns true on success.
    bool w25q512_bare_write(const uint8_t *src, uint32_t addr, uint32_t len);

    // Erase the 64 kB block containing addr (addr must be 64 kB-aligned).
    // Returns true on success.
    bool w25q512_bare_erase(uint32_t addr);

#ifdef __cplusplus
}
#endif

#endif // W25Q512_QSPI_BARE_H
