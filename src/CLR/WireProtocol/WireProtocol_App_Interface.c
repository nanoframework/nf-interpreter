//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_App_Interface.h>

static uint8_t receptionBuffer[sizeof(WP_Packet) + WP_PACKET_SIZE];

// Initialize to a packet sequence number impossible to encounter
uint32_t WireProtocolLastPacketSequence = 0x00FEFFFF;

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

#ifdef I_AM_NANOBOOTER

// This implementation is specific of nanoBooter

uint8_t Messaging_ProcessPayload(WP_Message *message)
{
    // Prevent processing duplicate packets
    if (message->m_header.m_seq == WireProtocolLastPacketSequence)
    {
        return false; // Do not even respond to a repeat packet
    }

    // save this packet sequence number
    WireProtocolLastPacketSequence = message->m_header.m_seq;

    if (message->m_header.m_flags & WP_Flags_c_NACK)
    {
        //
        // Bad packet...
        //
        return true;
    }

    uint8_t num;
    const CommandHandlerLookup *cmd;

    if (message->m_header.m_flags & WP_Flags_c_Reply)
    {
        num = GetSizeOfLookup_Reply();
        cmd = c_Lookup_Reply;
    }
    else
    {
        num = GetSizeOfLookup_Request();
        cmd = c_Lookup_Request;
    }

    while (num--)
    {
        if (cmd->command == message->m_header.m_cmd)
        {
            // execute command handler and save the result
            int commandHandlerExecuteResult = ((int (*)(WP_Message *))cmd->handler)(message);

            WP_ReplyToCommand(message, commandHandlerExecuteResult, false, NULL, 0);
            return true;
        }

        cmd++;
    }

    WP_ReplyToCommand(message, false, false, NULL, 0);

    return true;
}

#endif // I_AM_NANOBOOTER
