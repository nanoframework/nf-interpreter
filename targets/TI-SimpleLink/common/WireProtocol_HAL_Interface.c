//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// #include <FreeRTOS.h>
#include <ti/drivers/UART.h>
#include <board.h>
#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_HAL_interface.h>

UART_Handle uart = NULL;

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

int WP_ReceiveBytes(uint8_t* ptr, uint16_t* size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;

    //int readData = 0;
    // sanity check for request of 0 size
    if(*size)
    {
        //////////////////////////////////////////////////////////
        //               PORTING CHANGE REQUIRED HERE           //
        //////////////////////////////////////////////////////////
        // change here to read (size) bytes from the input stream
        // preferably with read timeout and being able to check 
        // if the requested number of bytes was actually read
        //////////////////////////////////////////////////////////
        
        // non blocking read from serial port with 500ms timeout
        size_t read = UART_read(uart, ptr, requestedSize);

        ptr  += read;
        *size -= read;

        // check if the requested read matches the actual read count
        return (requestedSize == read);
    }

    return true;
}

int WP_TransmitMessage(WP_Message* message)
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

    TRACE( TRACE_HEADERS, "TXMSG: 0x%08X, 0x%08X, 0x%08X\n", message->m_header.m_cmd, message->m_header.m_flags, message->m_header.m_size );

    // write header to uart
    writeResult = UART_write(uart, (const void *)&message->m_header, sizeof(message->m_header));

    if(writeResult == sizeof(message->m_header))
    {
        operationResult = true;

        // if there is anything on the payload send it to the output stream
        if(message->m_header.m_size && message->m_payload)
        {
            ///////////////////////////////////////////////////////////
            //              PORTING CHANGE REQUIRED HERE             //
            ///////////////////////////////////////////////////////////
            // see description above
            //////////////////////////////////////////////////////////

            // reset flag
            operationResult = false;

            writeResult = UART_write(uart, (const void *)message->m_payload, message->m_header.m_size);

            if(writeResult == message->m_header.m_size)
            {
                operationResult = true;

                TRACE0( TRACE_ERRORS, "TXMSG: OK\n");                    
            }
        }
    }

    return operationResult;
}
