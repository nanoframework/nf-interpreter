//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// clang-format off

#ifndef NX_CHIBIOS_ETH_DRIVER_H
#define NX_CHIBIOS_ETH_DRIVER_H


#ifdef   __cplusplus

/* Yes, C++ compiler is present.  Use standard C.  */
extern   "C" {
#endif

#include <hal.h>

/* Include ThreadX header file, if not already.  */

#ifndef TX_API_H
#include <tx_api.h>
#endif


/* Include NetX header file, if not already.  */

#ifndef NX_API_H
#include <nx_api.h>
#endif

// sanity check 
#ifdef ETH_PAD_SIZE
#error "NetX Duo ChibiOS driver doesn't have support for ETH_PAD_SIZE"
#endif


/* Determine if the driver's source file is being compiled. The constants and typdefs are only valid within
   the driver's source file compilation.  */

#ifdef NX_DRIVER_SOURCE


/****** DRIVER SPECIFIC ****** Start of part/vendor specific include area.  Include any such files here!  */
// #include <nx_eth_config.h>

/****** DRIVER SPECIFIC ****** End of part/vendor specific include file area!  */


/* Define generic constants and macros for all NetX Ethernet drivers.  */

#define NX_DRIVER_ETHERNET_IP                   0x0800
#define NX_DRIVER_ETHERNET_IPV6                 0x86dd
#define NX_DRIVER_ETHERNET_ARP                  0x0806
#define NX_DRIVER_ETHERNET_RARP                 0x8035

#define NX_DRIVER_ETHERNET_MTU                  1514
#define NX_DRIVER_ETHERNET_FRAME_SIZE           14

#define NX_DRIVER_DEFERRED_PACKET_RECEIVED      1
#define NX_DRIVER_DEFERRED_DEVICE_RESET         2
#define NX_DRIVER_DEFERRED_PACKET_TRANSMITTED   4

#define NX_DRIVER_STATE_NOT_INITIALIZED         1
#define NX_DRIVER_STATE_INITIALIZE_FAILED       2
#define NX_DRIVER_STATE_INITIALIZED             3
#define NX_DRIVER_STATE_LINK_ENABLED            4

#ifdef NX_DRIVER_INTERNAL_TRANSMIT_QUEUE
#define NX_DRIVER_MAX_TRANSMIT_QUEUE_DEPTH      10
#endif

#define NX_DRIVER_ERROR                         90


#define NX_DRIVER_ETHERNET_HEADER_REMOVE(p)   \
{   \
    p -> nx_packet_prepend_ptr =  p -> nx_packet_prepend_ptr + NX_DRIVER_ETHERNET_FRAME_SIZE;  \
    p -> nx_packet_length =  p -> nx_packet_length - NX_DRIVER_ETHERNET_FRAME_SIZE;    \
}


/****** DRIVER SPECIFIC ****** Start of part/vendor specific constants area.  Include any such constants here!  */

/* Define the number of descriptors and attached packets for transmit and receive operations. */

// #ifndef NX_DRIVER_TX_DESCRIPTORS
// #ifdef CHIBIOS_ETH_HAL_LEGACY
// #define NX_DRIVER_TX_DESCRIPTORS   ETH_TXBUFNB
// #else
// #define NX_DRIVER_TX_DESCRIPTORS   ETH_TX_DESC_CNT
// #endif
// #endif

// #ifndef NX_DRIVER_RX_DESCRIPTORS
// #ifdef CHIBIOS_ETH_HAL_LEGACY
// #define NX_DRIVER_RX_DESCRIPTORS   ETH_RXBUFNB
// #else
// #define NX_DRIVER_RX_DESCRIPTORS   ETH_RX_DESC_CNT
// #endif
// #endif

/****** DRIVER SPECIFIC ****** End of part/vendor specific constant area!  */

#define NX_DRIVER_CAPABILITY ( NX_INTERFACE_CAPABILITY_IPV4_TX_CHECKSUM   | \
                               NX_INTERFACE_CAPABILITY_IPV4_RX_CHECKSUM   | \
                               NX_INTERFACE_CAPABILITY_TCP_TX_CHECKSUM    | \
                               NX_INTERFACE_CAPABILITY_TCP_RX_CHECKSUM    | \
                               NX_INTERFACE_CAPABILITY_UDP_TX_CHECKSUM    | \
                               NX_INTERFACE_CAPABILITY_UDP_RX_CHECKSUM    | \
                               NX_INTERFACE_CAPABILITY_ICMPV4_TX_CHECKSUM   | \
                               NX_INTERFACE_CAPABILITY_ICMPV4_RX_CHECKSUM   | \
                               NX_INTERFACE_CAPABILITY_ICMPV6_TX_CHECKSUM | \
                               NX_INTERFACE_CAPABILITY_ICMPV6_RX_CHECKSUM )

