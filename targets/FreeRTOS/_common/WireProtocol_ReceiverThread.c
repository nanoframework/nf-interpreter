//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "FreeRTOS.h"
#include "task.h"
#include "WireProtocol_HAL_Interface.h"

extern WP_Message inboundMessage;

void WP_Message_Initialize(WP_Message* a);
void WP_Message_PrepareReception(WP_Message* a);
void WP_Message_Process(WP_Message* a);


void ReceiverThread(void * argument)
{
  (void)argument;

  // loop forever
  while (1) {

    WP_Message_Initialize(&inboundMessage);
    WP_Message_PrepareReception(&inboundMessage);

    WP_Message_Process(&inboundMessage);

    // Allow other tasks a chance to run
    taskYIELD();
  }

  // nothing to deinitialize or cleanup, so it's safe to return
}
