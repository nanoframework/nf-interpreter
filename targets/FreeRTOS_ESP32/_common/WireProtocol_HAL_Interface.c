//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <esp32_os.h>

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_HAL_Interface.h>

////////////////////////////////////////////////////////////////////
// Baudrate for the serial port                                   //
// Can be overriden by the build parameter TARGET_SERIAL_BAUDRATE //
////////////////////////////////////////////////////////////////////
#ifndef TARGET_SERIAL_BAUDRATE
#define TARGET_SERIAL_BAUDRATE 921600
#endif

bool WP_Initialise(COM_HANDLE port);

static bool WP_Port_Intitialised = false;
static uart_port_t WP_Port = UART_NUM_0;

#define ESP32_UART_RXD_PINS                                                                                            \
    {                                                                                                                  \
        3, 32, 16                                                                                                      \
    }
#define ESP32_UART_TXD_PINS                                                                                            \
    {                                                                                                                  \
        1, 33, 17                                                                                                      \
    }
#define ESP32_UART_CTS_PINS                                                                                            \
    {                                                                                                                  \
        19, 6, 8                                                                                                       \
    }
#define ESP32_UART_RTS_PINS                                                                                            \
    {                                                                                                                  \
        22, 11, 7                                                                                                      \
    }

// Pins
static const char g_ESP32_Uart_RxD_Pins[] = ESP32_UART_RXD_PINS;
static const char g_ESP32_Uart_TxD_Pins[] = ESP32_UART_TXD_PINS;

bool WP_Initialise(COM_HANDLE port)
{
    (void)port;

    if (WP_Port > UART_NUM_2)
        return false;

    uart_config_t uart_config = {
        // baudrate
        .baud_rate = TARGET_SERIAL_BAUDRATE,
        // baudrate
        .data_bits = UART_DATA_8_BITS,
        // parity mode
        .parity = UART_PARITY_DISABLE,
        // stop bit mode
        .stop_bits = UART_STOP_BITS_1,
        // hardware flow control(cts/rts)
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        // flow control threshold
        .rx_flow_ctrl_thresh = 120,
    };

    uart_param_config(WP_Port, &uart_config);

    uart_set_pin(
        WP_Port,
        g_ESP32_Uart_TxD_Pins[WP_Port],
        g_ESP32_Uart_RxD_Pins[WP_Port],
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE);

    // Setup UART driver(without UART queue)
    uart_driver_install(WP_Port, 1024 * 2, 512, 0, NULL, 0);

    WP_Port_Intitialised = true;

    return true;
}

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    // TODO: Initialise Port if not already done, Wire Protocol should be calling this directly at startup
    if (!WP_Port_Intitialised)
    {
        WP_Initialise(WP_Port);
    }

    // save for later comparison
    uint32_t requestedSize = *size;

    // check for request with 0 size
    if (*size)
    {
        // non blocking read from serial port with 100ms timeout
        size_t read = uart_read_bytes(WP_Port, *ptr, (uint32_t)requestedSize, (TickType_t)100 / portTICK_PERIOD_MS);

        *ptr += read;
        *size -= read;
    }
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
    if (!WP_Port_Intitialised)
    {
        WP_Initialise(WP_Port);
    }

    // TODO Check if timeout required
    // write header to output stream
    if (uart_write_bytes(WP_Port, (const char *)&message->m_header, sizeof(message->m_header)) !=
        sizeof(message->m_header))
    {
        return false;
    }

    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
        if (uart_write_bytes(WP_Port, (const char *)message->m_payload, message->m_header.m_size) !=
            (int)message->m_header.m_size)
        {
            return false;
        }
    }

    return true;
}
