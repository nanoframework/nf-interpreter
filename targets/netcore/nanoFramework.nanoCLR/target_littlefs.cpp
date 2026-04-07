//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"
#include <target_littlefs.h>
#include <hal_littlefs.h>

CLR_RT_Buffer *storageLfs0 = NULL;

// target specific implementation of hal_lfs_erase
int32_t hal_lfs_erase_0(const struct lfs_config *c, lfs_block_t block)
{
    Watchdog_Reset();

    uint32_t addr = block * c->block_size;

    if (!MemoryStorage_Erase_Block(addr))
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// target specific implementation of hal_lfs_read
int32_t hal_lfs_read_0(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t addr = block * c->block_size + off;

    if (!MemoryStorage_Read((uint8_t *)buffer, addr, size))
    {
        return LFS_ERR_IO;
    }

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
    uint32_t addr = block * c->block_size + off;

    if (!MemoryStorage_Write((const uint8_t *)buffer, addr, size))
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// target specific implementation of chip erase
bool hal_lfs_erase_chip_0()
{
    delete storageLfs0;
    storageLfs0 = new std::vector<byte>(LFS0_TOTAL_SIZE);

    return true;
}

// -- //

static bool MemoryStorage_Erase_Block(uint32_t addr)
{
    // sanity check for buffer overflow
    ASSERT(addr < storageLfs0->size());

    // erase block
    memset(&(*storageLfs0)[addr], 0xFF, LFS0_BLOCK_SIZE);

    return true;
}

static bool MemoryStorage_Read(uint8_t *pData, uint32_t readAddr, uint32_t size)
{
    // sanity check for buffer overflow
    ASSERT(readAddr + size <= storageLfs0->size());

    // copy to pointer
    memcpy(pData, &(*storageLfs0)[readAddr], size);

    return true;
}

static bool MemoryStorage_Write(const uint8_t *pData, uint32_t writeAddr, uint32_t size)
{
    // sanity check for buffer overflow
    ASSERT(writeAddr + size <= storageLfs0->size());

    // copy from buffer
    memcpy(&(*storageLfs0)[writeAddr], pData, size);

    return true;
}

int8_t target_lfs_init()
{
    // create RAM based storage for litlefs instance 0
    storageLfs0 = new std::vector<byte>(LFS0_TOTAL_SIZE);

    return LFS_ERR_OK;
}
