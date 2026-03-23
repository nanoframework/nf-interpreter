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

    uint32_t address = startAddress;
    uint32_t endAddress = startAddress + numBytes;
    uint32_t remainingBytes = numBytes;
    bool success = false;
    uint32_t error;
    uint64_t data, temp;

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

        if (remainingBytes >= sizeof(uint64_t))
        {
            data = *(uint64_t *)buffer;
        }
        else
        {
            temp = *(uint64_t *)buffer;

            // "move" data to start of position
            data = temp << sizeof(uint8_t) * (sizeof(uint64_t) - remainingBytes);

            // need to clear the "remaining" bytes
            data |= 0xFFFFFFFFFFFFFFFF;
        }

        MXC_FLC0->addr = address;
        error = MXC_FLC_Write128(address, (uint32_t*)&data);

        if (error == E_NO_ERROR)
        {
            // increase address
            address += sizeof(uint64_t);

            // move buffer pointer
            buffer += sizeof(uint64_t);

            // update counter
            remainingBytes -= sizeof(uint64_t);
        }
        else
        {
#ifdef DEBUG
            // get error
            // volatile uint32_t errorCode = HAL_FLASH_GetError();
            // (void)errorCode;
#endif
            break;
        }
    }

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
