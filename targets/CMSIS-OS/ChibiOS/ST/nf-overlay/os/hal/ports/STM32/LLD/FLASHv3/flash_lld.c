//
// Copyright (c) .NET Foundation and Contributors
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
  // Unlocking FLASH_PECR register access
  if(HAL_IS_BIT_SET(FLASH->PECR, FLASH_PECR_PELOCK))
  {
    WRITE_REG(FLASH->PEKEYR, FLASH_PEKEY1);
    WRITE_REG(FLASH->PEKEYR, FLASH_PEKEY2);
  }

  if (HAL_IS_BIT_SET(FLASH->PECR, FLASH_PECR_PRGLOCK))
  {
    // Unlocking the program memory access
    WRITE_REG(FLASH->PRGKEYR, FLASH_PRGKEY1);
    WRITE_REG(FLASH->PRGKEYR, FLASH_PRGKEY2); 
  }

  return true; 
}

// Locks the FLASH control register access
bool HAL_FLASH_Lock(void)
{
  // Set the PRGLOCK Bit to lock the FLASH Registers access
  SET_BIT(FLASH->PECR, FLASH_PECR_PRGLOCK);
  
  // Set the PELOCK Bit to lock the PECR Register access
  SET_BIT(FLASH->PECR, FLASH_PECR_PELOCK);

  return true;  
}

//  Wait for a FLASH operation to complete.
bool FLASH_WaitForLastOperation(uint32_t timeout)
{
    (void)timeout;

    // Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
    // Even if the FLASH operation fails, the BUSY flag will be reset and an error flag will be set
    // no need to overload this with a timeout workflow as the watchdog will quick-in if execution gets stuck
    
    while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY));

    // Check FLASH End of Operation flag 
    if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP))
    {
        // Clear FLASH End of Operation pending bit
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
    }

    if( __HAL_FLASH_GET_FLAG(FLASH_FLAG_WRPERR)     || 
        __HAL_FLASH_GET_FLAG(FLASH_FLAG_PGAERR)     || 
        __HAL_FLASH_GET_FLAG(FLASH_FLAG_SIZERR)     || 
        __HAL_FLASH_GET_FLAG(FLASH_FLAG_OPTVERR)    || 
        __HAL_FLASH_GET_FLAG(FLASH_FLAG_RDERR)      || 
        __HAL_FLASH_GET_FLAG(FLASH_FLAG_FWWERR)     || 
        __HAL_FLASH_GET_FLAG(FLASH_FLAG_NOTZEROERR) )
    {
        return false;
    }

    // There is no error flag set
    return true;
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
            // if buffer has enough data, program words (32 bits) in a single operation to speed up things
            // NOTE: assuming that the supply voltage is able to cope with half-word programming
            if((endAddress - cursor) >= 4)
            {
                *(__IO uint32_t*)cursor = *((uint32_t*)buffer);

                //*(__IO uint32_t*)cursor = 0xBEEFBEEF;

                // update flash and buffer pointers by the 'extra' byte that was programmed
                cursor += 4;
                buffer += 4;
            }
            else
            {
                // can only program words
                // TODO add error code?
                success = false;
                break;
            }
                
            // wait for any flash operation to be completed
            // timeout set to 0 on purpose
            // watchdog will quick-in if execution gets stuck
            success = FLASH_WaitForLastOperation(0);
            
            if(!success)
            {
                // quit on failure
                break;
            }
        }

        // lock the FLASH
        HAL_FLASH_Lock();
    }

    // done here
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

uint8_t flash_lld_getSector(uint32_t address)
{
  return (address - FLASH_BASE) / FLASH_PAGE_SIZE;
}

int flash_lld_erase(uint32_t address)
{
    bool success = true;

    // unlock the FLASH
    if(HAL_FLASH_Unlock())
    {
        // Clear pending flags (if any)
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

        // erase the page

        // Set the ERASE bit
        SET_BIT(FLASH->PECR, FLASH_PECR_ERASE);

        // Set PROG bit
        SET_BIT(FLASH->PECR, FLASH_PECR_PROG);

        // Write 00000000h to the first word of the program page to erase
        *(__IO uint32_t *)(uint32_t)(address & ~(FLASH_PAGE_SIZE - 1)) = 0x00000000;

        // wait for any flash operation to be completed
        // watchdog will quick-in if execution gets stuck
        success = FLASH_WaitForLastOperation(0);

        // If the erase operation is completed, disable the ERASE Bit
        CLEAR_BIT(FLASH->PECR, FLASH_PECR_PROG);
        CLEAR_BIT(FLASH->PECR, FLASH_PECR_ERASE);

        // lock the FLASH
        HAL_FLASH_Lock();
    }

    return success;
}

#endif
