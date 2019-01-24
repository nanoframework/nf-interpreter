//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

///////////////////////////////////////////////////////////////////////////////
// This driver has been tested with the following STM32 series: F4
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
  if((FLASH->CR & FLASH_CR_LOCK) != RESET)
  {
    // Authorize the FLASH Registers access
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
  }
  else
  {
    return false;
  }

  return true; 
}

// Locks the FLASH control register access
void HAL_FLASH_Lock(void)
{
  // Set the LOCK Bit to lock the FLASH Registers access
  FLASH->CR |= FLASH_CR_LOCK;
}

bool FLASH_WaitForLastOperation(uint32_t timeout)
{ 
    // Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
    //  Even if the FLASH operation fails, the BUSY flag will be reset and an error
    //  flag will be set

    systime_t start = chVTGetSystemTime();
    systime_t end = start + TIME_MS2I(timeout);
    
    while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) != RESET) 
    { 
        // do nothing until the timeout expires
        if(chVTIsSystemTimeWithin(start, end))
        {
            __NOP();
        }
        else
        {
            return false;
        }
    }
    
    if(__HAL_FLASH_GET_FLAG(FLASH_FLAG_ALL_ERRORS) != RESET)
    {
      return false;
    }

    // Check FLASH End of Operation flag
    if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP) != RESET)
    {
        // Clear FLASH End of Operation pending bit
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
    }

    // If there is no error flag set
    return true;
}  

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx) ||\
    defined(STM32F469xx) || defined(STM32F479xx) ||\
    defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F412Zx) ||\
    defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
    defined(STM32F401xC) ||\
    defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) ||\
    defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx)

