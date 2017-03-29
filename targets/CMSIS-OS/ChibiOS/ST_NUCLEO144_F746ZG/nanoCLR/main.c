//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include "usbcfg.h"
#include <CLR_Startup_Thread.h>
#include <WireProtocol_ReceiverThread.h>

void BlinkerThread(void const * argument)
{
  (void)argument;

  palSetPad(GPIOB, GPIOB_LED1);
  palSetPad(GPIOB, GPIOB_LED2);
  palSetPad(GPIOB, GPIOB_LED3);
  osDelay(1000);

  palClearPad(GPIOB, GPIOB_LED1);
  palClearPad(GPIOB, GPIOB_LED2);
  palClearPad(GPIOB, GPIOB_LED3);
  osDelay(250);

  while (true) {

      palSetPad(GPIOB, GPIOB_LED1);
      osDelay(125);

      palClearPad(GPIOB, GPIOB_LED1);
      palSetPad(GPIOB, GPIOB_LED2);
      osDelay(125);

      palClearPad(GPIOB, GPIOB_LED2);
      palSetPad(GPIOB, GPIOB_LED3);
      osDelay(125);

      palClearPad(GPIOB, GPIOB_LED3);
      palSetPad(GPIOB, GPIOB_LED2);
      osDelay(125);

      palClearPad(GPIOB, GPIOB_LED2);
      
  }
}
osThreadDef(BlinkerThread, osPriorityNormal, 128);

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityNormal, 1024);

// declare CLRStartup thread here
osThreadDef(CLRStartupThread, osPriorityNormal, 1024);

//  Application entry point.
int main(void) {

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are already enabled.
  osKernelInitialize();

  //  Initializes a serial-over-USB CDC driver.
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  // Activates the USB driver and then the USB bus pull-up on D+.
  // Note, a delay is inserted in order to not have to disconnect the cable after a reset
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  // Creates the blinker thread, it does not start immediately.
  osThreadCreate(osThread(BlinkerThread), NULL);

  // create the receiver thread
  osThreadCreate(osThread(ReceiverThread), NULL);
  
  // create the CLR Startup thread
  osThreadCreate(osThread(CLRStartupThread), NULL);

  // start kernel, after this the main() thread has priority osPriorityNormal by default
  osKernelStart();

  while (true) {
    osDelay(1000);
  }
}
