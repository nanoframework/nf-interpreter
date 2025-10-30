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

    // HAL initialization, this also initializes the configured device drivers
    // and performs the board-specific initializations.
    halInit();

    // turn off RGB LED
    palClearLine(LINE_RBG_GREEN);
    palClearLine(LINE_RBG_RED);
    palClearLine(LINE_RBG_BLUE);

    // init boot clipboard
    InitBootClipboard();

// init SWO as soon as possible to make it available to output ASAP
#if (SWO_OUTPUT == TRUE)
    SwoInit();
#endif

    // the following IF is not mandatory, it's just providing a way for a user to 'force'
    // the board to remain in nanoBooter and not launching nanoCLR
    // check if there is a request to remain on nanoBooter
    if (!IsToRemainInBooter())
    {
        // if the USER button (blue one) is pressed, skip the check for a valid CLR image and remain in booter
        // the user button in this board has a pull-up resistor so the check has to be inverted
        if (palReadPad(GPIOA, GPIOA_KEY_A))
        {
            // check for valid CLR image
            // this target DOES NOT have configuration block, so we need to use the __nanoImage_end__ address here
            if (CheckValidCLRImage((uint32_t)&__nanoImage_end__))
            {
                // there seems to be a valid CLR image
                // launch nanoCLR
                LaunchCLR((uint32_t)&__nanoImage_end__);
            }
        }
    }

    // The kernel is initialized but not started yet, this means that
    // main() is executing with absolute priority but interrupts are already enabled.
    osKernelInitialize();

#if (HAL_NF_USE_STM32_CRC == TRUE)
    // startup crc
    crcStart(NULL);
#endif

    // need to override the default config block to set oversampling
    SerialConfig serialConfig = {SERIAL_DEFAULT_BITRATE, 0, USART_CR2_STOP1_BITS, USART_CR1_OVER8};

    // starts the serial driver
    sdStart(&SERIAL_DRIVER, &serialConfig);

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
        palTogglePad(GPIOC, GPIOC_LED2);
        osDelay(250);
    }
}
