//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot flash_area_* porting layer for the ChibiOS MCUboot bootloader
// binary on ST_STM32F769I_DISCOVERY (STM32F769NI).
//
// This file is the bootloader-context counterpart to:
//   targets/ChibiOS/ST_STM32F769I_DISCOVERY/common/mcuboot_flash_map.c
//
// Secondary slots (SD card via FatFs) are stubbed out: mcuboot_ext_flash_init()
// returns -1, and all external flash operations return an error. MCUboot treats
// the secondary slots as unavailable and boots the primary slot directly.
// Full FatFs integration is deferred to a later implementation step.

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "mcuboot_config.h"

#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"

#include "stm32_f7xx_flash.h"
#include "mcuboot_flash_layout.h"
#include "mcuboot_board_iface.h"

// Forward declarations for nf-overlay internal flash API (hal_stm32_flash.h).
int stm32FlashWrite(uint32_t startAddress, uint32_t length, const uint8_t *buffer);
int stm32FlashErase(uint32_t address);

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

// Board interface: SD card FatFs secondary slots — not yet integrated.
// Returns -1 (non-fatal); MCUboot continues and boots the primary slot.
int mcuboot_ext_flash_init(void)
{
    return -1;
}

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
        memcpy(dst, (const void *)(uintptr_t)(area->fa_off + off), len);
        return 0;
    }
    else
    {
        // SD card FatFs not yet integrated.
        return -1;
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
        // SD card FatFs not yet integrated.
        return -1;
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
        // SD card FatFs not yet integrated.
        return -1;
    }

    return 0;
}

uint32_t flash_area_align(const struct flash_area *area)
{
    if (area->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        return 4U;
    }
    else
    {
        return 1U;
    }
}

uint8_t flash_area_erased_val(const struct flash_area *area)
{
    (void)area;
    return 0xFFU;
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

int flash_area_get_sector(const struct flash_area *area, uint32_t off, struct flash_sector *sector)
{
    if (off >= area->fa_size)
    {
        return -1;
    }

    if (area->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        uint32_t addr = area->fa_off;
        uint32_t end = area->fa_off + area->fa_size;
        uint32_t target = area->fa_off + off;

        while (addr < end)
        {
            uint32_t sz = stm32_f7xx_get_sector_size(addr);
            if (sz == 0U)
            {
                return -1;
            }

            if (target >= addr && target < addr + sz)
            {
                sector->fs_off = addr - area->fa_off;
                sector->fs_size = sz;
                return 0;
            }

            addr += sz;
        }

        return -1;
    }
    else
    {
        uint32_t idx = off / MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE;
        sector->fs_off = idx * MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE;
        sector->fs_size = MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE;
        return 0;
    }
}
