//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <targetHAL.h>
#include <WireProtocol_ReceiverThread.h>
#include <LaunchCLR.h>

void BlinkerThread(void const * argument)
{
  (void)argument;
  
    palClearPad(GPIOE, GPIOE_LED1);
    palClearPad(GPIOE, GPIOE_LED2);
    palClearPad(GPIOC, GPIOC_LED3);

  while (!chThdShouldTerminateX()) 
  {
    palSetPad(GPIOE, GPIOE_LED1);
    palSetPad(GPIOE, GPIOE_LED2);
    palSetPad(GPIOC, GPIOC_LED3);
    chThdSleepMilliseconds(100);
    palClearPad(GPIOE, GPIOE_LED1);
    palClearPad(GPIOE, GPIOE_LED2);
    palClearPad(GPIOC, GPIOC_LED3);
    chThdSleepMilliseconds(100);
  }
  chThdSleepMilliseconds(100);
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

  //  Initializes a serial-over-USB CDC driver.
  //sduObjectInit(&SDU1);
  //sduStart(&SDU1, &serusbcfg);

  // Activates the USB driver and then the USB bus pull-up on D+.
  // Note, a delay is inserted in order to not have to disconnect the cable after a reset.
  //usbDisconnectBus(serusbcfg.usbp);
  //chThdSleepMilliseconds(1500);
  //usbStart(serusbcfg.usbp, &usbcfg);
  //usbConnectBus(serusbcfg.usbp);

  // Creates the blinker thread, it does not start immediately.
  blinkerThreadId = osThreadCreate(osThread(BlinkerThread), NULL);

  // create the receiver thread
  receiverThreadId = osThreadCreate(osThread(ReceiverThread), NULL);

  // start kernel, after this the main() thread has priority osPriorityNormal by default
  osKernelStart();

  osDelay(2000);

      // Start the shutdown sequence

      // terminate threads
      osThreadTerminate(receiverThreadId);
      osThreadTerminate(blinkerThreadId);
      
      // stop the serial-over-USB CDC driver
      //sduStop(&SDU1);
      
      // check for valid CLR image at address contiguous to nanoBooter
    if(CheckValidCLRImage((uint32_t)&__nanoImage_end__))
    {
      // there seems to be a valid CLR image
      // launch nanoCLR
      LaunchCLR((uint32_t)&__nanoImage_end__);
    }
}

