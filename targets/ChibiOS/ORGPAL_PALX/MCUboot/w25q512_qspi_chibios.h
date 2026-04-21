//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

/**
 * @file    w25q512_qspi_chibios.h
 * @brief   W25Q512 QSPI flash driver using ChibiOS WSPIv1 (QUADSPIv1 LLD).
 *
 * All operations run in 1-line (SPI-compatible) indirect mode to avoid the
 * quad-enable sequence and simplify initialisation.
 *
 * Write unit : 1 byte (byte-granular page-program); MCUboot write alignment = 4.
 * Erase unit : 64 kB block (command 0xD8) — matches MCUboot erase granularity.
 */

#ifndef W25Q512_QSPI_CHIBIOS_H
#define W25Q512_QSPI_CHIBIOS_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief   Initialise QUADSPI and the W25Q512 device via ChibiOS WSPI HAL.
     * @return  true on success (JEDEC ID verified), false otherwise.
     */
    bool w25q512_chibios_init(void);

    /**
     * @brief   Read @p len bytes from flash address @p addr into @p dst.
     * @return  true on success.
     */
    bool w25q512_chibios_read(uint8_t *dst, uint32_t addr, uint32_t len);

    /**
     * @brief   Write @p len bytes from @p src to erased flash address @p addr.
     *          Handles page boundaries automatically.
     * @return  true on success.
     */
    bool w25q512_chibios_write(const uint8_t *src, uint32_t addr, uint32_t len);

    /**
     * @brief   Erase the 64 kB block containing @p addr (@p addr must be 64 kB-aligned).
     * @return  true on success.
     */
    bool w25q512_chibios_erase(uint32_t addr);

#ifdef __cplusplus
}
#endif

#endif /* W25Q512_QSPI_CHIBIOS_H */
