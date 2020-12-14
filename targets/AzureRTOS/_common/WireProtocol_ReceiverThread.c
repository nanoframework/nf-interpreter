//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>
#include <WireProtocol_Message.h>
#include <tx_api.h>

extern WP_Message inboundMessage;

__attribute__((noreturn))
void ReceiverThread_entry(uint32_t parameter)
{
    (void)parameter;

    tx_thread_sleep(50);

    // loop until thread receives a request to terminate
    while (1) 
    {
        WP_Message_Initialize(&inboundMessage);
        WP_Message_PrepareReception(&inboundMessage);

        WP_Message_Process(&inboundMessage);

        // pass control to the OS
        tx_thread_sleep(10);
    }

    // this function never returns
}
