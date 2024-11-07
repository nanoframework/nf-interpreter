//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetHAL.h>
#include <nanoCLR_Application.h>

#include <sl_system_init.h>
#include <sl_system_kernel.h>
#include <em_gpio.h>
#include <sl_event_handler.h>
#include <sl_board_init.h>
#include <sl_status.h>

#include <tx_api.h>

#include <nanoPAL_BlockStorage.h>
// #include <nanoHAL_ConfigurationManager.h>

// flags for hardware events
TX_EVENT_FLAGS_GROUP nanoHardwareEvents;
extern CLR_SETTINGS clrSettings;

// byte pool configuration and definitions
// need to be at least as big as the config sector
#define DEFAULT_BYTE_POOL_SIZE 0x2000
TX_BYTE_POOL byte_pool_0;
uint8_t memory_area[DEFAULT_BYTE_POOL_SIZE];

// threads definitions and configurations
#define BLINK_THREAD_STACK_SIZE 1024
#define BLINK_THREAD_PRIORITY   5

TX_THREAD blinkThread;
uint32_t blinkThreadStack[BLINK_THREAD_STACK_SIZE / sizeof(uint32_t)];

// receiver thread
#define RECEIVER_THREAD_STACK_SIZE 2048
#define RECEIVER_THREAD_PRIORITY   5

TX_THREAD receiverThread;
uint32_t receiverThreadStack[RECEIVER_THREAD_STACK_SIZE / sizeof(uint32_t)];
extern void ReceiverThread_entry(uint32_t parameter);

// CLR thread
#define CLR_THREAD_STACK_SIZE 4092
#define CLR_THREAD_PRIORITY   5

TX_THREAD clrStartupThread;
uint32_t clrStartupThreadStack[CLR_THREAD_STACK_SIZE / sizeof(uint32_t)];
extern void ClrStartupThread_entry(uint32_t parameter);

extern sl_status_t sl_usbd_vendor_read_bulk_sync(
    uint8_t class_nbr,
    void *p_buf,
    uint32_t buf_len,
    uint16_t timeout,
    uint32_t *p_xfer_len);
extern sl_status_t sl_usbd_vendor_is_enabled(uint8_t class_nbr, bool *p_enabled);
extern void UsbStackInit();

void BlinkThread_entry(uint32_t parameter)
{
    (void)parameter;

    GPIO_PinOutToggle(gpioPortB, 12);

    UsbStackInit();

    while (1)
    {
        GPIO_PinOutToggle(gpioPortB, 12);
        tx_thread_sleep(TX_TICKS_PER_MILLISEC(1500));
    }
}

void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;
    uint16_t status;

    // Create a byte memory pool from which to allocate the thread stacks.
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEFAULT_BYTE_POOL_SIZE);

    // start watchdog
    Watchdog_Init();

#if (TRACE_TO_STDIO == TRUE)
    StdioPort_Init();
#endif

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
    clrSettings.RevertToBooterOnFault = true;

    // do NOT enter debugger loop on RTM builds
    // this will cause the board to keep rebooting until there is an application deployed
#if defined(BUILD_RTM)
    clrSettings.EnterDebuggerLoopAfterExit = false;
#else
    clrSettings.EnterDebuggerLoopAfterExit = true;
#endif

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

    // create HW event group
    status = tx_event_flags_create(&nanoHardwareEvents, "");
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
    sl_system_init();

    // configure LED READY for output
    GPIO_PinModeSet(gpioPortB, 12, gpioModePushPull, 0);

    // init boot clipboard
    InitBootClipboard();

    // Enter the ThreadX kernel. Task(s) created in tx_application_define() will start running
    sl_system_kernel_start();
}
