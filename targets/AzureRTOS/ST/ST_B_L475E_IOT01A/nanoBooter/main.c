//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Headers.h>
#include <stm32l4xx_hal.h>
#include <stm32l475e_iot01.h>

#include <cmsis_utils.h>
#include <tx_api.h>

#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_ConfigurationManager.h>

void BoardInit(bool initSensors);

extern TX_EVENT_FLAGS_GROUP wpUartEvent;

// byte pool configuration and definitions
#define DEMO_BYTE_POOL_SIZE     4096
TX_BYTE_POOL            byte_pool_0;
uint8_t memory_area[DEMO_BYTE_POOL_SIZE];

// threads definitions and configurations
#define RECEIVER_THREAD_STACK_SIZE 2048
#define RECEIVER_THREAD_PRIORITY   4

TX_THREAD receiverThread;
uint32_t receiverThreadStack[RECEIVER_THREAD_STACK_SIZE / sizeof(uint32_t)];
extern void ReceiverThread_entry(uint32_t parameter);

// blink thread
#define BLINK_THREAD_STACK_SIZE 1024
#define BLINK_THREAD_PRIORITY   4

TX_THREAD blinkThread;
uint32_t blinkThreadStack[BLINK_THREAD_STACK_SIZE / sizeof(uint32_t)];

void BlinkThread_entry(uint32_t parameter)
{
    (void)parameter;

    while (1)
    {
        BSP_LED_Toggle(LED2);
        tx_thread_sleep(50);
    }
}

void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;

    systick_interval_set(TX_TIMER_TICKS_PER_SECOND);

    // Create a byte memory pool from which to allocate the thread stacks.
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEMO_BYTE_POOL_SIZE);

    // Create blink thread
    uint16_t status = tx_thread_create(
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

    // Create receiver thread
    status = tx_thread_create(
        &receiverThread,
        "Receiver Thread",
        ReceiverThread_entry,
        0,
        receiverThreadStack,
        RECEIVER_THREAD_STACK_SIZE,
        RECEIVER_THREAD_PRIORITY,
        RECEIVER_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }

    // create UART event group
    status = tx_event_flags_create(&wpUartEvent, "wpUart event");
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
    
    // report successfull nanoBooter execution
    ReportSuccessfullNanoBooter();
}

//  Application entry point.
int main(void)
{
    // init board WITHOUT sensors
    BoardInit(false);

    // init boot clipboard
    InitBootClipboard();

    // initialize block storage list and devices
    // in CLR this is called in nanoHAL_Initialize()
    // for nanoBooter we have to init it in order to provide the flash map for Monitor_FlashSectorMap command
    BlockStorageList_Initialize();
    BlockStorage_AddDevices();

    // initialize configuration manager
    // in CLR this is called in nanoHAL_Initialize()
    // for nanoBooter we have to init it here to have access to network configuration blocks
    ConfigurationManager_Initialize();

    // Enter the ThreadX kernel
    tx_kernel_enter();
}
