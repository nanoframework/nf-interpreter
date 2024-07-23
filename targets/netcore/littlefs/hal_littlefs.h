//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_LITTLEFS_H
#define HAL_LITTLEFS_H

// #include <hal.h>
// #include <ch.h>
// #include <hal_nf_community.h>
// #include <nanoCLR_Headers.h>
#include <target_littlefs.h>
#include <lfs.h>
// #include <nanoHAL_v2.h>
#include <nanoCLR_Headers.h>

#define NANO_LITTLEFS_ATTRIBUTE      0
#define NANO_LITTLEFS_ATTRIBUTE_SIZE (sizeof(uint32_t))

#ifdef __cplusplus
extern "C"
{
#endif

    extern lfs_t lfs[LITTLEFS_INSTANCES_COUNT];
    extern struct lfs_config lfsConfig[LITTLEFS_INSTANCES_COUNT];

    // declarations of hal/target implementations
    int8_t target_lfs_init();
    void hal_lfs_config();

     //int hal_lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
     //int hal_lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
     //int hal_lfs_erase(const struct lfs_config *c, lfs_block_t block);
     int hal_lfs_sync(const struct lfs_config *c);

    // int hal_lfs_lock(const struct lfs_config *c);
    // int hal_lfs_unlock(const struct lfs_config *c);

    void hal_lfs_mount();
    int32_t hal_lfs_mount_partition(int32_t index, bool forceFormat);

    lfs_t *hal_lfs_get_fs_from_index(int32_t index);
    // int32_t hal_lfs_get_instances_count();

#ifdef __cplusplus
}
#endif

#endif // HAL_LITTLEFS_H