/* Define the duplex mode for Legacy. */
#ifdef CHIBIOS_ETH_HAL_LEGACY
#define ETH_FULLDUPLEX_MODE   ETH_MODE_FULLDUPLEX
#define ETH_HALFDUPLEX_MODE   ETH_MODE_HALFDUPLEX
#endif

/* Define basic Ethernet driver information typedef. Note that this typedefs is designed to be used only
   in the driver's C file. */

typedef struct NX_DRIVER_INFORMATION_STRUCT
{
    /* NetX IP instance that this driver is attached to.  */
    NX_IP               *nx_driver_information_ip_ptr;

    /* Driver's current state.  */
    ULONG               nx_driver_information_state ;

    /* Packet pool used for receiving packets. */
    NX_PACKET_POOL      *nx_driver_information_packet_pool_ptr;

    /* Define the driver interface association.  */
    NX_INTERFACE        *nx_driver_information_interface;

    /* Define the deferred event field. This will contain bits representing events
       deferred from the ISR for processing in the thread context.  */
    ULONG               nx_driver_information_deferred_events;


    /****** DRIVER SPECIFIC ****** Start of part/vendor specific driver information area.  Include any such constants here!  */

    /* Indices to current receive/transmit descriptors.  */
    UINT                nx_driver_information_receive_current_index;
    UINT                nx_driver_information_transmit_current_index;

    /* Transmit release index.  */
    UINT                nx_driver_information_transmit_release_index;

    /* Define the number of transmit buffers in use.  */
    UINT                nx_driver_information_number_of_transmit_buffers_in_use;

    /* Define the association between buffer descriptors and NetX packets.  */
    NX_PACKET           *nx_driver_information_transmit_packets[STM32_MAC_TRANSMIT_BUFFERS];
    NX_PACKET           *nx_driver_information_receive_packets[STM32_MAC_RECEIVE_BUFFERS];

    /* Define the size of a rx buffer size.  */
    ULONG               nx_driver_information_rx_buffer_size;

    ULONG               nx_driver_information_multicast_count;

#ifdef NX_DRIVER_INTERNAL_TRANSMIT_QUEUE

    /* Define the parameters in the internal driver transmit queue.  The queue is maintained as a singularly
       linked-list with head and tail pointers.  The maximum number of packets on the queue is regulated by
       the constant NX_DRIVER_MAX_TRANSMIT_QUEUE_DEPTH, which is defined above. When this number is reached,
       the oldest packet is discarded after the new packet is queued.  */
    ULONG               nx_driver_transmit_packets_queued;
    NX_PACKET           nx_driver_transmit_queue_head;
    NX_PACKET           nx_driver_transmit_queue_tail;
#endif /* NX_DRIVER_INTERNAL_TRANSMIT_QUEUE */

    /****** DRIVER SPECIFIC ****** End of part/vendor specific driver information area.  */

}   NX_DRIVER_INFORMATION;

#endif


/****** DRIVER SPECIFIC ****** Start of part/vendor specific external function prototypes.  A typical NetX Ethernet driver
                               should expose its entry function as well as its interrupt handling function(s) here. All other
                               functions in the driver should have local scope, i.e., defined as static.  */

/* Define global driver entry function. */

VOID  nx_stm32_eth_driver(NX_IP_DRIVER *driver_req_ptr);

/****** DRIVER SPECIFIC ****** End of part/vendor specific external function prototypes.  */


#ifdef   __cplusplus
/* Yes, C++ compiler is present.  Use standard C.  */
    }
#endif
#endif /* NX_CHIBIOS_ETH_DRIVER_H */

// clang-format on
