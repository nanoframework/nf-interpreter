//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_BlockStorage_RP2040FlashDriver.h>
#include <hal_flash.h>
#include <string.h>

// RP2040 XIP flash base address
#define RP2040_XIP_BASE 0x10000000U

// RP2040 flash sector size (4KB)
#define RP2040_FLASH_SECTOR_SIZE 4096U

// RP2040 flash page size (256B) - minimum write unit
#define RP2040_FLASH_PAGE_SIZE 256U

// Reference to ChibiOS EFL driver instance
extern EFlashDriver EFLD1;

bool RP2040FlashDriver_InitializeDevice(void *context)
{
    (void)context;

    // Start the EFL driver
    eflStart(&EFLD1, NULL);

    return true;
}

bool RP2040FlashDriver_UninitializeDevice(void *context)
{
    (void)context;

    eflStop(&EFLD1);

    return true;
}

DeviceBlockInfo *RP2040FlashDriver_GetDeviceInfo(void *context)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = context;

    return config->BlockConfig.BlockDeviceInformation;
}

bool RP2040FlashDriver_Read(void *context, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer)
{
    (void)context;

    // RP2040 flash is XIP memory-mapped, so we can read directly
    memcpy(buffer, (const void *)startAddress, numBytes);

    return true;
}

bool RP2040FlashDriver_Write(
    void *context,
    ByteAddress startAddress,
    unsigned int numBytes,
    unsigned char *buffer,
    bool readModifyWrite)
{
    (void)context;
    (void)readModifyWrite;

    flash_offset_t offset = (flash_offset_t)(startAddress - RP2040_XIP_BASE);

    // flashProgram handles page-boundary splitting and atomic
    // exit_xip → program → busy-wait → enter_xip per page internally.
    flash_error_t err = flashProgram(&EFLD1, offset, numBytes, buffer);

    return (err == FLASH_NO_ERROR);
}

bool RP2040FlashDriver_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;

    // Check if all bytes are 0xFF (erased state)
    unsigned char *p = (unsigned char *)blockAddress;

    for (unsigned int i = 0; i < length; i++)
    {
        if (*p++ != 0xFF)
        {
            return false;
        }
    }

    return true;
}

bool RP2040FlashDriver_EraseBlock(void *context, ByteAddress address)
{
    (void)context;

    // Convert absolute address to flash offset
    flash_offset_t offset = (flash_offset_t)(address - RP2040_XIP_BASE);

    // Calculate sector number
    flash_sector_t sector = (flash_sector_t)(offset / RP2040_FLASH_SECTOR_SIZE);

    flash_error_t err = flashStartEraseSector(&EFLD1, sector);
    if (err != FLASH_NO_ERROR)
    {
        return false;
    }

    // Wait for erase to complete
    uint32_t wait_time;
    do
    {
        err = flashQueryErase(&EFLD1, &wait_time);
        if (err == FLASH_ERROR_ERASE)
        {
            return false;
        }
        if (wait_time > 0)
        {
            chThdSleepMilliseconds(wait_time);
        }
    } while (err == FLASH_BUSY_ERASING);

    return (err == FLASH_NO_ERROR);
}

// nanoBooter flash access functions (called from WireProtocol_MonitorCommands)
int nf_TargetFlashWrite(uint32_t startAddress, uint32_t length, const uint8_t *buffer)
{
    flash_offset_t offset = (flash_offset_t)(startAddress - RP2040_XIP_BASE);
    flash_error_t err = flashProgram(&EFLD1, offset, length, buffer);
    return (err == FLASH_NO_ERROR) ? 1 : 0;
}

int nf_TargetFlashErase(uint32_t address)
{
    flash_offset_t offset = (flash_offset_t)(address - RP2040_XIP_BASE);
    flash_sector_t sector = (flash_sector_t)(offset / RP2040_FLASH_SECTOR_SIZE);

    flash_error_t err = flashStartEraseSector(&EFLD1, sector);
    if (err != FLASH_NO_ERROR)
    {
        return 0;
    }

    uint32_t wait_time;
    do
    {
        err = flashQueryErase(&EFLD1, &wait_time);
        if (err == FLASH_ERROR_ERASE)
        {
            return 0;
        }
        if (wait_time > 0)
        {
            chThdSleepMilliseconds(wait_time);
        }
    } while (err == FLASH_BUSY_ERASING);

    return (err == FLASH_NO_ERROR) ? 1 : 0;
}
