//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <mxc_device.h>
#include <flc.h>
#include <flc_regs.h>
#include <Target_BlockStorage_MXCFlashDriver.h>

#define FLASH_ERASED_WORD ((uint32_t)0xFFFFFFFF)

bool MXCFlashDriver_InitializeDevice(void *context)
{
    (void)context;

    // nothing to do here
    return true;
}

bool MXCFlashDriver_UninitializeDevice(void *context)
{
    (void)context;

    // nothing to do here
    return true;
}

DeviceBlockInfo *MXCFlashDriver_GetDeviceInfo(void *context)
{

    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = context;

    return config->BlockConfig.BlockDeviceInformation;
}

bool MXCFlashDriver_Read(void *context, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer)
{
    (void)context;

    if (buffer == NULL)
    {
        return false;
    }

    volatile uint8_t *cursor = (volatile uint8_t *)startAddress;
    volatile uint8_t *endAddress = (volatile uint8_t *)(startAddress + numBytes);

    // copy contents from flash to buffer starting from the start address
    while (cursor < endAddress)
    {
        *buffer++ = *cursor++;
    }

    return true;
}

bool MXCFlashDriver_Write(
    void *context,
    ByteAddress startAddress,
    unsigned int numBytes,
    unsigned char *buffer,
    bool readModifyWrite)
{
    (void)context;
    (void)readModifyWrite;

    if (buffer == NULL)
    {
        return false;
    }

    uint32_t address = startAddress;
    uint32_t endAddress = startAddress + numBytes;
    uint32_t remainingBytes = numBytes;
    bool success = false;
    uint32_t error;

    // Check if flash controller is busy
    if (MXC_FLC0->ctrl & MXC_F_FLC_CTRL_PEND)
    {
        return false;
    }

    // The FLC supports write widths of 128-bits only. 32*64bits (32*4bytes)

    while (address < endAddress)
    {
        // Unlock the Flash to enable the flash control register access
        MXC_FLC0->ctrl &= ~MXC_F_FLC_CTRL_UNLOCK;
        MXC_FLC0->ctrl |= MXC_S_FLC_CTRL_UNLOCK_UNLOCKED;

        uint32_t data[4] = {FLASH_ERASED_WORD, FLASH_ERASED_WORD, FLASH_ERASED_WORD, FLASH_ERASED_WORD};
        uint8_t *dataBytes = (uint8_t *)data;
        uint32_t validBytes = remainingBytes >= sizeof(data) ? sizeof(data) : remainingBytes;

        // keep the valid bytes; pad the unused bytes with 0xFF (erased state)
        for (uint32_t i = 0; i < validBytes; i++)
        {
            dataBytes[i] = buffer[i];
        }

        MXC_FLC0->addr = address;
        error = MXC_FLC_Write128(address, data);

        if (error == E_NO_ERROR)
        {
            // increase address
            address += sizeof(data);

            // move buffer pointer
            buffer += sizeof(data);

            // update counter: tail branch sets remainingBytes < sizeof(data),
            // so cap to zero to avoid unsigned underflow
            if (remainingBytes >= sizeof(data))
            {
                remainingBytes -= sizeof(data);
            }
            else
            {
                remainingBytes = 0;
            }
        }
        else
        {
            break;
        }
    }

    success = (address >= endAddress);
    return success;
}

bool MXCFlashDriver_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;

    uint32_t *cursor = (uint32_t *)blockAddress;
    uint32_t *endAddress = (uint32_t *)(blockAddress + length);

    // an erased flash address has to read FLASH_ERASED_WORD
    // OK to check by word (32 bits) because the erase is performed by 'page' whose size is word multiple
    while (cursor < endAddress)
    {
        if (*cursor++ != FLASH_ERASED_WORD)
        {
            // found an address with something other than FLASH_ERASED_WORD!!
            return false;
        }
    }

    // reached here so the segment must be erased
    return true;
}

bool MXCFlashDriver_EraseBlock(void *context, ByteAddress address)
{
    (void)context;
    uint32_t error;

    // Check if flash controller is busy
    if (MXC_FLC0->ctrl & MXC_F_FLC_CTRL_PEND)
    {
        return false;
    }

    // Unlock the Flash to enable the flash control register access
    MXC_FLC0->ctrl &= ~MXC_F_FLC_CTRL_UNLOCK;
    MXC_FLC0->ctrl |= MXC_S_FLC_CTRL_UNLOCK_UNLOCKED;

    error = MXC_FLC_PageErase(address);

    return (error == E_NO_ERROR);
}
