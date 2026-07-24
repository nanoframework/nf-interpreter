//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <hal_nf_community.h>
#include <cmsis_os.h>

#include <serialcfg.h>
#include <swo.h>
#include <targetHAL.h>
#include <CLR_Startup_Thread.h>
#include <WireProtocol_ReceiverThread.h>
#include <nanoCLR_Application.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_v2.h>
#include <targetPAL.h>

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityHigh, 2048, "ReceiverThread");
// declare CLRStartup thread here
osThreadDef(CLRStartupThread, osPriorityNormal, 4096, "CLRStartupThread");

// TEMPORARY DIAGNOSTIC: samples the USART1 RX/TX queue fill levels to check whether bytes
// are physically arriving/leaving the UART during a deploy stall, independent of thread
// scheduling. Read via OpenOCD/gdb: g_diagMaxQueueBytes, g_diagLastQueueBytes,
// g_diagMaxTxQueueBytes, g_diagLastTxQueueBytes, g_diagSampleCount.
volatile uint32_t g_diagMaxQueueBytes = 0;
volatile uint32_t g_diagLastQueueBytes = 0;
volatile uint32_t g_diagMaxTxQueueBytes = 0;
volatile uint32_t g_diagLastTxQueueBytes = 0;
volatile uint32_t g_diagSampleCount = 0;

static THD_WORKING_AREA(waDiagThread, 256);
static THD_FUNCTION(DiagThread, arg)
{
    (void)arg;
    chRegSetThreadName("DiagThread");

    while (true)
    {
        chSysLock();
        uint32_t bytesWaiting = iqGetFullI(&SERIAL_DRIVER.iqueue);
        uint32_t bytesPendingTx = oqGetFullI(&SERIAL_DRIVER.oqueue);
        chSysUnlock();

        g_diagLastQueueBytes = bytesWaiting;
        if (bytesWaiting > g_diagMaxQueueBytes)
        {
            g_diagMaxQueueBytes = bytesWaiting;
        }

        g_diagLastTxQueueBytes = bytesPendingTx;
        if (bytesPendingTx > g_diagMaxTxQueueBytes)
        {
            g_diagMaxTxQueueBytes = bytesPendingTx;
        }

        g_diagSampleCount++;

        chThdSleepMilliseconds(2);
    }
}

//  Application entry point.
int main(void)
{
    halInit();

    // init boot clipboard
    InitBootClipboard();

    // init SWO as soon as possible to make it available to output ASAP
#if CONFIG_CHIBIOS_SWO_OUTPUT
    SwoInit();
#endif

    // The kernel is initialized but not started yet, this means that
    // main() is executing with absolute priority but interrupts are already enabled.
    osKernelInitialize();

    // start watchdog
    Watchdog_Init();

#if (HAL_NF_USE_STM32_CRC == TRUE)
    // startup crc
    crcStart(NULL);
#endif

    // starts the serial driver
    sdStart(&SERIAL_DRIVER, NULL);

    // TEMPORARY DIAGNOSTIC: start the RX queue sampling thread
    chThdCreateStatic(waDiagThread, sizeof(waDiagThread), NORMALPRIO, DiagThread, NULL);

    // create the receiver thread
    osThreadCreate(osThread(ReceiverThread), NULL);

    // CLR settings to launch CLR thread
    CLR_SETTINGS clrSettings;
    (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

    clrSettings.MaxContextSwitches = 50;
    clrSettings.WaitForDebugger = false;
    clrSettings.EnterDebuggerLoopAfterExit = true;

    // create the CLR Startup thread
    osThreadCreate(osThread(CLRStartupThread), &clrSettings);

    // start kernel, after this main() will behave like a thread with priority osPriorityNormal
    osKernelStart();

    while (true)
    {
        osDelay(100);
    }
}
