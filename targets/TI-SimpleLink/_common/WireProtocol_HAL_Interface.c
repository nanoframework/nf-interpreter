//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// #include <FreeRTOS.h>
#include <ti/drivers/UART2.h>
#include <ti/sysbios/knl/Clock.h>
#include <board.h>
#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_HAL_interface.h>

// UART operations timeout
#define UART_TIMEOUT_MILLISECONDS 500000

UART2_Handle uart = NULL;

WP_Message inboundMessage;

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// The functions below are the ones that need to be ported to new channels/HALs when required
// These are to be considered as a reference implementations when working on new ports
//
// This reference implementation provides communication through:
// - serial port (UART/USART)
//
////////////////////////////////////////////////////////////////////////////////////////////////

int WP_ReceiveBytes(uint8_t *ptr, uint16_t *size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;
    size_t read;

    // int readData = 0;
    // sanity check for request of 0 size
    if (*size)
    {
        //////////////////////////////////////////////////////////
        //               PORTING CHANGE REQUIRED HERE           //
        //////////////////////////////////////////////////////////
        // change here to read (size) bytes from the input stream
        // preferably with read timeout and being able to check
        // if the requested number of bytes was actually read
        //////////////////////////////////////////////////////////

        // non blocking read from serial port with 500ms timeout
        UART2_readTimeout(uart, ptr, requestedSize, &read, UART_TIMEOUT_MILLISECONDS / Clock_tickPeriod);

        ptr += read;
        *size -= read;

        // check if the requested read matches the actual read count
        return (requestedSize == read);
    }

    return true;
}

int WP_TransmitMessage(WP_Message *message)
{
    uint32_t writeResult;
    bool operationResult = false;

    ///////////////////////////////////////////////////////////
    //              PORTING CHANGE REQUIRED HERE             //
    ///////////////////////////////////////////////////////////
    // change here to write (size) bytes to the output stream
    // preferably with timeout and being able to check
    // if the write was successfull or at least buffered
    //////////////////////////////////////////////////////////

    TRACE(
        TRACE_HEADERS,
        "TXMSG: 0x%08X, 0x%08X, 0x%08X\n",
        message->m_header.m_cmd,
        message->m_header.m_flags,
        message->m_header.m_size);

    // write header to uart
    UART2_writeTimeout(
        uart,
        (const void *)&message->m_header,
        sizeof(message->m_header),
        &writeResult,
        UART_TIMEOUT_MILLISECONDS / Clock_tickPeriod);

    if (writeResult == sizeof(message->m_header))
    {
        operationResult = true;

        // if there is anything on the payload send it to the output stream
        if (message->m_header.m_size && message->m_payload)
        {
            ///////////////////////////////////////////////////////////
            //              PORTING CHANGE REQUIRED HERE             //
            ///////////////////////////////////////////////////////////
            // see description above
            //////////////////////////////////////////////////////////

            // reset flag
            operationResult = false;

            UART2_writeTimeout(
                uart,
                (const void *)message->m_payload,
                message->m_header.m_size,
                &writeResult,
                UART_TIMEOUT_MILLISECONDS / Clock_tickPeriod);

            if (writeResult == message->m_header.m_size)
            {
                operationResult = true;

                TRACE0(TRACE_ERRORS, "TXMSG: OK\n");
            }
        }
    }

    return operationResult;
}
