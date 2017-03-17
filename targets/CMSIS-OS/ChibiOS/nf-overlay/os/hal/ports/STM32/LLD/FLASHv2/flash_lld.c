//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

///////////////////////////////////////////////////////////////////////////////
// This driver has been tested with the following STM32 series: F4
///////////////////////////////////////////////////////////////////////////////

#include "hal.h"

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

bool flash_lld_write(uint32_t startAddress, uint32_t length, const uint8_t* buffer) {

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
                *((__IO uint16_t*)cursor++) = *((uint16_t*)buffer++);

                // update flash and buffer pointers by the 'extra' byte that was programmed
                cursor++;
                buffer++;
            }
            else
            {
                // program single byte 
                *cursor = *buffer++;
            }
            
            // wait for program operation to be completed 
            // TODO: add a timeout here using an OS function
            while(((FLASH->SR) & (FLASH_FLAG_BSY)) == (FLASH_FLAG_BSY)){};
        }

        // after the program operation is completed disable the PG Bit
        CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
        
        // lock the FLASH
        if(HAL_FLASH_Lock())
        {
            // lock succesfull, done here
            return true;
        }
    }

    // default to false
    return false;
}

bool flash_lld_isErased(uint32_t startAddress, uint32_t length) {

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
  uint8_t sector = 0;
  bool sectorInBank2 = false;

  if ((address - FLASH_BASE) >= 0x100000) {
    sectorInBank2 = true;
  } 

  // clever algorithm to find out the sector number knowing the address
  sector = sectorInBank2 ? ((address - 0x100000 - FLASH_BASE) >> 14) : ((address - FLASH_BASE) >> 14);
  
  if (sector >= 4) {
    sector = (sector >> 3) + 4;
  }

  return sector;
}

bool flash_lld_erase(uint32_t address) {
    
    uint32_t tmp_psize = 0;

    // unlock the FLASH
    if(HAL_FLASH_Unlock())
    {
        // get the sector number to erase
        uint8_t sectorNumber = flash_lld_getSector(address);

        /* Need to add offset of 4 when sector higher than FLASH_SECTOR_11 */
        if(sectorNumber > FLASH_SECTOR_11) 
        {
            sectorNumber += 4U;
        }

        // erase the sector
        CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);

        // set program size to half-word (16 bits) to be conservative
        FLASH->CR |= FLASH_PSIZE_HALF_WORD;
        CLEAR_BIT(FLASH->CR, FLASH_CR_SNB);
        FLASH->CR |= FLASH_CR_SER | (sectorNumber << POSITION_VAL(FLASH_CR_SNB));
        FLASH->CR |= FLASH_CR_STRT;
        
        // wait for erase operation to be completed 
        // TODO: add a timeout here using an OS function
        while(((FLASH->SR) & (FLASH_FLAG_BSY)) == (FLASH_FLAG_BSY)){};

        // after erase operation completed disable the SER and SNB Bits
        CLEAR_BIT(FLASH->CR, (FLASH_CR_SER | FLASH_CR_SNB));

        // lock the FLASH
        if(HAL_FLASH_Lock())
        {
            // lock succesfull, done here
            return true;
        }
    }

    // default to false
    return false;
}

#endif
