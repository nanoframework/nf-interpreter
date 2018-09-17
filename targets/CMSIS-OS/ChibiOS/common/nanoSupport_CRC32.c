//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <hal.h>

#if defined(STM32F0xx_MCUCONF) || defined(STM32F1xx_MCUCONF) || defined(STM32F2xx_MCUCONF) || \
	defined(STM32F4xx_MCUCONF) || defined(STM32F7xx_MCUCONF) || defined(STM32L0xx_MCUCONF) || \
	defined(STM32L1xx_MCUCONF) || defined(STM32H7xx_MCUCONF) 

// strong implementation of this function specific to the STM32 targets
unsigned int SUPPORT_ComputeCRC(const void* rgBlock, int nLength, unsigned int crc)
{
	crcAquireModule();

	crc = crcCompute(rgBlock, nLength, crc);

	crcReleaseModule();

    return crc;
};

#endif
