//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "hal_littlefs.h"

mutex_t lfs_mutex[LITTLEFS_INSTANCES_COUNT];

//bool lfsFileSystemReady;
lfs_t lfs[LITTLEFS_INSTANCES_COUNT];
struct lfs_config lfsConfig[LITTLEFS_INSTANCES_COUNT];
int8_t lfsInstanceIndex[LITTLEFS_INSTANCES_COUNT];

void *hal_lfs_getReadHandler(int32_t index)
{
    switch (index)
    {
        case 0:
            return &LFS0_READ_HANDLER;
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            return &LFS1_READ_HANDLER;
            break;
#endif

        default:
            return NULL;
    }
}

void *hal_lfs_getProgHandler(int32_t index)
{
    switch (index)
    {
        case 0:
            return &LFS0_PROG_HANDLER;
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            return &LFS1_PROG_HANDLER;
            break;
#endif

        default:
            return NULL;
    }
}

void *hal_lfs_getEraseHandler(int32_t index)
{
    switch (index)
    {
        case 0:
            return &LFS0_ERASE_HANDLER;
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            return &LFS1_ERASE_HANDLER;
            break;
#endif

        default:
            return NULL;
    }
}

void *hal_lfs_getSyncHandler(int32_t index)
{
    switch (index)
    {
        case 0:
            return &LFS0_SYNC_HANDLER;
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            return &LFS1_SYNC_HANDLER;
            break;
#endif

        default:
            return NULL;
    }
}

uint32_t hal_lfs_getReadSize(int32_t index)
{
    switch (index)
    {
        case 0:
            return LFS0_READ_SIZE;
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            return LFS1_READ_SIZE;
            break;
#endif

        default:
            return 0;
    }
}

uint32_t hal_lfs_getProgSize(int32_t index)
{
    switch (index)
    {
        case 0:
            return LFS0_PROG_SIZE;
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            return LFS1_PROG_SIZE;
            break;
#endif

        default:
            return 0;
    }
}

uint32_t hal_lfs_getBlockSize(int32_t index)
{
    switch (index)
    {
        case 0:
            return LFS0_BLOCK_SIZE;
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            return LFS1_BLOCK_SIZE;
            break;
#endif

        default:
            return 0;
    }
}

uint32_t hal_lfs_getBlockCount(int32_t index)
{
    switch (index)
    {
        case 0:
            return LFS0_BLOCK_COUNT;
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            return LFS1_BLOCK_COUNT;
            break;
#endif

        default:
            return 0;
    }
}

uint32_t hal_lfs_getBlockCycles(int32_t index)
{
    switch (index)
    {
#ifdef LFS0_BLOCK_CYCLES
        case 0:
            return LFS0_BLOCK_CYCLES;
            break;
#endif

#if LITTLEFS_INSTANCES_COUNT > 1 && defined(LFS1_BLOCK_CYCLES)
        case 1:
            return LFS1_BLOCK_CYCLES;
            break;
#endif

        default:
            // defaulting to 100
            return 100;
    }
}

uint32_t hal_lfs_getCacheSize(int32_t index)
{
    switch (index)
    {
#ifdef LFS0_CACHE_SIZE
        case 0:
            return LFS0_CACHE_SIZE;
            break;
#endif

#if LITTLEFS_INSTANCES_COUNT > 1 && defined(LFS1_CACHE_SIZE)
        case 1:
            return LFS1_CACHE_SIZE;
            break;
#endif

        default:
            // defaulting to 16
            return 16;
    }
}

uint32_t hal_lfs_getLookaheadSize(int32_t index)
{
    switch (index)
    {
#ifdef LFS0_LOOKAHEAD_SIZE
        case 0:
            return LFS0_LOOKAHEAD_SIZE;
            break;
#endif

#if LITTLEFS_INSTANCES_COUNT > 1 && defined(LFS1_LOOKAHEAD_SIZE)
        case 1:
            return LFS1_LOOKAHEAD_SIZE;
            break;
#endif

        default:
            // defaulting to 16
            return 16;
    }
}

uint8_t *hal_lfs_getReadBuffer(int32_t index)
{
    switch (index)
    {
        case 0:
            return LFS0_WRITE_BUFFER;
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            return LFS1_WRITE_BUFFER;
            break;
#endif

        default:
            return NULL;
    }
}

uint8_t *hal_lfs_getProgBuffer(int32_t index)
{
    switch (index)
    {
        case 0:
            return LFS0_READ_BUFFER;
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            return LFS1_READ_BUFFER;
            break;
#endif

        default:
            return NULL;
    }
}

uint8_t *hal_lfs_getLookaheadBuffer(int32_t index)
{
    switch (index)
    {
        case 0:
            return LFS0_LOOKAHEAD_BUFFER;
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            return LFS1_LOOKAHEAD_BUFFER;
            break;
#endif

        default:
            return NULL;
    }
}

