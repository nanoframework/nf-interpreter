//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <target_platform.h>
#include <esp32_idf.h>

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_HAL_Interface.h>

#if CONFIG_TINYUSB_CDC_ENABLED
#include <tinyusb.h>
#include <tinyusb_default_config.h>
#include <tinyusb_cdc_acm.h>
#endif

///////////////////////////////////////////////////////////////////////////
// Baudrate for the serial port                                          //
// Can be overriden by the build parameter CONFIG_TARGET_SERIAL_BAUDRATE //
///////////////////////////////////////////////////////////////////////////
#ifndef CONFIG_TARGET_SERIAL_BAUDRATE
#define CONFIG_TARGET_SERIAL_BAUDRATE 921600
#endif

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

static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-S3
// U0RXD GPIO44
// U0TXD GPIO43
// U1RXD GPIO24
// U1TXD GPIO23
#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32C3

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-C3
// U0RXD 20
// U0TXD 21

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32C5

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-C5
// U0RXD 16
// U0TXD 17

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32C6

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-C6
// U0RXD 16
// U0TXD 17

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32C61

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-C61
// U0RXD 10
// U0TXD 11

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32H2

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-H2
// U0RXD 23
// U0TXD 24

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32P4

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-P4
// U0RXD 23
// U0TXD 24

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM
#endif

// Select between USB Jtag or UART based WP transport based on where connected and configuration
#if (CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3) && CONFIG_TINYUSB_CDC_ENABLED
#define WP_USE_TINYUSB
#endif

#if CONFIG_SOC_USB_SERIAL_JTAG_SUPPORTED
#define WP_USE_USB_JTAG
#endif

// Select between USB Jtag or UART based WP transport based on where connected and configuration
static bool WP_Port_Initialised = false;
static enum { WP_TRANSPORT_UART, WP_TRANSPORT_USB_JTAG, WP_TRANSPORT_TINY_USB } WP_Transport = WP_TRANSPORT_UART;

// Disable TinyUSB on ESP32S3 so we can use USB/JTag instead for testing without changing Konfig config.
// Also haven't found a way yet to detect if USB cable is connected.
#if (CONFIG_IDF_TARGET_ESP32S3)
#undef WP_USE_TINYUSB
#endif

// WP using TinyUSB CDC
#if defined(WP_USE_TINYUSB)
static void WP_Cdc_Rx_Callback(int itf, cdcacm_event_t *event)
{
    BaseType_t shouldWakeHigherPriorityTask = pdFALSE;

    // signal data received
    extern TaskHandle_t ReceiverTask;
    vTaskNotifyGiveFromISR(ReceiverTask, &shouldWakeHigherPriorityTask);

    portYIELD_FROM_ISR(shouldWakeHigherPriorityTask);
}

// WP using embedded USB CDC
static bool WP_InitialiseTinyUsb(COM_HANDLE port)
{
    (void)port;

    // get configuration with default values
    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t amc_cfg = {
        .cdc_port = TINYUSB_CDC_ACM_0,
        .callback_rx = &WP_Cdc_Rx_Callback,
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL};

    ESP_ERROR_CHECK(tinyusb_cdcacm_init(&amc_cfg));

    // TinyUSB CDC ACM is connected and ready to use
    WP_Port_Intitialised = true;

    return true;
}

static void WP_ReceiveBytesTinyUsb(uint8_t **ptr, uint32_t *size)
{
    // save for later comparison
    uint32_t requestedSize = *size;

    // check for request with 0 size
    if (*size)
    {
        // reset read count
        size_t read = 0;

        // wait to try reading from CDC buffer
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));

        tinyusb_cdcacm_read(TINYUSB_CDC_ACM_0, *ptr, requestedSize, &read);

        *ptr += read;
        *size -= read;
    }
}

static uint8_t WP_TransmitMessageTinyUsb(WP_Message *message)
{
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

    // need to call flush with a timeout to have it behave cooperatively with the RTOS
    // OK to silently ignore errors here
    tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, pdMS_TO_TICKS(250));

    return true;
}
#endif // WP_USE_TINYUSB

#if defined(WP_USE_USB_JTAG)

