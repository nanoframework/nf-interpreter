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

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// The functions below are the ones that need to be ported to new channels/HALs when required
// These are to be considered as a reference implementations when working on new ports
// 
// This reference implementation provides communication through:
// - serial port (UART/USART) 
// - serial over USB (USB CDC class device)
//
////////////////////////////////////////////////////////////////////////////////////////////////

#if (HAL_USE_SERIAL_USB == TRUE)

int WP_ReceiveBytes(uint8_t* ptr, uint16_t* size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;

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

        // read from serial stream with 250ms timeout
        size_t read = chnReadTimeout(&SDU1, ptr, *size, TIME_MS2I(250)); 

        ptr  += read;
        *size -= read;

        TRACE( TRACE_STATE, "RXMSG: Expecting %d bytes, received %d.\n",requestedSize, read);
        
        // check if the requested read matches the actual read count
        return (requestedSize == read);
    }

    return true;
}
#elif (HAL_USE_SERIAL == TRUE)

int WP_ReceiveBytes(uint8_t* ptr, uint16_t* size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;

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
        
        // non blocking read from serial port with 250ms timeout
        volatile size_t read = chnReadTimeout(&SERIAL_DRIVER, ptr, *size, TIME_MS2I(250));

        ptr  += read;
        *size -= read;
        
        TRACE( TRACE_STATE, "RXMSG: Expecting %d bytes, received %d.\n",requestedSize, read);

        // check if the requested read matches the actual read count
        return (requestedSize == read);
    }

    return true;
}

#else
#error "Wire Protocol needs a transport. Please make sure that HAL_USE_SERIAL and/or HAL_USE_SERIAL_USB are set to TRUE in 'halconf.h'"
#endif

#if (HAL_USE_SERIAL_USB == TRUE)

int WP_TransmitMessage(WP_Message* message)
{
    uint32_t writeResult;
    bool operationResult = false;

    ///////////////////////////////////////////////////////////
    //              PORTING CHANGE REQUIRED HERE             //
    ///////////////////////////////////////////////////////////
    // change here to write (size) bytes to the output stream
    // preferably with timeout and being able to check 
    // if the write was successful or at least buffered
    //////////////////////////////////////////////////////////

    TRACE( TRACE_HEADERS, "TXMSG: 0x%08X, 0x%08X, 0x%08X\n", message->m_header.m_cmd, message->m_header.m_flags, message->m_header.m_size );

    // write header to output stream
    writeResult = chnWriteTimeout(&SDU1, (const uint8_t *)&message->m_header, sizeof(message->m_header), TIME_MS2I(250));

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

            writeResult = chnWriteTimeout(&SDU1, message->m_payload, message->m_header.m_size, TIME_MS2I(250));

            if(writeResult == message->m_header.m_size)
            {
                operationResult = true;

                TRACE0( TRACE_ERRORS, "TXMSG: OK\n");                    
            }
        }
    }

    return operationResult;
}
#elif (HAL_USE_SERIAL == TRUE)

int WP_TransmitMessage(WP_Message* message)
{
    uint32_t writeResult;
    bool operationResult = false;
    
    ///////////////////////////////////////////////////////////
    //              PORTING CHANGE REQUIRED HERE             //
    ///////////////////////////////////////////////////////////
    // change here to write (size) bytes to the output stream
    // preferably with timeout and being able to check 
    // if the write was sucessfull or at least buffered
    //////////////////////////////////////////////////////////
    
    TRACE( TRACE_HEADERS, "TXMSG: 0x%08X, 0x%08X, 0x%08X\n", message->m_header.m_cmd, message->m_header.m_flags, message->m_header.m_size );

    // write header to output stream
    writeResult = chnWriteTimeout(&SERIAL_DRIVER, (const uint8_t *)&message->m_header, sizeof(message->m_header), TIME_MS2I(250));

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

            writeResult = chnWriteTimeout(&SERIAL_DRIVER, message->m_payload, message->m_header.m_size, TIME_MS2I(250));

            if(writeResult == message->m_header.m_size)
            {
                operationResult = true;

                TRACE0( TRACE_ERRORS, "TXMSG: OK\n");                    
            }
        }
    }

    return operationResult;
}

#else
#error "Wire Protocol needs a transport. Please make sure that HAL_USE_SERIAL and/or HAL_USE_SERIAL_USB are set to TRUE in 'halconf.h'"
#endif
