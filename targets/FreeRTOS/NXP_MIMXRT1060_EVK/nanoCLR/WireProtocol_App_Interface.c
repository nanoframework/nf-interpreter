//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_MonitorCommands.h>

// declaration for wrapper function
extern int CLR_Messaging_ProcessPayload(WP_Message* msg);

////////////////////////////////////////////////////

int WP_App_ProcessHeader(WP_Message* message)
{
    // check for reception buffer overflow 
    if(message->m_header.m_size > sizeof(receptionBuffer))
    {
        return false;
    }

    message->m_payload = receptionBuffer;
    return true;
}

int WP_App_ProcessPayload(WP_Message* message)
{
    return CLR_Messaging_ProcessPayload(message);
}
