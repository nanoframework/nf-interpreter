//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

///////////////////////////////////////////////////////////////////////////////
// This driver has been tested with the following STM32 series: L4
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
STM32FlashDriver STM32FLASH;

///////////////////////////////////////////////////////////////////////////////
// Driver local variables and types.                                         //
///////////////////////////////////////////////////////////////////////////////
uint32_t CacheToReactivate;

// helper structure to write flash
union {
    uint32_t w[STM32_FLASH_LINE_SIZE / sizeof(uint32_t)];
    uint8_t b[STM32_FLASH_LINE_SIZE / sizeof(uint8_t)];
} line;

///////////////////////////////////////////////////////////////////////////////
// Driver local functions.                                                   //
///////////////////////////////////////////////////////////////////////////////

// Unlock the FLASH control register access
bool HAL_FLASH_Unlock(void)
{
    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U)
    {
        // Authorize the FLASH Registers access
        WRITE_REG(FLASH->KEYR, FLASH_KEY1);
        WRITE_REG(FLASH->KEYR, FLASH_KEY2);

        // Verify Flash is unlocked
        if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U)
        {
            return false;
        }
    }

    return true;
}

// Locks the FLASH control register access
bool HAL_FLASH_Lock(void)
{
    //   Set the LOCK Bit to lock the FLASH Registers access
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);

    return true;
}

//  Wait for a FLASH operation to complete.
void FLASH_WaitForLastOperation(uint32_t timeout)
{
    (void)timeout;

    // Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
    // Even if the FLASH operation fails, the BUSY flag will be reset and an error flag will be set
    // no need to overload this with a timeout workflow as the watchdog will quick-in if execution gets stuck

    while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
        ;
}

//  Check for flash operation errors
uint32_t FLASH_CheckErrors()
{
    uint32_t error = (FLASH->SR & FLASH_FLAG_SR_ERRORS);

    // Clear error programming flags
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_SR_ERRORS);

    return error;
}

