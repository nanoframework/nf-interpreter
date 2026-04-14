//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_LITTLEFS_H
#define TARGET_LITTLEFS_H

#include <hal.h>
#include <cache.h>
#include <nanoHAL_v2.h>
#include <lfs.h>

// need to define how many litllefs instances we'll be running
#define LITTLEFS_INSTANCES_COUNT 1

#define LFS_DRIVER_QSPI (1)

#include "target_ext_flash.h"

//////////////////////////////////
// remapping into littlefs defines

#define LFS0_READ_SIZE      1
#define LFS0_PROG_SIZE      1
#define LFS0_BLOCK_SIZE     W25Q512_SECTOR_SIZE
#define LFS0_BLOCK_COUNT    W25Q512_FLASH_SIZE / W25Q512_SECTOR_SIZE
#define LFS0_BLOCK_CYCLES   100
#define LFS0_CACHE_SIZE     W25Q512_PAGE_SIZE
#define LFS0_LOOKAHEAD_SIZE LFS0_BLOCK_COUNT / 8

#define LFS0_READ_HANDLER  hal_lfs_read_0
#define LFS0_PROG_HANDLER  hal_lfs_prog_0
#define LFS0_ERASE_HANDLER hal_lfs_erase_0
#define LFS0_SYNC_HANDLER  hal_lfs_sync_

#ifdef __cplusplus
extern "C"
{
#endif

    bool hal_lfs_erase_chip_0();
    int32_t hal_lfs_sync_(const struct lfs_config *c);
    int32_t hal_lfs_erase_0(const struct lfs_config *c, lfs_block_t block);
    int32_t hal_lfs_read_0(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
    int32_t hal_lfs_prog_0(
        const struct lfs_config *c,
        lfs_block_t block,
        lfs_off_t off,
        const void *buffer,
        lfs_size_t size);

#ifdef __cplusplus
}
#endif

#endif // TARGET_LITTLEFS_H
