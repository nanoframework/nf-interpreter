//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// STM32F76xx flash sector geometry — shared by all ChibiOS targets based on
// the STM32F76xx MCU family (ORGPAL_PALTHREE, ORGPAL_PALX, ST_STM32F769I_DISCOVERY).
//
// The STM32F76xx has two 1 MB banks with non-uniform sector sizes:
//   Sectors 0-3: 32 kB each
//   Sector 4:    128 kB
//   Sectors 5-7: 256 kB each
//   Bank 2 mirrors bank 1 starting at 0x08100000.
//
// Include this header from the board-level flash_map_backend.c to obtain the
// sector geometry table and the helper functions used by flash_area_erase and
// flash_area_get_sectors.

#ifndef STM32_F7XX_FLASH_H
#define STM32_F7XX_FLASH_H

#include <stdint.h>

// Sector table: one entry per erasable sector across both banks.
static const struct
{
    uint32_t start;
    uint32_t size;
} s_stm32_f7xx_sectors[] = {
    // clang-format off
    // Bank 1
    { 0x08000000U, 32U  * 1024U },   // sector 0
    { 0x08008000U, 32U  * 1024U },   // sector 1
    { 0x08010000U, 32U  * 1024U },   // sector 2
    { 0x08018000U, 32U  * 1024U },   // sector 3
    { 0x08020000U, 128U * 1024U },   // sector 4
    { 0x08040000U, 256U * 1024U },   // sector 5
    { 0x08080000U, 256U * 1024U },   // sector 6
    { 0x080C0000U, 256U * 1024U },   // sector 7
    // Bank 2 (same geometry, base + 0x100000)
    { 0x08100000U, 32U  * 1024U },   // sector 8
    { 0x08108000U, 32U  * 1024U },   // sector 9
    { 0x08110000U, 32U  * 1024U },   // sector 10
    { 0x08118000U, 32U  * 1024U },   // sector 11
    { 0x08120000U, 128U * 1024U },   // sector 12
    { 0x08140000U, 256U * 1024U },   // sector 13
    { 0x08180000U, 256U * 1024U },   // sector 14
    { 0x081C0000U, 256U * 1024U },   // sector 15
    // clang-format on
};

#define STM32_F7XX_SECTOR_COUNT (sizeof(s_stm32_f7xx_sectors) / sizeof(s_stm32_f7xx_sectors[0]))

// Returns the size in bytes of the STM32F76xx sector that contains addr.
// Returns 0 if addr is outside the known flash map.
static inline uint32_t stm32_f7xx_get_sector_size(uint32_t addr)
{
    for (uint32_t i = 0; i < STM32_F7XX_SECTOR_COUNT; i++)
    {
        if (addr >= s_stm32_f7xx_sectors[i].start &&
            addr < s_stm32_f7xx_sectors[i].start + s_stm32_f7xx_sectors[i].size)
        {
            return s_stm32_f7xx_sectors[i].size;
        }
    }
    return 0U;
}

// Returns the first address past the end of the STM32F76xx sector containing addr.
static inline uint32_t stm32_f7xx_next_sector_boundary(uint32_t addr)
{
    for (uint32_t i = 0; i < STM32_F7XX_SECTOR_COUNT; i++)
    {
        if (addr >= s_stm32_f7xx_sectors[i].start &&
            addr < s_stm32_f7xx_sectors[i].start + s_stm32_f7xx_sectors[i].size)
        {
            return s_stm32_f7xx_sectors[i].start + s_stm32_f7xx_sectors[i].size;
        }
    }
    return addr; // should never be reached
}

#endif // STM32_F7XX_FLASH_H
