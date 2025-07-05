//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#if !defined(VIRTUAL_DEVICE)
#include <nanoHAL_v2.h>
#endif
#include <nanoWeak.h>
#include <nanoSupport.h>
#include <targetHAL_Time.h>
#include "WireProtocol_Message.h"

// from nanoHAL_Time.h (TIME_CONVERSION__TO_SECONDS)
#define TIME_CONVERSION__TO_SYSTICKS 10000000

static uint16_t _lastOutboundMessage = 0;
static uint64_t _receiveExpiryTicks;
static uint8_t _rxState;
static uint8_t *_pos;
static uint32_t _size;
static WP_Message _inboundMessage;

#if defined(TRACE_MASK) && (TRACE_MASK & TRACE_VERBOSE) != 0
// used WP_Message_Process() and methods it calls to avoid flooding TRACE
int32_t traceLoopCounter = 0;
#endif

#ifdef DEBUG
volatile uint8_t _rxStatePrev;
#endif

// timeout to receive WP payload before bailing out
// 5 secs (100 nsecs units)
static const uint64_t c_PayloadTimeout = 5 * TIME_CONVERSION__TO_SYSTICKS;
// timeout to receive WP header before bailing out
// 2 secs (100 nsecs units)
static const uint64_t c_HeaderTimeout = 2 * TIME_CONVERSION__TO_SYSTICKS;

extern void debug_printf(const char *format, ...);

//////////////////////////////////////////
// helper functions

bool CheckTimeout(uint64_t expiryTicks)
{
    uint64_t now = HAL_Time_SysTicksToTime(HAL_Time_CurrentSysTicks());
    return expiryTicks > now;
}

void SetReceiveExpiryTicks(uint64_t timeout)
{
    _receiveExpiryTicks = HAL_Time_SysTicksToTime(HAL_Time_CurrentSysTicks()) + timeout;
}

void RestartStateMachine()
{
    _rxState = ReceiveState_Initialize;
}

bool IsMarkerMatched(void *header, const void *marker, size_t len)
{
    return memcmp(header, marker, len) == 0;
}

bool SyncToMessageStart()
{
    uint32_t len;

    while (true)
    {
        len = sizeof(_inboundMessage.m_header) - _size;

        if (len <= 0)
        {
            break;
        }

        size_t lenCmp = min(len, sizeof(((WP_Packet *)0)->m_signature));

        if (IsMarkerMatched(&_inboundMessage.m_header, MARKER_DEBUGGER_V1, lenCmp) ||
            IsMarkerMatched(&_inboundMessage.m_header, MARKER_PACKET_V1, lenCmp))
        {
            break;
        }

        // Calculate the source and destination pointers
        uint8_t *src = (uint8_t *)&_inboundMessage.m_header + 1;
        uint8_t *dst = (uint8_t *)&_inboundMessage.m_header;
        size_t moveLength = len - 1;

        // Ensure that the memory regions do not exceed allocated bounds
        if ((src + moveLength >= (uint8_t *)&_inboundMessage + sizeof(_inboundMessage)) ||
            (dst + moveLength >= (uint8_t *)&_inboundMessage + sizeof(_inboundMessage)))
        {
            return false;
        }

        // Perform the memory move
        memmove(dst, src, moveLength);

        // Update pointer and expected size
        _pos--;
        _size++;

        // Sanity checks
        if (_size > sizeof(_inboundMessage.m_header) || _pos < (uint8_t *)&_inboundMessage.m_header)
        {
            return false;
        }
    }

    return true;
}

void WP_ReplyToCommand(WP_Message *message, uint8_t fSuccess, uint8_t fCritical, void *ptr, uint32_t size)
{
    WP_Message msgReply;
    uint32_t flags = 0;

    //
    // Make sure we reply only once!
    //
    if (message->m_header.m_flags & WP_Flags_c_NonCritical)
    {
        return;
    }

    message->m_header.m_flags |= WP_Flags_c_NonCritical;

    //
    // No caching in the request, no caching in the reply...
    //
    if (message->m_header.m_flags & WP_Flags_c_NoCaching)
    {
        flags |= WP_Flags_c_NoCaching;
    }

    if (fSuccess)
    {
        flags |= WP_Flags_c_ACK;
    }
    else
    {
        flags |= WP_Flags_c_NACK;
    }

    if (!fCritical)
    {
        flags |= WP_Flags_c_NonCritical;
    }

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
    _pos = (uint8_t *)&_inboundMessage.m_header;
    _size = sizeof(_inboundMessage.m_header);

    // platform initializations
    WP_Message_PrepareReception_Platform();
}

