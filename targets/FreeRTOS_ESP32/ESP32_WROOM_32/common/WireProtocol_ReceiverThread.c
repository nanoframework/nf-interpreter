//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_os.h>
#include <WireProtocol_HAL_Interface.h>
#include <WireProtocol_Message.h>

void ReceiverThread(void const *argument)
{
    (void)argument;

    WP_Message_PrepareReception();

    // loop forever
    while (1)
    {

        WP_Message_Process();

        // Allow other tasks a chance to run
        vTaskDelay(0);
    }

    // nothing to deinitialize or cleanup, so it's safe to return
}
