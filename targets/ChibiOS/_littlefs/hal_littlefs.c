//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "hal_littlefs.h"

static mutex_t lfs_mutex[LITTLEFS_INSTANCES_COUNT];
static lfs_t lfs[LITTLEFS_INSTANCES_COUNT];
static struct lfs_config lfsConfig[LITTLEFS_INSTANCES_COUNT];
static int8_t lfsInstanceIndex[LITTLEFS_INSTANCES_COUNT];

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

// initialization of littlefs: configurations, data structures, drivers and lock
void hal_lfs_config()
{
    // clear struts for all instances
    memset(&lfs, 0, sizeof(lfs));
    memset(&lfsConfig, 0, sizeof(lfsConfig));
    memset(&lfs_mutex, 0, sizeof(lfs_mutex));

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

#ifdef LFS_THREADSAFE
        lfsConfig[i].lock = &hal_lfs_lock;
        lfsConfig[i].unlock = &hal_lfs_unlock;
#endif

        // setup littlefs configurations
        lfsConfig[i].read_size = hal_lfs_getReadSize(i);
        lfsConfig[i].prog_size = hal_lfs_getProgSize(i);
        lfsConfig[i].block_size = hal_lfs_getBlockSize(i);
        lfsConfig[i].block_count = hal_lfs_getBlockCount(i);
        lfsConfig[i].block_cycles = hal_lfs_getBlockCycles(i);
        lfsConfig[i].cache_size = hal_lfs_getCacheSize(i);
        lfsConfig[i].lookahead_size = hal_lfs_getLookaheadSize(i);

        lfsConfig[i].inline_max = -1;
        lfsConfig[i].attr_max = NANO_LITTLEFS_ATTRIBUTE_SIZE;
        lfsConfig[i].metadata_max = 0;
        lfsConfig[i].compact_thresh = -1;

        // store the instance index
        lfsInstanceIndex[i] = i;
        lfsConfig[i].context = &lfsInstanceIndex[i];
    }
}

void hal_lfs_erase_chip(int32_t index)
{
    switch (index)
    {
        case 0:
            hal_lfs_erase_chip_0();
            break;

#if LITTLEFS_INSTANCES_COUNT > 1
        case 1:
            hal_lfs_erase_chip_1();
            break;
#endif

        default:
            break;
    }
}

void hal_lfs_mount()
{
    hal_lfs_mount_partition(0, false);

#if LITTLEFS_INSTANCES_COUNT > 1
    hal_lfs_mount_partition(1, false);
#endif
}

int32_t hal_lfs_mount_partition(int32_t index, bool forceFormat)
{
    int32_t operationResult = 0;

    // mount the file system
    operationResult = lfs_mount(&lfs[index], &lfsConfig[index]);

    if (operationResult != 0)
    {
        // looks like littlefs is not formated (occuring at 1st boot)

        if (forceFormat)
        {
            // wipe out the chip
            hal_lfs_erase_chip(index);
        }

        lfs_format(&lfs[index], &lfsConfig[index]);

        // mount the file system again
        operationResult = lfs_mount(&lfs[index], &lfsConfig[index]);
    }

#if !defined(BUILD_RTM)
    ASSERT(operationResult == LFS_ERR_OK);
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

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // code reproducing the same workflow as the managed API
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // // get attributes on non existing file
    // char *filePath = "temp/testdir/file1.txt";
    // char *dir1Path = "temp";
    // char *dir2Path = "temp/testdir";

    // uint32_t attributes;
    // volatile int32_t result;
    // struct lfs_info info;
    // lfs_file_t file;
    // //lfs_dir_t dir;
    // struct lfs_attr attr;
    // struct lfs_file_config fileConfig;
    // uint32_t nanoAttributes;
    // (void)result;

    // // check file path
    // result = lfs_stat(&lfs[index], filePath, &info);

    // // try to get file attributes
    // result = lfs_getattr(
    //         &lfs[index],
    //         (const char *)filePath,
    //         NANO_LITTLEFS_ATTRIBUTE,
    //         &attributes,
    //         NANO_LITTLEFS_ATTRIBUTE_SIZE);

    // // create directories in the path
    // result = lfs_mkdir(&lfs[index], dir1Path);
    // result = lfs_mkdir(&lfs[index], dir2Path);

    // // open file
    // //result = lfs_file_open(&lfs[index], &file, filePath, LFS_O_CREAT | LFS_O_RDWR);

    // // close file
    // //result = lfs_file_close(&lfs[index], &file);

    // // check directory exists
    // //result = lfs_stat(&lfs[index], dir2Path, &info);

    // // // try to get directory attributes
    // // result = lfs_getattr(
    // //         &lfs[index],
    // //         (const char *)dir2Path,
    // //         NANO_LITTLEFS_ATTRIBUTE,
    // //         &attributes,
    // //         NANO_LITTLEFS_ATTRIBUTE_SIZE);

    // // check file path
    // //result = lfs_stat(&lfs[index], filePath, &info);

    // // set file attributes
    // // hardcoded to 128 which is value for FileAttributes_Normal
    // nanoAttributes = 128;
    // attr = (struct lfs_attr){
    //     .type = NANO_LITTLEFS_ATTRIBUTE,
    //     .size = NANO_LITTLEFS_ATTRIBUTE_SIZE,
    //     .buffer = &nanoAttributes
    // };

    // fileConfig = (struct lfs_file_config) {
    //     .buffer = NULL,
    //     .attrs = &attr,
    //     .attr_count = 1,
    // };

    // // create file
    // result =  lfs_file_opencfg(&lfs[index], &file, filePath,  LFS_O_CREAT | LFS_O_RDWR, &fileConfig);

    //  // sync file to save attributes
    // result = lfs_file_sync(&lfs[index], &file);

    // // read file attributes
    // result = lfs_getattr(
    //         &lfs[index],
    //         (const char *)filePath,
    //         NANO_LITTLEFS_ATTRIBUTE,
    //         &attributes,
    //         NANO_LITTLEFS_ATTRIBUTE_SIZE);

    // // close file
    // result = lfs_file_close(&lfs[index], &file);

    // // check file existance
    // result = lfs_stat(&lfs[index], filePath, &info);

    // // read file attributes
    // result = lfs_getattr(
    //         &lfs[index],
    //         (const char *)filePath,
    //         NANO_LITTLEFS_ATTRIBUTE,
    //         &attributes,
    //         NANO_LITTLEFS_ATTRIBUTE_SIZE);

    return operationResult;
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
    if (index >= LITTLEFS_INSTANCES_COUNT)
    {
        return NULL;
    }

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
