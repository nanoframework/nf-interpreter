//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// Using Devantech LCD03 display in I2C mode @ address 0xC8

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include "usbcfg.h"
#include <WireProtocol_ReceiverThread.h>
#include <nanoCLR_Application.h>
#include <nanoPAL_BlockStorage.h>

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

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityNormal, 2048, "ReceiverThread");

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

  // create the receiver thread
  osThreadCreate(osThread(ReceiverThread), NULL);

  // start kernel, after this main() will behave like a thread with priority osPriorityNormal
  osKernelStart();
  
  rtcInit();

  i2cStart(&I2CD1, &i2cconfig);
  
  DisplayClear (1);
  DisplayWrite (1,1, "Hello world !");
  DisplayWrite (2,3, "nanoFramework rules");
  
  osDelay(3000);
  DisplayClear (0);

  // preparation for the CLR startup
  BlockStorage_AddDevices();

  CLR_SETTINGS clrSettings;

  memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

  clrSettings.MaxContextSwitches         = 50;
  clrSettings.WaitForDebugger            = false;
  clrSettings.EnterDebuggerLoopAfterExit = true;

  // startup CLR now
  ClrStartup(clrSettings);

  while (true) { 
    osDelay(100);
  }
}