#include "driver/usb_serial_jtag.h"

#define USB_JTAG_BUFFER_SIZE 256

static size_t UsbSerialWrite(const uint8_t *data, size_t dataSize, TickType_t xTicksToWait);
static size_t UsbSerialRead(uint8_t *data, size_t dataSize, TickType_t xTicksToWait);

static bool WP_InitialiseUsbJtag(COM_HANDLE port)
{
    (void)port;

    // Configure USB SERIAL JTAG
    usb_serial_jtag_driver_config_t usb_serial_jtag_config = {
        .rx_buffer_size = USB_JTAG_BUFFER_SIZE,
        .tx_buffer_size = USB_JTAG_BUFFER_SIZE,
    };

    if (usb_serial_jtag_driver_install(&usb_serial_jtag_config) != ESP_OK)
    {
        return false;
    }

    if (usb_serial_jtag_is_connected())
    {
        WP_Port_Initialised = true;
    }
    else
    {
        usb_serial_jtag_driver_uninstall();
        return false;
    }

    return true;
}

static size_t UsbSerialWrite(const uint8_t *data, size_t dataSize, TickType_t xTicksToWait)
{
    ASSERT(data);
    ASSERT(dataSize >= 1);

    const TickType_t startTicks = xTaskGetTickCount();

    size_t writtenTotalSize = 0;

    do
    {
        size_t sizeToWrite = dataSize;

        // Make sure no bigger then buffer size, otherwise the write will fail. The rest of the data will be sent in the
        // next loop.
        if (sizeToWrite > USB_JTAG_BUFFER_SIZE)
        {
            sizeToWrite = USB_JTAG_BUFFER_SIZE;
        }

        const size_t writtenSize = usb_serial_jtag_write_bytes(data, sizeToWrite, xTicksToWait);
        ASSERT(writtenSize <= dataSize);

        data += writtenSize;
        dataSize -= writtenSize;
        writtenTotalSize += writtenSize;

        if (dataSize == 0)
        {
            break;
        }
    } while (xTaskGetTickCount() - startTicks < xTicksToWait);

    // Flush to ensure data is sent in a timely manner
    // usb_serial_jtag_wait_tx_done(pdMS_TO_TICKS(250));

    return writtenTotalSize;
}

static size_t UsbSerialRead(uint8_t *data, size_t dataSize, TickType_t xTicksToWait)
{
    ASSERT(data);
    ASSERT(dataSize >= 1);

    const TickType_t startTicks = xTaskGetTickCount();

    size_t readTotalSize = 0;
    do
    {
        const size_t readSize = usb_serial_jtag_read_bytes(data, dataSize, xTicksToWait);
        ASSERT(readSize <= dataSize);

        data += readSize;
        dataSize -= readSize;
        readTotalSize += readSize;

        if (dataSize == 0)
        {
            break;
        }
    } while (xTaskGetTickCount() - startTicks < xTicksToWait);

    return readTotalSize;
}

static void WP_ReceiveBytesUsbJtag(uint8_t **ptr, uint32_t *size)
{
    ASSERT(size);
    ASSERT(ptr);
    ASSERT(*ptr);

    if (*size == 0)
    {
        return;
    }

    const size_t read = UsbSerialRead(*ptr, *size, pdMS_TO_TICKS(250));
    ASSERT(read <= *size);

    *ptr += read;
    *size -= read;
}

static uint8_t WP_TransmitMessageUsbJtag(WP_Message *message)
{
    ASSERT(message);

    if (UsbSerialWrite((const uint8_t *)&message->m_header, sizeof(message->m_header), pdMS_TO_TICKS(250)) !=
        sizeof(message->m_header))
    {
        return false;
    }

    if (message->m_header.m_size && message->m_payload)
    {
        if (UsbSerialWrite(message->m_payload, message->m_header.m_size, pdMS_TO_TICKS(250)) !=
            message->m_header.m_size)
        {
            return false;
        }
    }

    return true;
}
#endif // WP_USE_USB_JTAG

