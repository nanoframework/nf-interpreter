//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//
#include <esp32_os.h>

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_HAL_interface.h>

WP_Message inboundMessage;

bool WP_Initialise(COM_HANDLE port);

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// The functions bellow are the ones that need to be ported to new channels/HALs when required
// These are to be considered as a reference implementations when working on new ports
// 
// This reference implementation provides communication through:
// - serial port (UART/USART) 
//
////////////////////////////////////////////////////////////////////////////////////////////////
#if (HAL_USE_SERIAL == TRUE)

static bool WP_Port_Intitialised = false;
static uart_port_t WP_Port = UART_NUM_0;

#define ESP32_UART_RXD_PINS {3,32,16}    
#define ESP32_UART_TXD_PINS {1,33,17}  
#define ESP32_UART_CTS_PINS {19,6,8}
#define ESP32_UART_RTS_PINS {22,11,7}


// Pins
static const char g_ESP32_Uart_RxD_Pins[] = ESP32_UART_RXD_PINS;
static const char g_ESP32_Uart_TxD_Pins[] = ESP32_UART_TXD_PINS;


bool WP_Initialise(COM_HANDLE port)
{
    (void)port;

    if ( WP_Port > UART_NUM_2 ) return false;
 
    uart_config_t uart_config = {
        .baud_rate = 115200,                                 //baudrate
        .data_bits = UART_DATA_8_BITS,                       //data bit mode
        .parity    = UART_PARITY_DISABLE,                    //parity mode
        .stop_bits = UART_STOP_BITS_1,                       //stop bit mode
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,               //hardware flow control(cts/rts)
        .rx_flow_ctrl_thresh = 120,                          //flow control threshold
    };
    
    uart_param_config(WP_Port, &uart_config);

    uart_set_pin(WP_Port, g_ESP32_Uart_TxD_Pins[WP_Port], g_ESP32_Uart_RxD_Pins[WP_Port], UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    
    // Setup UART driver(without UART queue)
    uart_driver_install(WP_Port, 1024 * 2, 512, 0, NULL, 0);
    
    WP_Port_Intitialised = true;
 
    return true;
}


int WP_ReceiveBytes(uint8_t* ptr, uint16_t* size)
{
    // TODO: Initialise Port if not already done, Wire Protocol should be calling this directly at startup
    if (!WP_Port_Intitialised) WP_Initialise(WP_Port);

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
        
        // non blocking read from serial port with 100ms timeout
        volatile size_t read = uart_read_bytes( WP_Port, ptr, (uint32_t)requestedSize, (TickType_t) 100 / portTICK_PERIOD_MS);

        ptr  += read;
        *size -= read;

        // check if the requested read matches the actual read count
        return (requestedSize == read);
    }

    return true;
}

#else
#error "Wire Protocol needs a transport. Please make sure that HAL_USE_SERIAL and/or HAL_USE_SERIAL_USB are set to TRUE in 'halconf.h'"
#endif

#if (HAL_USE_SERIAL == TRUE)

int WP_TransmitMessage(WP_Message* message)
{
    ///////////////////////////////////////////////////////////
    //              PORTING CHANGE REQUIRED HERE             //
    ///////////////////////////////////////////////////////////
    // change here to write (size) bytes to the output stream
    // preferably with timeout and being able to check 
    // if the write was sucessfull or at least buffered
    //////////////////////////////////////////////////////////

    if (!WP_Port_Intitialised) WP_Initialise(WP_Port);

    //TODO Check if timeout required
    // write header to output stream 
    
    if ( uart_write_bytes( WP_Port, (const char*)&message->m_header, sizeof(message->m_header) ) != sizeof(message->m_header)) return false;

    // if there is anything on the payload send it to the output stream
    if(message->m_header.m_size && message->m_payload)
    {
        ///////////////////////////////////////////////////////////
        //              PORTING CHANGE REQUIRED HERE             //
        ///////////////////////////////////////////////////////////
        // see description above
        //////////////////////////////////////////////////////////
        if (uart_write_bytes(WP_Port, (const char*)message->m_payload, message->m_header.m_size ) != (int)message->m_header.m_size ) return false;
    }

    return true;    
}

#else
#error "Wire Protocol needs a transport. Please make sure that HAL_USE_SERIAL and/or HAL_USE_SERIAL_USB are set to TRUE in 'halconf.h'"
#endif

