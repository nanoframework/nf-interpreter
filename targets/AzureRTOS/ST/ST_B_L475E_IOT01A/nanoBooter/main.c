//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Headers.h>
#include <stm32l4xx_hal.h>
#include <stm32l475e_iot01.h>

// #include "tx_api.h"

// #include "board_init.h"
// #include "cmsis_utils.h"

void BoardInit(bool initSensors);

//  Application entry point.
int main(void)
{
    // init board WITHOUT sensors
    BoardInit(false);

    // Enter the ThreadX kernel
    //tx_kernel_enter();

    while (1)
    {
        BSP_LED_Toggle(LED2);
        HAL_Delay(500);
    }
}
