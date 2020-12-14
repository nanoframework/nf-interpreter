//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>

#ifdef HAL_CRC_MODULE_ENABLED

extern CRC_HandleTypeDef CrcHandle;

// strong implementation of this function specific to the STM32 targets
uint32_t SUPPORT_ComputeCRC(const void *rgBlock, const uint32_t nLength, const uint32_t initialCrc)
{
    uint32_t index = 0U;
    uint32_t iterations;
    uint32_t crc = 0;

    // anything to do here?
    if (nLength == 0)
    {
        return initialCrc;
    }

    // we'll be reading the buffer in steps of 4 bytes, so the size must be recalculated accordingly
    iterations = nLength >> 2;

    // get pointer to buffer
    uint8_t *ptr = (uint8_t *)rgBlock;

    uint16_t data;
    __IO uint16_t *pReg;

    // need to reset CRC peripheral if:
    // - CRC initial value is 0
    // - the initial CRC is NOT already loaded in the calculation register
    if (initialCrc != DEFAULT_CRC_INITVALUE || (CrcHandle.Instance->DR != initialCrc))
    {
        // Reset CRC Calculation Unit
        CrcHandle.Instance->CR |= CRC_CR_RESET;

        // set initial CRC value
        WRITE_REG(CrcHandle.Instance->INIT, initialCrc);
    }

    // Processing time optimization: 4 bytes are entered in a row with a single word write,
    // last bytes must be carefully fed to the CRC calculator to ensure a correct type
    // handling by the IP */
    for (index = 0; index < iterations; index++)
    {
        CrcHandle.Instance->DR = ((uint32_t)ptr[4 * index] << 24) | ((uint32_t)ptr[4 * index + 1] << 16) |
                                 ((uint32_t)ptr[4 * index + 2] << 8) | (uint32_t)ptr[4 * index + 3];
    }

    // last bytes specific handling
    if ((nLength % 4) != 0)
    {
        if (nLength % 4 == 1)
        {
            *(__IO uint8_t *)(__IO void *)(&CrcHandle.Instance->DR) = (uint8_t)ptr[4 * index];
        }
        if (nLength % 4 == 2)
        {
            data = ((uint16_t)((uint16_t)(ptr[4 * index]) << 8) | (uint16_t)(ptr[4 * index + 1]));
            pReg = (__IO uint16_t *)(__IO void *)(&CrcHandle.Instance->DR);
            *pReg = data;
        }
        if (nLength % 4 == 3)
        {
            data = ((uint16_t)((uint16_t)(ptr[4 * index]) << 8) | (uint16_t)(ptr[4 * index + 1]));
            pReg = (__IO uint16_t *)(__IO void *)(&CrcHandle.Instance->DR);
            *pReg = data;

            *(__IO uint8_t *)(__IO void *)(&CrcHandle.Instance->DR) = (uint8_t)ptr[(4 * index) + 2];
        }
    }

    crc = CrcHandle.Instance->DR;

    return crc;
};

#endif // HAL_CRC_MODULE_ENABLED
