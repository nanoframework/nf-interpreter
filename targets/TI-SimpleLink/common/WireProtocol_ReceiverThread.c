//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "WireProtocol_HAL_Interface.h"
#include <ti/sysbios/knl/Task.h>
#include <xdc/std.h>
// #include <FreeRTOS.h>
// #include <task.h>

extern WP_Message inboundMessage;

void WP_Message_Initialize(WP_Message* a);
void WP_Message_PrepareReception(WP_Message* a);
void WP_Message_Process(WP_Message* a);

void ReceiverThread(UArg arg0, UArg arg1)
{
    (void)arg0;
    (void)arg1;

    // loop forever
    while (1) 
    {
        WP_Message_Initialize(&inboundMessage);
        WP_Message_PrepareReception(&inboundMessage);

        WP_Message_Process(&inboundMessage);

        // Allow other tasks a chance to run
        Task_yield();
    }
}
