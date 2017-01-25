//
// Copyright (c) 2017 The nano Framework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <WireProtocol_ReceiverThread.h>
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
    
  // nothing to deinitialize or cleanup, so it's safe to return
}
osThreadDef(BlinkerThread, osPriorityNormal, 128);

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityNormal, 1024);

//  Application entry point.
int main(void) {

  osThreadId blinkerThreadId;
  osThreadId receiverThreadId;

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are already enabled.
  osKernelInitialize();

  // Prepares the serial driver 2 using UART2
  sdStart(&SD2, NULL);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(1)); // USART2 TX
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(1)); // USART2 RX

  // Creates the blinker thread, it does not start immediately.
  blinkerThreadId = osThreadCreate(osThread(BlinkerThread), NULL);

  // create the receiver thread
  receiverThreadId = osThreadCreate(osThread(ReceiverThread), NULL);

  // start kernel, after this the main() thread has priority osPriorityNormal by default
  osKernelStart();

  //  Normal main() thread
  while (true) {

    // check for button pressed
    if (!palReadPad(GPIOC, GPIOC_BUTTON))
    {
      // Start the shutdown sequence

      // terminate threads
      osThreadTerminate(receiverThreadId);
      osThreadTerminate(blinkerThreadId);
      
      // stop serial driver 2
      sdStop(&SD2);
      
      // launch nanoCLR
      LaunchCLR(0x08004000);
    }
    
    osDelay(500);
  }
}
