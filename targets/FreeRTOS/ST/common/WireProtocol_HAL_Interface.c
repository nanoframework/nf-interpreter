//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_HAL_interface.h>
#include <stm32f0xx_hal.h>

#include <FreeRTOS.h>
#include <task.h>

WP_Message inboundMessage;
bool WP_Initialise();

UART_HandleTypeDef WPUartHandle;
uint32_t receivedBytes;

int WP_ReceiveBytes(uint8_t* ptr, uint16_t* size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;
    // reset value
    receivedBytes = 0;
    
    //int readData = 0;
    // sanity check for request of 0 size
    if(*size)
    {
        // read from serial stream with 250ms timeout
        if(HAL_UART_Receive_IT(&WPUartHandle, ptr, *size) == HAL_OK)
        {
            // wait for notification
            xTaskNotifyWait(0,
                            UINT32_MAX,
                            &receivedBytes,
                            250 / portTICK_PERIOD_MS);

            // abort any ongoing UART operation
            HAL_UART_Abort(&WPUartHandle);
            
            ptr  += receivedBytes;
            *size -= receivedBytes;
        }
       
        TRACE( TRACE_STATE, "RXMSG: Expecting %d bytes, received %d.\n",requestedSize, receivedBytes);

        // check if the requested read matches the actual read count
        return (requestedSize == receivedBytes);
    }

    return true;
}

int WP_TransmitMessage(WP_Message* message)
{
    TRACE( TRACE_HEADERS, "TXMSG: 0x%08X, 0x%08X, 0x%08X\n", message->m_header.m_cmd, message->m_header.m_flags, message->m_header.m_size );

    // write header with 250ms timeout
    if(HAL_UART_Transmit(&WPUartHandle, (uint8_t*)&message->m_header, sizeof(message->m_header), 250)!= HAL_OK)
    {
        return false;
    }

    // if there is anything on the payload send it to the output stream
    if(message->m_header.m_size && message->m_payload)
    {
        if(HAL_UART_Transmit(&WPUartHandle, (uint8_t*)message->m_payload, message->m_header.m_size, 500) != HAL_OK)
        {
            return false;
        }
    }

    return true;    
}
