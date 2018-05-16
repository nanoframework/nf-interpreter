//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>
#include "WireProtocol_HAL_Interface.h"

extern WP_Message inboundMessage;
extern SerialUSBDriver SDU1;

// This thread needs to be implemented at ChibiOS level because it has to include a call to chThdShouldTerminateX()
// in case the thread is requested to terminate by the CMSIS call osThreadTerminate()

__attribute__((noreturn))
void ReceiverThread(void const * argument)
{
  osDelay(500);

  #if (HAL_USE_STM32_CRC == TRUE)
  // startup crc
  crcStart(NULL);
  #endif

  // loop until thread receives a request to terminate
  while (1) {

    if (SDU1.config->usbp->state == USB_ACTIVE)
    {
      WP_Message_Initialize(&inboundMessage);
      WP_Message_PrepareReception(&inboundMessage);

      WP_Message_Process(&inboundMessage);

      // pass control to the OS
      osThreadYield();
    }
    else
    {
      osDelay(1000);      
    }
  }

  // this function never returns
}