void FLASH_FlushCaches(void)
{
  // Flush instruction cache 
  if(READ_BIT(FLASH->ACR, FLASH_ACR_ICEN)!= RESET)
  {
    // Disable instruction cache
    __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
    // Reset instruction cache
    __HAL_FLASH_INSTRUCTION_CACHE_RESET();
    // Enable instruction cache
    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
  }
  
  // Flush data cache
  if(READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != RESET)
  {
    // Disable data cache 
    __HAL_FLASH_DATA_CACHE_DISABLE();
    // Reset data cache
    __HAL_FLASH_DATA_CACHE_RESET();
    // Enable data cache
    __HAL_FLASH_DATA_CACHE_ENABLE();
  }
}
#endif // define STM32F4xxxx

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

    // default to false
    bool success = false;

    // unlock the FLASH
    if(HAL_FLASH_Unlock())
    {
        // Clear pending flags (if any)
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

        while(cursor < endAddress)
        {
            // if buffer has enough data, program half-words (16 bits) in a single operation to speed up things
            // NOTE: assuming that the supply voltage is able to cope with half-word programming
            if((endAddress - cursor) >= 2)
            {
                // clear the program size mask
                FLASH->CR &= CR_PSIZE_MASK;
                // set the size of of the programming word to HALF WORD
                FLASH->CR |= FLASH_PSIZE_HALF_WORD;
                // proceed to program the flash by setting the PG Bit
                FLASH->CR |= FLASH_CR_PG;

                *(__IO uint16_t*)cursor = *((uint16_t*)buffer);

#if defined(STM32F756xx) || defined(STM32F746xx) || defined(STM32F745xx) || defined(STM32F767xx) || \
    defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx) || defined(STM32F722xx) || \
	defined(STM32F723xx) || defined(STM32F732xx) || defined(STM32F733xx)
    
                // Data synchronous Barrier, forcing the CPU to respect the sequence of instruction without optimization
                __DSB();
#endif
                // update flash and buffer pointers by the 'extra' byte that was programmed
                cursor += 2;
                buffer += 2;
            }
            else
            {
                // program single byte

                // clear the program size mask
                FLASH->CR &= CR_PSIZE_MASK;
                // set the size of of the programming word to BYTE
                FLASH->CR |= FLASH_PSIZE_BYTE;
                // proceed to program the flash by setting the PG Bit
                FLASH->CR |= FLASH_CR_PG;

                *(__IO uint8_t*)cursor = *buffer;

#if defined(STM32F756xx) || defined(STM32F746xx) || defined(STM32F745xx) || defined(STM32F767xx) || \
    defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx) || defined(STM32F722xx) || \
	defined(STM32F723xx) || defined(STM32F732xx) || defined(STM32F733xx)
    
                // Data synchronous Barrier, forcing the CPU to respect the sequence of instruction without optimization
                __DSB();
#endif                

                // update flash pointer by the 'extra' byte that was programmed
                cursor += 2;
            }
                
            // wait 500ms for any flash operation to be completed
            success = FLASH_WaitForLastOperation(500);
            
            if(!success)
            {
                // quit on failure
                break;
            }
        }

        // after the program operation is completed disable the PG Bit
        FLASH->CR &= (~FLASH_CR_PG);
                
        // lock the FLASH
        HAL_FLASH_Lock();
    }

    return success;
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
  uint8_t sector = 0;

  if((address < ADDR_FLASH_SECTOR_1) && (address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((address < ADDR_FLASH_SECTOR_2) && (address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((address < ADDR_FLASH_SECTOR_3) && (address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((address < ADDR_FLASH_SECTOR_4) && (address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;
  }

// need to wrap the else ifs bellow because not all target devices have all the sectors defined
#if defined(FLASH_SECTOR_4)
  else if((address < ADDR_FLASH_SECTOR_5) && (address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;
  }
#endif
#if defined(FLASH_SECTOR_5)
  else if((address < ADDR_FLASH_SECTOR_6) && (address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;
  }
#endif
#if defined(FLASH_SECTOR_6)
  else if((address < ADDR_FLASH_SECTOR_7) && (address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;
  }
#endif
// the next one is for devices that have FLASH sector 8 defined (see next one for devices WITHOUT sector 8)
#if defined(FLASH_SECTOR_7) && defined(FLASH_SECTOR_8)
  else if((address < ADDR_FLASH_SECTOR_8) && (address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;
  }
#endif
// this next one needs to be here for devices that don't have FLASH sector 8
#if defined(FLASH_SECTOR_7) && !defined(FLASH_SECTOR_8)
  else if(address >= ADDR_FLASH_SECTOR_7)
  {
    sector = FLASH_SECTOR_7;
  }
#endif
#if defined(FLASH_SECTOR_8)
  else if((address < ADDR_FLASH_SECTOR_9) && (address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;
  }
#endif
#if defined(FLASH_SECTOR_9)
  else if((address < ADDR_FLASH_SECTOR_10) && (address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;
  }
#endif
#if defined(FLASH_SECTOR_10)
  else if((address < ADDR_FLASH_SECTOR_11) && (address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;
  }
#endif  
  else /* (address < FLASH_END_ADDR) && (address >= ADDR_FLASH_SECTOR_11) */
  {
    sector = FLASH_SECTOR_11;
  }

  return sector;
}

int flash_lld_erase(uint32_t address) {

    // default to false
    bool success = false;

    // unlock the FLASH
    if(HAL_FLASH_Unlock())
    {
        // Clear pending flags (if any)  
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
        
        // get the sector number to erase
        uint8_t sectorNumber = flash_lld_getSector(address);

        // Need to add offset of 4 when sector higher than FLASH_SECTOR_11
        if(sectorNumber > FLASH_SECTOR_11) 
        {
            sectorNumber += 4U;
        }

        // If the previous operation is completed, proceed to erase the sector
        CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
        SET_BIT(FLASH->CR, FLASH_PSIZE_HALF_WORD);
        CLEAR_BIT(FLASH->CR, FLASH_CR_SNB);
        FLASH->CR |= FLASH_CR_SER | (sectorNumber << POSITION_VAL(FLASH_CR_SNB));
        
        // start erase operation
        FLASH->CR |= FLASH_CR_STRT;

#if defined(STM32F756xx) || defined(STM32F746xx) || defined(STM32F745xx) || defined(STM32F767xx) || \
    defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx) || defined(STM32F722xx) || \
	  defined(STM32F723xx) || defined(STM32F732xx) || defined(STM32F733xx)
    
        // Data synchronous Barrier, forcing the CPU to respect the sequence of instruction without optimization
        __DSB();
#endif

        // wait 500ms for any flash operation to be completed
        success = FLASH_WaitForLastOperation(2000);

        // after erase operation completed disable the SER and SNB Bits
        CLEAR_BIT(FLASH->CR, (FLASH_CR_SER | FLASH_CR_SNB));

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx) ||\
    defined(STM32F469xx) || defined(STM32F479xx) ||\
    defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F412Zx) ||\
    defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
    defined(STM32F401xC) ||\
    defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) ||\
    defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx)

        // Flush the caches to be sure of the data consistency
        FLASH_FlushCaches();
#endif

        // lock the FLASH
        HAL_FLASH_Lock();
    }

    return success;
}

#endif
