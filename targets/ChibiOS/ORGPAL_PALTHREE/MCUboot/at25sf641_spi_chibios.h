//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// at25sf641_spi_chibios.h — AT25SF641 8 MB SPI flash driver using ChibiOS SPIv2 HAL.
//
// This header presents the same API as the former at25sf641_spi_bare.h so that
// mcuboot_flash_map_boot.c requires only an include-path change.

#ifndef AT25SF641_SPI_CHIBIOS_H
#define AT25SF641_SPI_CHIBIOS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialise SPI1 and the AT25SF641 device.
// Returns true on success (device responds with the correct JEDEC ID).
bool at25sf641_bare_init(void);

// Read len bytes from the flash at the given byte address into dst.
bool at25sf641_bare_read(uint8_t *dst, uint32_t addr, uint32_t len);

// Write len bytes from src to the flash starting at the given byte address.
// The caller is responsible for erasing the target sector first.
bool at25sf641_bare_write(const uint8_t *src, uint32_t addr, uint32_t len);

// Erase the 4 kB sector (block4k=true) or 64 kB block (block4k=false)
// that contains addr.
bool at25sf641_bare_erase(uint32_t addr, bool block4k);

#ifdef __cplusplus
}
#endif

#endif /* AT25SF641_SPI_CHIBIOS_H */
