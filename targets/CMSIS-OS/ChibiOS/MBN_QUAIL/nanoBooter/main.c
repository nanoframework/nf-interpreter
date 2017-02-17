//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include "usbcfg.h"
#include <WireProtocol_ReceiverThread.h>


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
osThreadDef(ReceiverThread, osPriorityNormal, 1024);

int main(void) 
{
  osThreadId blinkerThreadId;
  osThreadId receiverThreadId;

  halInit();
  osKernelInitialize();

  //  Initializes a serial-over-USB CDC driver.
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);
  
  receiverThreadId = osThreadCreate(osThread(ReceiverThread), NULL);

  blinkerThreadId = osThreadCreate(osThread(BlinkerThread), NULL);

  osKernelStart();

  // Christophe : 
  // Infinite loop here only for testing purposes
  // It should be removed when/if the USB-CDC driver is working as expected
  while (true)
  {
    osDelay(500);
  }

  //  Normal main() thread activity
  osDelay (2000);

  osThreadTerminate(receiverThreadId);
  sduStop(&SDU1);

  osThreadTerminate(blinkerThreadId); 

  LaunchCLR(0x08008000);
}

