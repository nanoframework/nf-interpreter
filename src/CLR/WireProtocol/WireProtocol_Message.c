//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include <nanoSupport.h>
#include <targetHAL_Time.h>
#include "WireProtocol_Message.h"

uint8_t receptionBuffer[sizeof(WP_Packet) + WP_PACKET_SIZE];
static uint16_t _lastOutboundMessage = 65535;
uint64_t _payloadTicks;
static uint8_t *_marker;
static uint8_t *_pos;
static uint16_t _size;
static uint8_t _rxState;
static WP_Message _inboundMessage;

// timeout to receive WP payload before bailing out
// 5 secs (100 nsecs units)
static const uint64_t c_PayloadTimeout = 50000000;

extern void debug_printf(const char *format, ...);
extern uint64_t HAL_Time_SysTicksToTime_C(uint64_t sysTicks);

//////////////////////////////////////////
// helper functions

void WP_ReplyToCommand(WP_Message *message, int fSuccess, int fCritical, void *ptr, int size)
{
    WP_Message msgReply;
    uint32_t flags = 0;

    //
    // Make sure we reply only once!
    //
    if (message->m_header.m_flags & WP_Flags_c_NonCritical)
        return;
    message->m_header.m_flags |= WP_Flags_c_NonCritical;

    //
    // No caching in the request, no caching in the reply...
    //
    if (message->m_header.m_flags & WP_Flags_c_NoCaching)
        flags |= WP_Flags_c_NoCaching;

    if (fSuccess)
        flags |= WP_Flags_c_ACK;
    else
        flags |= WP_Flags_c_NACK;
    if (!fCritical)
        flags |= WP_Flags_c_NonCritical;

    if (fSuccess == false)
    {
        ptr = NULL;
        size = 0;
    }

    WP_Message_Initialize(&msgReply);

    WP_Message_PrepareReply(&msgReply, &message->m_header, flags, size, (uint8_t *)ptr);

    WP_TransmitMessage(&msgReply);
}

void WP_Message_Initialize(WP_Message *message)
{
    memset(&message->m_header, 0, sizeof(message->m_header));
    message->m_payload = NULL;
}

void WP_Message_PrepareReception()
{
    _rxState = ReceiveState_Initialize;

    WP_Message_Initialize(&_inboundMessage);
}

void WP_Message_PrepareRequest(
    WP_Message *message,
    uint32_t cmd,
    uint32_t flags,
    uint32_t payloadSize,
    uint8_t *payload)
{
    memcpy(
        &message->m_header.m_signature,
        _marker ? _marker : (uint8_t *)MARKER_PACKET_V1,
        sizeof(message->m_header.m_signature));

#if defined(WP_IMPLEMENTS_CRC32)
    message->m_header.m_crcData = SUPPORT_ComputeCRC(payload, payloadSize, 0);
#else
    message->m_header.m_crcData = 0;
#endif
    message->m_header.m_cmd = cmd;
    message->m_header.m_seq = _lastOutboundMessage++;
    message->m_header.m_seqReply = 0;
    message->m_header.m_flags = flags;
    message->m_header.m_size = payloadSize;
    message->m_payload = payload;

    //
    // The CRC for the header is computed setting the CRC field to zero and then running the CRC algorithm.
    //
    message->m_header.m_crcHeader = 0;
#if defined(WP_IMPLEMENTS_CRC32)
    message->m_header.m_crcHeader = SUPPORT_ComputeCRC((uint8_t *)&message->m_header, sizeof(message->m_header), 0);
#endif
}

void WP_Message_PrepareReply(
    WP_Message *message,
    const WP_Packet *req,
    uint32_t flags,
    uint32_t payloadSize,
    uint8_t *payload)
{
    memcpy(
        &message->m_header.m_signature,
        _marker ? _marker : (uint8_t *)MARKER_PACKET_V1,
        sizeof(message->m_header.m_signature));

#if defined(WP_IMPLEMENTS_CRC32)
    message->m_header.m_crcData = SUPPORT_ComputeCRC(payload, payloadSize, 0);
#else
    message->m_header.m_crcData = 0;
#endif
    message->m_header.m_cmd = req->m_cmd;
    message->m_header.m_seq = _lastOutboundMessage++;
    message->m_header.m_seqReply = req->m_seq;
    message->m_header.m_flags = flags | WP_Flags_c_Reply;
    message->m_header.m_size = payloadSize;
    message->m_payload = payload;

    //
    // The CRC for the header is computed setting the CRC field to zero and then running the CRC algorithm.
    //
    message->m_header.m_crcHeader = 0;
#if defined(WP_IMPLEMENTS_CRC32)
    message->m_header.m_crcHeader = SUPPORT_ComputeCRC((uint8_t *)&message->m_header, sizeof(message->m_header), 0);
#endif
}

