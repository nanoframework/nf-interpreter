//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <usbcfg.h>
#include <targetHAL.h>
#include <WireProtocol_ReceiverThread.h>
#include <LaunchCLR.h>

void BlinkerThread(void const * argument)
{
  (void)argument;

  // loop until thread receives a request to terminate
  while (!chThdShouldTerminateX()) {

      palSetPad(GPIOD, GPIOD_LED3);
      osDelay(250);

      palClearPad(GPIOD, GPIOD_LED3);
      palSetPad(GPIOD, GPIOD_LED5);
      osDelay(250);

      palClearPad(GPIOD, GPIOD_LED5);
      palSetPad(GPIOD, GPIOD_LED6);
      osDelay(250);

      palClearPad(GPIOD, GPIOD_LED6);
      palSetPad(GPIOD, GPIOD_LED4);
      osDelay(250);
      
      palClearPad(GPIOD, GPIOD_LED4);
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

  // the following IF is not mandatory, it's just providing a way for a user to 'force'
  // the board to remain in nanoBooter and not launching nanoCLR

  // if the USER button (blue one) is pressed, skip the check for a valid CLR image and remain in booter
  // the user button in this board has a pull-up resistor so the check has to be inverted
  if (!palReadPad(GPIOA, GPIOA_BUTTON))
  {
    // check for valid CLR image 
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

  //  Normal main() thread
  while (true) {
    osDelay(500);
  }
}

