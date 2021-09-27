//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef WIREPROTOCOL_MESSAGE_H
#define WIREPROTOCOL_MESSAGE_H

#include "WireProtocol.h"
#include "WireProtocol_App_Interface.h"
#include "WireProtocol_HAL_Interface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
#define TRACE_ERRORS  1
#define TRACE_HEADERS 2
#define TRACE_STATE   4
#define TRACE_NODATA  8
#define TRACE_VERBOSE 16

#if defined(TRACE_MASK) && TRACE_MASK != 0
#define TRACE0(f, msg)                                                                                                 \
    if ((f)&TRACE_MASK)                                                                                                \
    debug_printf(msg)
#define TRACE(f, msg, ...)                                                                                             \
    if ((f)&TRACE_MASK)                                                                                                \
    debug_printf(msg, __VA_ARGS__)

#if defined(TRACE_MASK) && (TRACE_MASK & TRACE_VERBOSE) != 0
#define TRACE0_LIMIT(f, modCount, msg)                                                                                 \
    if (((traceLoopCounter++) % modCount == 0) && (f)&TRACE_MASK)                                                      \
    debug_printf(msg)
#define TRACE_LIMIT(f, modCount, msg, ...)                                                                             \
    if (((traceLoopCounter++) % modCount == 0) && (f)&TRACE_MASK)                                                      \
    debug_printf(msg, __VA_ARGS__)
#endif

#else
#define TRACE0(msg, ...)
#define TRACE(msg, ...)
#define TRACE0_LIMIT(...)
#define TRACE_LIMIT(...)
#endif

#if defined(TRACE_MASK) && TRACE_MASK & TRACE_HEADERS != 0
#define WP_TXMSG            "TXMSG: "
#define WP_RXMSG            "RXMSG: "
#define WP_RXMSG_Hdr_OK     "RXHOK: "
#define WP_RXMSG_Payload_Ok "RXPOK: "
#define WP_RXMSG_NAK        "RXNAK: "
void WP_TraceHeader(const char *pstrLabel, WP_Message *message);
#define TRACE_WP_HEADER(l, m) WP_TraceHeader(l, m)
#else
#define TRACE_WP_HEADER(l, m)
#endif

////////////////////////////////////////////////////
// function declarations (related with WP_Message)

#ifdef __cplusplus
extern "C"
{
#endif

    void WP_Message_PrepareReception();
    void WP_Message_Initialize(WP_Message *message);
    void WP_Message_PrepareRequest(
        WP_Message *message,
        uint32_t cmd,
        uint32_t flags,
        uint32_t payloadSize,
        uint8_t *payload);
    void WP_Message_PrepareReply(
        WP_Message *message,
        const WP_Packet *req,
        uint32_t flags,
        uint32_t payloadSize,
        uint8_t *payload);
    uint8_t WP_Message_VerifyHeader(WP_Message *message);
    uint8_t WP_Message_VerifyPayload(WP_Message *message);
    void WP_Message_Process();

    void WP_Message_PrepareReception_Platform();

#ifdef __cplusplus
}
#endif

//////////////////////////////////////////
// helper functions

#ifdef __cplusplus
extern "C"
{
#endif

    void WP_ReplyToCommand(WP_Message *message, uint8_t fSuccess, uint8_t fCritical, void *ptr, uint32_t size);
    void WP_SendProtocolMessage(WP_Message *message);
    void WP_PrepareAndSendProtocolMessage(uint32_t cmd, uint32_t payloadSize, uint8_t *payload, uint32_t flags);

#ifdef __cplusplus
}
#endif

#endif // WIREPROTOCOL_MESSAGE_H
