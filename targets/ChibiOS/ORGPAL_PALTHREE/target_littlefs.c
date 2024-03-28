//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <target_littlefs.h>
#include <hal_littlefs.h>

#ifdef LFS_SPI1

static const SPIConfig spiConfig = {
    .circular = false,
    .slave = false,
    .data_cb = NULL,
    .error_cb = NULL,
    // CPHA=0, CPOL=0, MSb first
    .cr1 = 0U, // SPI_CR1_CPOL | SPI_CR1_BR_0,
    // transfer length to 8bit
    .cr2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0};

#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t writeBuffer[AT25SF641_PAGE_SIZE];
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t readBuffer[AT25SF641_PAGE_SIZE];

///////////////
// Definitions
#define CS_SELECT   palClearPad(PAL_PORT(LINE_FLASH_SPI1_CS), PAL_PAD(LINE_FLASH_SPI1_CS))
#define CS_UNSELECT palSetPad(PAL_PORT(LINE_FLASH_SPI1_CS), PAL_PAD(LINE_FLASH_SPI1_CS))

///////////////
// declarations
bool SPI_Erase_Block(uint32_t addr);
bool SPI_Read(uint8_t *pData, uint32_t readAddr, uint32_t size);
bool SPI_Write(const uint8_t *pData, uint32_t writeAddr, uint32_t size);

extern uint32_t HAL_GetTick(void);

// target specific implementation of hal_lfs_erase
int32_t hal_lfs_erase_0(const struct lfs_config *c, lfs_block_t block)
{
    (void)c;

    Watchdog_Reset();

    uint32_t addr = block * AT25SF641_PAGE_SIZE;

    if (!SPI_Erase_Block(addr))
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// target specific implementation of hal_lfs_read
int32_t hal_lfs_read_0(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    (void)c;

    uint32_t addr = block * AT25SF641_PAGE_SIZE + off;

    if (!SPI_Read(buffer, addr, size))
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
    (void)c;

    uint32_t addr = block * AT25SF641_PAGE_SIZE + off;

    if (!SPI_Write(buffer, addr, size))
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

bool SPI_WaitOnBusy()
{
    uint32_t tickstart = HAL_GetTick();

    writeBuffer[0] = READ_STATUS_REG1_CMD;

    CS_SELECT;

    // send read status register 1
    spiSend(&SPID1, 1, writeBuffer);

    while (true)
    {
        // read register value
        spiReceive(&SPID1, 1, readBuffer);

        if (!(readBuffer[0] & AT25SF641_SR_BUSY))
        {
            // BuSY bit is cleared
            break;
        }

        if ((HAL_GetTick() - tickstart) > HAL_SPI_TIMEOUT_DEFAULT_VALUE)
        {
            // operation timeout

            // unselect SPI
            CS_UNSELECT;

            return false;
        }
    }

    CS_UNSELECT;

    return true;
}

bool SPI_Erase_Block(uint32_t addr)
{
    // send write enable
    writeBuffer[0] = WRITE_ENABLE_CMD;

    CS_SELECT;
    spiSend(&SPID1, 1, writeBuffer);
    CS_UNSELECT;

    // send block erase
    writeBuffer[0] = SECTOR_ERASE_CMD;
    writeBuffer[1] = (uint8_t)(addr >> 16);
    writeBuffer[2] = (uint8_t)(addr >> 8);
    writeBuffer[3] = (uint8_t)addr;

    // flush DMA buffer to ensure cache coherency
    // (only required for Cortex-M7)
    cacheBufferFlush(writeBuffer, 4);

    CS_SELECT;
    spiSend(&SPID1, 4, writeBuffer);
    CS_UNSELECT;

    // wait for erase operation to complete
    return SPI_WaitOnBusy();
}

bool SPI_Read(uint8_t *pData, uint32_t readAddr, uint32_t size)
{
    // send read page command
    writeBuffer[0] = READ_CMD;
    writeBuffer[1] = (uint8_t)(readAddr >> 16);
    writeBuffer[2] = (uint8_t)(readAddr >> 8);
    writeBuffer[3] = (uint8_t)readAddr;

    // flush DMA buffer to ensure cache coherency
    // (only required for Cortex-M7)
    cacheBufferFlush(writeBuffer, 4);

    CS_SELECT;
    spiSend(&SPID1, 4, writeBuffer);

    // clear read buffer
    memset(readBuffer, 0, AT25SF641_PAGE_SIZE);

    spiReceive(&SPID1, size, readBuffer);
    CS_UNSELECT;

    // invalidate cache
    // (only required for Cortex-M7)
    cacheBufferInvalidate(readBuffer, size);

    // copy to pointer
    memcpy(pData, readBuffer, size);

    return true;
}

bool SPI_Write(const uint8_t *pData, uint32_t writeAddr, uint32_t size)
{
    // send write enable
    writeBuffer[0] = WRITE_ENABLE_CMD;

    CS_SELECT;
    spiSend(&SPID1, 1, writeBuffer);
    CS_UNSELECT;

    // send write page
    writeBuffer[0] = PAGE_PROG_CMD;
    writeBuffer[1] = (uint8_t)(writeAddr >> 16);
    writeBuffer[2] = (uint8_t)(writeAddr >> 8);
    writeBuffer[3] = (uint8_t)writeAddr;

    // flush DMA buffer to ensure cache coherency
    // (only required for Cortex-M7)
    cacheBufferFlush(writeBuffer, 4);

    CS_SELECT;
    spiSend(&SPID1, 4, writeBuffer);

    // copy from buffer
    memcpy(writeBuffer, pData, size);

    // flush DMA buffer to ensure cache coherency
    // (only required for Cortex-M7)
    cacheBufferFlush(writeBuffer, size);

    spiSend(&SPID1, size, writeBuffer);
    CS_UNSELECT;

    // wait for operation to complete
    SPI_WaitOnBusy();

    // send write disable
    writeBuffer[0] = WRITE_DISABLE_CMD;
    cacheBufferFlush(writeBuffer, 1);

    CS_SELECT;
    spiSend(&SPID1, 1, writeBuffer);
    CS_UNSELECT;

    return true;
}

#endif // LFS_SPI1

int8_t target_lfs_init()
{
#ifdef LFS_SPI1

    // init driver
    spiAcquireBus(&SPID1);
    spiStart(&SPID1, &spiConfig);

    ////////////////////////////////////////////////////////////////////////
    // no need to worry with cache issues at this early stage of the boot //
    ////////////////////////////////////////////////////////////////////////

    // resume from deep power down
    // have to send this to make sure device is functional after sleep
    writeBuffer[0] = RESUME_DEEP_PD_CMD;

    CS_SELECT;
    spiSend(&SPID1, 1, writeBuffer);
    CS_UNSELECT;

    // sanity check: read device ID and unique ID
    writeBuffer[0] = READ_ID_CMD2;

    // flush DMA buffer to ensure cache coherency
    cacheBufferFlush(writeBuffer, 1);

    CS_SELECT;
    spiExchange(&SPID1, 4, writeBuffer, readBuffer);
    CS_UNSELECT;

    // constants from ID Definitions table in AT25SF641 datasheet
    ASSERT(readBuffer[1] == AT25SF641_MANUFACTURER_ID);
    ASSERT(readBuffer[2] == AT25SF641_DEVICE_ID1);
    ASSERT(readBuffer[3] == AT25SF641_DEVICE_ID2);

#endif // LFS_SPI1

    return LFS_ERR_OK;
}
