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
#include <WireProtocol_ReceiverThread.h>
#include <nanoPAL_BlockStorage.h>
#include <LaunchCLR.h>

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityHigh, 2048, "ReceiverThread");

//  Application entry point.
int main(void)
{
    halInit();

    // init boot clipboard
    InitBootClipboard();

    // the following IF is not mandatory, it's just providing a way for a user to 'force'
    // the board to remain in nanoBooter and not launching nanoCLR

// init SWO as soon as possible to make it available to output ASAP
#if (SWO_OUTPUT == TRUE)
    SwoInit();
#endif

    // The kernel is initialized but not started yet, this means that
    // main() is executing with absolute priority but interrupts are already enabled.
    osKernelInitialize();
    osDelay(20); // Let init stabilize

    // check if there is a request to remain on nanoBooter
    if (!IsToRemainInBooter())
    {
        // if the USER button (blue one) is pressed, skip the check for a valid CLR image and remain in booter
        if (palReadLine(LINE_BUTTON_USER))
        {
            // check for valid CLR image
            // we are checking for a valid image at the deployment address, which is pointing to the CLR address
            if (CheckValidCLRImage((uint32_t)&__deployment_start__))
            {
                // there seems to be a valid CLR image
                // launch nanoCLR
                LaunchCLR((uint32_t)&__deployment_start__);
            }
        }
    }

#if (HAL_NF_USE_STM32_CRC == TRUE)
    // startup crc
    crcStart(NULL);
#endif

    // starts the serial driver
    sdStart(&SERIAL_DRIVER, NULL);

    // create the receiver thread
    osThreadCreate(osThread(ReceiverThread), NULL);

    // start kernel, after this main() will behave like a thread with priority osPriorityNormal
    osKernelStart();

    // initialize block storage list and devices
    // in CLR this is called in nanoHAL_Initialize()
    // for nanoBooter we have to init it in order to provide the flash map for Monitor_FlashSectorMap command
    BlockStorageList_Initialize();
    BlockStorage_AddDevices();

    // report successfull nanoBooter execution
    ReportSuccessfullNanoBooter();

    //  Normal main() thread
    while (true)
    {
        palTogglePad(GPIOB, GPIOB_LD2);
        osDelay(500);
    }
}
