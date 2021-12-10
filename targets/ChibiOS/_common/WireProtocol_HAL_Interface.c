//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>

#if defined(TRACE_MASK) && (TRACE_MASK & TRACE_VERBOSE) != 0
// used WP_Message_Process() and methods it calls to avoid flooding TRACE
extern uint32_t traceLoopCounter;
#endif

#if (HAL_USE_SERIAL_USB == TRUE)
#include <usbcfg.h>
#elif (HAL_USE_SERIAL == TRUE)
#include <serialcfg.h>
#endif

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    // save for later comparison
    uint32_t requestedSize = *size;
    (void)requestedSize;

    // check for request with 0 size
    if (*size)
    {
        // non blocking read from serial port with 100ms timeout
        size_t read = chnReadTimeout(&SERIAL_DRIVER, *ptr, requestedSize, TIME_MS2I(100));

        // Warning: Including TRACE_VERBOSE will NOT output the following TRACE on every loop
        //          of the statemachine to avoid flooding the trace.
        TRACE_LIMIT(TRACE_VERBOSE, 100, "RXMSG: Expecting %d bytes, received %d.\n", requestedSize, read);

        *ptr += read;
        *size -= read;
    }
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
    uint32_t writeResult;
    bool operationResult = false;

    TRACE_WP_HEADER(WP_TXMSG, message);

    // write header to output stream
    writeResult =
        chnWriteTimeout(&SERIAL_DRIVER, (const uint8_t *)&message->m_header, sizeof(message->m_header), TIME_MS2I(10));

    if (writeResult == sizeof(message->m_header))
    {
        operationResult = true;

        // if there is anything on the payload send it to the output stream
        if (message->m_header.m_size && message->m_payload)
        {
            // reset flag
            operationResult = false;

            writeResult = chnWriteTimeout(&SERIAL_DRIVER, message->m_payload, message->m_header.m_size, TIME_MS2I(50));

            if (writeResult == message->m_header.m_size)
            {
                operationResult = true;

                TRACE0(TRACE_ERRORS, "TXMSG: OK\n");
            }
        }
    }

    return operationResult;
}
