//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>

#include <platform.h>
#include <tx_api.h>

#include <sl_iostream_init_instances.h>
#include <sl_iostream_handles.h>

#if defined(TRACE_MASK) && (TRACE_MASK & TRACE_VERBOSE) != 0
// used WP_Message_Process() and methods it calls to avoid flooding TRACE
extern uint32_t traceLoopCounter;
#endif

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    // save for later comparison
    uint32_t requestedSize = *size;
    size_t bytesRead;
    sl_status_t requestResult;

    // check for requests with 0 size
    if (*size)
    {
        // blocking receive as SL API does not support non-blocking
        requestResult = sl_iostream_read(sl_iostream_vcom_handle, *ptr, requestedSize, &bytesRead);

        // Warning: Including TRACE_VERBOSE will NOT output the following TRACE on every loop of the statemachine to
        // avoid flooding the trace.
        TRACE_LIMIT(TRACE_VERBOSE, 100, "RXMSG: Expecting %d bytes, received %d.\n", requestedSize, read);

        if (requestResult == SL_STATUS_EMPTY)
        {
            // hang here for a bit
            tx_thread_sleep(10);
        }
        else if (requestResult == SL_STATUS_OK)
        {
            // update pointer and size
            *ptr += bytesRead;
            *size -= bytesRead;
        }
    }

    return;
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
    TRACE_WP_HEADER(WP_TXMSG, message);

    // non-blocking transmit
    if (sl_iostream_write(sl_iostream_vcom_handle, (uint8_t *)&message->m_header, sizeof(message->m_header)) !=
        SL_STATUS_OK)
    {
        return false;
    }

    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
        // non-blocking transmit
        if (sl_iostream_write(sl_iostream_vcom_handle, message->m_payload, message->m_header.m_size) != SL_STATUS_OK)
        {
            return false;
        }
    }

    return true;
}
