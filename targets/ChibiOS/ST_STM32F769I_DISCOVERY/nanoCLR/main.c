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

#ifdef NF_FEATURE_HAS_MCUBOOT
#include <MCUboot_StartupPolicy.h>
#endif

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityHigh, 2048, "ReceiverThread");
// declare CLRStartup thread here
osThreadDef(CLRStartupThread, osPriorityNormal, 6144, "CLRStartupThread");

#if HAL_USE_SDC
// declare SD Card working thread here
osThreadDef(SdCardWorkingThread, osPriorityNormal, 1024, "SDCWT");
#endif
#if HAL_USBH_USE_MSD
// declare USB MSD thread here
osThreadDef(UsbMsdWorkingThread, osPriorityNormal, 1024, "USBMSDWT");
#endif

//  Application entry point.
int main(void)
{
    // HAL initialization, this also initializes the configured device drivers
    // and performs the board-specific initializations.
    halInit();

#ifdef NF_FEATURE_HAS_MCUBOOT
    // Initialize MCUboot startup policies for both CLR and deploy images
    nf_mcuboot_startup_init();
#endif

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

    // config and init external memory
    // this has to be called after osKernelInitialize, otherwise an hard fault will occur
    Target_ExternalMemoryInit();

    // starts the serial driver
    sdStart(&SERIAL_DRIVER, NULL);

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

#if HAL_USE_SDC
    // creates the SD card working thread
    osThreadCreate(osThread(SdCardWorkingThread), NULL);
#endif

#if HAL_USBH_USE_MSD
    // create the USB MSD working thread
    osThreadCreate(osThread(UsbMsdWorkingThread), &MSBLKD[0]);
#endif

    // start kernel, after this main() will behave like a thread with priority osPriorityNormal
    osKernelStart();

    while (true)
    {
        osDelay(100);
    }
}
