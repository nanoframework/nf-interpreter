//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_BlockStorage_STM32FlashDriver.h>


bool STM32FlashDriver_InitializeDevice(void* context)
{
    (void)context;

    // ChibiOS driver takes care of this, so always true
    return true;
}

bool STM32FlashDriver_UninitializeDevice(void* context)
{
    (void)context;

    // ChibiOS driver takes care of this, so always true
    return true;
}

DeviceBlockInfo* STM32FlashDriver_GetDeviceInfo(void* context)
{
    
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = context;
    
    return config->BlockConfig.BlockDeviceInformation;  
}

bool STM32FlashDriver_Read(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer)
{
    (void)context;

    // driver doesn't return anything so: we have to perform the operation here...
    stm32FlashReadBytes(startAddress, numBytes, buffer);

    // ... and always return true
    return true;
}

bool STM32FlashDriver_Write(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite)
{
    (void)context;
    (void)readModifyWrite;

    return stm32FlashWrite(startAddress, numBytes, buffer);
}

bool STM32FlashDriver_IsBlockErased(void* context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;

    return stm32FlashIsErased(blockAddress, length);
}

bool STM32FlashDriver_EraseBlock(void* context, ByteAddress address)
{
    (void)context;

    return stm32FlashErase(address);
}
