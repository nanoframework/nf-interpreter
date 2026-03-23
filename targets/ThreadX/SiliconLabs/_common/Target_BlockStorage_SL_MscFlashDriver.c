//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_BlockStorage_SL_MscFlashDriver.h>

#define FLASH_ERASED_WORD ((uint32_t)0xFFFFFFFF)

bool SL_MscFlashDriver_InitializeDevice(void *context)
{
    (void)context;

    // Gecko SDK driver takes care of this, so always true
    return true;
}

bool SL_MscFlashDriver_UninitializeDevice(void *context)
{
    (void)context;

    // Gecko SDK driver takes care of this, so always true
    return true;
}

DeviceBlockInfo *SL_MscFlashDriver_GetDeviceInfo(void *context)
{

    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = context;

    return config->BlockConfig.BlockDeviceInformation;
}

bool SL_MscFlashDriver_Read(void *context, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer)
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

bool SL_MscFlashDriver_Write(
    void *context,
    ByteAddress startAddress,
    unsigned int numBytes,
    unsigned char *buffer,
    bool readModifyWrite)
{
    (void)context;
    (void)readModifyWrite;

    MSC_Status_TypeDef mscReturn;

    MSC_Init();

    mscReturn = MSC_WriteWord((uint32_t *)startAddress, (void const *)buffer, numBytes);

    MSC_Deinit();

    return mscReturn == mscReturnOk;
}

bool SL_MscFlashDriver_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;
    (void)blockAddress;
    (void)length;

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

bool SL_MscFlashDriver_EraseBlock(void *context, ByteAddress address)
{
    (void)context;

    return MSC_ErasePage((uint32_t *)address) == mscReturnOk;
}
