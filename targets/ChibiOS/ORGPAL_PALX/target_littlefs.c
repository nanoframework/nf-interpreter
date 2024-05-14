//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <target_littlefs.h>
#include <hal_littlefs.h>
#include <hal_stm32_qspi.h>

extern uint32_t HAL_GetTick(void);

static const WSPIConfig wspiConfig = {
    .end_cb = NULL,
    .error_cb = NULL,
    .dcr = STM32_DCR_FSIZE(23U) | STM32_DCR_CSHT(5U)};

#if CACHE_LINE_SIZE > 0
CC_ALIGN_DATA(CACHE_LINE_SIZE)
#endif
uint8_t dataBuffer_0[CACHE_SIZE_ALIGN(uint8_t, W25Q128_PAGE_SIZE)];

///////////////
// declarations

static void WSPI_ResetMemory();
static void WSPI_EnableQuad();
static bool WSPI_WriteEnable();
static bool WSPI_WaitOnBusy();
static bool WSPI_ReadChipID(uint8_t *buffer);

static bool WSPI_Read(uint8_t *pData, uint32_t readAddr, uint32_t size);
static bool WSPI_Write(const uint8_t *pData, uint32_t writeAddr, uint32_t size);
static bool WSPI_Erase_Block(uint32_t blockAddress, bool largeBlock);
static bool WSPI_Erase_Chip();

