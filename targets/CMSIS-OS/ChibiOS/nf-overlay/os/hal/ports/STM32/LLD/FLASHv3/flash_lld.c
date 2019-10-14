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
  if (HAL_IS_BIT_SET(FLASH->PECR, FLASH_PECR_PRGLOCK))
  {
    /* Unlocking FLASH_PECR register access*/
    if(HAL_IS_BIT_SET(FLASH->PECR, FLASH_PECR_PELOCK))
    {  
      WRITE_REG(FLASH->PEKEYR, FLASH_PEKEY1);
      WRITE_REG(FLASH->PEKEYR, FLASH_PEKEY2);
    }
    
    /* Unlocking the program memory access */
    WRITE_REG(FLASH->PRGKEYR, FLASH_PRGKEY1);
    WRITE_REG(FLASH->PRGKEYR, FLASH_PRGKEY2);  
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK; 
}

// Locks the FLASH control register access
HAL_StatusTypeDef HAL_FLASH_Lock(void)
{
  /* Set the PRGLOCK Bit to lock the FLASH Registers access */
  SET_BIT(FLASH->PECR, FLASH_PECR_PRGLOCK);
  
  return HAL_OK;  
}

//  Wait for a FLASH operation to complete.
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t timeout)
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
        // Save the error code
        return HAL_ERROR;
    }

    // There is no error flag set
    return HAL_OK;
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

    HAL_StatusTypeDef status = HAL_ERROR;

    __IO uint8_t* cursor = (__IO uint8_t*)startAddress;
    __IO uint8_t* endAddress = (__IO uint8_t*)(startAddress + length);

    // unlock the FLASH
    if(HAL_FLASH_Unlock())
    {
        while(cursor < endAddress)
        {
            // Wait for last operation to be completed
            status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
            
            if(status == HAL_OK)
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
            }
        }
        
        // lock the FLASH
        HAL_FLASH_Lock();
    }

    // done here
    return (status == HAL_OK);
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
  return (address - FLASH_BASE) / FLASH_PAGE_SIZE;
}

int flash_lld_erase(uint32_t address) {

    // unlock the FLASH
    if(HAL_FLASH_Unlock())
    {
        // erase the page

        // Set the ERASE bit
        SET_BIT(FLASH->PECR, FLASH_PECR_ERASE);

        // Set PROG bit
        SET_BIT(FLASH->PECR, FLASH_PECR_PROG);

        // Write 00000000h to the first word of the program page to erase
        *(__IO uint32_t *)(uint32_t)(address & ~(FLASH_PAGE_SIZE - 1)) = 0x00000000;

        // Wait for last operation to be completed
        FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

        // If the erase operation is completed, disable the ERASE Bit
        CLEAR_BIT(FLASH->PECR, FLASH_PECR_PROG);
        CLEAR_BIT(FLASH->PECR, FLASH_PECR_ERASE);

        // lock the FLASH
        HAL_FLASH_Lock();

        // done here
        return true;
    }

    // default to false
    return false;
}

#endif
