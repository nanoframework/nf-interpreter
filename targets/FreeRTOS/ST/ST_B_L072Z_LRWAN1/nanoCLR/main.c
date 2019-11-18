//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// #include <stdio.h>
// #include "board.h"
// #include "peripherals.h"
// #include "pin_mux.h"
// #include "clock_config.h"
// #include "MIMXRT1062.h"
// #include "fsl_debug_console.h"

// #include "cmsis_os.h"

#include "FreeRTOS.h"
#include "task.h"

#include <nanoHAL_v2.h>

#include <WireProtocol_ReceiverThread.h>
// #include <nanoCLR_Application.h>
// #include "Target_BlockStorage_iMXRTFlashDriver.h"
// #include "CLR_Startup_Thread.h"

//configure heap memory
__attribute__((section(".noinit.$SRAM_OC.ucHeap")))
uint8_t ucHeap[configTOTAL_HEAP_SIZE];

// #define LED_GPIO GPIO1
// #define LED_GPIO_PIN (9U)

int main(void)
{
//       /* STM32L0xx HAL library initialization:
//        - Configure the Flash prefetch, Flash preread and Buffer caches
//        - Systick timer is configured by default as source of time base, but user 
//              can eventually implement his proper time base source (a general purpose 
//              timer for example or other time source), keeping in mind that Time base 
//              duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
//              handled in milliseconds basis.
//        - Low Level Initialization
//      */
//     HAL_Init();

//     /* Configure the System clock to 2 MHz */
//     SystemClock_Config();

    
    //xTaskCreate(ReceiverThread, "ReceiverThread", 2048, NULL, configMAX_PRIORITIES - 1, NULL);
    // xTaskCreate(CLRStartupThread, "CLRStartupThread", 8192, NULL, configMAX_PRIORITIES - 2, NULL);
    // xTaskCreate(SdCardThread, "SDCardThread", configMINIMAL_STACK_SIZE + 100, NULL, configMAX_PRIORITIES - 2, NULL);
    vTaskStartScheduler();

    for (;;);
}
