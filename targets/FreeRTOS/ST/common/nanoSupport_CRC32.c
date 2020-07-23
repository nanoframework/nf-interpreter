//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <common.h>

CRC_HandleTypeDef   CrcHandle;

// strong implementation of this function specific to the TI Simple Link targets
uint32_t SUPPORT_ComputeCRC(const void* rgBlock, const uint32_t nLength, const uint32_t crc)
{
	// anything to do here?
    if(nLength == 0)
    {
        return crc;
    }

    if(crc == 0)
    {
        return HAL_CRC_Calculate(&CrcHandle, (uint32_t *)rgBlock, nLength);
    }
    else
    {
        return HAL_CRC_Accumulate(&CrcHandle, (uint32_t *)rgBlock, nLength);
    }
};

void STM32_CRC_Init()
{
    CrcHandle.Instance = CRC;

    CrcHandle.Init.DefaultPolynomialUse    = DEFAULT_POLYNOMIAL_ENABLE;
    CrcHandle.Init.DefaultInitValueUse     = DEFAULT_INIT_VALUE_DISABLE;
    CrcHandle.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_NONE;
    CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    CrcHandle.InputDataFormat              = CRC_INPUTDATA_FORMAT_BYTES;

    if (HAL_CRC_Init(&CrcHandle) != HAL_OK)
    {
        /* Initialization Error */
        //Error_Handler();
    }   
}
