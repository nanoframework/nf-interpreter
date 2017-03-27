//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoSupport.h>
#include "WireProtocol_Message.h"

uint8_t receptionBuffer[2048];
static uint16_t lastOutboundMessage;
// FIXME #146 uint32_t m_payloadTicks;
static uint8_t* marker;

//////////////////////////////////////////
// helper functions

void WP_ReplyToCommand(WP_Message* message, bool fSuccess, bool fCritical, void* ptr, int size)
{
    WP_Message msgReply;
    uint32_t     flags = 0;

    //
    // Make sure we reply only once!
    //
    if(message->m_header.m_flags & WP_Flags_c_NonCritical) return;
    message->m_header.m_flags |= WP_Flags_c_NonCritical;

    //
    // No caching in the request, no caching in the reply...
    //
    if(message->m_header.m_flags & WP_Flags_c_NoCaching) flags |= WP_Flags_c_NoCaching;

    if(fSuccess  ) flags |= WP_Flags_c_ACK;
    else           flags |= WP_Flags_c_NACK;    
    if(!fCritical) flags |= WP_Flags_c_NonCritical;

    if(fSuccess == false)
    {
        ptr  = NULL;
        size = 0;
    }

    WP_Message_Initialize(&msgReply);

    WP_Message_PrepareReply(&msgReply, &message->m_header, flags, size, (uint8_t*)ptr );

    WP_TransmitMessage(&msgReply);
}

void WP_Message_Initialize(WP_Message* message)
{
    memset(&message->m_header, 0, sizeof(message->m_header));
    message->m_payload = NULL;
    message->m_pos     = NULL;
    message->m_size    = 0;
    message->m_rxState = ReceiveState_Idle;
}

void WP_Message_PrepareReception(WP_Message* message)
{
    message->m_rxState = ReceiveState_Initialize;
}

void WP_Message_PrepareRequest(WP_Message* message, uint32_t cmd, uint32_t flags, uint32_t payloadSize, uint8_t* payload)
{
    memcpy(&message->m_header.m_signature, marker ? marker : (uint8_t*)MARKER_PACKET_V1, sizeof(message->m_header.m_signature));

    message->m_header.m_crcData   = SUPPORT_ComputeCRC(payload, payloadSize, 0);
    message->m_header.m_cmd       = cmd;
    message->m_header.m_seq       = lastOutboundMessage++;
    message->m_header.m_seqReply  = 0;
    message->m_header.m_flags     = flags;
    message->m_header.m_size      = payloadSize;
    message->m_payload            = payload;

    //
    // The CRC for the header is computed setting the CRC field to zero and then running the CRC algorithm.
    //
    message->m_header.m_crcHeader = 0;
    message->m_header.m_crcHeader = SUPPORT_ComputeCRC((uint8_t*)&message->m_header, sizeof(message->m_header), 0);
}


void WP_Message_PrepareReply(WP_Message* message, const WP_Packet* req, uint32_t flags, uint32_t payloadSize,  uint8_t* payload)
{
    memcpy(&message->m_header.m_signature, marker ? marker : (uint8_t*)MARKER_PACKET_V1, sizeof(message->m_header.m_signature));

    message->m_header.m_crcData   = SUPPORT_ComputeCRC(payload, payloadSize, 0);
    message->m_header.m_cmd       = req->m_cmd;
    message->m_header.m_seq       = lastOutboundMessage++;
    message->m_header.m_seqReply  = req->m_seq;
    message->m_header.m_flags     = flags | WP_Flags_c_Reply;
    message->m_header.m_size      = payloadSize;
    message->m_payload            = payload;

    //
    // The CRC for the header is computed setting the CRC field to zero and then running the CRC algorithm.
    //
    message->m_header.m_crcHeader = 0;
    message->m_header.m_crcHeader = SUPPORT_ComputeCRC((uint8_t*)&message->m_header, sizeof(message->m_header), 0);
}

void WP_Message_SetPayload(WP_Message* message, uint8_t* payload)
{
    message->m_payload = payload;
}

void WP_Message_Release(WP_Message* message)
{
    if(message->m_payload)
    {
        message->m_payload = NULL;
    }
}

bool WP_Message_VerifyHeader(WP_Message* message)
{
    uint32_t crc = message->m_header.m_crcHeader;
    message->m_header.m_crcHeader = 0;
    uint32_t computedCrc = SUPPORT_ComputeCRC((uint8_t*)&message->m_header, sizeof(message->m_header), 0);
    message->m_header.m_crcHeader = crc;

    if(computedCrc != crc)
    {
        return false;
    }
    return true;
}

bool WP_Message_VerifyPayload(WP_Message* message)
{
    if(message->m_payload == NULL && message->m_header.m_size)
    {
        return false;
    }

    uint32_t computedCrc = SUPPORT_ComputeCRC(message->m_payload, message->m_header.m_size, 0);
    if(computedCrc != message->m_header.m_crcData)
    {
        return false;
    }
    return true;
}

