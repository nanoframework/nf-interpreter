//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "WireProtocol_HAL_Interface.h"
#include <ti/sysbios/knl/Task.h>
#include <xdc/std.h>

void WP_Message_PrepareReception();
void WP_Message_Process();

void ReceiverThread(UArg arg0, UArg arg1)
{
    (void)arg0;
    (void)arg1;

    WP_Message_PrepareReception();

    // loop forever
    while (1)
    {
        WP_Message_Process();

        // Allow other tasks a chance to run
        Task_yield();
    }
}

void WP_Message_PrepareReception_Platform()
{
    // empty on purpose, nothing to configure
}
