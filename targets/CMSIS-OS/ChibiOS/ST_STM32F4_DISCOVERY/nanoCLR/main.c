//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include "usbcfg.h"
#include <nanoCLR_Application.h>
#include <WireProtocol_ReceiverThread.h>

void BlinkerThread(void const * argument)
{
  (void)argument;

  palSetPad(GPIOD, GPIOD_LED3);
  palSetPad(GPIOD, GPIOD_LED4);
  palSetPad(GPIOD, GPIOD_LED5);
  palSetPad(GPIOD, GPIOD_LED6);
  osDelay(1000);

  palClearPad(GPIOD, GPIOD_LED3);
  palClearPad(GPIOD, GPIOD_LED4);
  palClearPad(GPIOD, GPIOD_LED5);
  palClearPad(GPIOD, GPIOD_LED6);
  osDelay(250);

  while (true) {

      palSetPad(GPIOD, GPIOD_LED3);
      osDelay(125);

      palClearPad(GPIOD, GPIOD_LED3);
      palSetPad(GPIOD, GPIOD_LED4);
      osDelay(125);

      palClearPad(GPIOD, GPIOD_LED4);
      palSetPad(GPIOD, GPIOD_LED6);
      osDelay(125);

      palClearPad(GPIOD, GPIOD_LED6);
      palSetPad(GPIOD, GPIOD_LED5);
      osDelay(125);
      
      palClearPad(GPIOD, GPIOD_LED5);
  }
}
osThreadDef(BlinkerThread, osPriorityNormal, 128);

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityNormal, 1024);

//  Application entry point.
int main(void) {

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are already enabled.
  osKernelInitialize();

  // //  Initializes a serial-over-USB CDC driver.
  // sduObjectInit(&SDU1);
  // sduStart(&SDU1, &serusbcfg);

  // // Activates the USB driver and then the USB bus pull-up on D+.
  // // Note, a delay is inserted in order to not have to disconnect the cable after a reset
  // usbDisconnectBus(serusbcfg.usbp);
  // chThdSleepMilliseconds(1500);
  // usbStart(serusbcfg.usbp, &usbcfg);
  // usbConnectBus(serusbcfg.usbp);

  // Creates the blinker thread, it does not start immediately.
  osThreadCreate(osThread(BlinkerThread), NULL);

  // create the receiver thread
  osThreadCreate(osThread(ReceiverThread), NULL);

  // start kernel, after this the main() thread has priority osPriorityNormal by default
  osKernelStart();

  CLR_SETTINGS clrSettings;

  memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

  clrSettings.MaxContextSwitches         = 50;
  clrSettings.WaitForDebugger            = false;
  clrSettings.EnterDebuggerLoopAfterExit = true;

  //ClrStartup(clrSettings);

  // while (true) {
  //   osDelay(1000);
  // }
}