static uint32_t GetBank(uint32_t address)
{
    uint32_t bank = 0;

    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
    {
        /* No Bank swap */
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
        /* Bank swap */
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

static uint32_t GetPage(uint32_t address)
{
    uint32_t page = 0;

    if (address < (FLASH_BASE + FLASH_BANK_SIZE))
    {
        /* Bank 1 */
        page = (address - FLASH_BASE) / FLASH_PAGE_SIZE;
    }
    else
    {
        /* Bank 2 */
        page = (address - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
    }

    return page;
}

///////////////////////////////////////////////////////////////////////////////
// Driver exported functions.                                                //
///////////////////////////////////////////////////////////////////////////////

void flash_lld_init()
{
    stm32FlashObjectInit(&STM32FLASH);
}

void flash_lld_readBytes(uint32_t startAddress, uint32_t length, uint8_t *buffer)
{

    __IO uint8_t *cursor = (__IO uint8_t *)startAddress;
    __IO uint8_t *endAddress = (__IO uint8_t *)(startAddress + length);

    // copy contents from flash to buffer starting from the start address
    while (cursor < endAddress)
    {
        *buffer++ = *cursor++;
    }
}

int flash_lld_write(uint32_t startAddress, uint32_t length, const uint8_t *buffer)
{
    uint32_t offset = startAddress - FLASH_BASE;
    uint32_t remainingBytes = length;
    uint32_t error = FLASH_NO_ERROR;

    // Unlock the Flash to enable the flash control register access
    HAL_FLASH_Unlock();

    // Set PG bit
    SET_BIT(FLASH->CR, FLASH_CR_PG);

    while (remainingBytes > 0)
    {
        volatile uint32_t *address;

        // unwritten bytes are initialized to flahs erase value
        line.w[0] = 0xFFFFFFFFU;
        line.w[1] = 0xFFFFFFFFU;

        // Programming address aligned to flash lines.
        address = (volatile uint32_t *)(FLASH_BASE + (offset & ~STM32_FLASH_LINE_MASK));

        // Copying data inside the prepared line
        do
        {
            line.b[offset & STM32_FLASH_LINE_MASK] = *buffer;
            offset++;
            remainingBytes--;
            buffer++;
        } while ((remainingBytes > 0U) & ((offset & STM32_FLASH_LINE_MASK) != 0U));

        // write to flash
        address[0] = line.w[0];

        // Barrier to ensure programming is performed in 2 steps, in right order
        // (independently of compiler optimization behavior)
        __ISB();

        address[1] = line.w[1];

        FLASH_WaitForLastOperation(0);

        error = FLASH_CheckErrors();
        if (error != FLASH_NO_ERROR)
        {
            break;
        }
    }

    // disable the PG or FSTPG Bit
    CLEAR_BIT(FLASH->CR, FLASH_CR_PG);

    // Lock the Flash to disable the flash control register access
    HAL_FLASH_Lock();

    // done here
    return (error == FLASH_NO_ERROR);
}

int flash_lld_isErased(uint32_t startAddress, uint32_t length)
{
    __IO uint32_t *cursor = (__IO uint32_t *)startAddress;
    __IO uint32_t *endAddress = (__IO uint32_t *)(startAddress + length);

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

uint8_t flash_lld_getSector(uint32_t address)
{
    return (address - FLASH_BASE) / FLASH_PAGE_SIZE;
}

int flash_lld_erase(uint32_t address)
{
    bool success = true;
    uint32_t bankNumber = GetBank(address);
    uint32_t page = GetPage(address);

    // unlock the FLASH
    if (HAL_FLASH_Unlock())
    {
        // Clear pending flags (if any)
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

#if defined(STM32L471xx) || defined(STM32L475xx) || defined(STM32L476xx) || defined(STM32L485xx) ||                    \
    defined(STM32L486xx) || defined(STM32L496xx) || defined(STM32L4A6xx) || defined(STM32L4P5xx) ||                    \
    defined(STM32L4Q5xx) || defined(STM32L4R5xx) || defined(STM32L4R7xx) || defined(STM32L4R9xx) ||                    \
    defined(STM32L4S5xx) || defined(STM32L4S7xx) || defined(STM32L4S9xx)
#if defined(STM32L4P5xx) || defined(STM32L4Q5xx) || defined(STM32L4R5xx) || defined(STM32L4R7xx) ||                    \
    defined(STM32L4R9xx) || defined(STM32L4S5xx) || defined(STM32L4S7xx) || defined(STM32L4S9xx)
        if (READ_BIT(FLASH->OPTR, FLASH_OPTR_DBANK) == 0U)
        {
            CLEAR_BIT(FLASH->CR, FLASH_CR_BKER);
        }
        else
#endif
        {
            if ((bankNumber & FLASH_BANK_1) != 0U)
            {
                CLEAR_BIT(FLASH->CR, FLASH_CR_BKER);
            }
            else
            {
                SET_BIT(FLASH->CR, FLASH_CR_BKER);
            }
        }
#endif

        // Proceed to erase the page
        MODIFY_REG(FLASH->CR, FLASH_CR_PNB, ((page & 0xFFU) << FLASH_CR_PNB_Pos));
        SET_BIT(FLASH->CR, FLASH_CR_PER);
        SET_BIT(FLASH->CR, FLASH_CR_STRT);

        // wait for any flash operation to be completed
        // watchdog will quick-in if execution gets stuck
        FLASH_WaitForLastOperation(0);
        success = (FLASH_CheckErrors() == FLASH_NO_ERROR);

        // If the erase operation is completed, disable the PER Bit
        CLEAR_BIT(FLASH->CR, (FLASH_CR_PER | FLASH_CR_PNB));

        // lock the FLASH
        HAL_FLASH_Lock();
    }

    return success;
}

#endif
