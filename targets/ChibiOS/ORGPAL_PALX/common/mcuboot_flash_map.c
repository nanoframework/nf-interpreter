//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot flash_area_* porting layer for ORGPAL_PALX (STM32F769).
//
// PRIMARY SLOTS (FLASH_DEVICE_INTERNAL_FLASH):
//   Uses the STM32 HAL flash driver (stm32FlashWrite / stm32FlashErase).
//   Internal flash is memory-mapped (XIP); reads use direct memcpy.
//
// SECONDARY SLOTS (FLASH_DEVICE_EXTERNAL_FLASH):
//   W25Q512 64 MB QSPI flash.
//   Accessed via W25Q512_Read / W25Q512_Write / W25Q512_Erase from
//   targets/ChibiOS/ORGPAL_PALX/common/target_ext_flash.c.
//
// Flash layout (from targets/ChibiOS/ORGPAL_PALX/MCUboot-flash-layout.md):
//   FLASH_AREA_BOOTLOADER        (0): 0x08000000, 32 kB  (sector 0)
//   FLASH_AREA_IMAGE_0_PRIMARY   (1): 0x08010000, 704 kB (sectors 2-6, bank 1)
//   FLASH_AREA_IMAGE_0_SECONDARY (2): W25Q512 @ 0x000000, 704 kB  (11 × 64 kB)
//   FLASH_AREA_IMAGE_1_PRIMARY   (4): 0x080C0000, 1280 kB (sector 7 + bank 2)
//   FLASH_AREA_IMAGE_1_SECONDARY (5): W25Q512 @ 0x0B0000, 1280 kB (20 × 64 kB)
//
// Sector geometry: STM32F76xx non-uniform sectors — see stm32_f7xx_flash.h.
// Write alignment: 4 bytes (FLASH_CR_PSIZE_WORD).

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"
#include "mcuboot_config.h"

// stm32_f7xx_flash.h provides sector geometry helpers (stm32_f7xx_get_sector_size,
// stm32_f7xx_next_sector_boundary). Forward-declare the HAL flash driver functions
// rather than including the full ChibiOS HAL chain (not available in nf_mcuboot_port scope).
#include "stm32_f7xx_flash.h"
int stm32FlashWrite(uint32_t startAddress, uint32_t length, const uint8_t *buffer);
int stm32FlashErase(uint32_t address);

#include "target_ext_flash.h"
#include "mcuboot_flash_layout.h"

// clang-format off
static const struct flash_area s_flash_areas[] = {
    { .fa_id = FLASH_AREA_BOOTLOADER,        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH, .fa_off = NF_MCUBOOT_SLOT_BOOTLOADER_OFF, .fa_size = NF_MCUBOOT_SLOT_BOOTLOADER_SIZE },
    { .fa_id = FLASH_AREA_IMAGE_0_PRIMARY,   .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH, .fa_off = NF_MCUBOOT_SLOT_IMG0_PRI_OFF,   .fa_size = NF_MCUBOOT_SLOT_IMG0_PRI_SIZE   },
    { .fa_id = FLASH_AREA_IMAGE_0_SECONDARY, .fa_device_id = FLASH_DEVICE_EXTERNAL_FLASH, .fa_off = NF_MCUBOOT_SLOT_IMG0_SEC_OFF,   .fa_size = NF_MCUBOOT_SLOT_IMG0_SEC_SIZE   },
    { .fa_id = FLASH_AREA_IMAGE_1_PRIMARY,   .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH, .fa_off = NF_MCUBOOT_SLOT_IMG1_PRI_OFF,   .fa_size = NF_MCUBOOT_SLOT_IMG1_PRI_SIZE   },
    { .fa_id = FLASH_AREA_IMAGE_1_SECONDARY, .fa_device_id = FLASH_DEVICE_EXTERNAL_FLASH, .fa_off = NF_MCUBOOT_SLOT_IMG1_SEC_OFF,   .fa_size = NF_MCUBOOT_SLOT_IMG1_SEC_SIZE   },
};
// clang-format on

