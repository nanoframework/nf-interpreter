//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>

#include <WireProtocol.h>
#include <WireProtocol_Message.h>

#if (HAL_USE_SERIAL_USB == TRUE)
#include "usbcfg.h"
#elif (HAL_USE_SERIAL == TRUE)
#include <serialcfg.h>
#endif

WP_Message inboundMessage;

#if (HAL_USE_SERIAL_USB == TRUE)

bool WP_ReceiveBytes(uint8_t *ptr, uint16_t *size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;

    // check for request with 0 size
    if (*size)
    {
        // read from serial stream with 250ms timeout
        size_t read = chnReadTimeout(&SDU1, ptr, *size, TIME_MS2I(250));

        ptr += read;
        *size -= read;

        TRACE(TRACE_STATE, "RXMSG: Expecting %d bytes, received %d.\n", requestedSize, read);

        // check if any bytes where read
        return read > 0;
    }

    return true;
}
#elif (HAL_USE_SERIAL == TRUE)

bool WP_ReceiveBytes(uint8_t *ptr, uint16_t *size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;
    (void)requestedSize;

    // check for request with 0 size
    if (*size)
    {
        // non blocking read from serial port with 10ms timeout
        volatile size_t read = chnReadTimeout(&SERIAL_DRIVER, ptr, *size, TIME_MS2I(10));

        ptr += read;
        *size -= read;

        TRACE(TRACE_STATE, "RXMSG: Expecting %d bytes, received %d.\n", requestedSize, read);

        // check if any bytes where read
        return read > 0;
    }

    return true;
}

#else
#error                                                                                                                 \
    "Wire Protocol needs a transport. Please make sure that HAL_USE_SERIAL and/or HAL_USE_SERIAL_USB are set to TRUE in 'halconf.h'"
#endif

#if (HAL_USE_SERIAL_USB == TRUE)

bool WP_TransmitMessage(WP_Message *message)
{
    uint32_t writeResult;
    bool operationResult = false;

    TRACE(
        TRACE_HEADERS,
        "TXMSG: 0x%08X, 0x%08X, 0x%08X\n",
        message->m_header.m_cmd,
        message->m_header.m_flags,
        message->m_header.m_size);

    // write header to output stream
    writeResult =
        chnWriteTimeout(&SDU1, (const uint8_t *)&message->m_header, sizeof(message->m_header), TIME_MS2I(10));

    if (writeResult == sizeof(message->m_header))
    {
        operationResult = true;

        // if there is anything on the payload send it to the output stream
        if (message->m_header.m_size && message->m_payload)
        {
            // reset flag
            operationResult = false;

            writeResult = chnWriteTimeout(&SDU1, message->m_payload, message->m_header.m_size, TIME_MS2I(50));

            if (writeResult == message->m_header.m_size)
            {
                operationResult = true;

                TRACE0(TRACE_ERRORS, "TXMSG: OK\n");
            }
        }
    }

    return operationResult;
}
#elif (HAL_USE_SERIAL == TRUE)

bool WP_TransmitMessage(WP_Message *message)
{
    uint32_t writeResult;
    bool operationResult = false;

    TRACE(
        TRACE_HEADERS,
        "TXMSG: 0x%08X, 0x%08X, 0x%08X\n",
        message->m_header.m_cmd,
        message->m_header.m_flags,
        message->m_header.m_size);

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

#else
#error                                                                                                                 \
    "Wire Protocol needs a transport. Please make sure that HAL_USE_SERIAL and/or HAL_USE_SERIAL_USB are set to TRUE in 'halconf.h'"
#endif