void WP_Message_ReplyBadPacket(uint32_t flags)
{
    WP_Message message;
    WP_Message_Initialize(&message);
    WP_Message_PrepareRequest(&message, 0, WP_Flags_c_NonCritical | WP_Flags_c_NACK | flags, 0, NULL);

    WP_TransmitMessage(&message);
}

bool WP_Message_Process(WP_Message* message)
{
    uint8_t* buf = (uint8_t*)&message->m_header;
    int len;

    while(true)
    {
        switch(message->m_rxState)
        {
            case ReceiveState_Idle:
                return true;

            case ReceiveState_Initialize:
                WP_Message_Release(message);

                message->m_rxState = ReceiveState_WaitingForHeader;
                message->m_pos     = (uint8_t*)&message->m_header;
                message->m_size    = sizeof(message->m_header);
                break;

            case ReceiveState_WaitingForHeader:
                //TRACE0(TRACE_STATE, "RxState==WaitForHeader\n");
                if(WP_ReceiveBytes(message->m_pos, &message->m_size) == false)
                {
                    // didn't receive the expected amount of bytes, returning false
                    return false;
                }

                // Synch to the start of a message.
                while(true)
                {
                    len = sizeof(message->m_header) - message->m_size;
                    if(len <= 0)
                    {
                        break;
                    }

                    size_t lenCmp = min(len, sizeof(message->m_header.m_signature));

                    if(memcmp(&message->m_header, MARKER_DEBUGGER_V1, lenCmp) == 0)
                    {
                        break;
                    }
                    if(memcmp(&message->m_header, MARKER_PACKET_V1, lenCmp) == 0)
                    {
                        break;
                    }

                    memmove(&buf[0], &buf[1], len-1);

                    message->m_pos--;
                    message->m_size++;
                }

                if(len >= sizeof(message->m_header.m_signature))
                {
                    message->m_rxState = ReceiveState_ReadingHeader;
                }
                break;

            case ReceiveState_ReadingHeader:
                if(WP_ReceiveBytes(message->m_pos, &message->m_size) == false)
                {
                    // didn't receive the expected amount of bytes, returning false
                    return false;
                }

                if(message->m_size == 0)
                {
                    message->m_rxState = ReceiveState_CompleteHeader;
                }
                break;

            case ReceiveState_CompleteHeader:
            {
                bool fBadPacket = true;

                if(WP_Message_VerifyHeader(message))
                {
                    if(WP_App_ProcessHeader(message))
                    {
                        fBadPacket = false;

                        if(message->m_header.m_size)
                        {
                            if(message->m_payload == NULL) // Bad, no buffer...
                            {
                                message->m_rxState = ReceiveState_Initialize;
                            }
                            else
                            {
                                // FIXME #146 m_payloadTicks = HAL_Time_CurrentSysTicks();
                                message->m_rxState = ReceiveState_ReadingPayload;
                                message->m_pos     = message->m_payload;
                                message->m_size    = message->m_header.m_size;
                            }
                        }
                        else
                        {
                            message->m_rxState = ReceiveState_CompletePayload;
                        }
                    }
                }

                if(fBadPacket)
                {
                    if((message->m_header.m_flags & WP_Flags_c_NonCritical) == 0)
                    {
                        WP_Message_ReplyBadPacket(WP_Flags_c_BadHeader);
                    }

                    message->m_rxState = ReceiveState_Initialize;
                }
            }
            break;

            case ReceiveState_ReadingPayload:
                {
                    if(WP_ReceiveBytes(message->m_pos, &message->m_size) == false)
                    {
                        // didn't receive the expected amount of bytes, returning false
                        return false;
                    }

                    if(message->m_size == 0)
                    {
                        message->m_rxState = ReceiveState_CompletePayload;
                    }
                }
                break;

            case ReceiveState_CompletePayload:
                if(WP_Message_VerifyPayload(message) == true)
                {
                    WP_App_ProcessPayload(message);
                }
                else
                {
                    WP_Message_ReplyBadPacket(WP_Flags_c_BadPayload);
                }

                message->m_rxState = ReceiveState_Initialize;
                break;

            default:
                // unknow state
                return false;
        }
    }
}

void WP_SendProtocolMessage(WP_Message *message)
{
    WP_TransmitMessage(&message);
}

void WP_PrepareAndSendProtocolMessage(uint32_t cmd, uint32_t payloadSize, uint8_t* payload, uint32_t flags)
{
    WP_Message message;
    WP_Message_Initialize(&message);
    WP_Message_PrepareRequest(&message, cmd, flags, payloadSize, payload);

    WP_TransmitMessage(&message);
}
