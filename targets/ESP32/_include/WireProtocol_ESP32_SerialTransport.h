//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef WIREPROTOCOL_ESP32_SERIALTRANSPORT_H
#define WIREPROTOCOL_ESP32_SERIALTRANSPORT_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        WP_ESP32_SERIAL_TRANSPORT_NONE,
        WP_ESP32_SERIAL_TRANSPORT_UART,
        WP_ESP32_SERIAL_TRANSPORT_USB_JTAG,
        WP_ESP32_SERIAL_TRANSPORT_TINY_USB
    } WP_ESP32_SerialTransportType;

    extern WP_ESP32_SerialTransportType g_WP_ESP32_SerialTransport;

#ifdef __cplusplus
}
#endif

#endif // WIREPROTOCOL_ESP32_SERIALTRANSPORT_H
