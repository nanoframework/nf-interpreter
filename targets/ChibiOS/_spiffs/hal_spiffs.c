//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "hal_spiffs.h"

mutex_t spiffs_mutex[SPIFFS_INSTANCES_COUNT];

spiffs fs[SPIFFS_INSTANCES_COUNT];
spiffs_config spiffs_cfg[SPIFFS_INSTANCES_COUNT];
bool spiffsFileSystemReady;

#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t spiffs_work_buffer[SPIFFS_INSTANCES_COUNT][SPIFFS_WORK_BUFFER_SIZE];

#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t spiffs_fd_space[SPIFFS_INSTANCES_COUNT][SPIFFS_FILE_DESCRIPTORS_SPACE];

#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t spiffs_cache[SPIFFS_INSTANCES_COUNT][SPIFFS_CACHE_SIZE];

// initialization of SPIFFS: configurations, data structures, drivers and lock
uint8_t hal_spiffs_config()
{
    uint8_t mountResult;

    // low level hardware and drivers initializations
    target_spiffs_init();

    for (int i = 0; i < SPIFFS_INSTANCES_COUNT; i++)
    {
        // init mutexes
        osalMutexObjectInit(&spiffs_mutex[i]);

        // setup SPIFFS configurations

        spiffs_cfg[i].phys_size = hal_spiffs_get_totalsize(i);
        spiffs_cfg[i].phys_addr = 0;
        spiffs_cfg[i].phys_erase_block = hal_spiffs_get_eraseblocksize(i);
        spiffs_cfg[i].log_block_size = hal_spiffs_get_logicalblocksize(i);
        spiffs_cfg[i].log_page_size = SPIFFS_LOGICAL_PAGE_SIZE;

        // setup pointers to HAL functions
        if (i == 0)
        {
            spiffs_cfg[0].hal_erase_f = hal_spiffs_erase_0;
            spiffs_cfg[0].hal_read_f = hal_spiffs_read_0;
            spiffs_cfg[0].hal_write_f = hal_spiffs_write_0;
        }
        else if (i == 1)
        {
            spiffs_cfg[1].hal_erase_f = hal_spiffs_erase_1;
            spiffs_cfg[1].hal_read_f = hal_spiffs_read_1;
            spiffs_cfg[1].hal_write_f = hal_spiffs_write_1;
        }

        mountResult = SPIFFS_mount(
            &fs[i],
            &spiffs_cfg[i],
            spiffs_work_buffer[i],
            spiffs_fd_space[i],
            SPIFFS_FILE_DESCRIPTORS_SPACE,
            spiffs_cache[i],
            SPIFFS_CACHE_SIZE,
            0);

        if (mountResult != SPIFFS_OK && SPIFFS_errno(&fs[i]) == SPIFFS_ERR_NOT_A_FS)
        {
            // looks like SPIFFS is not formated

            // need to unmount 1st
            SPIFFS_unmount(&fs[i]);

            // now format
            if (SPIFFS_format(&fs[i]) != SPIFFS_OK)
            {
                return -1;
            }

            // finally try mounting it again
            mountResult = SPIFFS_mount(
                &fs[i],
                &spiffs_cfg[i],
                spiffs_work_buffer[i],
                spiffs_fd_space[i],
                SPIFFS_FILE_DESCRIPTORS_SPACE,
                spiffs_cache,
                SPIFFS_CACHE_SIZE,
                0);
        }

#if !defined(BUILD_RTM)

        ASSERT(mountResult == SPIFFS_OK);

#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // // code block to assist testing SPIFFS
    // char writeBuf[] = {"Hello! if you get this message, congratulations, that's because SPIFFS is working on your
    // device!!"}; char readBuf[sizeof(writeBuf)];

    // spiffs_file fd = SPIFFS_open(&fs, "file1.txt", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
    // if (SPIFFS_write(&fs, fd, writeBuf, sizeof(writeBuf)) < 0)
    // {
    //     return -1;
    // }

    // SPIFFS_close(&fs, fd);
    // fd = SPIFFS_open(&fs, "file1.txt", SPIFFS_RDWR, 0);
    // if (SPIFFS_read(&fs, fd, readBuf, sizeof(readBuf)) < 0)
    // {
    //     return -1;
    // }

    // SPIFFS_close(&fs, fd);

    // uint32_t total = 0;
    // uint32_t used_space = 0;
    // SPIFFS_info(&fs, &total, &used_space);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // mirror SPIFFS mount result into global flag
    spiffsFileSystemReady = (mountResult == SPIFFS_OK);

    return mountResult;
}

void hal_spiffs_lock(spiffs *fsInstance)
{
    osalMutexLock(&spiffs_mutex[hal_spiffs_get_fs_index(fsInstance)]);
}

void hal_spiffs_unlock(spiffs *fsInstance)
{
    osalMutexUnlock(&spiffs_mutex[hal_spiffs_get_fs_index(fsInstance)]);
}

uint32_t hal_spiffs_get_totalsize(int spiffsIndex)
{
    switch (spiffsIndex)
    {
        case 0:
            return SPIFFS0_TOTAL_SIZE;
            break;

#if SPIFFS_INSTANCES_COUNT > 0
        case 1:
            return SPIFFS1_TOTAL_SIZE;
            break;
#endif

        default:
            return 0;
    }
}

uint32_t hal_spiffs_get_eraseblocksize(int spiffsIndex)
{
    switch (spiffsIndex)
    {
        case 0:
            return SPIFFS0_ERASE_BLOCK_SIZE;
            break;

#if SPIFFS_INSTANCES_COUNT > 0
        case 1:
            return SPIFFS1_ERASE_BLOCK_SIZE;
            break;
#endif

        default:
            return 0;
    }
}

uint32_t hal_spiffs_get_logicalblocksize(int spiffsIndex)
{
    switch (spiffsIndex)
    {
        case 0:
            return SPIFFS0_LOGICAL_BLOCK_SIZE;
            break;

#if SPIFFS_INSTANCES_COUNT > 0
        case 1:
            return SPIFFS0_LOGICAL_BLOCK_SIZE;
            break;
#endif

        default:
            return 0;
    }
}

int32_t hal_spiffs_get_fs_index(spiffs *fsInstance)
{
    if (fsInstance == &fs[0])
    {
        return 0;
    }
#if SPIFFS_INSTANCES_COUNT > 0
    if (fsInstance == &fs[1])
    {
        return 1;
    }
#endif
    else
    {
        HAL_AssertEx();
        return -1;
    }
}

spiffs *hal_spiffs_get_fs_from_index(int32_t index)
{
    return &fs[index];
}

int32_t hal_spiffs_get_instances_count()
{
    // there is at least one
    int32_t count = 1;

#if SPIFFS_INSTANCES_COUNT > 0
    count++;
#endif

    return count;
}
