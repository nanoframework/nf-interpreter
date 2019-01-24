//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_nf_community.h>

#if defined(STM32F0XX) || defined(STM32F1XX) || defined(STM32F2XX) || \
	defined(STM32F4XX) || defined(STM32F7XX) || defined(STM32L0XX) || \
	defined(STM32L1XX) || defined(STM32H7XX) 

// strong implementation of this function specific to the STM32 targets
unsigned int SUPPORT_ComputeCRC(const void* rgBlock, int nLength, unsigned int crc)
{
	crcAquireModule();

	crc = crcCompute(rgBlock, nLength, crc);

	crcReleaseModule();

    return crc;
};

#endif
