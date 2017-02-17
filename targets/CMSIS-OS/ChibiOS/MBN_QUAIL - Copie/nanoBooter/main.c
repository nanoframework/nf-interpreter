//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <usbcfg.h>
#include <WireProtocol_ReceiverThread.h>
#include <LaunchCLR.h>

void BlinkerThread(void const * argument)
{
  (void)argument;

  // loop until thread receives a request to terminate
  while (!chThdShouldTerminateX()) {

      palClearPad(GPIOC, GPIOC_LED3);
      palSetPad(GPIOE, GPIOE_LED1);
      //osDelay(250);
      chThdSleepMilliseconds(250);

      palClearPad(GPIOE, GPIOE_LED1);
      palSetPad(GPIOE, GPIOE_LED2);
      //osDelay(250);
      chThdSleepMilliseconds(250);

      palClearPad(GPIOE, GPIOE_LED2);
      palSetPad(GPIOC, GPIOC_LED3);
      osDelay(250);
      //chThdSleepMilliseconds(250);
  }
}
osThreadDef(BlinkerThread, osPriorityNormal, 128);

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityNormal, 1024);


//  Application entry point.
int main(void)
{

  osThreadId blinkerThreadId;
  osThreadId receiverThreadId;

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
  // Note, a delay is inserted in order to not have to disconnect the cable after a reset.
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  // Creates the blinker thread, it does not start immediately.
  blinkerThreadId = osThreadCreate(osThread(BlinkerThread), NULL);

  // create the receiver thread
  receiverThreadId = osThreadCreate(osThread(ReceiverThread), NULL);

  // start kernel, after this the main() thread has priority osPriorityNormal by default
  osKernelStart();

  //osDelay(4000);
  chThdSleepMilliseconds(4000);
  
  osThreadTerminate(receiverThreadId);
  osThreadTerminate(blinkerThreadId);
  sduStop(&SDU1);
  
  LaunchCLR(0x08008000);
}