#pragma region WP uart
// WP using UART (default)
static bool WP_InitialiseUart(COM_HANDLE port)
{
    (void)port;

    ASSERT(ESP32_WP_UART <= SOC_UART_NUM);

    // uninstall driver for console
    // ESP_ERROR_CHECK(uart_driver_delete(ESP32_WP_UART));

    uart_config_t uart_config = {// baudrate
                                 .baud_rate = CONFIG_TARGET_SERIAL_BAUDRATE,
                                 // baudrate
                                 .data_bits = UART_DATA_8_BITS,
                                 // parity mode
                                 .parity = UART_PARITY_DISABLE,
                                 // stop bit mode
                                 .stop_bits = UART_STOP_BITS_1,
                                 // hardware flow control(cts/rts)
                                 .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

    ESP_ERROR_CHECK(uart_param_config(ESP32_WP_UART, &uart_config));

    ESP_ERROR_CHECK(
        uart_set_pin(ESP32_WP_UART, ESP32_WP_TX_PIN, ESP32_WP_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // setup UART driver with UART queue
    ESP_ERROR_CHECK(uart_driver_install(ESP32_WP_UART, 256, 256, 0, NULL, ESP_INTR_FLAG_IRAM));

    WP_Port_Initialised = true;

    return true;
}

static void WP_ReceiveBytesUart(uint8_t **ptr, uint32_t *size)
{
    // save for later comparison
    uint32_t requestedSize = *size;

    // check for request with 0 size
    if (*size)
    {
        // non blocking read from serial port with 100ms timeout
        size_t read = uart_read_bytes(ESP32_WP_UART, *ptr, (uint32_t)requestedSize, pdMS_TO_TICKS(250));

        *ptr += read;
        *size -= read;
    }
}

static uint8_t WP_TransmitMessageUart(WP_Message *message)
{
    // TODO Check if timeout required
    // write header to output stream
    if (uart_write_bytes(ESP32_WP_UART, (const char *)&message->m_header, sizeof(message->m_header)) !=
        sizeof(message->m_header))
    {
        return false;
    }

    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
        if (uart_write_bytes(ESP32_WP_UART, (const char *)message->m_payload, message->m_header.m_size) !=
            (int)message->m_header.m_size)
        {
            return false;
        }
    }

    return true;
}
#pragma endregion WP uart

#pragma region WP main interface
static bool WP_Initialise(COM_HANDLE port)
{
#if defined(WP_USE_TINYUSB)
    if (WP_InitialiseTinyUsb(port))
    {
        WP_Transport = WP_TRANSPORT_TINY_USB;
        return true;
    }
#endif

#if defined(WP_USE_USB_JTAG)
    if (WP_InitialiseUsbJtag(port))
    {
        WP_Transport = WP_TRANSPORT_USB_JTAG;
        return true;
    }

#endif

    WP_Transport = WP_TRANSPORT_UART;
    return WP_InitialiseUart(port);
}

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    // TODO: Initialise Port if not already done, Wire Protocol should be calling this directly at startup
    if (!WP_Port_Initialised)
    {
        WP_Initialise(ESP32_WP_UART);
    }

    switch (WP_Transport)
    {
        default:
        case WP_TRANSPORT_UART:
            WP_ReceiveBytesUart(ptr, size);
            break;

#if defined(WP_USE_USB_JTAG)
        case WP_TRANSPORT_USB_JTAG:
            WP_ReceiveBytesUsbJtag(ptr, size);
            break;
#endif

#if defined(WP_USE_TINYUSB)
        case WP_TRANSPORT_TINY_USB:
            WP_ReceiveBytesTinyUsb(ptr, size);
            break;
#endif
    }
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
    if (!WP_Port_Initialised)
    {
        WP_Initialise(ESP32_WP_UART);
    }

    switch (WP_Transport)
    {
        default:
        case WP_TRANSPORT_UART:
            return WP_TransmitMessageUart(message);

#if defined(WP_USE_USB_JTAG)
        case WP_TRANSPORT_USB_JTAG:
            return WP_TransmitMessageUsbJtag(message);
#endif

#if defined(WP_USE_TINYUSB)
        case WP_TRANSPORT_TINY_USB:
            return WP_TransmitMessageTinyUsb(message);
#endif
    }
}
#pragma endregion WP main interface
