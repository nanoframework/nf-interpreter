//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <Target_BlockStorage_STM32FlashDriver.h>

// -- //

bool STM32FlashDriver_InitializeDevice(void *context)
{
    (void)context;

    // nothing to do here
    return true;
}

bool STM32FlashDriver_UninitializeDevice(void *context)
{
    (void)context;

    // nothing to do here
    return true;
}

DeviceBlockInfo *STM32FlashDriver_GetDeviceInfo(void *context)
{

    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = context;

    return config->BlockConfig.BlockDeviceInformation;
}

bool STM32FlashDriver_Read(void *context, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer)
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

bool STM32FlashDriver_Write(
    void *context,
    ByteAddress startAddress,
    unsigned int numBytes,
    unsigned char *buffer,
    bool readModifyWrite)
{
    (void)context;
    (void)readModifyWrite;

    return flashProgram(
        &EFLD1, 
        startAddress,
        numBytes,
        buffer) == FLASH_NO_ERROR;
}

bool STM32FlashDriver_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;
(void)length;
    
    return flashVerifyErase(
        &EFLD1,
        blockAddress) == FLASH_NO_ERROR;
}

bool STM32FlashDriver_EraseBlock(void *context, ByteAddress address)
{
    (void)context;

#ifdef DEBUG    
    flash_error_t flashError;
#endif

#ifdef DEBUG    
    flashError = flashStartEraseSector(&EFLD1, address);
#else
    if(flashStartEraseSector(&EFLD1, address) == FLASH_NO_ERROR)
#endif
    {

#ifdef DEBUG    
        flashError = flashQueryErase(&EFLD1, NULL);

        return flashError == FLASH_NO_ERROR;
#else
        return flashQueryErase(&EFLD1, NULL) == FLASH_NO_ERROR;
#endif
    }

    return false;
}
