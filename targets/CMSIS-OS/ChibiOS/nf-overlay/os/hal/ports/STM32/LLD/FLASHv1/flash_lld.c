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

#if (HAL_NF_USE_STM32_FLASH == TRUE)

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
  bool status = true;

  if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
  {
    // Authorize the FLASH Registers access
    WRITE_REG(FLASH->KEYR, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR, FLASH_KEY2);

    // Verify Flash is unlocked
    if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
    {
      status = false;
    }
  }

  return status; 
}

// Locks the FLASH control register access
bool HAL_FLASH_Lock(void)
{
  // Set the LOCK Bit to lock the FLASH Registers access
  SET_BIT(FLASH->CR, FLASH_CR_LOCK);
  
  return true;  
}

bool FLASH_WaitForLastOperation(uint32_t timeout)
{
    (void)timeout;

    bool success = true;
 
    // Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
    // Even if the FLASH operation fails, the BUSY flag will be reset and an error flag will be set
    // no need to overload this with a timeout workflow as the watchdog will quick-in if execution gets stuck

    while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));
        
    // Check FLASH End of Operation flag 
    if( __HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP) )
    {
        // Clear FLASH End of Operation pending bit
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
    }

    if( __HAL_FLASH_GET_FLAG(FLASH_FLAG_WRPERR) ||
        __HAL_FLASH_GET_FLAG(FLASH_FLAG_PGERR))
    {
        // at this point we don't care about the details of the programming error
        success = false;
    }

    return success;
}

///////////////////////////////////////////////////////////////////////////////
// Driver exported functions.                                                //
///////////////////////////////////////////////////////////////////////////////

void flash_lld_init()
{
    stm32FlashObjectInit(&STM32FLASH);
}

void flash_lld_readBytes(uint32_t startAddress, uint32_t length, uint8_t* buffer)
{

    __IO uint8_t* cursor = (__IO uint8_t*)startAddress;
    __IO uint8_t* endAddress = (__IO uint8_t*)(startAddress + length);

    // copy contents from flash to buffer starting from the start address 
    while(cursor < endAddress)
    {
        *buffer++ = *cursor++;
    }
}

int flash_lld_write(uint32_t startAddress, uint32_t length, const uint8_t* buffer)
{
    bool success = true;
    
    __IO uint8_t* cursor = (__IO uint8_t*)startAddress;
    __IO uint8_t* endAddress = (__IO uint8_t*)(startAddress + length);

    // unlock the FLASH
    if(HAL_FLASH_Unlock())
    {
        // Clear pending flags (if any)
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

        while(cursor < endAddress)
        {
            // proceed to program the flash by setting the PG Bit
            SET_BIT(FLASH->CR, FLASH_CR_PG);

            // if buffer has enough data, program half-words (16 bits) in a single operation to speed up things
            // NOTE: assuming that the supply voltage is able to cope with half-word programming
            if((endAddress - cursor) >= 2)
            {
                // Data synchronous Barrier, forcing the CPU to respect the sequence of instruction without optimization
                __DSB();
                
                *(__IO uint16_t*)cursor = *((uint16_t*)buffer);

                // update flash and buffer pointers by the 'extra' byte that was programmed
                cursor += 2;
                buffer += 2;
            }
            else
            {
                // Data synchronous Barrier, forcing the CPU to respect the sequence of instruction without optimization
                __DSB();

                // program single byte
                *(__IO uint8_t*)cursor = *buffer;
                // update flash pointer by the 'extra' byte that was programmed
                cursor += 2;
            }

            // wait for any flash operation to be completed
            // timeout set to 0 on purpose
            // watchdog will quick-in if execution gets stuck
            success = FLASH_WaitForLastOperation(0);

            // after each program operation disable the PG Bit
            CLEAR_BIT(FLASH->CR, FLASH_CR_PG);

            if(!success)
            {
                // quit on failure
                break;
            }
        }
        
        // lock the FLASH
        HAL_FLASH_Lock();
    }

    return success;
}

int flash_lld_isErased(uint32_t startAddress, uint32_t length)
{
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

int flash_lld_erase(uint32_t address)
{
    bool success = true;

    //unlock the FLASH
    if(HAL_FLASH_Unlock())
    {
        // Clear pending flags (if any)
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

        // erase the page
        SET_BIT(FLASH->CR, FLASH_CR_PER);
        WRITE_REG(FLASH->AR, address);
        SET_BIT(FLASH->CR, FLASH_CR_STRT);    

        // wait for any flash operation to be completed
        // watchdog will quick-in if execution gets stuck
        success = FLASH_WaitForLastOperation(0);

        // after erase operation completed disable the PER Bit
        CLEAR_BIT(FLASH->CR, FLASH_CR_PER);

        // lock the FLASH
        HAL_FLASH_Lock();
    }

    return success;
}

#endif
