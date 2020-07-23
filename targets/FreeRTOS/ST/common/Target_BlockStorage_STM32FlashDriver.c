//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <common.h>
#include <Target_BlockStorage_STM32FlashDriver.h>

#define FLASH_ERASED_WORD        0x0FFFFFFFFU

bool STM32FlashDriver_InitializeDevice(void* context)
{
    (void)context;

    // STM32 HAL takes care of this, so always true
    return true;
}

bool STM32FlashDriver_UninitializeDevice(void* context)
{
    (void)context;

    // STM32 HAL takes care of this, so always true
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

    __IO uint8_t* cursor = (__IO uint8_t*)startAddress;
    __IO uint8_t* endAddress = (__IO uint8_t*)(startAddress + numBytes);

    // copy contents from flash to buffer starting from the start address 
    while(cursor < endAddress)
    {
        *buffer++ = *cursor++;
    }

    // ... and always return true
    return true;
}

bool STM32FlashDriver_Write(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite)
{
    (void)context;
    (void)readModifyWrite;
    
    bool success = true;
    
    __IO uint16_t* cursor = (__IO uint16_t*)startAddress;
    __IO uint16_t* endAddress = (__IO uint16_t*)(startAddress + numBytes);

    HAL_FLASH_Unlock();

    while (cursor < endAddress)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, *(__IO uint16_t*)cursor, *buffer) == HAL_OK)
        {
            cursor++;
        }
        else
        {
            success = false;
            break;
        }
    }

    HAL_FLASH_Lock();

    return success;
}

bool STM32FlashDriver_IsBlockErased(void* context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;

    __IO uint32_t* cursor = (__IO uint32_t*)blockAddress;
    __IO uint32_t* endAddress = (__IO uint32_t*)(blockAddress + length);

    // an erased flash address has to read FLASH_ERASED_WORD
    // OK to check by word (32 bits) because the erase is performed by 'page' whose size is word multiple
    while(cursor < endAddress)
    {
        if(*cursor++ != FLASH_ERASED_WORD)
        {
            // found an address with something other than FLASH_ERASED_WORD!!
            return false;
        }
    }
    
    // reached here so the segment must be erased
    return true;
}

bool STM32FlashDriver_EraseBlock(void* context, ByteAddress address)
{
    (void)context;
    bool success;
    uint32_t pageError = 0;

    FLASH_EraseInitTypeDef eraseInitStruct;

    HAL_FLASH_Unlock();

    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.PageAddress = address;
    eraseInitStruct.NbPages = 1;

    success = (HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) == HAL_OK); 

    HAL_FLASH_Lock();

    return success;
}
