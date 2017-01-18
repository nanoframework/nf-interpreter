//
// Copyright (c) 2017 The nano Framework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <WireProtocol_ReceiverThread.h>

void BlinkerThread(void const * argument)
{
  (void)argument;

  while (true) {
    palClearPad(GPIOA, GPIOA_LED_GREEN);
    osDelay(500);
    palSetPad(GPIOA, GPIOA_LED_GREEN);
    osDelay(500);
  }
}
osThreadDef(BlinkerThread, osPriorityNormal, 128);

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityNormal, 1024);

//  Application entry point.
int main(void) {

  //   HAL initialization, this also initializes the configured device drivers
  //      and performs the board-specific initializations.
  halInit();

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are
  // already enabled.
  osKernelInitialize();

  //   Prepares the Serial driver 2 using UART2
  sdStart(&SD2, NULL);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(1)); // USART2 TX
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(1)); // USART2 RX

  // Creates the blinker thread, it does not start immediately.
  osThreadCreate(osThread(BlinkerThread), NULL);

  // create the receiver thread
  osThreadCreate(osThread(ReceiverThread), NULL);

  // Kernel started, the main() thread has priority osPriorityNormal
  // by default.
  osKernelStart();

  //  Normal main() thread activity it does nothing except sleeping in a loop 
  while (true) {
    osDelay(1000);
  }
}
