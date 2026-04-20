//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Bare-metal STM32F7 FLASHv2 driver — no ChibiOS, no HAL dependencies.
//
// STM32F769ZI FLASHv2 register map (RM0410 Rev 5):
//   FLASH->KEYR   (0x40023C04)  — unlock key register
//   FLASH->SR     (0x40023C0C)  — status register
//   FLASH->CR     (0x40023C10)  — control register
//
// Unlock sequence: write FLASH_KEY1 then FLASH_KEY2 to KEYR.
// Programming:     set PSIZE=0b10 (word), set PG, write 4 bytes, wait BSY, clear PG.
// Sector erase:    set PSIZE=0b10, set SER + SNB field, set STRT, wait BSY, clear SER+SNB.
//
// SNB encoding for STM32F76x dual-bank (SNB is 5 bits, FLASH_CR[7:3]):
//   Bank 1 sectors 0-7:  SNB = sector index (0-7)
//   Bank 2 sectors 8-15: SNB = (sector index - 8) | 0x10  (bit 4 selects bank 2)
//
// All operations assume that the caller provides properly aligned addresses
// and that MCUboot has already validated that no write overlaps an unerased area.

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <stm32f769xx.h>

#include "stm32f7_flash_bare.h"
#include "stm32_f7xx_flash.h"

#define FLASH_KEY1 0x45670123U
#define FLASH_KEY2 0xCDEF89ABU

// PSIZE field value for 32-bit (word) programming.
// FLASH_CR PSIZE bits [9:8] = 0b10 → word (4 bytes).
#define FLASH_PSIZE_WORD (2U << 8U)
#define FLASH_CR_PSIZE_MASK (~(3U << 8U))

// FLASH_SR error bits (RM0410 Table 4).
// Note: RM0410 uses ERSERR (bit 7, erase sequence error) — CMSIS uses ERSERR_Msk.
// There is no RDERR bit in STM32F769 FLASH_SR.
#define FLASH_SR_ERRORS                                                                                                    \
    (FLASH_SR_ERSERR | FLASH_SR_PGPERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR | FLASH_SR_OPERR)

// ----------------------------------------------------------------------- //
// Internal helpers                                                         //
// ----------------------------------------------------------------------- //

static void flash_unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

static void flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

static bool flash_wait_ready(void)
{
    // Spin until BSY clears (no watchdog in bootloader context).
    while (FLASH->SR & FLASH_SR_BSY)
    {
        // busy wait
    }

    if (FLASH->SR & FLASH_SR_ERRORS)
    {
        // Clear all error flags.
        FLASH->SR = FLASH_SR_ERRORS;
        return false;
    }

    // Acknowledge EOP if set.
    if (FLASH->SR & FLASH_SR_EOP)
    {
        FLASH->SR = FLASH_SR_EOP;
    }

    return true;
}

// Returns the 5-bit SNB value for the sector containing addr, or 0xFF if unknown.
static uint8_t addr_to_snb(uint32_t addr)
{
    for (uint32_t i = 0; i < STM32_F7XX_SECTOR_COUNT; i++)
    {
        if (addr >= s_stm32_f7xx_sectors[i].start &&
            addr < s_stm32_f7xx_sectors[i].start + s_stm32_f7xx_sectors[i].size)
        {
            // Bank 1: sectors 0-7 → SNB = i
            // Bank 2: sectors 8-15 → SNB = (i - 8) | 0x10  (bit 4 = bank select)
            return (i < 8U) ? (uint8_t)i : (uint8_t)((i - 8U) | 0x10U);
        }
    }
    return 0xFFU;
}

// ----------------------------------------------------------------------- //
// Public API                                                               //
// ----------------------------------------------------------------------- //

void stm32f7_flash_init(void)
{
    // FLASHv2 requires no explicit initialisation after reset.
    // Instruction and data caches are enabled by default on STM32F7 after reset;
    // they are flushed automatically by the hardware on erase/write completion.
}

int stm32f7_flash_write(uint32_t addr, uint32_t len, const uint8_t *buf)
{
    // Caller guarantees addr and len are 4-byte aligned.
    flash_unlock();

    // Clear any pre-existing error flags.
    FLASH->SR = FLASH_SR_ERRORS;

    FLASH->CR = (FLASH->CR & FLASH_CR_PSIZE_MASK) | FLASH_PSIZE_WORD | FLASH_CR_PG;

    const uint32_t *src = (const uint32_t *)(const void *)buf;
    volatile uint32_t *dst = (volatile uint32_t *)addr;
    uint32_t words = len / 4U;

    for (uint32_t i = 0; i < words; i++)
    {
        *dst++ = *src++;
        __DSB();

        if (!flash_wait_ready())
        {
            FLASH->CR &= ~FLASH_CR_PG;
            flash_lock();
            return -1;
        }
    }

    FLASH->CR &= ~FLASH_CR_PG;
    flash_lock();
    return 0;
}

int stm32f7_flash_erase(uint32_t addr)
{
    uint8_t snb = addr_to_snb(addr);
    if (snb == 0xFFU)
    {
        return -1;
    }

    flash_unlock();

    // Clear any pre-existing error flags.
    FLASH->SR = FLASH_SR_ERRORS;

    // Configure sector erase: PSIZE=word, SER=1, SNB=<sector>.
    uint32_t cr = FLASH->CR;
    cr &= FLASH_CR_PSIZE_MASK;
    cr &= ~FLASH_CR_SNB_Msk;
    cr |= FLASH_PSIZE_WORD | FLASH_CR_SER | ((uint32_t)snb << FLASH_CR_SNB_Pos);
    FLASH->CR = cr;

    // Start erase.
    FLASH->CR |= FLASH_CR_STRT;
    __DSB();

    bool ok = flash_wait_ready();

    // Clear SER and SNB.
    FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB_Msk);
    flash_lock();

    return ok ? 0 : -1;
}
