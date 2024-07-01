//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_nf_community.h>

#include "usbcfg.h"
#include <nanoCLR_Application.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_v2.h>
#include <targetPAL.h>

extern CLR_SETTINGS clrSettings;

// TODO
// #if HAL_USE_SDC
// // declare SD Card working thread here
// osThreadDef(SdCardWorkingThread, osPriorityNormal, 1024, "SDCWT");
// #endif
// #if HAL_USBH_USE_MSD
// // declare USB MSD thread here
// osThreadDef(UsbMsdWorkingThread, osPriorityNormal, 1024, "USBMSDWT");
// #endif

// byte pool configuration and definitions
// need to be at least as big as the config sector
#define DEFAULT_BYTE_POOL_SIZE 0x9000
TX_BYTE_POOL byte_pool_0;
uint8_t memory_area[DEFAULT_BYTE_POOL_SIZE];

// threads definitions and configurations

// receiver thread
#define RECEIVER_THREAD_STACK_SIZE 2048
#define RECEIVER_THREAD_PRIORITY   5

TX_THREAD receiverThread;
uint32_t receiverThreadStack[RECEIVER_THREAD_STACK_SIZE / sizeof(uint32_t)];
extern void ReceiverThread_entry(uint32_t parameter);

TX_THREAD netxDuoThread;
uint32_t netxDuoThreadStack[RECEIVER_THREAD_STACK_SIZE / sizeof(uint32_t)];
extern void NetXDuoThread_entry(uint32_t parameter);

// CLR thread
#define CLR_THREAD_STACK_SIZE 4096
#define CLR_THREAD_PRIORITY   5

TX_THREAD clrStartupThread;
uint32_t clrStartupThreadStack[CLR_THREAD_STACK_SIZE / sizeof(uint32_t)];
extern void ClrStartupThread_entry(uint32_t parameter);

void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;
    uint16_t status;

    // Create a byte memory pool from which to allocate the thread stacks.
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEFAULT_BYTE_POOL_SIZE);

    // start watchdog
    Watchdog_Init();

#if (HAL_NF_USE_STM32_CRC == TRUE)
    // startup crc
    crcStart(NULL);
#endif

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
        // TODO replace with proper handling
        while (1)
        {
        }
    }

    // Create NetX Duo thread
    status = tx_thread_create(
        &netxDuoThread,
        "NetXDuo Thread",
        NetXDuoThread_entry,
        0,
        netxDuoThreadStack,
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
        // TODO replace with proper handling
        while (1)
        {
        }
    }
}

//  Application entry point.
int main(void)
{
    // find out wakeup reason
    if ((PWR->CSR1 & PWR_CSR1_WUIF) == PWR_CSR1_WUIF)
    {
        // standby, match WakeupReason_FromStandby enum
        WakeupReasonStore = 1;
    }
    else if ((PWR->CSR2 & (PWR_CSR2_WUPF3 | PWR_CSR2_WUPF2 | PWR_CSR2_WUPF1)))
    {
        // wake from pin, match WakeupReason_FromPin enum
        WakeupReasonStore = 2;
    }
    else
    {
        // undetermined reason, match WakeupReason_Undetermined enum
        WakeupReasonStore = 0;
    }

    // first things first: need to clear any possible wakeup flags
    // clear wakeup flags from GPIOs
    PWR->CR2 |= (PWR_CR2_CWUPF6 | PWR_CR2_CWUPF5 | PWR_CR2_CWUPF4 | PWR_CR2_CWUPF3 | PWR_CR2_CWUPF2 | PWR_CR2_CWUPF1);
    // clear standby Flag
    PWR->CR1 |= PWR_CR1_CSBF;

    __ISB();

    halInit();

    // init boot clipboard
    InitBootClipboard();

    // set default values for GPIOs
    palClearPad(GPIOE, GPIOE_PIN4);
    palClearLine(LINE_RELAY);
    palSetPad(GPIOJ, GPIOJ_PIN13);
    palClearPad(GPIOJ, GPIOJ_PIN14);
    palClearLine(LINE_LCD_ENABLE);

    // config and init external memory
    // this has to be called after osKernelInitialize, otherwise an hard fault will occur
    Target_ExternalMemoryInit();

    //  Initializes a serial-over-USB CDC driver.
    sduObjectInit(&SERIAL_DRIVER);
    sduStart(&SERIAL_DRIVER, &serusbcfg);

    // Activates the USB driver and then the USB bus pull-up on D+.
    // Note, a delay is inserted in order to not have to disconnect the cable after a reset.
    usbDisconnectBus(serusbcfg.usbp);
    osalThreadSleepS(OSAL_MS2I(100));
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    // Enter the ThreadX kernel
    tx_kernel_enter();
}
