//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <FreeRTOS.h>
#include <task.h>

#include <nanoHAL_v2.h>
#include <LaunchCLR.h>
#include <targetHAL.h>
#include <targetPAL.h>

#include <WireProtocol_ReceiverThread.h>
#include <nanoPAL_BlockStorage.h>

#include <stm32f0xx_hal.h>

extern TaskHandle_t xReceiverThreadHandle;

void BlinkThread(void *argument);

//configure heap memory
__attribute__((section(".noinit.$SRAM_OC.ucHeap")))
uint8_t ucHeap[configTOTAL_HEAP_SIZE];

extern void SystemClock_Config(void);

// Application entry point.
int main(void)
{
    HAL_Init();

    NVIC_SetPriorityGrouping(0U);

    SystemClock_Config();

    STM32_CRC_Init();

    STM32_UART_Init();

    STM32_GPIO_Init();

    // the following IF is not mandatory, it's just providing a way for a user to 'force'
    // the board to remain in nanoBooter and not launching nanoCLR

    // if the USER button (blue one) is pressed, skip the check for a valid CLR image and remain in booter
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
    {
        // check for valid CLR image at address contiguous to nanoBooter
        // this target DOES NOT have configuration block, so we need to use the __nanoImage_end__ address here
        if(CheckValidCLRImage((uint32_t)&__nanoImage_end__))
        {
            // there seems to be a valid CLR image
            // launch nanoCLR
            LaunchCLR((uint32_t)&__nanoImage_end__);
        }
    }
    
    // initialize block storage list and devices
    // in CLR this is called in nanoHAL_Initialize()
    // for nanoBooter we have to init it in order to provide the flash map for Monitor_FlashSectorMap command
    BlockStorageList_Initialize();
    BlockStorage_AddDevices();


    xTaskCreate(
        BlinkThread, 
        "BlinkThread", 
        1024, 
        NULL, 
        configMAX_PRIORITIES - 5, 
        NULL);

    xTaskCreate(
        ReceiverThread, 
        "ReceiverThread", 
        1024, 
        NULL, 
        configMAX_PRIORITIES - 1, 
        &xReceiverThreadHandle);

    vTaskStartScheduler();
}

void BlinkThread(void *argument)
{
    (void)argument;

    // loop forever
    while (true) 
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(500);
    }
}
