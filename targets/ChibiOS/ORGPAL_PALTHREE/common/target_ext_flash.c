//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// AT25SF641 SPI flash driver for ORGPAL_PALTHREE (STM32F769ZI, SPI1).
//
// Provides the shared AT25SF641_* API declared in target_ext_flash.h.
// This implementation uses ChibiOS SPI (SPID1) and is shared between:
//   - The littlefs driver  (targets/ChibiOS/ORGPAL_PALTHREE/target_littlefs.c)
//   - The MCUboot porting layer (common/mcuboot_flash_map.c)
//
// Callers must call spiStart(&SPID1, ...) before invoking any function here.
// The DMA transfer buffer (dataBuffer_0) lives in the .nocache section to
// satisfy the Cortex-M7 D-cache / DMA coherency requirement.

#include <ch.h>
#include <hal.h>
#include <cache.h>
#include <string.h>
#include <nanoHAL_v2.h>

#include "target_ext_flash.h"

// AT25SF641 command bytes (internal to this translation unit)
#define READ_CMD              0x03U
#define BLOCK_ERASE_CMD       0xD8U
#define SECTOR_ERASE_CMD      0x20U
#define PAGE_PROG_CMD         0x02U
#define WRITE_ENABLE_CMD      0x06U
#define WRITE_DISABLE_CMD     0x04U
#define READ_STATUS_REG1_CMD  0x05U
#define WRITE_STATUS_REG1_CMD 0x01U
#define READ_STATUS_REG2_CMD  0x35U
#define WRITE_STATUS_REG2_CMD 0x31U
#define READ_ID_CMD           0x90U
#define READ_ID_CMD2          0x9FU
#define RESUME_DEEP_PD_CMD    0xABU
#define DEEP_POWER_DOWN_CMD   0xB9U

#define CS_SELECT   palClearPad(PAL_PORT(LINE_FLASH_SPI1_CS), PAL_PAD(LINE_FLASH_SPI1_CS))
#define CS_UNSELECT palSetPad(PAL_PORT(LINE_FLASH_SPI1_CS), PAL_PAD(LINE_FLASH_SPI1_CS))

#if CACHE_LINE_SIZE > 0
CC_ALIGN_DATA(CACHE_LINE_SIZE)
uint8_t dataBuffer_0[CACHE_SIZE_ALIGN(uint8_t, AT25SF641_PAGE_SIZE)] __attribute__((section(".nocache")));
#else
uint8_t dataBuffer_0[AT25SF641_PAGE_SIZE];
#endif

extern uint32_t HAL_GetTick(void);

bool AT25SF641_WaitReady(void)
{
    uint32_t tickstart = HAL_GetTick();

    memset(dataBuffer_0, 0xFF, 1);

    dataBuffer_0[0] = READ_STATUS_REG1_CMD;
    cacheBufferFlush(dataBuffer_0, sizeof(dataBuffer_0));

    CS_SELECT;

    spiSend(&SPID1, 1, dataBuffer_0);

    while (true)
    {
        spiReceive(&SPID1, 1, dataBuffer_0);
        cacheBufferInvalidate(dataBuffer_0, sizeof(dataBuffer_0));

        if (!(dataBuffer_0[0] & AT25SF641_SR_BUSY))
        {
            break;
        }

        if ((HAL_GetTick() - tickstart) > HAL_SPI_TIMEOUT_DEFAULT_VALUE)
        {
            CS_UNSELECT;
            return false;
        }
    }

    CS_UNSELECT;

    return true;
}

bool AT25SF641_Erase(uint32_t addr, bool large_block)
{
    // send write enable
    dataBuffer_0[0] = WRITE_ENABLE_CMD;
    cacheBufferFlush(dataBuffer_0, sizeof(dataBuffer_0));

    CS_SELECT;
    spiSend(&SPID1, 1, dataBuffer_0);
    CS_UNSELECT;

    // send erase command with 24-bit address
    dataBuffer_0[0] = large_block ? BLOCK_ERASE_CMD : SECTOR_ERASE_CMD;
    dataBuffer_0[1] = (uint8_t)(addr >> 16);
    dataBuffer_0[2] = (uint8_t)(addr >> 8);
    dataBuffer_0[3] = (uint8_t)addr;

    CS_SELECT;
    spiSend(&SPID1, 4, dataBuffer_0);
    CS_UNSELECT;

    return AT25SF641_WaitReady();
}

