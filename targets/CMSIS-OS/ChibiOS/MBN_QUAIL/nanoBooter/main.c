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
  
  palClearPad(GPIOE, GPIOE_LED1);
  palClearPad(GPIOE, GPIOE_LED2);
  palClearPad(GPIOC, GPIOC_LED3);

  while (!chThdShouldTerminateX()) 
  {

    palSetPad(GPIOE, GPIOE_LED1);
    chThdSleepMilliseconds(500);
    palClearPad(GPIOE, GPIOE_LED1);
    chThdSleepMilliseconds(500);

  }
}

osThreadDef(BlinkerThread, osPriorityNormal, 128, "BlinkerThread");

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityNormal, 2048, "ReceiverThread");

//  Application entry point.
int main(void) {

  osThreadId blinkerThreadId;
  osThreadId receiverThreadId;

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();
  
  // check for valid CLR image at address contiguous to nanoBooter
  if(CheckValidCLRImage((uint32_t)&__nanoImage_end__))
  {
    // there seems to be a valid CLR image
    // launch nanoCLR
    LaunchCLR((uint32_t)&__nanoImage_end__);
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

  // start kernel, after this main() will behave like a thread with priority osPriorityNormal
  osKernelStart();

  // initialize block storage device
  // in CLR this is called in nanoHAL_Initialize()
  // for nanoBooter we have to init it in order to provide the flash map for Monitor_FlashSectorMap command
  BlockStorage_AddDevices();

  //  Normal main() thread
  while (true) {
    osDelay(500);
  }
}