void WP_Message_SetPayload(WP_Message *message, uint8_t *payload)
{
    message->m_payload = payload;
}

void WP_Message_Release(WP_Message *message)
{
    if (message->m_payload)
    {
        message->m_payload = NULL;
    }
}

int WP_Message_VerifyHeader(WP_Message *message)
{
#if defined(WP_IMPLEMENTS_CRC32)

    uint32_t incommingCrc = message->m_header.m_crcHeader;
    message->m_header.m_crcHeader = 0;

    uint32_t computedCrc = SUPPORT_ComputeCRC((uint8_t *)&message->m_header, sizeof(message->m_header), 0);

    message->m_header.m_crcHeader = incommingCrc;

    if (computedCrc != incommingCrc)
    {
        TRACE(
            TRACE_ERRORS,
            "Header CRC check failed: computed: 0x%08X; got: 0x%08X\n",
            computedCrc,
            message->m_header.m_crcHeader);
        return false;
    }
#else

    (void)message;

#endif

    return true;
}

int WP_Message_VerifyPayload(WP_Message *message)
{
    if (message->m_payload == NULL && message->m_header.m_size)
    {
        return false;
    }

#if defined(WP_IMPLEMENTS_CRC32)

    uint32_t computedCrc = SUPPORT_ComputeCRC(message->m_payload, message->m_header.m_size, 0);
    if (computedCrc != message->m_header.m_crcData)
    {
        TRACE(
            TRACE_ERRORS,
            "Payload CRC check failed: computed: 0x%08X; got: 0x%08X\n",
            computedCrc,
            message->m_header.m_crcData);
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

int WP_Message_Process()
{
    uint8_t *buf = (uint8_t *)&_inboundMessage.m_header;
    uint16_t len;

    while (true)
    {
        switch (_rxState)
        {
            case ReceiveState_Idle:
                TRACE0(TRACE_STATE, "RxState==IDLE\n");
                return true;

            case ReceiveState_Initialize:
                TRACE0(TRACE_STATE, "RxState==INIT\n");
                WP_Message_Initialize(&_inboundMessage);

                _rxState = ReceiveState_WaitingForHeader;
                _pos = (uint8_t *)&_inboundMessage.m_header;
                _size = sizeof(_inboundMessage.m_header);

                break;

            case ReceiveState_WaitingForHeader:
                TRACE0(TRACE_STATE, "RxState==WaitForHeader\n");

                if (WP_ReceiveBytes(_pos, &_size) == false)
                {
                    // didn't receive the expected amount of bytes, returning false
                    TRACE0(TRACE_NODATA, "ReceiveBytes returned false - bailing out\n");

                    return false;
                }

                // Synch to the start of a message by looking for a valid MARKER
                while (true)
                {
                    len = sizeof(_inboundMessage.m_header) - _size;

                    if (len <= 0)
                    {
                        break;
                    }

                    size_t lenCmp = min(len, sizeof(_inboundMessage.m_header.m_signature));

                    if (memcmp(&_inboundMessage.m_header, MARKER_DEBUGGER_V1, lenCmp) == 0)
                    {
                        break;
                    }
                    if (memcmp(&_inboundMessage.m_header, MARKER_PACKET_V1, lenCmp) == 0)
                    {
                        break;
                    }

                    memmove(&buf[0], &buf[1], len - 1);

                    _pos--;
                    _size++;
                }

                if (len >= sizeof(_inboundMessage.m_header.m_signature))
                {
                    _rxState = ReceiveState_ReadingHeader;
                }
                break;

            case ReceiveState_ReadingHeader:
                TRACE0(TRACE_STATE, "RxState==ReadingHeader\n");
                if (WP_ReceiveBytes(_pos, &_size) == false)
                {
                    // didn't receive the expected amount of bytes, returning false
                    TRACE0(TRACE_NODATA, "ReceiveBytes returned false - bailing out\n");
                    return false;
                }

                if (_size == 0)
                {
                    _rxState = ReceiveState_CompleteHeader;
                }
                break;

            case ReceiveState_CompleteHeader:
            {
                TRACE0(TRACE_STATE, "RxState=CompleteHeader\n");
                bool fBadPacket = true;

                if (WP_Message_VerifyHeader(&_inboundMessage))
                {
                    TRACE(
                        TRACE_HEADERS,
                        "RXMSG: 0x%08X, 0x%08X, 0x%08X\n",
                        _inboundMessage.m_header.m_cmd,
                        _inboundMessage.m_header.m_flags,
                        _inboundMessage.m_header._size);

                    if (WP_App_ProcessHeader(&_inboundMessage))
                    {
                        fBadPacket = false;

                        if (_inboundMessage.m_header.m_size)
                        {
                            if (_inboundMessage.m_payload == NULL)
                            {
                                // Bad, no buffer...
                                _rxState = ReceiveState_Initialize;
                            }
                            else
                            {
                                _payloadTicks = HAL_Time_CurrentSysTicks();
                                _rxState = ReceiveState_ReadingPayload;
                                _pos = _inboundMessage.m_payload;
                                _size = _inboundMessage.m_header.m_size;
                            }
                        }
                        else
                        {
                            _rxState = ReceiveState_CompletePayload;
                        }
                    }
                }

                if (fBadPacket)
                {
                    if ((_inboundMessage.m_header.m_flags & WP_Flags_c_NonCritical) == 0)
                    {
                        WP_Message_ReplyBadPacket(WP_Flags_c_BadHeader);
                    }

                    _rxState = ReceiveState_Initialize;
                }
            }
            break;

            case ReceiveState_ReadingPayload:
            {
                TRACE(TRACE_STATE, "RxState=ReadingPayload. Expecting %d bytes.\n", message->_size);

                uint64_t curTicks = HAL_Time_CurrentSysTicks();

                // If the time between consecutive payload bytes exceeds the timeout threshold then assume that
                // the rest of the payload is not coming. Reinitialize to synch on the next header.

                if (HAL_Time_SysTicksToTime_C(curTicks - _payloadTicks) < c_PayloadTimeout)
                {
                    _payloadTicks = curTicks;

                    if (WP_ReceiveBytes(_pos, &_size) == false)
                    {
                        // didn't receive the expected amount of bytes, returning false
                        TRACE0(TRACE_NODATA, "ReceiveBytes returned false - bailing out\n");
                        return false;
                    }

                    if (_size == 0)
                    {
                        _rxState = ReceiveState_CompletePayload;
                    }
                }
                else
                {
                    TRACE0(TRACE_ERRORS, "RxError: Payload InterCharacterTimeout exceeded\n");
                    _rxState = ReceiveState_Initialize;
                }
            }
            break;

            case ReceiveState_CompletePayload:
                TRACE0(TRACE_STATE, "RxState=CompletePayload\n");
                if (WP_Message_VerifyPayload(&_inboundMessage) == true)
                {
                    WP_App_ProcessPayload(&_inboundMessage);
                }
                else
                {
                    WP_Message_ReplyBadPacket(WP_Flags_c_BadPayload);
                }

                _rxState = ReceiveState_Initialize;
                break;

            default:
                // unknown state
                TRACE0(TRACE_ERRORS, "RxState=UNKNOWN!!\n");
                return false;
        }
    }
}

void WP_SendProtocolMessage(WP_Message *message)
{
    TRACE(
        TRACE_HEADERS,
        "TXMSG: 0x%08X, 0x%08X, 0x%08X\n",
        message->m_header.m_cmd,
        message->m_header.m_flags,
        message->m_header._size);
    WP_TransmitMessage(message);
}

void WP_PrepareAndSendProtocolMessage(uint32_t cmd, uint32_t payloadSize, uint8_t *payload, uint32_t flags)
{
    WP_Message message;
    WP_Message_Initialize(&message);
    WP_Message_PrepareRequest(&message, cmd, flags, payloadSize, payload);

    TRACE(
        TRACE_HEADERS,
        "TXMSG: 0x%08X, 0x%08X, 0x%08X\n",
        message.m_header.m_cmd,
        message.m_header.m_flags,
        message.m_header._size);
    WP_TransmitMessage(&message);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// weak implementations of the functions (to be replaced with _strong_ implementations if and when required) //

__nfweak void debug_printf(const char *format, ...)
{
    (void)format;

    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
