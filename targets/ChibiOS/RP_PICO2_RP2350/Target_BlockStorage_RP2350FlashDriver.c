//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_BlockStorage_RP2350FlashDriver.h>
#include <hal_flash.h>
#include <string.h>

// RP2350 XIP flash base address
#define RP2350_XIP_BASE 0x10000000U

// RP2350 flash sector size (4KB)
#define RP2350_FLASH_SECTOR_SIZE 4096U

// Reference to ChibiOS EFL driver instance
extern EFlashDriver EFLD1;
extern MEMORY_MAPPED_NOR_BLOCK_CONFIG Device_BlockStorageConfig;

static bool IsValidAddressRange(
    const MEMORY_MAPPED_NOR_BLOCK_CONFIG *config,
    ByteAddress startAddress,
    unsigned int numBytes)
{
    if (config == NULL)
    {
        return false;
    }

    const uint32_t baseAddress = config->Memory.BaseAddress;
    const uint32_t sizeInBytes = config->Memory.SizeInBytes;

    if (startAddress < baseAddress)
    {
        return false;
    }

    const uint32_t offset = startAddress - baseAddress;
    if (offset > sizeInBytes)
    {
        return false;
    }

    return numBytes <= (sizeInBytes - offset);
}

static bool IsValidBlockStartAddress(const MEMORY_MAPPED_NOR_BLOCK_CONFIG *config, ByteAddress address)
{
    if (!IsValidAddressRange(config, address, 1))
    {
        return false;
    }

    DeviceBlockInfo *blockInfo = config->BlockConfig.BlockDeviceInformation;
    if (blockInfo == NULL || blockInfo->Regions == NULL)
    {
        return false;
    }

    for (uint32_t i = 0; i < blockInfo->NumRegions; i++)
    {
        const BlockRegionInfo *region = &blockInfo->Regions[i];
        const uint64_t regionStart = region->Start;
        const uint64_t regionEnd = regionStart + ((uint64_t)region->NumBlocks * region->BytesPerBlock);

        if ((uint64_t)address >= regionStart && (uint64_t)address < regionEnd)
        {
            return ((address - region->Start) % region->BytesPerBlock) == 0;
        }
    }

    return false;
}

bool RP2350FlashDriver_InitializeDevice(void *context)
{
    (void)context;

    // Start the EFL driver
    eflStart(&EFLD1, NULL);

    return true;
}

bool RP2350FlashDriver_UninitializeDevice(void *context)
{
    (void)context;

    eflStop(&EFLD1);

    return true;
}

DeviceBlockInfo *RP2350FlashDriver_GetDeviceInfo(void *context)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = context;

    return config->BlockConfig.BlockDeviceInformation;
}

bool RP2350FlashDriver_Read(void *context, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;

    if (buffer == NULL)
    {
        return false;
    }

    if (numBytes == 0)
    {
        return true;
    }

    if (!IsValidAddressRange(config, startAddress, numBytes))
    {
        return false;
    }

    // RP2350 flash is XIP memory-mapped, so we can read directly
    memcpy(buffer, (const void *)startAddress, numBytes);

    return true;
}

bool RP2350FlashDriver_Write(
    void *context,
    ByteAddress startAddress,
    unsigned int numBytes,
    unsigned char *buffer,
    bool readModifyWrite)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;
    (void)readModifyWrite;

    if (buffer == NULL)
    {
        return false;
    }

    if (numBytes == 0)
    {
        return true;
    }

    if (!IsValidAddressRange(config, startAddress, numBytes))
    {
        return false;
    }

    // Convert absolute address to flash offset for EFL driver
    flash_offset_t offset = (flash_offset_t)(startAddress - config->Memory.BaseAddress);

    flash_error_t err = flashProgram(&EFLD1, offset, numBytes, buffer);

    return (err == FLASH_NO_ERROR);
}

bool RP2350FlashDriver_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;

    if (length == 0)
    {
        return true;
    }

    if (!IsValidAddressRange(config, blockAddress, length))
    {
        return false;
    }

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

bool RP2350FlashDriver_EraseBlock(void *context, ByteAddress address)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;

    if (!IsValidBlockStartAddress(config, address))
    {
        return false;
    }

    // Convert absolute address to flash offset
    flash_offset_t offset = (flash_offset_t)(address - config->Memory.BaseAddress);

    // Calculate sector number
    flash_sector_t sector = (flash_sector_t)(offset / RP2350_FLASH_SECTOR_SIZE);

    flash_error_t err = flashStartEraseSector(&EFLD1, sector);
    if (err != FLASH_NO_ERROR)
    {
        return false;
    }

    // Wait for erase to complete
    uint32_t wait_time = 0;
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
    if (buffer == NULL)
    {
        return 0;
    }

    if (length == 0)
    {
        return 1;
    }

    if (!IsValidAddressRange(&Device_BlockStorageConfig, startAddress, length))
    {
        return 0;
    }

    flash_offset_t offset = (flash_offset_t)(startAddress - Device_BlockStorageConfig.Memory.BaseAddress);
    flash_error_t err = flashProgram(&EFLD1, offset, length, buffer);
    return (err == FLASH_NO_ERROR) ? 1 : 0;
}

int nf_TargetFlashErase(uint32_t address)
{
    if (!IsValidBlockStartAddress(&Device_BlockStorageConfig, address))
    {
        return 0;
    }

    flash_offset_t offset = (flash_offset_t)(address - Device_BlockStorageConfig.Memory.BaseAddress);
    flash_sector_t sector = (flash_sector_t)(offset / RP2350_FLASH_SECTOR_SIZE);

    flash_error_t err = flashStartEraseSector(&EFLD1, sector);
    if (err != FLASH_NO_ERROR)
    {
        return 0;
    }

    uint32_t wait_time = 0;
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
