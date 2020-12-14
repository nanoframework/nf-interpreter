//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>
#include <tx_api.h>

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    (void)hrtc;

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    // Configue LSE as RTC clock source
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while (1)
        {
        }
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        while (1)
        {
        }
    }

    // Enable RTC Clock
    __HAL_RCC_RTC_ENABLE();
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
    (void)hrtc;

    // Reset peripherals
    __HAL_RCC_RTC_DISABLE();
}

void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
    if (hrng->Instance == RNG)
    {
        // Peripheral clock enable
        __HAL_RCC_RNG_CLK_ENABLE();
    }
}

void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng)
{
    if (hrng->Instance == RNG)
    {
        // Peripheral clock disable
        __HAL_RCC_RNG_CLK_DISABLE();
    }
}

uint32_t HAL_GetTick(void)
{
    return tx_time_get() * 10;
}
