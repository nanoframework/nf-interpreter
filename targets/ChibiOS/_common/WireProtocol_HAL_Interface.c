//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_Transport.h>

#if defined(TRACE_MASK) && (TRACE_MASK & TRACE_VERBOSE) != 0
// used WP_Message_Process() and methods it calls to avoid flooding TRACE
extern uint32_t traceLoopCounter;
#endif

#if (HAL_USE_SERIAL_USB == TRUE)
#include <usbcfg.h>
#elif (HAL_USE_SERIAL == TRUE) || (HAL_USE_SIO == TRUE)
#include <serialcfg.h>
#endif

void WP_Serial_ReceiveBytes(uint8_t **ptr, uint32_t *size)
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

uint8_t WP_Serial_TransmitMessage(WP_Message *message)
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

#if CONFIG_NF_WP_CHANNEL_NETWORK && !defined(I_AM_NANOBOOTER)

///////////////////////////////////////////////////////////////////////////////////////////////////
// transmission lock
// messages are transmitted from the CLR thread and from the receiver thread, the lock makes sure that the header and
// payload of a message are never interleaved with another one

static MUTEX_DECL(s_txMutex);

bool WP_Transport_Lock(void)
{
    chMtxLock(&s_txMutex);

    return true;
}

void WP_Transport_Unlock(void)
{
    chMtxUnlock(&s_txMutex);
}

#endif // CONFIG_NF_WP_CHANNEL_NETWORK
