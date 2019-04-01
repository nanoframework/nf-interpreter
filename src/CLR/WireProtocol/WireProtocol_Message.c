//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include <nanoSupport.h>
#include <targetHAL_Time.h>
#include "WireProtocol_Message.h"

uint8_t receptionBuffer[sizeof(WP_Packet) + WP_PACKET_SIZE];
static uint16_t lastOutboundMessage;
static uint8_t* marker;

// timeout to receive WP payload before bailing out
// 5 secs (100 nsecs units)
static const uint64_t c_PayloadTimeout = 50000000;

extern void debug_printf( const char* format, ... );
extern uint64_t HAL_Time_SysTicksToTime_C(unsigned int sysTicks);

//////////////////////////////////////////
// helper functions

void WP_ReplyToCommand(WP_Message* message, int fSuccess, int fCritical, void* ptr, int size)
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
    message->m_payloadTicks = 0; 
    message->m_rxState = ReceiveState_Idle;
}

void WP_Message_PrepareReception(WP_Message* message)
{
    message->m_rxState = ReceiveState_Initialize;
}

void WP_Message_PrepareRequest(WP_Message* message, uint32_t cmd, uint32_t flags, uint32_t payloadSize, uint8_t* payload)
{
    memcpy(&message->m_header.m_signature, marker ? marker : (uint8_t*)MARKER_PACKET_V1, sizeof(message->m_header.m_signature));

#if defined(WP_IMPLEMENTS_CRC32)
    message->m_header.m_crcData   = SUPPORT_ComputeCRC(payload, payloadSize, 0);
#else
    message->m_header.m_crcData   = 0;
#endif
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
#if defined(WP_IMPLEMENTS_CRC32)
    message->m_header.m_crcHeader = SUPPORT_ComputeCRC((uint8_t*)&message->m_header, sizeof(message->m_header), 0);
#endif
}


