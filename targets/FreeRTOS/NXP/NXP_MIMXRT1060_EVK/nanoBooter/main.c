//
// Copyright (c) .NET Foundation and Contributors
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

#include <targetHAL.h>
#include <WireProtocol_ReceiverThread.h>
#include <nanoPAL_BlockStorage.h>
#include "nanoHAL_ConfigurationManager.h"
#include "Target_BlockStorage_iMXRTFlashDriver.h"

//configure heap memory
__attribute__((section(".noinit.$SRAM_OC.ucHeap")))
uint8_t ucHeap[configTOTAL_HEAP_SIZE];

#define LED_GPIO GPIO1
#define LED_GPIO_PIN (9U)

static void blink_task(void *pvParameters)
{
    (void)pvParameters;

    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

    /* Init output LED GPIO. */
    GPIO_PinInit(LED_GPIO, LED_GPIO_PIN, &led_config);

    for (;;)
    {
        vTaskDelay(100);
        GPIO_PortToggle(LED_GPIO, 1u << LED_GPIO_PIN);
        vTaskDelay(500);
        GPIO_PortToggle(LED_GPIO, 1u << LED_GPIO_PIN);
    }
}

static void  boot_nanoCLR(void){

    uint32_t button = 1;

    /* Define the init structure for the input button pin*/
    gpio_pin_config_t button_config = {kGPIO_DigitalInput, 0, kGPIO_NoIntmode};

     /* Init input GPIO*/
    GPIO_PinInit(BOARD_USER_BUTTON_GPIO, BOARD_USER_BUTTON_GPIO_PIN, &button_config);
    button = GPIO_PinRead(BOARD_USER_BUTTON_GPIO, BOARD_USER_BUTTON_GPIO_PIN);

    /* Button is active low.
       Load nanoCLR program through resetISR or if button is pressed init reciver Task */
    if (button) 
    {
        void (*nanoCLR)(void);
        nanoCLR = (void *) *((uint32_t *) 0x60020004); // resetISR address
        nanoCLR();
    } 
}

int main(void)
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

   // SCB_DisableDCache();

    boot_nanoCLR();

    iMXRTFlexSPIDriver_InitializeDevice(NULL);
    
    // initialize block storage list and devices
    // in CLR this is called in nanoHAL_Initialize()
    // for nanoBooter we have to init it in order to provide the flash map for Monitor_FlashSectorMap command
    BlockStorageList_Initialize();
    BlockStorage_AddDevices();

    // initialize configuration manager
    // in CLR this is called in nanoHAL_Initialize()
    // for nanoBooter we have to init it here to have access to network configuration blocks
    ConfigurationManager_Initialize();    

    xTaskCreate(blink_task, "blink_task", configMINIMAL_STACK_SIZE + 10, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(ReceiverThread, "ReceiverThread", 2048, NULL, configMAX_PRIORITIES - 1, NULL);
    vTaskStartScheduler();

    for (;;)
        ;
    return 0;
}