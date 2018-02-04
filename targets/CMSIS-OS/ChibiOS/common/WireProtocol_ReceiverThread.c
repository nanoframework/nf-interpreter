//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <cmsis_os.h>
#include "WireProtocol_HAL_Interface.h"

extern WP_Message inboundMessage;

// This thread needs to be implemented at ChibiOS level because it has to include a call to chThdShouldTerminateX()
// in case the thread is requested to terminate by the CMSIS call osThreadTerminate()

void ReceiverThread(void const * argument)
{
  (void)argument;

  osDelay(500);

  // loop until thread receives a request to terminate
  while (!chThdShouldTerminateX()) {

    WP_Message_Initialize(&inboundMessage);
    WP_Message_PrepareReception(&inboundMessage);

    WP_Message_Process(&inboundMessage);

    // delay here to give other threads a chance to run
    osDelay(100);
  }

  // nothing to deinitialize or cleanup, so it's safe to return
}
