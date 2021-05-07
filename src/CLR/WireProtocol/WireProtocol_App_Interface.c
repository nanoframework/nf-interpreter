//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include "WireProtocol_App_Interface.h"

extern uint8_t Messaging_ProcessPayload(WP_Message *message);
static uint8_t receptionBuffer[sizeof(WP_Packet) + WP_PACKET_SIZE];

uint8_t WP_App_ProcessHeader(WP_Message *message)
{
    // check for reception buffer overflow
    if (message->m_header.m_size > WP_PACKET_SIZE)
    {
        return false;
    }

    message->m_payload = receptionBuffer;
    
    return true;
}

uint8_t WP_App_ProcessPayload(WP_Message *message)
{
    return Messaging_ProcessPayload(message);
}