void WP_Message_PrepareRequest(
    WP_Message *message,
    uint32_t cmd,
    uint32_t flags,
    uint32_t payloadSize,
    uint8_t *payload)
{
    memcpy(&message->m_header.m_signature, (uint8_t *)MARKER_PACKET_V1, sizeof(message->m_header.m_signature));

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
    memcpy(&message->m_header.m_signature, (uint8_t *)MARKER_PACKET_V1, sizeof(message->m_header.m_signature));

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

uint8_t WP_Message_VerifyHeader(WP_Message *message)
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

    // check for reception buffer overflow
    if (message->m_header.m_size > WP_PACKET_SIZE)
    {
        return false;
    }

    return true;
}

uint8_t WP_Message_VerifyPayload(WP_Message *message)
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

void WP_ReportBadPacket(uint32_t flag)
{
    WP_Message msg;
    uint32_t flags = flag | WP_Flags_c_NonCritical | WP_Flags_c_NACK;

    WP_Message_Initialize(&msg);

    WP_Message_PrepareRequest(&msg, 0, flags, 0, NULL);

    WP_TransmitMessage(&msg);
}

void WP_Message_Process()
{
    uint32_t len;

    while (true)
    {

#if !defined(VIRTUAL_DEVICE)
        ASSERT(_rxState >= ReceiveState_Idle && _rxState <= ReceiveState_CompletePayload);
#endif

#ifdef DEBUG
        // store this here to debug issues with wrong sequence of state machine
        _rxStatePrev = _rxState;
#endif
        switch (_rxState)
        {
            case ReceiveState_Idle:
                TRACE0(TRACE_STATE, "RxState==IDLE\n");
                break;

            case ReceiveState_Initialize:
                TRACE0(TRACE_STATE, "RxState==INIT\n");
                WP_Message_Initialize(&_inboundMessage);

                _rxState = ReceiveState_WaitingForHeader;
                _pos = (uint8_t *)&_inboundMessage.m_header;
                _size = sizeof(_inboundMessage.m_header);

                // reset timeout to start receiving the header
                SetReceiveExpiryTicks(c_HeaderTimeout);
                break;

            case ReceiveState_WaitingForHeader:
                // Warning: Including TRACE_VERBOSE will NOT output the following TRACE on every loop
                //          of the statemachine to avoid flooding the trace.
                TRACE0_LIMIT(TRACE_VERBOSE, 100, "RxState==WaitForHeader\n");

                len = _size;

                WP_ReceiveBytes(&_pos, &_size);

                if (_size == len)
                {
                    // no new bytes received...
                    if (CheckTimeout(_receiveExpiryTicks))
                    {
                        // still waiting for header bytes
                        break;
                    }
                    else
                    {
                        // timeout expired, something went wrong
                        TRACE0(TRACE_ERRORS, "RxError: Header InterCharacterTimeout exceeded\n");

                        RestartStateMachine();

                        // exit the loop to allow other RTOS threads to run
                        return;
                    }
                }

                if (!SyncToMessageStart())
                {
                    // something went wrong
                    TRACE0(TRACE_ERRORS, "RxError: Failed to sync to message start\n");

                    RestartStateMachine();

                    // exit the loop to allow other RTOS threads to run
                    return;
                }

                if (len >= sizeof(_inboundMessage.m_header.m_signature))
                {
                    // still missing some bytes for the header
                    _rxState = ReceiveState_ReadingHeader;
                    SetReceiveExpiryTicks(c_HeaderTimeout);
                }

                break;

            case ReceiveState_ReadingHeader:
                TRACE0(TRACE_STATE, "RxState==ReadingHeader\n");

                // If the time between consecutive header bytes exceeds the timeout threshold then assume that
                // the rest of the header is not coming. Reinitialize to sync with the next header.

                if (CheckTimeout(_receiveExpiryTicks))
                {
                    WP_ReceiveBytes(&_pos, &_size);

                    if (_size == 0)
                    {
                        _rxState = ReceiveState_CompleteHeader;
                    }
                }
                else
                {
                    TRACE0(TRACE_ERRORS, "RxError: Header InterCharacterTimeout exceeded\n");
                    RestartStateMachine();

                    return;
                }

                break;

            case ReceiveState_CompleteHeader:

                TRACE0(TRACE_STATE, "RxState==CompleteHeader\n");

                if (WP_Message_VerifyHeader(&_inboundMessage))
                {
                    TRACE_WP_HEADER(WP_RXMSG_Hdr_OK, &_inboundMessage);

                    if (WP_App_ProcessHeader(&_inboundMessage))
                    {
                        if (_inboundMessage.m_header.m_size != 0)
                        {
                            if (_inboundMessage.m_payload != NULL)
                            {
                                SetReceiveExpiryTicks(c_PayloadTimeout);
                                _pos = _inboundMessage.m_payload;
                                _size = _inboundMessage.m_header.m_size;

                                _rxState = ReceiveState_ReadingPayload;

                                break;
                            }
                            else
                            {
                                // Bad, no buffer...
                            }
                        }
                        else
                        {
                            _rxState = ReceiveState_CompletePayload;

                            break;
                        }
                    }
                }

                // one the verifications above failed, report...
                WP_ReportBadPacket(WP_Flags_c_BadHeader);

                //... and restart state machine
                _rxState = ReceiveState_Initialize;
                return;

            case ReceiveState_ReadingPayload:

                TRACE(TRACE_STATE, "RxState==ReadingPayload. Expecting %d bytes.\n", _inboundMessage.m_header.m_size);

                // If the time between consecutive payload bytes exceeds the timeout threshold then assume that
                // the rest of the payload is not coming. Reinitialize to sync with the next header.

                if (CheckTimeout(_receiveExpiryTicks))
                {
                    WP_ReceiveBytes(&_pos, &_size);

                    if (_size == 0)
                    {
                        _rxState = ReceiveState_CompletePayload;
                    }
                }
                else
                {
                    TRACE0(TRACE_ERRORS, "RxError: Payload InterCharacterTimeout exceeded\n");
                    RestartStateMachine();

                    return;
                }

                break;

            case ReceiveState_CompletePayload:
                TRACE0(TRACE_STATE, "RxState==CompletePayload\n");
                if (WP_Message_VerifyPayload(&_inboundMessage))
                {
                    TRACE_WP_HEADER(WP_RXMSG_Payload_Ok, &_inboundMessage);
                    WP_App_ProcessPayload(&_inboundMessage);
                }
                else
                {
                    TRACE_WP_HEADER(WP_RXMSG_NAK, &_inboundMessage);
                    WP_ReportBadPacket(WP_Flags_c_BadPayload);
                }

                RestartStateMachine();

                return;

            default:
                // unknown state
                TRACE0(TRACE_ERRORS, "RxState==UNKNOWN!!\n");
                return;
        }
    }
}

