//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>

#include <serialcfg.h>

uint8_t WP_ReceiveBytes(uint8_t *ptr, uint32_t *size)
{
    volatile uint32_t read;

    // save for later comparison
    uint32_t requestedSize = *size;
    (void)requestedSize;

    // check for request with 0 size
    if (*size)
    {
        // non blocking read from serial port with 20ms timeout
        read = chnReadTimeout(&SERIAL_DRIVER, ptr, requestedSize, OSAL_MS2I(20));

        TRACE(TRACE_STATE, "RXMSG: Expecting %d bytes, received %d.\n", requestedSize, read);

        // check if any bytes where read
        if (read == 0)
        {
            return false;
        }

        ptr += read;
        *size -= read;
    }

    return true;
}

uint8_t WP_TransmitMessage(WP_Message *message)
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
        chnWriteTimeout(&SERIAL_DRIVER, (const uint8_t *)&message->m_header, sizeof(message->m_header), OSAL_MS2I(10));

    if (writeResult == sizeof(message->m_header))
    {
        operationResult = true;

        // if there is anything on the payload send it to the output stream
        if (message->m_header.m_size && message->m_payload)
        {
            // reset flag
            operationResult = false;

            writeResult = chnWriteTimeout(&SERIAL_DRIVER, message->m_payload, message->m_header.m_size, OSAL_MS2I(50));

            if (writeResult == message->m_header.m_size)
            {
                operationResult = true;

                TRACE0(TRACE_ERRORS, "TXMSG: OK\n");
            }
        }
    }

    return operationResult;
}
