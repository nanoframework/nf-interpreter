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

//  Application entry point.
int main(void) {

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

  // init SWO as soon as possible to make it available to output ASAP
  #if (SWO_OUTPUT == TRUE)  
  SwoInit();
  #endif

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are already enabled.
  osKernelInitialize();
  osDelay(20);    // Let init stabilize

  // the following IF is not mandatory, it's just providing a way for a user to 'force'
  // the board to remain in nanoBooter and not launching nanoCLR

  // // if the USER button (blue one) is pressed, skip the check for a valid CLR image and remain in booter
  // // the user button in this board has a pull-up resistor so the check has to be inverted
  // if (!palReadPad(GPIOA, GPIOA_BUTTON))
  // {
  //   // check for valid CLR image 
  //   // this target DOES NOT have configuration block, so we need to use the __nanoImage_end__ address here
  //   if(CheckValidCLRImage((uint32_t)&__nanoImage_end__))
  //   {
  //     // there seems to be a valid CLR image
  //     // launch nanoCLR
  //     LaunchCLR((uint32_t)&__nanoImage_end__);
  //   }
  // }

  // starts the serial driver
  sdStart(&SERIAL_DRIVER, NULL);

  // create the receiver thread
  osThreadCreate(osThread(ReceiverThread), NULL);

  // start kernel, after this main() will behave like a thread with priority osPriorityNormal
  osKernelStart();

  // initialize block storage device
  // in CLR this is called in nanoHAL_Initialize()
  // for nanoBooter we have to init it in order to provide the flash map for Monitor_FlashSectorMap command
  BlockStorage_AddDevices();

  //  Normal main() thread
  while (true) {
      palSetPad(GPIOC, GPIOC_LED2);
      osDelay(250);
      palClearPad(GPIOC, GPIOC_LED2);
      osDelay(250);
  }
}
