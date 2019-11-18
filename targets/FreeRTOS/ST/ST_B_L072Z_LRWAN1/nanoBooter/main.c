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
// #include "board.h"


#include "FreeRTOS.h"
#include "task.h"

#include <nanoHAL_v2.h>

#include <WireProtocol_ReceiverThread.h>
// #include <nanoCLR_Application.h>
// #include "Target_BlockStorage_iMXRTFlashDriver.h"
// #include "CLR_Startup_Thread.h"

// #include "stm32l0xx_hal.h"

#include <board.h>

#include <uart.h>
extern Uart_t Uart2;

//configure heap memory
__attribute__((section(".noinit.$SRAM_OC.ucHeap")))
uint8_t ucHeap[configTOTAL_HEAP_SIZE];

// extern void SystemClock_Config(void);
// extern UART_HandleTypeDef UartHandle;

// #define LED_GPIO GPIO1
// #define LED_GPIO_PIN (9U)

int main(void)
{
    // HAL_Init();

    // SystemClock_Config();
      
    // DBG_Init();

    // HW_Init();


    // Target board initialization
    BoardInitMcu( );
    BoardInitPeriph( );
    // HAL_Init();

    // SystemClock_Config();

    // UartHandle.Instance        = USART2;

    // UartHandle.Init.BaudRate   = 921600;
    // UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    // UartHandle.Init.StopBits   = UART_STOPBITS_1;
    // UartHandle.Init.Parity     = UART_PARITY_NONE;
    // UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    // UartHandle.Init.Mode       = UART_MODE_TX_RX;
    // if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
    // {
    //     //Error_Handler();
    // }  
    // if(HAL_UART_Init(&UartHandle) != HAL_OK)
    // {
    //     //Error_Handler();
    // }

// for(;;)
// {
//     if( IsFifoFull( &Uart2.FifoRx ))
//     {

//     }
// }
    xTaskCreate(ReceiverThread, "ReceiverThread", 1024, NULL, configMAX_PRIORITIES - 1, NULL);
    // xTaskCreate(CLRStartupThread, "CLRStartupThread", 8192, NULL, configMAX_PRIORITIES - 2, NULL);
    // xTaskCreate(SdCardThread, "SDCardThread", configMINIMAL_STACK_SIZE + 100, NULL, configMAX_PRIORITIES - 2, NULL);
    vTaskStartScheduler();
    /* Start scheduler */
    //osKernelStart();

    for (;;);
}