// initialization of littlefs: configurations, data structures, drivers and lock
void hal_lfs_config()
{
    // clear struts for all instances
    memset(&lfs, 0, sizeof(lfs));
    memset(&lfsConfig, 0, sizeof(lfsConfig));

    // low level hardware and drivers initializations
    target_lfs_init();

    for (int i = 0; i < LITTLEFS_INSTANCES_COUNT; i++)
    {
        // init mutexes
#if CH_CFG_USE_MUTEXES
        chMtxObjectInit(&lfs_mutex[i]);
#elif CH_CFG_USE_SEMAPHORES
        chSemObjectInit((semaphore_t *)&lfs_mutex[i], 1);
#endif

        // setup littlefs handlers
        lfsConfig[i].read = hal_lfs_getReadHandler(i);
        lfsConfig[i].prog = hal_lfs_getProgHandler(i);
        lfsConfig[i].erase = hal_lfs_getEraseHandler(i);
        lfsConfig[i].sync = hal_lfs_getSyncHandler(i);
        lfsConfig[i].lock = &hal_lfs_lock;
        lfsConfig[i].unlock = &hal_lfs_unlock;

        // setup littlefs configurations
        lfsConfig[i].read_size = hal_lfs_getReadSize(i);
        lfsConfig[i].prog_size = hal_lfs_getProgSize(i);
        lfsConfig[i].block_size = hal_lfs_getBlockSize(i);
        lfsConfig[i].block_count = hal_lfs_getBlockCount(i);
        lfsConfig[i].block_cycles = hal_lfs_getBlockCycles(i);
        lfsConfig[i].cache_size = hal_lfs_getCacheSize(i);
        lfsConfig[i].lookahead_size = hal_lfs_getLookaheadSize(i);
        lfsConfig[i].read_buffer = hal_lfs_getReadBuffer(i);
        lfsConfig[i].prog_buffer = hal_lfs_getProgBuffer(i);
        lfsConfig[i].lookahead_buffer = hal_lfs_getLookaheadBuffer(i);

        // store the instance index
        lfsInstanceIndex[i] = i;
        lfsConfig[i].context = &lfsInstanceIndex[i];
    }
}

int32_t hal_lfs_mount(int32_t index)
{
    int32_t mountResult = 0;
    // mount the file system
    mountResult = lfs_mount(&lfs[index], &lfsConfig[index]);

    if (mountResult != 0)
    {
        // looks like littlefs is not formated (occuring at 1st boot)
        lfs_format(&lfs[index], &lfsConfig[index]);

        // try mounting again
        mountResult = lfs_mount(&lfs[index], &lfsConfig[index]);
    }

    // mirror littlefs mount result into global flag
    //lfsFileSystemReady = (mountResult == LFS_ERR_OK);

#if !defined(BUILD_RTM)
    ASSERT(mountResult == LFS_ERR_OK);
#endif

    // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // // code block to assist testing littlefs
    // int32_t lfsIndex = 1;
    // char writeBuf[] = {
    //     "Hello! if you get this message, congratulations, that's because littlefs is working on your device!!"};
    // char readBuf[sizeof(writeBuf)];

    // lfs_file_t file;
    // lfs_file_open(&lfs[lfsIndex], &file, "file1.txt", LFS_O_RDWR | LFS_O_CREAT);

    // if (lfs_file_write(&lfs[lfsIndex], &file, writeBuf, sizeof(writeBuf)) != sizeof(writeBuf))
    // {
    //     // something went wrong
    //     return -1;
    // }

    // lfs_file_close(&lfs[lfsIndex], &file);

    // // reopen the file and read it
    // lfs_file_open(&lfs[lfsIndex], &file, "file1.txt", LFS_O_RDONLY);
    // if (lfs_file_read(&lfs[lfsIndex], &file, readBuf, sizeof(readBuf)) != sizeof(writeBuf))
    // {
    //     // something went wrong
    //     return -1;
    // }

    // lfs_file_close(&lfs[lfsIndex], &file);

    // if (memcmp(writeBuf, readBuf, sizeof(writeBuf)) != 0)
    // {
    //     // content doesn't match
    //     return -1;
    // }

    // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    return mountResult;
}

int hal_lfs_lock(const struct lfs_config *c)
{
    uint8_t *index = (uint8_t *)c->context;
    chMtxLock(&lfs_mutex[*index]);

    // nothing interesting to return...
    return 0;
}

int hal_lfs_unlock(const struct lfs_config *c)
{
    uint8_t *index = (uint8_t *)c->context;
    chMtxUnlock(&lfs_mutex[*index]);

    // nothing interesting to return...
    return 0;
}

int hal_lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    (void)c;
    (void)block;
    (void)off;
    (void)buffer;
    (void)size;

    return 0;
};

int hal_lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    (void)c;
    (void)block;
    (void)off;
    (void)buffer;
    (void)size;

    return 0;
};

int hal_lfs_erase(const struct lfs_config *c, lfs_block_t block)
{
    (void)c;
    (void)block;

    return 0;
};

int hal_lfs_sync(const struct lfs_config *c)
{
    (void)c;

    return 0;
}

lfs_t *hal_lfs_get_fs_from_index(int32_t index)
{
    return &lfs[index];
}

int32_t hal_lfs_get_instances_count()
{
    // there is at least one
    int32_t count = 1;

#if LITTLEFS_INSTANCES_COUNT > 1
    count++;
#endif

    return count;
}

// Software CRC implementation with small lookup table
uint32_t lfs_crc(uint32_t crc, const void *buffer, size_t size)
{
    static const uint32_t rtable[16] = {
        0x00000000,
        0x1db71064,
        0x3b6e20c8,
        0x26d930ac,
        0x76dc4190,
        0x6b6b51f4,
        0x4db26158,
        0x5005713c,
        0xedb88320,
        0xf00f9344,
        0xd6d6a3e8,
        0xcb61b38c,
        0x9b64c2b0,
        0x86d3d2d4,
        0xa00ae278,
        0xbdbdf21c,
    };

    const uint8_t *data = buffer;

    for (size_t i = 0; i < size; i++)
    {
        crc = (crc >> 4) ^ rtable[(crc ^ (data[i] >> 0)) & 0xf];
        crc = (crc >> 4) ^ rtable[(crc ^ (data[i] >> 4)) & 0xf];
    }

    return crc;
}
