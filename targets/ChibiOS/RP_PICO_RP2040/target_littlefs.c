//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <target_littlefs.h>
#include <hal_littlefs.h>
#include <hal_flash.h>
#include <string.h>

// Reference to ChibiOS EFL driver instance
extern EFlashDriver EFLD1;

// Buffers required by littlefs_FS_Driver
uint8_t lfs_inputBuffer[RP2040_FLASH_PAGE_SIZE];
uint8_t lfs_outputBuffer[RP2040_FLASH_PAGE_SIZE];

int32_t lfs_inputBufferSize = RP2040_FLASH_PAGE_SIZE;
int32_t lfs_outputBufferSize = RP2040_FLASH_PAGE_SIZE;

// Convert a littlefs block+offset to a flash offset relative to XIP base
static inline flash_offset_t lfs_to_flash_offset(const struct lfs_config *c, lfs_block_t block, lfs_off_t off)
{
    return (flash_offset_t)((RP2040_LFS_BASE - RP2040_XIP_BASE) + (block * c->block_size) + off);
}

// target specific implementation of hal_lfs_sync
int32_t hal_lfs_sync_(const struct lfs_config *c)
{
    (void)c;

    __DSB();

    return 0;
}

// target specific implementation of hal_lfs_erase
int32_t hal_lfs_erase_0(const struct lfs_config *c, lfs_block_t block)
{
    flash_offset_t offset = lfs_to_flash_offset(c, block, 0);
    flash_sector_t sector = (flash_sector_t)(offset / RP2040_FLASH_SECTOR_SIZE);

    flash_error_t err = flashStartEraseSector(&EFLD1, sector);
    if (err != FLASH_NO_ERROR)
    {
        return LFS_ERR_IO;
    }

    // Wait for erase to complete (synchronous with ROM functions)
    uint32_t wait_time;
    do
    {
        err = flashQueryErase(&EFLD1, &wait_time);
        if (err == FLASH_ERROR_ERASE)
        {
            return LFS_ERR_IO;
        }
        if (wait_time > 0)
        {
            chThdSleepMilliseconds(wait_time);
        }
    } while (err == FLASH_BUSY_ERASING);

    return (err == FLASH_NO_ERROR) ? LFS_ERR_OK : LFS_ERR_IO;
}

// target specific implementation of hal_lfs_read
int32_t hal_lfs_read_0(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    // RP2040 flash is XIP memory-mapped, so we can read directly
    uint32_t addr = RP2040_LFS_BASE + (block * c->block_size) + off;
    memcpy(buffer, (const void *)addr, size);

    return LFS_ERR_OK;
}

// target specific implementation of hal_lfs_prog
int32_t hal_lfs_prog_0(
    const struct lfs_config *c,
    lfs_block_t block,
    lfs_off_t off,
    const void *buffer,
    lfs_size_t size)
{
    flash_offset_t offset = lfs_to_flash_offset(c, block, off);

    flash_error_t err = flashProgram(&EFLD1, offset, size, (const uint8_t *)buffer);

    return (err == FLASH_NO_ERROR) ? LFS_ERR_OK : LFS_ERR_IO;
}

// target specific implementation of chip erase
bool hal_lfs_erase_chip_0()
{
    flash_offset_t base_offset = (flash_offset_t)(RP2040_LFS_BASE - RP2040_XIP_BASE);
    uint32_t sector_count = RP2040_LFS_SIZE / RP2040_FLASH_SECTOR_SIZE;

    for (uint32_t i = 0; i < sector_count; i++)
    {
        flash_sector_t sector = (flash_sector_t)((base_offset / RP2040_FLASH_SECTOR_SIZE) + i);

        flash_error_t err = flashStartEraseSector(&EFLD1, sector);
        if (err != FLASH_NO_ERROR)
        {
            return false;
        }

        uint32_t wait_time;
        do
        {
            err = flashQueryErase(&EFLD1, &wait_time);
            if (err == FLASH_ERROR_ERASE)
            {
                return false;
            }
            if (wait_time > 0)
            {
                chThdSleepMilliseconds(wait_time);
            }
        } while (err == FLASH_BUSY_ERASING);

        if (err != FLASH_NO_ERROR)
        {
            return false;
        }
    }

    return true;
}

// Low-level initialization for the littlefs flash target.
// The EFL driver is already started by the block storage driver,
// so we just need to ensure it's running.
int8_t target_lfs_init()
{
    // Start the EFL driver if not already started
    eflStart(&EFLD1, NULL);

    return LFS_ERR_OK;
}
