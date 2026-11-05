//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

///////////////////////////////////////////////////////////////////////////////
// This driver has been tested with the following STM32 series: H7 (single bank)
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

// timeout for a flash operation (sector erase is the longest one, up to 4 seconds on STM32H7)
#define FLASH_OPERATION_TIMEOUT_MS 5000U

// buffer for a flash word
static union
{
    uint32_t w[FLASH_NB_32BITWORD_IN_FLASHWORD];
    uint8_t b[STM32_FLASH_LINE_SIZE];
} line;

///////////////////////////////////////////////////////////////////////////////
// Driver local functions.                                                   //
///////////////////////////////////////////////////////////////////////////////

static bool FLASH_Unlock(void)
{
    if ((FLASH->CR1 & FLASH_CR_LOCK) != 0U)
    {
        // Authorize the FLASH Bank1 Registers access
        FLASH->KEYR1 = FLASH_KEY1;
        FLASH->KEYR1 = FLASH_KEY2;

        // check that unlock succeeded
        if ((FLASH->CR1 & FLASH_CR_LOCK) != 0U)
        {
            return false;
        }
    }

    return true;
}

static void FLASH_Lock(void)
{
    // Set the LOCK Bit to lock the FLASH Bank1 Control Register access
    FLASH->CR1 |= FLASH_CR_LOCK;
}

static void FLASH_ClearErrors(void)
{
    // CCR1 clear bits are at the same position as the SR1 flags
    FLASH->CCR1 = FLASH_FLAG_ALL_ERRORS_BANK1 | FLASH_SR_EOP;
}

static bool FLASH_WaitForLastOperation(void)
{
    // Wait for the FLASH operation to complete by polling on QW flag to be reset.
    // the timeout has to be longer than the worst case sector erase time
    systime_t start = osalOsGetSystemTimeX();

    while ((FLASH->SR1 & FLASH_SR_QW) != 0U)
    {
        if (osalTimeDiffX(start, osalOsGetSystemTimeX()) > OSAL_MS2I(FLASH_OPERATION_TIMEOUT_MS))
        {
            return false;
        }
    }

    if ((FLASH->SR1 & FLASH_FLAG_ALL_ERRORS_BANK1) != 0U)
    {
        FLASH_ClearErrors();
        return false;
    }

    // Clear FLASH End of Operation pending bit
    if ((FLASH->SR1 & FLASH_SR_EOP) != 0U)
    {
        FLASH->CCR1 = FLASH_CCR_CLR_EOP;
    }

    return true;
}

static void FLASH_InvalidateCache(uint32_t address, uint32_t length)
{
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    // flash contents changed behind the D-cache back
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U)
    {
        SCB_InvalidateDCache_by_Addr((void *)address, (int32_t)length);
    }
#else
    (void)address;
    (void)length;
#endif
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

// H7 flash is programmed by flash words (256 bits) with ECC, which means that a flash word can be programmed only once
// after an erase. Partial writes are merged with the current flash word contents. Writing to a flash word that is
// already programmed is only allowed if the contents don't change.
int flash_lld_write(uint32_t startAddress, uint32_t length, const uint8_t *buffer)
{
    uint32_t offset = startAddress;
    uint32_t remainingBytes = length;
    bool success = true;

    // the range to write has to be inside the flash bank (the comparisons are written so they can't overflow)
    if (startAddress < FLASH_BANK1_BASE || startAddress > FLASH_END || length > FLASH_END - startAddress + 1U)
    {
        return false;
    }

    if (!FLASH_Unlock())
    {
        return false;
    }

    FLASH_ClearErrors();

    while (remainingBytes > 0U)
    {
        // Programming address aligned to flash words
        volatile uint32_t *address = (volatile uint32_t *)(offset & ~STM32_FLASH_LINE_MASK);
        bool isErased = true;
        bool isChanged = false;

        // load current flash word contents
        for (uint32_t i = 0; i < FLASH_NB_32BITWORD_IN_FLASHWORD; i++)
        {
            line.w[i] = address[i];

            if (line.w[i] != FLASH_ERASED_WORD)
            {
                isErased = false;
            }
        }

        // copy data inside the prepared flash word
        do
        {
            if (line.b[offset & STM32_FLASH_LINE_MASK] != *buffer)
            {
                line.b[offset & STM32_FLASH_LINE_MASK] = *buffer;
                isChanged = true;
            }

            offset++;
            remainingBytes--;
            buffer++;
        } while ((remainingBytes > 0U) && ((offset & STM32_FLASH_LINE_MASK) != 0U));

        if (!isChanged)
        {
            // nothing to program here
            continue;
        }

        if (!isErased)
        {
            // can't program a flash word twice
            success = false;
            break;
        }

        // proceed to program the flash by setting the PG Bit
        SET_BIT(FLASH->CR1, FLASH_CR_PG);

        __ISB();
        __DSB();

        // program the flash word
        for (uint32_t i = 0; i < FLASH_NB_32BITWORD_IN_FLASHWORD; i++)
        {
            address[i] = line.w[i];
        }

        __ISB();
        __DSB();

        success = FLASH_WaitForLastOperation();

        // after the program operation is completed disable the PG Bit
        CLEAR_BIT(FLASH->CR1, FLASH_CR_PG);

        if (!success)
        {
            // quit on failure
            break;
        }
    }

    FLASH_Lock();

    FLASH_InvalidateCache(startAddress & ~STM32_FLASH_LINE_MASK, length + STM32_FLASH_LINE_SIZE);

    return success;
}

int flash_lld_isErased(uint32_t startAddress, uint32_t length)
{
    __IO uint32_t *cursor = (__IO uint32_t *)startAddress;
    __IO uint32_t *endAddress = (__IO uint32_t *)(startAddress + length);

    // an erased flash address has to read FLASH_ERASED_WORD
    // OK to check by word (32 bits) because the erase is performed by 'sector' whose size is word multiple
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
    return (uint8_t)((address - FLASH_BANK1_BASE) / FLASH_SECTOR_SIZE);
}

int flash_lld_erase(uint32_t address)
{
    bool success;

    if (!FLASH_Unlock())
    {
        return false;
    }

    FLASH_ClearErrors();

    // erase sector
    MODIFY_REG(
        FLASH->CR1,
        FLASH_CR_SNB,
        FLASH_CR_SER | ((uint32_t)flash_lld_getSector(address) << FLASH_CR_SNB_Pos));

    // start erase operation
    FLASH->CR1 |= FLASH_CR_START;

    __DSB();

    success = FLASH_WaitForLastOperation();

    // after erase operation completed disable the SER and SNB Bits
    CLEAR_BIT(FLASH->CR1, (FLASH_CR_SER | FLASH_CR_SNB));

    FLASH_Lock();

    FLASH_InvalidateCache(address & ~(FLASH_SECTOR_SIZE - 1U), FLASH_SECTOR_SIZE);

    return success;
}

#endif
