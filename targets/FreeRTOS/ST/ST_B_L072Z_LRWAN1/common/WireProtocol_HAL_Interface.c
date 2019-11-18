//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <WireProtocol.h>
#include <WireProtocol_Message.h>

#include <uart.h>
// #include "stm32l0xx_hal.h"

WP_Message inboundMessage;

extern Uart_t Uart2;
// extern UART_HandleTypeDef UartHandle;

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
        uint16_t read = 0;
        UartGetBuffer(&Uart2, ptr, requestedSize, &read);
        // if(HAL_UART_Receive(&UartHandle, ptr, requestedSize, 500) != HAL_OK)
        // {
        //     return false;
        // }
        // ptr  += read;
        // *size -= read;

        // // check if the requested read matches the actual read count
        // return (requestedSize == read);
    }

    return true;
}

int WP_TransmitMessage(WP_Message* message)
{
    uint32_t writeResult = 0;
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
    writeResult = UartPutBuffer(&Uart2, (uint8_t *)&message->m_header, sizeof(message->m_header));
    // HAL_UART_Transmit(&UartHandle, (uint8_t *)&message->m_header, sizeof(message->m_header), 500);

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

            writeResult = UartPutBuffer(&Uart2, (uint8_t *)message->m_payload, message->m_header.m_size);
            // if(HAL_UART_Transmit(&UartHandle, (uint8_t *)&message->m_payload, message->m_header.m_size, 500) == HAL_OK)
           if(writeResult == message->m_header.m_size)
            {
                operationResult = true;

                TRACE0( TRACE_ERRORS, "TXMSG: OK\n");                    
            }
        }
    }

    return operationResult;
}
