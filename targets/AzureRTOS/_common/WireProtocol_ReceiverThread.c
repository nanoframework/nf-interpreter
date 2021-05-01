//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <WireProtocol_Message.h>

__attribute__((noreturn)) void ReceiverThread_entry(uint32_t parameter)
{
    (void)parameter;

    tx_thread_sleep(50);

    WP_Message_PrepareReception();

    // loop until thread receives a request to terminate
    while (1) 
    {
        WP_Message_Process();

        // pass control to the OS
        tx_thread_relinquish();
    }

    // this function never returns
}

void WP_Message_PrepareReception_Platform()
{
    // empty on purpose, nothing to configure
}
