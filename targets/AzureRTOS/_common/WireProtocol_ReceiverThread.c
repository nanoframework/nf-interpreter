//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetHAL.h>
#include <WireProtocol_Message.h>

// defined as weak so it can be replaced with platform specific one
__nfweak __attribute__((noreturn)) void ReceiverThread_entry(uint32_t parameter)
{
    (void)parameter;

    tx_thread_sleep(50);

    WP_Message_PrepareReception();

    // loop until thread receives a request to terminate
    while (1)
    {
        WP_Message_Process();

        // pass control to the OS
        tx_thread_sleep(TX_TICKS_PER_MILLISEC(10));
    }

    // this function never returns
}

void WP_Message_PrepareReception_Platform()
{
    // empty on purpose, nothing to configure
}
