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
#include <WireProtocol_Uart_Pins.h>

#if CONFIG_TINYUSB_CDC_ENABLED
#include <tinyusb.h>
#include <tinyusb_default_config.h>
#include <tinyusb_cdc_acm.h>
#endif

// The WP HAL interface implementation for ESP32 targets, supporting multiple transport layers (UART, USB CDC, USB/JTAG)
// based on the target and configuration. Based on the configuration, the WP will try to use USB CDC first, then
// USB/JTAG, and finally fallback to UART if the previous options are not available or fail to initialize. It can only
// be USB CDC or USB/JTAG not both. The ESP will reset if tinyusb has been initialized/deinitialized then try USB/JTAG
// as they use USB port in different modes

// Only ESP32S2 uses TinyUSB (USB CDC)
// Select between USB Jtag or UART based WP transport based on where connected and configuration
#if CONFIG_IDF_TARGET_ESP32S2 && CONFIG_TINYUSB_CDC_ENABLED &&                                                         \
    (CONFIG_NF_WP_TRANSPORT_USB_CDC || CONFIG_NF_WP_TRANSPORT_USB_CDC_OR_SERIAL)
#define WP_USE_TINYUSB
#endif

#if CONFIG_SOC_USB_SERIAL_JTAG_SUPPORTED && (CONFIG_NF_WP_TRANSPORT_USB_CDC || CONFIG_NF_WP_TRANSPORT_USB_CDC_OR_SERIAL)
#define WP_USE_USB_JTAG
#endif

#if (CONFIG_NF_WP_TRANSPORT_SERIAL || CONFIG_NF_WP_TRANSPORT_USB_CDC_OR_SERIAL)
#define WP_USE_UART
#endif

// Select between USB Jtag or UART based WP transport based on where connected and configuration
static bool WP_Port_Initialised = false;
enum
{
    WP_TRANSPORT_NONE,
    WP_TRANSPORT_UART,
    WP_TRANSPORT_USB_JTAG,
    WP_TRANSPORT_TINY_USB
} WP_Transport = WP_TRANSPORT_NONE;

// WP using TinyUSB CDC
#if defined(WP_USE_TINYUSB)

// TinyUSB stack doesn't work well with USB/JTAG, so disable it if Tinyusb is selected
// #undef WP_USE_USB_JTAG

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

    if (tinyusb_driver_install(&tusb_cfg) != ESP_OK)
    {
        return false;
    }

    tinyusb_config_cdcacm_t amc_cfg = {
        .cdc_port = TINYUSB_CDC_ACM_0,
        .callback_rx = &WP_Cdc_Rx_Callback,
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL};

    if (tinyusb_cdcacm_init(&amc_cfg) != ESP_OK)
    {
        tinyusb_driver_uninstall();
        return false;
    }

    // delay for a while to allow the host to recognize the device and enumerate it
    // Also if no delay then the tinyusb_driver_uninstall() crashes
    vTaskDelay(pdMS_TO_TICKS(100));

    // Check if the device is connected to host, if not, deinit and return false to fallback to other transport
    if (!tud_connected())
    {
        tinyusb_cdcacm_deinit(TINYUSB_CDC_ACM_0);
        tinyusb_driver_uninstall();
        return false;
    }

    // TinyUSB CDC ACM is connected and ready to use
    WP_Port_Initialised = true;

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

    // Delay for a while to allow the host to recognize the device and enumerate it
    vTaskDelay(pdMS_TO_TICKS(100));

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

#if defined(WP_USE_UART)

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
#endif // WP_USE_UART

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

#if defined(WP_USE_UART)
    if (WP_InitialiseUart(port))
    {
        WP_Transport = WP_TRANSPORT_UART;
        return true;
    }
#endif

    WP_Transport = WP_TRANSPORT_NONE;
    return false;
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
        // No transport selected on startup
        case WP_TRANSPORT_NONE:
            *size = 0;
            // delay to avoid busy loop if no transport selected
            vTaskDelay(pdMS_TO_TICKS(1000));
            break;

#if defined(WP_USE_UART)
        case WP_TRANSPORT_UART:
            WP_ReceiveBytesUart(ptr, size);
            break;
#endif

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
            // No transport selected on startup
        case WP_TRANSPORT_NONE:
            // delay to avoid busy loop if no transport selected
            vTaskDelay(pdMS_TO_TICKS(1000));
            return 0;

#if defined(WP_USE_UART)
        case WP_TRANSPORT_UART:
            return WP_TransmitMessageUart(message);
#endif

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
