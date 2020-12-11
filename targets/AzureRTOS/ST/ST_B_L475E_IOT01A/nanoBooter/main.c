//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdio.h>
#include <stm32l4xx_hal.h>
#include <stm32l475e_iot01.h>

// #include "tx_api.h"

// #include "board_init.h"
// #include "cmsis_utils.h"

void SystemClock_Config(void);

//  Application entry point.
int main(void)
{
    HAL_Init();

    SystemClock_Config();

    BSP_LED_Init(LED2);

    // Initialize the board
    // board_init();

    // Enter the ThreadX kernel
    //tx_kernel_enter();

    while (1)
    {
        BSP_LED_Toggle(LED2);
        HAL_Delay(500);
    }
}