#define FLASH_AREA_TABLE_COUNT (sizeof(s_flash_areas) / sizeof(s_flash_areas[0]))

static_assert(
    NF_MCUBOOT_SLOT_IMG1_SEC_SIZE / MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE <= MCUBOOT_MAX_IMG_SECTORS,
    "Deploy secondary sector count exceeds MCUBOOT_MAX_IMG_SECTORS");

int flash_area_open(uint8_t id, const struct flash_area **area_outp)
{
    for (uint32_t i = 0; i < FLASH_AREA_TABLE_COUNT; i++)
    {
        if (s_flash_areas[i].fa_id == id)
        {
            *area_outp = &s_flash_areas[i];
            return 0;
        }
    }
    return -1;
}

void flash_area_close(const struct flash_area *area)
{
    (void)area;
}

int flash_area_read(const struct flash_area *area, uint32_t off, void *dst, uint32_t len)
{
    if (area->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        memcpy(dst, (const void *)(area->fa_off + off), len);
        return 0;
    }
    else
    {
        return W25Q512_Read((uint8_t *)dst, area->fa_off + off, len) ? 0 : -1;
    }
}

int flash_area_write(const struct flash_area *area, uint32_t off, const void *src, uint32_t len)
{
    if (area->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        return stm32FlashWrite(area->fa_off + off, len, (const uint8_t *)src);
    }
    else
    {
        return W25Q512_Write((uint8_t *)src, area->fa_off + off, len) ? 0 : -1;
    }
}

int flash_area_erase(const struct flash_area *area, uint32_t off, uint32_t len)
{
    if (area->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        uint32_t erase_addr = area->fa_off + off;
        uint32_t end = erase_addr + len;

        while (erase_addr < end)
        {
            if (stm32FlashErase(erase_addr) != 0)
            {
                return -1;
            }
            erase_addr = stm32_f7xx_next_sector_boundary(erase_addr);
        }
    }
    else
    {
        uint32_t erase_addr = area->fa_off + off;
        uint32_t end = erase_addr + len;

        while (erase_addr < end)
        {
            if (!W25Q512_Erase(erase_addr, true))
            {
                return -1;
            }
            erase_addr += MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE;
        }
    }

    return 0;
}

uint32_t flash_area_align(const struct flash_area *area)
{
    if (area->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        // STM32F76xx FLASHv2: minimum write unit is 4 bytes (FLASH_CR_PSIZE_WORD).
        return 4U;
    }
    else
    {
        // W25Q512: byte-granular page-program.
        return 1U;
    }
}

uint8_t flash_area_erased_val(const struct flash_area *area)
{
    (void)area;
    return 0xFF;
}

int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors)
{
    const struct flash_area *fa = NULL;

    for (uint32_t i = 0; i < FLASH_AREA_TABLE_COUNT; i++)
    {
        if (s_flash_areas[i].fa_id == (uint8_t)fa_id)
        {
            fa = &s_flash_areas[i];
            break;
        }
    }

    if (fa == NULL)
    {
        return -1;
    }

    if (fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        uint32_t addr = fa->fa_off;
        uint32_t end = fa->fa_off + fa->fa_size;
        uint32_t n = 0U;
        uint32_t cap = *count;

        while (addr < end)
        {
            if (n >= cap)
            {
                return -1;
            }

            uint32_t sz = stm32_f7xx_get_sector_size(addr);
            if (sz == 0U)
            {
                return -1;
            }

            sectors[n].fs_off = addr - fa->fa_off;
            sectors[n].fs_size = sz;
            addr += sz;
            n++;
        }

        *count = n;
    }
    else
    {
        uint32_t n = fa->fa_size / MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE;

        if (n > *count)
        {
            return -1;
        }

        for (uint32_t i = 0U; i < n; i++)
        {
            sectors[i].fs_off = i * MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE;
            sectors[i].fs_size = MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE;
        }

        *count = n;
    }

    return 0;
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
