
// // Simplelink includes
// #include "simplelink.h"

// // Driverlib includes
// #include "hw_types.h"
// #include "hw_ints.h"
// #include "hw_memmap.h"
// #include "hw_common_reg.h"
// #include "interrupt.h"
// #include "hw_apps_rcm.h"
// #include "prcm.h"
// #include "rom.h"
// #include "rom_map.h"
// #include "prcm.h"
// #include "gpio.h"
// #include "utils.h"

// //Free_rtos/ti-rtos includes
// #include "osi.h"

// // Common interface includes
// #include "gpio_if.h"
// #include "common.h"
// #include "pinmux.h"


void BlinkerThread(void const * argument)
{
  (void)argument;

  // palSetLineMode(1, PAL_MODE_OUTPUT_PUSHPULL);


  // loop until thread receives a request to terminate
  while (!chThdShouldTerminateX()) {

      // palSetPad(GPIOD, GPIOD_LED3);
      // osDelay(500);
      // palClearPad(GPIOD, GPIOD_LED3);
      // osDelay(500);
      
  }
  // nothing to deinitialize or cleanup, so it's safe to return
}
osThreadDef(BlinkerThread, osPriorityNormal, 128, "BlinkerThread");

//  Application entry point.
int main(void) {

  osThreadId blinkerThreadId;
 
  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

//   // init SWO as soon as possible to make it available to output ASAP
//   #if (SWO_OUTPUT == TRUE)  
//   SwoInit();
//   #endif

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are already enabled.
  osKernelInitialize();
  osDelay(20);    // Let init stabilize

//   // the following IF is not mandatory, it's just providing a way for a user to 'force'
//   // the board to remain in nanoBooter and not launching nanoCLR

//   // if the USER button (blue one) is pressed, skip the check for a valid CLR image and remain in booter
//   // the user button in this board has a pull-up resistor so the check has to be inverted
//   if (palReadPad(GPIOA, GPIOA_BUTTON))
//   {
//     // check for valid CLR image 
//     if(CheckValidCLRImage((uint32_t)&__nanoImage_end__))
//     {
//       // there seems to be a valid CLR image
//       // launch nanoCLR
//       LaunchCLR((uint32_t)&__nanoImage_end__);
//     }
//   }

//   //  Initializes a serial-over-USB CDC driver.
//   sduObjectInit(&SDU1);
//   sduStart(&SDU1, &serusbcfg);

//   // Activates the USB driver and then the USB bus pull-up on D+.
//   // Note, a delay is inserted in order to not have to disconnect the cable after a reset.
//   usbDisconnectBus(serusbcfg.usbp);
//   chThdSleepMilliseconds(1500);
//   usbStart(serusbcfg.usbp, &usbcfg);
//   usbConnectBus(serusbcfg.usbp);

  // Creates the blinker thread, it does not start immediately.
  blinkerThreadId = osThreadCreate(osThread(BlinkerThread), NULL);

//   // create the receiver thread
//   receiverThreadId = osThreadCreate(osThread(ReceiverThread), NULL);

  // start kernel, after this main() will behave like a thread with priority osPriorityNormal
  osKernelStart();

  //  Normal main() thread
  while (true) {
    osDelay(500);
  }
}