// target specific implementation of hal_lfs_erase
int32_t hal_lfs_erase_0(const struct lfs_config *c, lfs_block_t block)
{
    Watchdog_Reset();

    uint32_t addr = block * c->block_size;

    if (!WSPI_Erase_Block(addr, false))
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// target specific implementation of hal_lfs_read
int32_t hal_lfs_read_0(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t addr = block * c->block_size + off;

    if (!WSPI_Read(buffer, addr, size))
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

    if (WSPI_Write((const uint8_t *)buffer, addr, size) != TRUE)
    {
        return LFS_ERR_IO;
    }

#ifdef DEBUG

    uint8_t tempBuffer[size];
    memset(tempBuffer, 0xBB, size);

    // read back and compare
    WSPI_Read(tempBuffer, (block * c->block_size + off), size);
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
    return WSPI_Erase_Chip();
}

static bool WSPI_Erase_Chip()
{
    // need to do this one one block at a time to avoid watchdog reset
    for (uint32_t i = 0; i < W25Q128_FLASH_SIZE / W25Q128_SECTOR_SIZE; i++)
    {
        if (WSPI_Erase_Block(i * W25Q128_SECTOR_SIZE, true) != TRUE)
        {
            return FALSE;
        }

        // reset watchdog
        Watchdog_Reset();
    }

    return TRUE;
}

static bool WSPI_WaitOnBusy()
{
    uint32_t tickstart = HAL_GetTick();

    wspi_command_t cmdp = {
        .cfg =
            (WSPI_CFG_CMD_MODE_ONE_LINE | WSPI_CFG_ADDR_MODE_NONE | WSPI_CFG_ALT_MODE_NONE |
             WSPI_CFG_DATA_MODE_ONE_LINE | WSPI_CFG_CMD_SIZE_8 | WSPI_CFG_ADDR_SIZE_24),
        .cmd = READ_STATUS_REG1_CMD,
        .addr = 0,
        .alt = 0,
        .dummy = 0};

    while (true)
    {
        // clear read buffer
        memset(dataBuffer_0, 0xFF, 1);

        wspiReceive(&WSPID1, &cmdp, 1, dataBuffer_0);

        cacheBufferInvalidate(dataBuffer_0, 1);

        if (!(dataBuffer_0[0] & W25Q128_SR_WIP))
        {
            // BuSY bit is cleared
            return true;
        }
       
        if ((HAL_GetTick() - tickstart) > HAL_SPI_TIMEOUT_DEFAULT_VALUE)
        {
            // operation timeout
            return false;
        }
    }
}

static bool WSPI_WriteEnable()
{
    wspi_command_t cmdp = {
        .cfg =
            (WSPI_CFG_CMD_MODE_ONE_LINE | WSPI_CFG_ADDR_MODE_NONE | WSPI_CFG_ALT_MODE_NONE | WSPI_CFG_DATA_MODE_NONE |
             WSPI_CFG_CMD_SIZE_8 | WSPI_CFG_ADDR_SIZE_24),
        .cmd = WRITE_ENABLE_CMD,
        .addr = 0,
        .alt = 0,
        .dummy = 0};

    return wspiCommand(&WSPID1, &cmdp);
}

static void WSPI_ResetMemory()
{
    wspi_command_t cmdp = {
        .cfg =
            (WSPI_CFG_CMD_MODE_ONE_LINE | WSPI_CFG_ADDR_MODE_NONE | WSPI_CFG_ALT_MODE_NONE | WSPI_CFG_DATA_MODE_NONE |
             WSPI_CFG_CMD_SIZE_8 | WSPI_CFG_ADDR_SIZE_24),
        .cmd = RESET_ENABLE_CMD,
        .addr = 0,
        .alt = 0,
        .dummy = 0};

    // set enable reset command
    wspiCommand(&WSPID1, &cmdp);

    cmdp.cmd = RESET_MEMORY_CMD;

    // reset memory
    wspiCommand(&WSPID1, &cmdp);
}

static void WSPI_EnableQuad()
{
    // read status register 2
    wspi_command_t cmdp = {
        .cfg =
            (WSPI_CFG_CMD_MODE_ONE_LINE | WSPI_CFG_ADDR_MODE_NONE | WSPI_CFG_ALT_MODE_NONE |
             WSPI_CFG_DATA_MODE_ONE_LINE | WSPI_CFG_CMD_SIZE_8 | WSPI_CFG_ADDR_SIZE_24),
        .cmd = READ_STATUS_REG2_CMD,
        .addr = 0,
        .alt = 0,
        .dummy = 0};

    // clear read buffer
    memset(dataBuffer_0, 0xFF, 1);
    wspiReceive(&WSPID1, &cmdp, 1, dataBuffer_0);

    // set the quad enable bit
    dataBuffer_0[0] |= W25Q128_SR2_QE;

    // set command to write status register 2
    cmdp.cmd = WRITE_STATUS_REG2_CMD;

    // send write enable
    WSPI_WriteEnable();

    // write status register 2
    wspiSend(&WSPID1, &cmdp, 1, dataBuffer_0);
}

static bool WSPI_Erase_Block(uint32_t blockAddress, bool largeBlock)
{
    // send write enable
    WSPI_WriteEnable();

    wspi_command_t cmdp = {
        .cfg =
            (WSPI_CFG_CMD_MODE_ONE_LINE | WSPI_CFG_ADDR_MODE_ONE_LINE | WSPI_CFG_ALT_MODE_NONE |
             WSPI_CFG_DATA_MODE_NONE | WSPI_CFG_CMD_SIZE_8 | WSPI_CFG_ADDR_SIZE_24),
        .cmd = largeBlock ? SECTOR_ERASE_CMD : SUBSECTOR_ERASE_CMD,
        .addr = blockAddress,
        .alt = 0,
        .dummy = 0};

    wspiCommand(&WSPID1, &cmdp);

    // Configure automatic polling mode to wait for end of erase
    return (WSPI_WaitOnBusy(&WSPID1, W25Q128_SECTOR_ERASE_MAX_TIME) == TRUE);
}

static bool WSPI_Read(uint8_t *pData, uint32_t readAddr, uint32_t size)
{
    // sanity check for buffer overflow
    ASSERT(size <= W25Q128_PAGE_SIZE);

    // clear read buffer
    memset(dataBuffer_0, 0xDD, size);

    wspi_command_t cmdp = {
        .cfg =
            (WSPI_CFG_CMD_MODE_ONE_LINE | WSPI_CFG_ADDR_MODE_ONE_LINE | WSPI_CFG_ALT_MODE_NONE |
             WSPI_CFG_DATA_MODE_FOUR_LINES | WSPI_CFG_CMD_SIZE_8 | WSPI_CFG_ADDR_SIZE_24),
        .cmd = QUAD_OUT_FAST_READ_CMD,
        .addr = readAddr,
        .alt = 0,
        .dummy = W25Q128_DUMMY_CYCLES_READ_QUAD};

    wspiReceive(&WSPID1, &cmdp, size, dataBuffer_0);

    // invalidate cache
    // (only required for Cortex-M7)
    cacheBufferInvalidate(dataBuffer_0, size);

    // copy to pointer
    memcpy(pData, dataBuffer_0, size);

    return true;
}

static bool WSPI_Write(const uint8_t *pData, uint32_t writeAddr, uint32_t size)
{
    uint32_t writeSize;
    uint32_t address = writeAddr;

    wspi_command_t cmdp = {
        .cfg =
            (WSPI_CFG_CMD_MODE_ONE_LINE | WSPI_CFG_ADDR_MODE_ONE_LINE | WSPI_CFG_ALT_MODE_NONE |
             WSPI_CFG_DATA_MODE_ONE_LINE | WSPI_CFG_CMD_SIZE_8 | WSPI_CFG_ADDR_SIZE_24),
        .cmd = PAGE_PROG_CMD,
        .addr = address,
        .alt = 0,
        .dummy = 0};

    // perform paged program
    while (size > 0)
    {
        // send write enable
        WSPI_WriteEnable();

        // calculate write size
        writeSize = __builtin_fmin(W25Q128_PAGE_SIZE - (address % W25Q128_PAGE_SIZE), size);

        // adjust address if writeSize is the full page
        cmdp.addr = writeSize == W25Q128_PAGE_SIZE ? address & 0xFFFFFFF0 : address;

        // copy from buffer
        memcpy(dataBuffer_0, pData, writeSize);

        // flush DMA buffer to ensure cache coherency
        // (only required for Cortex-M7)
        cacheBufferFlush(dataBuffer_0, writeSize);

        wspiSend(&WSPID1, &cmdp, writeSize, dataBuffer_0);

        // wait for operation to complete
        WSPI_WaitOnBusy();

        address += writeSize;
        pData += writeSize;
        size -= writeSize;
    }

    return true;
}

static bool WSPI_ReadChipID(uint8_t *buffer)
{
    wspi_command_t cmdp = {
        .cfg =
            (WSPI_CFG_CMD_MODE_ONE_LINE | WSPI_CFG_ADDR_MODE_NONE | WSPI_CFG_ALT_MODE_NONE |
             WSPI_CFG_DATA_MODE_ONE_LINE | WSPI_CFG_CMD_SIZE_8 | WSPI_CFG_ADDR_SIZE_24),
        .cmd = READ_ID_CMD2,
        .addr = 0,
        .alt = 0,
        .dummy = 0};

    return wspiReceive(&WSPID1, &cmdp, 3, buffer);
}

int8_t target_lfs_init()
{
    // init driver
    wspiAcquireBus(&WSPID1);
    wspiStart(&WSPID1, &wspiConfig);

    memset(dataBuffer_0, 0, 3);

    WSPI_ReadChipID(dataBuffer_0);

    // constants from ID Definitions table in W25Q128 datasheet
    ASSERT(dataBuffer_0[0] == W25Q128_MANUFACTURER_ID);
    ASSERT(dataBuffer_0[1] == W25Q128_DEVICE_ID1);
    ASSERT(dataBuffer_0[2] == W25Q128_DEVICE_ID2);

    WSPI_ResetMemory();
    WSPI_EnableQuad();

    return LFS_ERR_OK;
}
