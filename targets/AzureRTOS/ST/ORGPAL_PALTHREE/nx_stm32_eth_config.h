//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation. All rights reserved.
// See LICENSE file in the project root for full license information.
//

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef NX_STM32_ETH_CONFIG_H
#define NX_STM32_ETH_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f7xx_hal.h"
#include "lan8742.h"

    extern ETH_HandleTypeDef heth;
    extern void MX_ETH_Init(void);

#define eth_handle  heth
#define nx_eth_init MX_ETH_Init

// Enable the legacy Ethernet API for STM32F7
#define STM32_ETH_HAL_LEGACY

#ifdef __cplusplus
}
#endif

#endif // NX_STM32_ETH_CONFIG_H
