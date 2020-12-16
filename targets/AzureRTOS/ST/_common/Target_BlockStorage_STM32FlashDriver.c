//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>
#include <Target_BlockStorage_STM32FlashDriver.h>

#define FLASH_ERASED_WORD 0xFFU

// copied from stm32l4xx_hal_flash.c
#if defined(STM32L4P5xx) || defined(STM32L4Q5xx) || defined(STM32L4R5xx) || defined(STM32L4R7xx) ||                    \
    defined(STM32L4R9xx) || defined(STM32L4S5xx) || defined(STM32L4S7xx) || defined(STM32L4S9xx)
#define FLASH_NB_DOUBLE_WORDS_IN_ROW 64
#else
#define FLASH_NB_DOUBLE_WORDS_IN_ROW 32
#endif

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

    uint32_t address = startAddress;
    uint32_t endAddress = startAddress + numBytes;
    uint32_t remainingBytes = numBytes;
    bool success = false;
    bool useFastProgram;
    uint32_t programType;
    uint64_t data, temp;

    // Unlock the Flash to enable the flash control register access
    HAL_FLASH_Unlock();

    // TODO
    // fast program isn't working, using _just_ double word programming for now
    // check if data is multiple of 32*64bits (32*4bytes) = 128 bytes
    // if ((endAddress - address) % (sizeof(uint64_t) * FLASH_NB_DOUBLE_WORDS_IN_ROW) == 0)
    // {
    //     useFastProgram = true;
    //     programType = FLASH_TYPEPROGRAM_FAST;
    // }
    // else
    {
        programType = FLASH_TYPEPROGRAM_DOUBLEWORD;
        useFastProgram = false;
    }

    while (address < endAddress)
    {
        if (useFastProgram)
        {
            data = (uint64_t)((uint32_t)buffer);

            if (remainingBytes == (sizeof(uint64_t) * FLASH_NB_DOUBLE_WORDS_IN_ROW))
            {

                // this is the last one
                programType = FLASH_TYPEPROGRAM_FAST_AND_LAST;
            }
        }
        else if (remainingBytes > sizeof(uint64_t))
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

        success = (HAL_FLASH_Program(programType, address, data) == HAL_OK);
        if (success)
        {
            if (programType == FLASH_TYPEPROGRAM_FAST_AND_LAST || programType == FLASH_TYPEPROGRAM_FAST)
            {
                // increase address
                address += (sizeof(uint64_t) * FLASH_NB_DOUBLE_WORDS_IN_ROW);

                // move buffer pointer
                buffer += (sizeof(uint64_t) * FLASH_NB_DOUBLE_WORDS_IN_ROW);

                // update counter
                remainingBytes -= (sizeof(uint64_t) * FLASH_NB_DOUBLE_WORDS_IN_ROW);
            }
            else
            {
                // increase address
                address += sizeof(uint64_t);

                // move buffer pointer
                buffer += sizeof(uint64_t);

                // update counter
                remainingBytes -= sizeof(uint64_t);
            }
        }
        else
        {
#ifdef DEBUG
            // get error
            volatile uint32_t errorCode = HAL_FLASH_GetError();
            (void)errorCode;
#endif
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
    bool success = (HAL_FLASHEx_Erase(&EraseInitStruct, &error) == HAL_OK);

#ifdef DEBUG
    if (!success)
    {
        // DEBUG ONLY
        // get error
        volatile uint32_t errorCode = HAL_FLASH_GetError();
        (void)errorCode;
    }
#endif

    // Lock the Flash to disable the flash control register access
    HAL_FLASH_Lock();

    return success;
}
