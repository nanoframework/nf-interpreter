//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <usbcfg.h>
#include <targetHAL.h>
#include <WireProtocol_ReceiverThread.h>
#include <nanoPAL_BlockStorage.h>
#include <LaunchCLR.h>

// RP2350 memory windows used to validate CLR vector table before launch.
#define RP2350_SRAM_START         0x20000000UL
#define RP2350_SRAM_END_EXCLUSIVE 0x20090000UL
// CLR region ends at 0x10194000 on Pico 2 W (1536KB CLR, expanded for WiFi+TLS)
#define RP2350_DEPLOYMENT_START   0x10194000UL

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityHigh, 2048, "ReceiverThread");

//  Application entry point.
int main(void)
{
    // HAL initialization, this also initializes the configured device drivers
    // and performs the board-specific initializations.
    halInit();

    // init boot clipboard
    InitBootClipboard();

    // The kernel is initialized but not started yet, this means that
    // main() is executing with absolute priority but interrupts are already enabled.
    osKernelInitialize();

    // check if there is a request to remain on nanoBooter
    if (!IsToRemainInBooter())
    {
        // check for valid CLR image at address contiguous to nanoBooter
        // NOTE: The standard CheckValidCLRImage() opcode check fails on RP2350 due to XIP flash read
        // behavior. Use a simple vector table validation instead.
        uint32_t clrBase = (uint32_t)&__nanoImage_end__;
        uint32_t clrEndExclusive = RP2350_DEPLOYMENT_START;
        volatile uint32_t *clrVector = (volatile uint32_t *)clrBase;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
        uint32_t msp = clrVector[0];
        uint32_t resetHandler = clrVector[1];
#pragma GCC diagnostic pop

        // Reset handler must be a Thumb address and point inside the CLR image range.
        uint32_t resetHandlerAddress = resetHandler & (~1UL);
        bool validMsp = (msp >= RP2350_SRAM_START) && (msp < RP2350_SRAM_END_EXCLUSIVE) && ((msp & 0x7UL) == 0UL);
        bool validResetHandler = ((resetHandler & 0x1UL) != 0UL) && (resetHandlerAddress >= clrBase) &&
                                 (resetHandlerAddress < clrEndExclusive);

        if (validMsp && validResetHandler)
        {
            // there seems to be a valid CLR image
            // launch nanoCLR
            LaunchCLR(clrBase);
        }
    }

    //  Initializes a serial-over-USB CDC driver.
    sduObjectInit(&SERIAL_DRIVER);
    sduStart(&SERIAL_DRIVER, &serusbcfg);

    // Activates the USB driver and then the USB bus pull-up on D+.
    // Note, a delay is inserted in order to not have to disconnect the cable after a reset.
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(100);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    // Initialize block storage before the receiver thread starts.
    // ReceiverThread runs at higher priority than main() so it can handle
    // Monitor_FlashSectorMap immediately after osKernelStart(); devices must
    // already be registered at that point.
    // in CLR this is called in nanoHAL_Initialize()
    BlockStorageList_Initialize();
    BlockStorage_AddDevices();
    BlockStorageList_InitializeDevices();

    // create the receiver thread
    osThreadId receiverThread = osThreadCreate(osThread(ReceiverThread), NULL);
    if (receiverThread == NULL)
    {
        chSysHalt("ReceiverThread create failed");
    }

    // start kernel, after this main() will behave like a thread with priority osPriorityNormal
    osKernelStart();

    // report successful nanoBooter execution
    ReportSuccessfullNanoBooter();

    //  Normal main() thread
    //  NOTE: On Pico 2 W, GP25 is the CYW43 WiFi SPI CS — do NOT toggle it as LED.
    //  The on-board LED is connected to WL_GPIO0 on the CYW43 chip.
    while (true)
    {
        osDelay(500);
    }
}