bool AT25SF641_Read(uint8_t *buf, uint32_t addr, uint32_t size)
{
    // send read command with 24-bit address; CS stays asserted for the data phase
    dataBuffer_0[0] = READ_CMD;
    dataBuffer_0[1] = (uint8_t)(addr >> 16);
    dataBuffer_0[2] = (uint8_t)(addr >> 8);
    dataBuffer_0[3] = (uint8_t)addr;

    CS_SELECT;
    spiSend(&SPID1, 4, dataBuffer_0);

    // read in page-sized chunks through the DMA buffer for cache coherency
    uint32_t remaining = size;
    uint8_t *out = buf;

    while (remaining > 0)
    {
        uint32_t chunk = (remaining < AT25SF641_PAGE_SIZE) ? remaining : AT25SF641_PAGE_SIZE;

        spiReceive(&SPID1, chunk, dataBuffer_0);
        cacheBufferInvalidate(dataBuffer_0, sizeof(dataBuffer_0));
        memcpy(out, dataBuffer_0, chunk);

        out += chunk;
        remaining -= chunk;
    }

    CS_UNSELECT;

    return true;
}

bool AT25SF641_Write(const uint8_t *buf, uint32_t addr, uint32_t size)
{
    uint32_t writeSize;
    uint32_t address = addr;

    while (size > 0)
    {
        // send write enable
        dataBuffer_0[0] = WRITE_ENABLE_CMD;
        cacheBufferFlush(dataBuffer_0, sizeof(dataBuffer_0));

        CS_SELECT;
        spiSend(&SPID1, 1, dataBuffer_0);
        CS_UNSELECT;

        // clamp to the remaining space in the current page
        writeSize = (uint32_t)__builtin_fmin(AT25SF641_PAGE_SIZE - (address % AT25SF641_PAGE_SIZE), size);

        dataBuffer_0[0] = PAGE_PROG_CMD;
        dataBuffer_0[1] = (uint8_t)(address >> 16);
        dataBuffer_0[2] = (uint8_t)(address >> 8);
        // when writing a full page the LSB wraps to 0 per the AT25SF641 protocol
        dataBuffer_0[3] = (uint8_t)(writeSize == AT25SF641_PAGE_SIZE ? address & 0xFFFFFF00U : address);

        CS_SELECT;
        spiSend(&SPID1, 4, dataBuffer_0);

        memcpy(dataBuffer_0, buf, writeSize);

        spiSend(&SPID1, writeSize, dataBuffer_0);
        CS_UNSELECT;

        AT25SF641_WaitReady();

        address += writeSize;
        buf += writeSize;
        size -= writeSize;
    }

    return true;
}

bool AT25SF641_Init(void)
{
    // resume from deep power down; required to make the device functional after sleep
    dataBuffer_0[0] = RESUME_DEEP_PD_CMD;

    CS_SELECT;
    spiSend(&SPID1, 1, dataBuffer_0);
    CS_UNSELECT;

    // from AT25SF641 datasheet: device needs time to become fully responsive
    dataBuffer_0[0] = READ_ID_CMD2;
    chThdSleepMilliseconds(10);

    CS_SELECT;
    spiSend(&SPID1, 1, dataBuffer_0);
    spiReceive(&SPID1, 3, dataBuffer_0);
    CS_UNSELECT;

    // verify JEDEC ID (constants from AT25SF641 datasheet, ID Definitions table)
    ASSERT(dataBuffer_0[0] == AT25SF641_MANUFACTURER_ID);
    ASSERT(dataBuffer_0[1] == AT25SF641_DEVICE_ID1);
    ASSERT(dataBuffer_0[2] == AT25SF641_DEVICE_ID2);

    return (dataBuffer_0[0] == AT25SF641_MANUFACTURER_ID) && (dataBuffer_0[1] == AT25SF641_DEVICE_ID1) &&
           (dataBuffer_0[2] == AT25SF641_DEVICE_ID2);
}

bool AT25SF641_EraseChip(void)
{
    // erase one 64 kB block at a time to avoid watchdog expiry during long chip erases
    for (uint32_t i = 0; i < AT25SF641_FLASH_SIZE / AT25SF641_SECTOR_SIZE; i++)
    {
        if (!AT25SF641_Erase(i * AT25SF641_SECTOR_SIZE, true))
        {
            return false;
        }

        Watchdog_Reset();
    }

    return true;
}
