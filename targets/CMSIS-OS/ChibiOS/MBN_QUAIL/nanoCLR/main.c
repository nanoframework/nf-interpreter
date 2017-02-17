//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

//#include "usbcfg.h"
//#include <WireProtocol_ReceiverThread.h>

void BlinkerThread(void const * argument)
{
  (void)argument;

    palSetPad(GPIOE, GPIOE_LED1);
    palClearPad(GPIOE, GPIOE_LED2);
    palClearPad(GPIOC, GPIOC_LED3);
    palClearPad(GPIOA, GPIOA_INT1);

  // loop until thread receives a request to terminate
  while (!chThdShouldTerminateX()) 
  {
    palClearPad(GPIOC, GPIOC_LED3);
    palSetPad(GPIOE, GPIOE_LED1);
    osDelay(200);
    palClearPad(GPIOE, GPIOE_LED1);
    palSetPad(GPIOE, GPIOE_LED2);
    osDelay(200);
    palClearPad(GPIOE, GPIOE_LED2);
    palSetPad(GPIOC, GPIOC_LED3);
    osDelay(200);
/*
    palClearPad(GPIOA, GPIOA_INT1);
    palClearPad(GPIOE, GPIOE_LED2);
    chThdSleepMicroseconds(10);
    palSetPad(GPIOA, GPIOA_INT1);
    palSetPad(GPIOE, GPIOE_LED2);
    chThdSleepMicroseconds(10);
*/
  }
}

osThreadDef(BlinkerThread, osPriorityNormal, 128);

// need to declare the Receiver thread here
//osThreadDef(ReceiverThread, osPriorityNormal, 1024);

static PWMConfig pwmTimer1 = 
  {
  200000, /* 200Khz PWM clock frequency*/
  1024, /* PWM period of 1024 ticks ~ 0.005 second */
  NULL, /* No callback */
  /* Only channel 1 enabled */
  {
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_DISABLED, NULL},
    {PWM_OUTPUT_DISABLED, NULL},
    {PWM_OUTPUT_DISABLED, NULL}
  }
};

static PWMConfig pwmTimer4 = 
  {
  500000, /* 500Khz PWM clock frequency*/
  1024, /* PWM period of 1024 ticks ~ 0.005 second */
  NULL, /* No callback */
  /* Only channel 1 enabled */
  {
    {PWM_OUTPUT_DISABLED, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  }
};

void TestPWM(void const * argument)
{
 (void)argument;

pwmStart(&PWMD1, &pwmTimer1);
pwmEnableChannel(&PWMD1, 0, 512);

pwmStart(&PWMD4, &pwmTimer4);
pwmEnableChannel(&PWMD4, 3, 256);
pwmEnableChannel(&PWMD4, 1, 512);
pwmEnableChannel(&PWMD4, 2, 1024);
}

//  Application entry point.
int main(void) {

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are already enabled.
  osKernelInitialize();

/*
  //  Initializes a serial-over-USB CDC driver.
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  // Activates the USB driver and then the USB bus pull-up on D+.
  // Note, a delay is inserted in order to not have to disconnect the cable after a reset
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);
*/
  // Creates the blinker thread, it does not start immediately.
  osThreadCreate(osThread(BlinkerThread), NULL);

  // create the receiver thread
  //osThreadCreate(osThread(ReceiverThread), NULL);

  // start kernel, after this the main() thread has priority osPriorityNormal by default
  osKernelStart();
  //osDelay(200);

  

  //  Normal main() thread activity it does nothing except sleeping in a loop 
  while (true) {
    osDelay(1000);
  }
}

