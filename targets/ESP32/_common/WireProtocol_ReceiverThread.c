//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_idf.h>
#include <WireProtocol_HAL_Interface.h>

void WP_Message_Process();
void WP_Message_PrepareReception();

void ReceiverThread(void const *argument)
{
    (void)argument;

    WP_Message_PrepareReception();

    // loop forever
    while (1)
    {
        WP_Message_Process();

        // Allow other tasks a chance to run
        vTaskDelay(1);
    }

    // nothing to deinitialize or cleanup, so it's safe to return
}

void WP_Message_PrepareReception_Platform()
{
    // empty on purpose, nothing to configure
}
