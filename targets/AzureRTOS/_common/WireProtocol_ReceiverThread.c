//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <WireProtocol_Message.h>
#include <tx_api.h>

// Wire Protocol mutex name
#if DEBUG
#define WP_MUTEX_NAME "wp_tx_mutex"
#else
#define WP_MUTEX_NAME NULL
#endif

TX_MUTEX wpTxMutex;

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
    // create mutex for TX operation
    tx_mutex_create(&wpTxMutex, WP_MUTEX_NAME, TX_NO_INHERIT);
}
