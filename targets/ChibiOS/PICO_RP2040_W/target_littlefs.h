//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_LITTLEFS_H
#define TARGET_LITTLEFS_H

#include <hal.h>
#include <nanoHAL_v2.h>
#include <lfs.h>

// Single littlefs instance on internal flash
#define LITTLEFS_INSTANCES_COUNT 1

//////////////////////////////////
// RP2040 Internal Flash (W25Q16JV)
// 2 MB total, littlefs region: 0x101F8000 - 0x101FFFFF (32 KB)

// XIP base address
#define RP2040_XIP_BASE 0x10000000U

// littlefs partition start address (XIP-mapped)
#define RP2040_LFS_BASE 0x101F8000U

// littlefs partition size
#define RP2040_LFS_SIZE (32U * 1024U)

// Flash geometry
#define RP2040_FLASH_SECTOR_SIZE 0x1000  // 4 KB
#define RP2040_FLASH_PAGE_SIZE   0x100   // 256 B

//////////////////////////////////
// Remap into littlefs defines

#define LFS0_READ_SIZE      1
#define LFS0_PROG_SIZE      1
#define LFS0_BLOCK_SIZE     RP2040_FLASH_SECTOR_SIZE
#define LFS0_BLOCK_COUNT    (RP2040_LFS_SIZE / RP2040_FLASH_SECTOR_SIZE)
#define LFS0_BLOCK_CYCLES   100
#define LFS0_CACHE_SIZE     RP2040_FLASH_PAGE_SIZE
#define LFS0_LOOKAHEAD_SIZE (LFS0_BLOCK_COUNT / 8)

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
    int32_t hal_lfs_read_0(
        const struct lfs_config *c,
        lfs_block_t block,
        lfs_off_t off,
        void *buffer,
        lfs_size_t size);
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
