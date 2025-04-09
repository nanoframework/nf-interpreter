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
#include <nanoHAL_StorageOperation.h>
#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>
#include <nanoPAL_COM.h>
#include <target_stdio_config.h>

extern TX_EVENT_FLAGS_GROUP wpUartEvent;
extern CLR_SETTINGS clrSettings;

// byte pool configuration and definitions
// need to be at least as big as the config sector
#define DEFAULT_BYTE_POOL_SIZE 0x2000
TX_BYTE_POOL byte_pool_0;
uint8_t __attribute__((section(".ram4"))) memory_area[DEFAULT_BYTE_POOL_SIZE];

// threads definitions and configurations

// receiver thread
#define RECEIVER_THREAD_STACK_SIZE 2048
#define RECEIVER_THREAD_PRIORITY   5

TX_THREAD receiverThread;
uint32_t __attribute__((section(".ram4"))) receiverThreadStack[RECEIVER_THREAD_STACK_SIZE / sizeof(uint32_t)];
extern void ReceiverThread_entry(uint32_t parameter);

// CLR thread
#define CLR_THREAD_STACK_SIZE 4092
#define CLR_THREAD_PRIORITY   5

TX_THREAD clrStartupThread;
uint32_t __attribute__((section(".ram4"))) clrStartupThreadStack[CLR_THREAD_STACK_SIZE / sizeof(uint32_t)];
extern void ClrStartupThread_entry(uint32_t parameter);

void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;
    uint16_t status;

    // Create a byte memory pool from which to allocate the thread stacks.
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEFAULT_BYTE_POOL_SIZE);

    // start watchdog
    Watchdog_Init();

    // turn LED2 off
    palClearPad(GPIOB, GPIOB_LD2);

#if (HAL_NF_USE_STM32_CRC == TRUE)
    // startup crc
    crcStart(NULL);
#endif

    // starts the serial driver
    sdStart(&SERIAL_DRIVER, NULL);

#if (TRACE_TO_STDIO == TRUE)
    StdioPort_Init();
#endif

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

    // CLR settings to launch CLR thread
    memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

    clrSettings.MaxContextSwitches = 50;
    clrSettings.WaitForDebugger = false;
    clrSettings.EnterDebuggerLoopAfterExit = true;

    // Create CLR startup thread
    status = tx_thread_create(
        &clrStartupThread,
        "CLR Thread",
        ClrStartupThread_entry,
        (uint32_t)&clrSettings,
        clrStartupThreadStack,
        CLR_THREAD_STACK_SIZE,
        CLR_THREAD_PRIORITY,
        CLR_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}

//  Application entry point.
int main(void)
{
    halInit();

    // init boot clipboard
    InitBootClipboard();

    // Enter the ThreadX kernel
    tx_kernel_enter();
}
