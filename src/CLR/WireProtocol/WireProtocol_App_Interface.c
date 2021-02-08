//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include "WireProtocol_App_Interface.h"

extern int Messaging_ProcessPayload(WP_Message *message);

int WP_App_ProcessHeader(WP_Message *message)
{
    // check for reception buffer overflow
    if (message->m_header.m_size > sizeof(receptionBuffer))
    {
        return false;
    }

    message->m_payload = receptionBuffer;
    return true;
}

int WP_App_ProcessPayload(WP_Message *message)
{
    return Messaging_ProcessPayload(message);
}