void WP_Message_PrepareReply(WP_Message* message, const WP_Packet* req, uint32_t flags, uint32_t payloadSize,  uint8_t* payload)
{
    memcpy(&message->m_header.m_signature, marker ? marker : (uint8_t*)MARKER_PACKET_V1, sizeof(message->m_header.m_signature));

#if defined(WP_IMPLEMENTS_CRC32)
    message->m_header.m_crcData   = SUPPORT_ComputeCRC(payload, payloadSize, 0);
#else
    message->m_header.m_crcData   = 0;
#endif
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
#if defined(WP_IMPLEMENTS_CRC32)
    message->m_header.m_crcHeader = SUPPORT_ComputeCRC((uint8_t*)&message->m_header, sizeof(message->m_header), 0);
#endif
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

int WP_Message_VerifyHeader(WP_Message* message)
{
    message->m_header.m_crcHeader = 0;

#if defined(WP_IMPLEMENTS_CRC32)

    uint32_t crc = message->m_header.m_crcHeader;

    uint32_t computedCrc = SUPPORT_ComputeCRC((uint8_t*)&message->m_header, sizeof(message->m_header), 0);
    message->m_header.m_crcHeader = crc;

    if(computedCrc != crc)
    {
        TRACE( TRACE_ERRORS, "Header CRC check failed: computed: 0x%08X; got: 0x%08X\n", computedCrc, message->m_header.m_crcHeader );
        return false;
    }

#endif

    return true;
}

int WP_Message_VerifyPayload(WP_Message* message)
{
    if(message->m_payload == NULL && message->m_header.m_size)
    {
        return false;
    }

#if defined(WP_IMPLEMENTS_CRC32)

    uint32_t computedCrc = SUPPORT_ComputeCRC(message->m_payload, message->m_header.m_size, 0);
    if(computedCrc != message->m_header.m_crcData)
    {
        TRACE( TRACE_ERRORS, "Payload CRC check failed: computed: 0x%08X; got: 0x%08X\n", computedCrc, message->m_header.m_crcData );
        return false;
    }

#endif

    return true;
}

void WP_Message_ReplyBadPacket(uint32_t flags)
{
    WP_Message message;
    WP_Message_Initialize(&message);
    WP_Message_PrepareRequest(&message, 0, WP_Flags_c_NonCritical | WP_Flags_c_NACK | flags, 0, NULL);

    WP_TransmitMessage(&message);
}

int WP_Message_Process(WP_Message* message)
{
    uint8_t* buf = (uint8_t*)&message->m_header;
    uint16_t len;

    while(true)
    {
        switch(message->m_rxState)
        {
            case ReceiveState_Idle:
                TRACE0( TRACE_STATE, "RxState==IDLE\n");
                return true;

            case ReceiveState_Initialize:
                TRACE0( TRACE_STATE, "RxState==INIT\n");
                WP_Message_Release(message);

                message->m_rxState = ReceiveState_WaitingForHeader;
                message->m_pos     = (uint8_t*)&message->m_header;
                message->m_size    = sizeof(message->m_header);
                break;

            case ReceiveState_WaitingForHeader:
                TRACE0( TRACE_STATE, "RxState==WaitForHeader\n");
                if(WP_ReceiveBytes(message->m_pos, &message->m_size) == false)
                {
                    // didn't receive the expected amount of bytes, returning false
                    TRACE0( TRACE_NODATA, "ReceiveBytes returned false - bailing out\n");
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
                TRACE0( TRACE_STATE, "RxState==ReadingHeader\n");
                if(WP_ReceiveBytes(message->m_pos, &message->m_size) == false)
                {
                    // didn't receive the expected amount of bytes, returning false
                    TRACE0( TRACE_NODATA, "ReceiveBytes returned false - bailing out\n");
                    return false;
                }

                if(message->m_size == 0)
                {
                    message->m_rxState = ReceiveState_CompleteHeader;
                }
                break;

            case ReceiveState_CompleteHeader:
            {
                TRACE0( TRACE_STATE, "RxState=CompleteHeader\n");
                bool fBadPacket = true;

                if(WP_Message_VerifyHeader(message))
                {
                    TRACE( TRACE_HEADERS, "RXMSG: 0x%08X, 0x%08X, 0x%08X\n", message->m_header.m_cmd, message->m_header.m_flags, message->m_header.m_size );

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
                                message->m_payloadTicks = HAL_Time_CurrentSysTicks();
                                message->m_rxState      = ReceiveState_ReadingPayload;
                                message->m_pos          = message->m_payload;
                                message->m_size         = message->m_header.m_size;
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
                    TRACE( TRACE_STATE, "RxState=ReadingPayload. Expecting %d bytes.\n", message->m_size);

                    uint64_t curTicks = HAL_Time_CurrentSysTicks();

                    // If the time between consecutive payload bytes exceeds the timeout threshold then assume that
                    // the rest of the payload is not coming. Reinitialize to synch on the next header. 

                    if(HAL_Time_SysTicksToTime_C( curTicks - message->m_payloadTicks ) < c_PayloadTimeout)
                    {
                        message->m_payloadTicks = curTicks;

                        if(WP_ReceiveBytes(message->m_pos, &message->m_size) == false)
                        {
                            // didn't receive the expected amount of bytes, returning false
                            TRACE0( TRACE_NODATA, "ReceiveBytes returned false - bailing out\n");
                            return false;
                        }

                        if(message->m_size == 0)
                        {
                            message->m_rxState = ReceiveState_CompletePayload;
                        }
                    }
                    else
                    {
                        TRACE0( TRACE_ERRORS, "RxError: Payload InterCharacterTimeout exceeded\n");
                        message->m_rxState = ReceiveState_Initialize;
                    }
                }
                break;

            case ReceiveState_CompletePayload:
                TRACE0( TRACE_STATE, "RxState=CompletePayload\n");
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
                TRACE0( TRACE_ERRORS, "RxState=UNKNOWN!!\n");
                return false;
        }
    }
}

void WP_SendProtocolMessage(WP_Message *message)
{
    TRACE( TRACE_HEADERS, "TXMSG: 0x%08X, 0x%08X, 0x%08X\n", message->m_header.m_cmd, message->m_header.m_flags, message->m_header.m_size );
    WP_TransmitMessage(message);
}

void WP_PrepareAndSendProtocolMessage(uint32_t cmd, uint32_t payloadSize, uint8_t* payload, uint32_t flags)
{
    WP_Message message;
    WP_Message_Initialize(&message);
    WP_Message_PrepareRequest(&message, cmd, flags, payloadSize, payload);

    TRACE( TRACE_HEADERS, "TXMSG: 0x%08X, 0x%08X, 0x%08X\n", message.m_header.m_cmd, message.m_header.m_flags, message.m_header.m_size );
    WP_TransmitMessage(&message);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// weak implementations of the functions (to be replaced with _strong_ implementations if and when required) //

__nfweak void debug_printf( const char* format, ... )
{
    (void)format;

    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
