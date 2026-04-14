//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <target_littlefs.h>
#include <hal_littlefs.h>

uint8_t lfs_inputBuffer[W25Q512_PAGE_SIZE];
uint8_t lfs_outputBuffer[W25Q512_PAGE_SIZE];

int32_t lfs_inputBufferSize = W25Q512_PAGE_SIZE;
int32_t lfs_outputBufferSize = W25Q512_PAGE_SIZE;

#ifdef DEBUG
uint8_t tempBuffer[W25Q512_PAGE_SIZE];
#endif

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
    uint32_t addr = block * c->block_size;

    if (!W25Q512_Erase(addr, false))
    {
        __NOP();
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// target specific implementation of hal_lfs_read
int32_t hal_lfs_read_0(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t addr = block * c->block_size + off;

    if (!W25Q512_Read((uint8_t *)buffer, addr, size))
    {
        __NOP();
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

    if (!W25Q512_Write((uint8_t *)buffer, addr, size))
    {
        __NOP();
        return LFS_ERR_IO;
    }

#ifdef DEBUG

    memset(tempBuffer, 0xBB, size);

    // read back and compare
    W25Q512_Read(tempBuffer, (block * c->block_size + off), size);
    for (lfs_size_t i = 0; i < size; i++)
    {
        ASSERT(((const uint8_t *)buffer)[i] == tempBuffer[i]);
    }

#endif

    return LFS_ERR_OK;
}

// target specific implementation of chip erase
bool hal_lfs_erase_chip_0()
{
    return W25Q512_EraseChip();
}

int8_t target_lfs_init()
{
    return W25Q512_Init() ? LFS_ERR_OK : LFS_ERR_IO;
}