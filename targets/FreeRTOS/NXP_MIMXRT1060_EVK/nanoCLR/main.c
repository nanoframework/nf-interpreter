//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1062.h"
#include "fsl_debug_console.h"

#include "FreeRTOS.h"
#include "task.h"

#include <nanoHAL_v2.h>

#include <WireProtocol_ReceiverThread.h>
#include <nanoCLR_Application.h>
#include "Target_BlockStorage_iMXRTFlashDriver.h"
#include "CLR_Startup_Thread.h"

//configure heap memory
__attribute__((section(".noinit.$SRAM_OC.ucHeap")))
uint8_t ucHeap[configTOTAL_HEAP_SIZE];

#define LED_GPIO GPIO1
#define LED_GPIO_PIN (9U)

int main(void)
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitSEMC();
    BOARD_USDHCClockConfiguration();
    //SCB_DisableDCache();

    for (volatile uint32_t i = 0; i < 100000000; i++)
    {
        __asm("nop");
    }

    iMXRTFlexSPIDriver_InitializeDevice(NULL);
    
    xTaskCreate(ReceiverThread, "ReceiverThread", 2048, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(CLRStartupThread, "CLRStartupThread", 8192, NULL, configMAX_PRIORITIES - 2, NULL);
    xTaskCreate(SdCardThread, "SDCardThread", configMINIMAL_STACK_SIZE + 100, NULL, configMAX_PRIORITIES - 2, NULL);
    vTaskStartScheduler();

    for (;;)
        ;
    return 0;
}