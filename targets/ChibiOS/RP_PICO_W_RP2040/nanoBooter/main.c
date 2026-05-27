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

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityHigh, 2048, "ReceiverThread");

// Application entry point.
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

    // the following IF is not mandatory, it's just providing a way for a user to 'force'
    // the board to remain in nanoBooter and not launching nanoCLR

    // check if there is a request to remain on nanoBooter
    if (!IsToRemainInBooter())
    {
        // check for valid CLR image at address contiguous to nanoBooter
        volatile uint32_t *clrVector = (volatile uint32_t *)(uint32_t)&__nanoImage_end__;
        uint32_t msp = clrVector[0];
        uint32_t resetHandler = clrVector[1];

        if (msp != 0xFFFFFFFF && msp != 0x00000000 &&
            resetHandler > 0x10000000 && resetHandler < 0x10200000)
        {
            // there seems to be a valid CLR image
            // launch nanoCLR
            LaunchCLR((uint32_t)&__nanoImage_end__);
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

    // create the receiver thread
    osThreadCreate(osThread(ReceiverThread), NULL);

    // start kernel, after this main() will behave like a thread with priority osPriorityNormal
    osKernelStart();

    // initialize block storage list and devices
    // in CLR this is called in nanoHAL_Initialize()
    // for nanoBooter we have to init it in order to provide the flash map for Monitor_FlashSectorMap command
    BlockStorageList_Initialize();
    BlockStorage_AddDevices();
    BlockStorageList_InitializeDevices();

    // report successful nanoBooter execution
    ReportSuccessfullNanoBooter();

    //  Normal main() thread
    //  NOTE: On Pico W, GP25 is the CYW43 WiFi SPI CS — do NOT toggle it as LED.
    while (true)
    {
        osDelay(500);
    }
}