void WP_SendProtocolMessage(WP_Message *message)
{
    WP_TransmitMessage(message);
}

void WP_PrepareAndSendProtocolMessage(uint32_t cmd, uint32_t payloadSize, uint8_t *payload, uint32_t flags)
{
    WP_Message message;

    WP_Message_Initialize(&message);

    WP_Message_PrepareRequest(&message, cmd, flags, payloadSize, payload);

    WP_TransmitMessage(&message);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// weak implementations of the functions (to be replaced with _strong_ implementations if and when required) //

#if !defined(BUILD_RTM)
__nfweak void debug_printf(const char *format, ...)
{
    (void)format;

    return;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(TRACE_MASK) && TRACE_MASK & TRACE_HEADERS != 0
void WP_TraceHeader(const char *pstrLabel, WP_Message *message)
{
    TRACE(
        TRACE_HEADERS,
        "%scmd=0x%08X, flags=0x%08X, hCRC=0x%08X, pCRC=0x%08X, seq=0x%04X replySeq=0x%04X len=%d\n",
        pstrLabel,
        message->m_header.m_cmd,
        message->m_header.m_flags,
        message->m_header.m_crcHeader,
        message->m_header.m_crcData,
        message->m_header.m_seq,
        message->m_header.m_seqReply,
        message->m_header.m_size);
}
#endif
