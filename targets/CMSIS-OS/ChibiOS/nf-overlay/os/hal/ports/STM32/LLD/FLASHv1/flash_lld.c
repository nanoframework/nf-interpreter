//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

///////////////////////////////////////////////////////////////////////////////
// This driver has been tested with the following STM32 series: F0
///////////////////////////////////////////////////////////////////////////////

#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_USE_STM32_FLASH == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver local definitions.                                                 //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver exported variables.                                                //
///////////////////////////////////////////////////////////////////////////////
SMT32FlashDriver STM32FLASH;

///////////////////////////////////////////////////////////////////////////////
// Driver local variables and types.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver local functions.                                                   //
///////////////////////////////////////////////////////////////////////////////

// Unlock the FLASH control register access
bool HAL_FLASH_Unlock(void)
{
  if (HAL_IS_BIT_SET(FLASH->CR, FLASH_CR_LOCK))
  {
    /* Authorize the FLASH Registers access */
    WRITE_REG(FLASH->KEYR, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR, FLASH_KEY2);
  }
  else
  {
    return false;
  }

  return true; 
}

// Locks the FLASH control register access
bool HAL_FLASH_Lock(void)
{
  /* Set the LOCK Bit to lock the FLASH Registers access */
  SET_BIT(FLASH->CR, FLASH_CR_LOCK);
  
  return true;  
}

///////////////////////////////////////////////////////////////////////////////
// Driver exported functions.                                                //
///////////////////////////////////////////////////////////////////////////////

void flash_lld_init() {
    stm32FlashObjectInit(&STM32FLASH);
}

void flash_lld_readBytes(uint32_t startAddress, uint32_t length, uint8_t* buffer) {

    __IO uint8_t* cursor = (__IO uint8_t*)startAddress;
    __IO uint8_t* endAddress = (__IO uint8_t*)(startAddress + length);

    // copy contents from flash to buffer starting from the start address 
    while(cursor < endAddress)
    {
        *buffer++ = *cursor++;
    }
}

int flash_lld_write(uint32_t startAddress, uint32_t length, const uint8_t* buffer) {

    __IO uint8_t* cursor = (__IO uint8_t*)startAddress;
    __IO uint8_t* endAddress = (__IO uint8_t*)(startAddress + length);

    // unlock the FLASH
    if(HAL_FLASH_Unlock())
    {
        // proceed to program the flash by setting the PG Bit
        SET_BIT(FLASH->CR, FLASH_CR_PG);

        while(cursor < endAddress)
        {
            // if buffer has enough data, program half-words (16 bits) in a single operation to speed up things
            // NOTE: assuming that the supply voltage is able to cope with half-word programming
            if((endAddress - cursor) >= 2)
            {
                *(__IO uint16_t*)cursor = *((uint16_t*)buffer);

                // update flash and buffer pointers by the 'extra' byte that was programmed
                cursor += 2;
                buffer += 2;
            }
            else
            {
                // program single byte
                *(__IO uint8_t*)cursor = *buffer;

                // update flash pointer by the 'extra' byte that was programmed
                cursor += 2;
            }
            
            // wait for program operation to be completed 
            // TODO: add a timeout here using an OS function
            while(((FLASH->SR) & (FLASH_FLAG_BSY)) == (FLASH_FLAG_BSY)){};
        }

        // after the program operation is completed disable the PG Bit
        CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
        
        // lock the FLASH
        HAL_FLASH_Lock();
        
        // done here
        return true;
    }

    // default to false
    return false;
}

int flash_lld_isErased(uint32_t startAddress, uint32_t length) {

    __IO uint32_t* cursor = (__IO uint32_t*)startAddress;
    __IO uint32_t* endAddress = (__IO uint32_t*)(startAddress + length);

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

uint8_t flash_lld_getSector(uint32_t address)
{
  return (address - FLASH_BASE) / F0_SERIES_SECTOR_SIZE;
}

int flash_lld_erase(uint32_t address) {

    // unlock the FLASH
    if(HAL_FLASH_Unlock())
    {
        // erase the page
        SET_BIT(FLASH->CR, FLASH_CR_PER);
        WRITE_REG(FLASH->AR, address);
        SET_BIT(FLASH->CR, FLASH_CR_STRT);    

        // wait for erase operation to be completed 
        // TODO: add a timeout here using an OS function
        while(((FLASH->SR) & (FLASH_FLAG_BSY)) == (FLASH_FLAG_BSY)){};

        // after erase operation completed disable the PER Bit
        CLEAR_BIT(FLASH->CR, FLASH_CR_PER);

        // lock the FLASH
        HAL_FLASH_Lock();

        // done here
        return true;
    }

    // default to false
    return false;
}

#endif
