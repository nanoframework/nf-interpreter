//
// Copyright (c) 2017 The nano Framework project contributors
// See LICENSE file in the project root for full license information.
//

#include <cmsis_os.h>

#include "WireProtocol.h"
#include "WireProtocol_HAL_Interface.h"

void ReceiverThread(void const * argument)
{
  (void)argument;

  // Initialize to a packet sequence number impossible to encounter
  lastPacketSequence = 0x00FEFFFF;

  while (true) {
    WP_CheckAvailableIncomingData();
    osDelay(500);
  }
}
