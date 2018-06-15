//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <swo.h>
#include <targetHAL.h>
#include <WireProtocol_ReceiverThread.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_ConfigurationManager.h>
#include <LaunchCLR.h>

void BlinkerThread(void const * argument)
{
  (void)argument;

  // loop until thread receives a request to terminate
  while (!chThdShouldTerminateX()) {

    palClearPad(GPIOA, GPIOA_LED_GREEN);
    osDelay(500);
    palSetPad(GPIOA, GPIOA_LED_GREEN);
    osDelay(500);
  }
}
osThreadDef(BlinkerThread, osPriorityNormal, 128, "BlinkerThread");

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityHigh, 2048, "ReceiverThread");

// Application entry point.
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

  // if the USER button (blue one) is pressed, skip the check for a valid CLR image and remain in booter
  if (palReadPad(GPIOC, GPIOC_BUTTON))
  {
    // check for valid CLR image at address contiguous to nanoBooter
    if(CheckValidCLRImage((uint32_t)&__nanoImage_end__))
    {
      // there seems to be a valid CLR image
      // launch nanoCLR
      LaunchCLR((uint32_t)&__nanoImage_end__);
    }
  }

  // Prepares the serial driver 2 using UART2
  sdStart(&SD2, NULL);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(1)); // USART2 TX
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(1)); // USART2 RX

  // Creates the blinker thread, it does not start immediately.
  osThreadCreate(osThread(BlinkerThread), NULL);

  // create the receiver thread
  osThreadCreate(osThread(ReceiverThread), NULL);

  // start kernel, after this main() will behave like a thread with priority osPriorityNormal
  osKernelStart();

  // initialize block storage device
  // in CLR this is called in nanoHAL_Initialize()
  // for nanoBooter we have to init it in order to provide the flash map for Monitor_FlashSectorMap command
  BlockStorage_AddDevices();
 
  // initialize configuration manager
  // in CLR this is called in nanoHAL_Initialize()
  // for nanoBooter we have to init it here to have access to network configuration blocks
  ConfigurationManager_Initialize();

  //  Normal main() thread
  while (true) {
    osDelay(500);
  }
}
