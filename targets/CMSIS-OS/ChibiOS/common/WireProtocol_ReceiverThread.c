//
// Copyright (c) 2017 The nano Framework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <cmsis_os.h>

#include "WireProtocol.h"
#include "WireProtocol_HAL_Interface.h"


// This thread needs to be implemented at ChibiOS level because it has to include a call to chThdShouldTerminateX()
// in case the thread is requested to terminate by the CMSIS call osThreadTerminate()

void ReceiverThread(void const * argument)
{
  (void)argument;

  // Initialize to a packet sequence number impossible to encounter
  lastPacketSequence = 0x00FEFFFF;

  // loop until thread receives a request to terminate
  while (!chThdShouldTerminateX()) {
    
    WP_CheckAvailableIncomingData();
    
    osDelay(500);
  }

  // nothing to deinitialize or cleanup, so it's safe to return
}
