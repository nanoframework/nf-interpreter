//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <esp32_idf.h>

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_HAL_Interface.h>
#include "UsbSerial.h"

////////////////////////////////////////////////////////////////////
// Baudrate for the serial port                                   //
// Can be overriden by the build parameter TARGET_SERIAL_BAUDRATE //
////////////////////////////////////////////////////////////////////
#ifndef TARGET_SERIAL_BAUDRATE
#define TARGET_SERIAL_BAUDRATE 921600
#endif

static bool WP_Port_Intitialised = false;

#ifdef CONFIG_IDF_TARGET_ESP32

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32
// U0RXD GPIO1
// U0TXD GPIO3
// U1RXD GPIO32
// U1TXD GPIO33
// U2RXD GPIO16
// U2TXD GPIO17

#define ESP32_WP_RX_PIN GPIO_NUM_3
#define ESP32_WP_TX_PIN GPIO_NUM_1

#elif CONFIG_IDF_TARGET_ESP32S2

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-S2
// U0RXD GPIO44
// U0TXD GPIO43
// U1RXD GPIO17
// U1TXD GPIO18

// we are using the same GPIOs as IDF UART0, so no need to reconfigure
#define ESP32_WP_RX_PIN GPIO_NUM_44
#define ESP32_WP_TX_PIN GPIO_NUM_43

#elif CONFIG_IDF_TARGET_ESP32S3

#error "NOT IMPLEMENTED (YET)"

#elif CONFIG_IDF_TARGET_ESP32C3

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-C3
// U0RXD 20
// U0TXD 21

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#endif

#if CONFIG_USB_CDC_ENABLED

extern TaskHandle_t ReceiverTask;

void WP_Cdc_Rx_Callback(int itf, cdcacm_event_t *event)
{
    BaseType_t shouldWakeHigherPriorityTask = pdFALSE;

    // signal data received
    vTaskNotifyGiveFromISR(ReceiverTask, &shouldWakeHigherPriorityTask);

    portYIELD_FROM_ISR(shouldWakeHigherPriorityTask);
}

// WP using embedded USB CDC
bool WP_Initialise(COM_HANDLE port)
{
    (void)port;

    // get configuration with default values
    tinyusb_config_t tusb_cfg = {};

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t amc_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,
        .cdc_port = TINYUSB_CDC_ACM_0,
        .rx_unread_buf_sz = 1056,
        .callback_rx = &WP_Cdc_Rx_Callback,
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL};

    ESP_ERROR_CHECK(tusb_cdc_acm_init(&amc_cfg));

    WP_Port_Intitialised = true;

    return true;
}

#else

// WP using UART
bool WP_Initialise(COM_HANDLE port)
{
    (void)port;

    UsbSerialIntitialise();

    WP_Port_Intitialised = true;

    return true;
}

#endif // CONFIG_USB_CDC_ENABLED

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    // TODO: Initialise Port if not already done, Wire Protocol should be calling this directly at startup
    if (!WP_Port_Intitialised)
    {
        WP_Initialise(ESP32_WP_UART);
    }

    // save for later comparison
    uint32_t requestedSize = *size;

    // check for request with 0 size
    if (*size)
    {

#if CONFIG_USB_CDC_ENABLED

        // reset read count
        size_t read = 0;

        // wait to try reading from CDC buffer
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));

        tinyusb_cdcacm_read(TINYUSB_CDC_ACM_0, *ptr, requestedSize, &read);

#else
        const size_t read = UsbSerialRead(*ptr, requestedSize, pdMS_TO_TICKS(250));
#endif

        *ptr += read;
        *size -= read;
    }
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
    if (!WP_Port_Intitialised)
    {
        WP_Initialise(ESP32_WP_UART);
    }

#if CONFIG_USB_CDC_ENABLED

    // write header to output stream
    if (tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, (uint8_t *)&message->m_header, sizeof(message->m_header)) !=
        sizeof(message->m_header))
    {
        return false;
    }

    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
        if (tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, (uint8_t *)message->m_payload, message->m_header.m_size) !=
            message->m_header.m_size)
        {
            return false;
        }
    }

    tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);

#else

    if (UsbSerialWrite((const uint8_t *)&message->m_header, sizeof(message->m_header), pdMS_TO_TICKS(250)) !=
        sizeof(message->m_header))
        return false;

    if (message->m_header.m_size && message->m_payload)
    {
        if (UsbSerialWrite(message->m_payload, message->m_header.m_size, pdMS_TO_TICKS(250)) !=
            message->m_header.m_size)
            return false;
    }

#endif

    return true;
}
