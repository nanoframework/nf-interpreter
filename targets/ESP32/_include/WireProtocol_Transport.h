//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef WIREPROTOCOL_TRANSPORT_H
#define WIREPROTOCOL_TRANSPORT_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        WP_TRANSPORT_NONE,
        WP_TRANSPORT_UART,
        WP_TRANSPORT_USB_JTAG,
        WP_TRANSPORT_TINY_USB
    } WP_TransportType;

    extern WP_TransportType g_WP_Transport;

#ifdef __cplusplus
}
#endif

#endif // WIREPROTOCOL_TRANSPORT_H
