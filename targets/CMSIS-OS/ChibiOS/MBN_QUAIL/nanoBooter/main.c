//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#ifdef _USB_
#include "usbcfg.h"
#include <WireProtocol_ReceiverThread.h>
#endif


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
#ifdef _USB_
osThreadDef(ReceiverThread, osPriorityNormal, 1024);
#endif

int main(void) 
{
  osThreadId blinkerThreadId;
#ifdef _USB_
  osThreadId receiverThreadId;
#endif

  halInit();
  osKernelInitialize();

#ifdef _USB_
  //  Initializes a serial-over-USB CDC driver.
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);
  
  receiverThreadId = osThreadCreate(osThread(ReceiverThread), NULL);
#endif

  blinkerThreadId = osThreadCreate(osThread(BlinkerThread), NULL);

  osKernelStart();

  // Christophe : 
  // Infinite loop here only for testing purposes
  // I am trying to have USB-Serial working in nanoBooter only, at the moment, 
  // hence the infinite loop to stay in it and do not call nanoCLR
  // It should be removed when/if the USB-CDC driver is working as expected
/*
  while (true)
  {
    osDelay(500);
  }
*/
  //  Normal main() thread activity
  osDelay (2000);

#ifdef _USB_
  osThreadTerminate(receiverThreadId);
  sduStop(&SDU1);
#endif

  osThreadTerminate(blinkerThreadId); 

  LaunchCLR(0x08008000);
}

