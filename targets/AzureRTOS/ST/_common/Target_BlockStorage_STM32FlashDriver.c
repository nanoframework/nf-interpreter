//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>
#include <Target_BlockStorage_STM32FlashDriver.h>

#define FLASH_ERASED_WORD 0xFFU

// helper functions
// Gets the page of a given address
static uint32_t GetPage(uint32_t address)
{
    uint32_t page = 0;

    if (address < (FLASH_BASE + FLASH_BANK_SIZE))
    {
        // Bank 1
        page = (address - FLASH_BASE) / FLASH_PAGE_SIZE;
    }
    else
    {
        // Bank 2
        page = (address - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
    }

    return page;
}

// Gets the bank of a given address
static uint32_t GetBank(uint32_t address)
{
    uint32_t bank = 0;

    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
    {
        // No Bank swap
        if (address < (FLASH_BASE + FLASH_BANK_SIZE))
        {
            bank = FLASH_BANK_1;
        }
        else
        {
            bank = FLASH_BANK_2;
        }
    }
    else
    {
        // Bank swap
        if (address < (FLASH_BASE + FLASH_BANK_SIZE))
        {
            bank = FLASH_BANK_2;
        }
        else
        {
            bank = FLASH_BANK_1;
        }
    }

    return bank;
}

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

    bool success = false;

    // Unlock the Flash to enable the flash control register access
    HAL_FLASH_Unlock();

    uint32_t address = startAddress;
    uint32_t endAddress = startAddress + numBytes;
    uint32_t programType = FLASH_TYPEPROGRAM_FAST;
    uint32_t remainingBytes = numBytes;
    uint32_t data32;

    while (address < endAddress)
    {
        if(remainingBytes > 4)
        {
            data32 = *(uint32_t*)buffer;
        }
        else
        {
            data32 = *(uint32_t*)buffer;

            // need to clear the "remaining" bytes
            for(uint32_t i = 0; i < remainingBytes; i++)
            {
                data32 |= 0xFF << 8 * i;
            }

            // this is the last one
            programType = FLASH_TYPEPROGRAM_FAST_AND_LAST;
        }
        

        success = HAL_FLASH_Program(programType, address, data32) == HAL_OK;
        if (success)
        {
            // increse address
            address = address + 4;
            
            // move buffer pointer
            (uint32_t*)buffer++;

            // update counter
            remainingBytes -=4;
        }
        else
        {
            break;
        }
    }

    // Lock the Flash to disable the flash control register access
    HAL_FLASH_Lock();

    return success;
}

bool STM32FlashDriver_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;

    volatile uint8_t *cursor = (volatile uint8_t *)blockAddress;
    volatile uint8_t *endAddress = (volatile uint8_t *)(blockAddress + length);

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

bool STM32FlashDriver_EraseBlock(void *context, ByteAddress address)
{
    (void)context;

    // Unlock the Flash to enable the flash control register access
    HAL_FLASH_Unlock();

    uint32_t page = GetPage(address);
    uint32_t bank = GetBank(address);

    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks = bank;
    EraseInitStruct.Page = page;
    EraseInitStruct.NbPages = 1;

    uint32_t error = 0;
    bool success = (HAL_FLASHEx_Erase(&EraseInitStruct, &error) != HAL_OK);

    // Lock the Flash to disable the flash control register access
    HAL_FLASH_Lock();

    return success;
}
