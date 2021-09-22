//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_SPIFFS_H
#define HAL_SPIFFS_H

#include <hal.h>
#include <hal_nf_community.h>
#include <nanoCLR_Headers.h>
#include <target_spiffs.h>
#include <spiffs.h>
#include <nanoHAL_v2.h>

////////////////////////////////////
// buffers configuration, if not defined at target level
#ifndef SPIFFS_LOGICAL_PAGE_SIZE
#define SPIFFS_LOGICAL_PAGE_SIZE (256)
#endif

#ifndef SPIFFS_WORK_BUFFER_SIZE
#define SPIFFS_WORK_BUFFER_SIZE (SPIFFS_LOGICAL_PAGE_SIZE * 2)
#endif

#ifndef SPIFFS_FILE_DESCRIPTORS_SPACE
#define SPIFFS_FILE_DESCRIPTORS_SPACE (32 * 2)
#endif

#ifndef SPIFFS_CACHE_SIZE
#define SPIFFS_CACHE_SIZE ((SPIFFS_LOGICAL_PAGE_SIZE + 32) * 4)
#endif

#if SPIFFS_SINGLETON

// we're running a SINGLETON implementation, set
#define SPIFFS_INSTANCES_COUNT 1

#else

// instance count has to be defined at target level
#ifndef SPIFFS_INSTANCES_COUNT
#error "Count of SPIFFS instances wasn't defined at target level. Please check target_spiffs.h."
#endif

#endif // SPIFFS_SINGLETON

#define SPIFFS_TOTAL_SIZE(index)         SPIFFS##index##_TOTAL_SIZE
#define SPIFFS_ERASE_BLOCK_SIZE(index)   SPIFFS##index##_ERASE_BLOCK_SIZE
#define SPIFFS_LOGICAL_BLOCK_SIZE(index) SPIFFS##index##_LOGICAL_BLOCK_SIZE

#ifdef __cplusplus
extern "C"
{
#endif

#define SPIFFS_SUCCESS (0)
#define SPIFFS_ERROR   (-1)

    extern spiffs fs[SPIFFS_INSTANCES_COUNT];
    extern spiffs_config spiffs_cfg[SPIFFS_INSTANCES_COUNT];
    extern bool spiffsFileSystemReady;

    uint8_t target_spiffs_init();
    uint32_t hal_spiffs_get_totalsize(int spiffsIndex);
    uint32_t hal_spiffs_get_eraseblocksize(int spiffsIndex);
    uint32_t hal_spiffs_get_logicalblocksize(int spiffsIndex);
    int32_t hal_spiffs_get_fs_index(spiffs *fsInstance);
    spiffs *hal_spiffs_get_fs_from_index(int32_t index);
    int32_t hal_spiffs_get_instances_count();

    // declarations of hal/target implementations
    uint8_t hal_spiffs_config();

#if SPIFFS_SINGLETON
    s32_t hal_spiffs_erase(u32_t addr, u32_t size);
    s32_t hal_spiffs_read(u32_t addr, u32_t size, u8_t *dst);
    s32_t hal_spiffs_write(u32_t addr, u32_t size, u8_t *src);
#else
s32_t hal_spiffs_erase_0(u32_t addr, u32_t size);
s32_t hal_spiffs_read_0(u32_t addr, u32_t size, u8_t *dst);
s32_t hal_spiffs_write_0(u32_t addr, u32_t size, u8_t *src);
#if SPIFFS_INSTANCES_COUNT > 0
s32_t hal_spiffs_erase_1(u32_t addr, u32_t size);
s32_t hal_spiffs_read_1(u32_t addr, u32_t size, u8_t *dst);
s32_t hal_spiffs_write_1(u32_t addr, u32_t size, u8_t *src);
#endif

#endif // SPIFFS_SINGLETON

#ifdef __cplusplus
}
#endif

#endif // HAL_SPIFFS_H
