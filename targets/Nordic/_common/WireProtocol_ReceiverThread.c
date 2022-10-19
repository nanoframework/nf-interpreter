//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <zephyr.h>

#include "WireProtocol_HAL_Interface.h"

void WP_Message_Process();
void WP_Message_PrepareReception();

// void ReceiverThread(void *argument)
void ReceiverThread(void *p1, void *p2, void *p3)
{
    (void)p1;
    (void)p2;
    (void)p3;

    WP_Message_PrepareReception();

    // loop forever
    while (1)
    {
        WP_Message_Process();

        // Allow other tasks a chance to run
        k_yield();
    }
}

void WP_Message_PrepareReception_Platform()
{
    // empty on purpose, nothing to configure
}
