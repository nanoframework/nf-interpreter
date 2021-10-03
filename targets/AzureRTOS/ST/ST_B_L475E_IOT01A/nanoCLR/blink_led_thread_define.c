//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_nf_community.h>

#include <tx_api.h>

#include <serialcfg.h>
#include <LaunchCLR.h>
#include <targetHAL.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_ConfigurationManager.h>
#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>
#include <nanoPAL_COM.h>
#include <target_stdio_config.h>

extern TX_EVENT_FLAGS_GROUP wpUartEvent;
extern CLR_SETTINGS clrSettings;


// threads definitions and configurations

// threads definitions and configurations
// blink thread
#define BLINK_THREAD_STACK_SIZE 1024
#define BLINK_THREAD_PRIORITY   5
TX_THREAD blinkThread;
uint32_t blinkThreadStack[BLINK_THREAD_STACK_SIZE / sizeof(uint32_t)];

void BlinkThread_entry(uint32_t parameter)
{
    (void)parameter;

    while (1)
    {
        palTogglePad(GPIOB, GPIOB_LD2);
        tx_thread_sleep(TX_TICKS_PER_MILLISEC(500));
    }
}
void blink_led_thread_define(void *first_unused_memory)
{
    (void)first_unused_memory;
    uint16_t status;
    // Create blink thread
    status = tx_thread_create(
        &blinkThread,
        "Blink Thread",
        BlinkThread_entry,
        0,
        blinkThreadStack,
        BLINK_THREAD_STACK_SIZE,
        BLINK_THREAD_PRIORITY,
        BLINK_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }

}

