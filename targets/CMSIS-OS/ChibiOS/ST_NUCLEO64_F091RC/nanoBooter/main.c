//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <serialcfg.h>
#include <targetHAL.h>
#include <WireProtocol_ReceiverThread.h>
#include <nanoPAL_BlockStorage.h>
#include <LaunchCLR.h>

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityHigh, 2048, "ReceiverThread");

// Application entry point.
int main(void) {

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

  // the following IF is not mandatory, it's just providing a way for a user to 'force'
  // the board to remain in nanoBooter and not launching nanoCLR

  // if the USER button (blue one) is pressed, skip the check for a valid CLR image and remain in booter
  if (palReadPad(GPIOC, GPIOC_BUTTON))
  {
    // check for valid CLR image at address contiguous to nanoBooter
    // this target DOES NOT have configuration block, so we need to use the __nanoImage_end__ address here
    if(CheckValidCLRImage((uint32_t)&__nanoImage_end__))
    {
      // there seems to be a valid CLR image
      // launch nanoCLR
      LaunchCLR((uint32_t)&__nanoImage_end__);
    }
  }

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are already enabled.
  osKernelInitialize();

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

  //  Normal main() thread
  while (true) {
    palClearPad(GPIOA, GPIOA_LED_GREEN);
    osDelay(500);
    palSetPad(GPIOA, GPIOA_LED_GREEN);
    osDelay(500);
  }
}
