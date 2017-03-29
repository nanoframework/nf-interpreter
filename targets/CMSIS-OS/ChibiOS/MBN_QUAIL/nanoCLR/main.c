//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// Using Devantech LCD03 display in I2C mode @ address 0xC8

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include "usbcfg.h"
#include <CLR_Startup_Thread.h>
#include <WireProtocol_ReceiverThread.h>

void BlinkerThread()
{	
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
	}
}

void DisplayClear(uint8_t backLight)
{
	  uint8_t init[] = {0, 4, 0, 12, 0, 20-backLight};
	  
	  i2cAcquireBus(&I2CD1);
	  i2cMasterTransmitTimeout(&I2CD1, 0xC8>>1, &init, sizeof(init), NULL, 0, TIME_INFINITE);
	  i2cReleaseBus(&I2CD1);
}

void DisplayWrite(uint8_t x, uint8_t y, char * text)
{
    uint8_t data[25] ;
	  
    data[0] = 0;
    data[1] = 3;
    data[2] = y;
    data[3] = x;
    data[4] = 0;
    memcpy(data+5, (uint8_t *)text, strlen(text));

	  i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0xC8>>1, &data, strlen(text)+5, NULL, 0, TIME_INFINITE);
	  i2cReleaseBus(&I2CD1);
}

osThreadDef(BlinkerThread, osPriorityNormal, 128);

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityNormal, 1024);

// declare CLRStartup thread here
osThreadDef(CLRStartupThread, osPriorityNormal, 1024);

static const I2CConfig i2cconfig = { OPMODE_I2C, 100000U, STD_DUTY_CYCLE };

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

  i2cStart(&I2CD1, &i2cconfig);
  
  DisplayClear (1);
  DisplayWrite (1,1, "Hello world !");
  DisplayWrite (2,3, "nanoFramework rules");
  
  osDelay(3000);
  DisplayClear (0);


  while (true)
  {
     osDelay(1000);
  }
}
