//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

/* Indicate that driver source is being compiled.  */

#define NX_DRIVER_SOURCE

/****** DRIVER SPECIFIC ****** Start of part/vendor specific include area.  Include driver-specific include file here!
 */

#ifndef NX_CHIBIOS_ETH_DRIVER_H

/* Determine if the driver uses IP deferred processing or direct ISR processing.  */

//#define NX_DRIVER_ENABLE_DEFERRED /* Define this to enable deferred ISR processing.  */

/* Determine if the packet transmit queue logic is required for this driver.   */
/* No, not required for this driver.  #define NX_DRIVER_INTERNAL_TRANSMIT_QUEUE   */

/* Include driver specific include file.  */
#include "nx_chibios_eth_driver.h"

/* Include the phy driver header */
#include "nx_chibios_phy_driver.h"

#endif /* NX_CHIBIOS_ETH_DRIVER_H */

/****** DRIVER SPECIFIC ****** End of part/vendor specific include file area!  */

/* Define the driver information structure that is only available within this file.  */
/* Place Ethernet BD at uncacheable memory*/
static NX_DRIVER_INFORMATION nx_driver_information;

// extern ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
// extern ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

// ETH_TxPacketConfig TxPacketCfg;
// ETH_MACFilterConfigTypeDef FilterConfig;
event_source_t *dataReceived;
// extern uint8_t *MacAddress;

/****** DRIVER SPECIFIC ****** Start of part/vendor specific data area.  Include hardware-specific data here!  */

/****** DRIVER SPECIFIC ****** End of part/vendor specific data area!  */

/* Define the routines for processing each driver entry request.  The contents of these routines will change with
each driver. However, the main driver entry function will not change, except for the entry function name.  */

static VOID _nx_driver_interface_attach(NX_IP_DRIVER *driver_req_ptr);
static VOID _nx_driver_initialize(NX_IP_DRIVER *driver_req_ptr);
static VOID _nx_driver_enable(NX_IP_DRIVER *driver_req_ptr);
static VOID _nx_driver_disable(NX_IP_DRIVER *driver_req_ptr);
static VOID _nx_driver_packet_send(NX_IP_DRIVER *driver_req_ptr);
static VOID _nx_driver_multicast_join(NX_IP_DRIVER *driver_req_ptr);
static VOID _nx_driver_multicast_leave(NX_IP_DRIVER *driver_req_ptr);
static VOID _nx_driver_get_status(NX_IP_DRIVER *driver_req_ptr);
#ifdef NX_ENABLE_INTERFACE_CAPABILITY
static VOID _nx_driver_capability_get(NX_IP_DRIVER *driver_req_ptr);
static VOID _nx_driver_capability_set(NX_IP_DRIVER *driver_req_ptr);
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

static VOID _nx_driver_deferred_processing(NX_IP_DRIVER *driver_req_ptr);

static VOID _nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr);
#ifdef NX_DRIVER_INTERNAL_TRANSMIT_QUEUE
static VOID _nx_driver_transmit_packet_enqueue(
    NX_PACKET *packet_ptr) static NX_PACKET *_nx_driver_transmit_packet_dequeue(VOID)
#endif // NX_DRIVER_INTERNAL_TRANSMIT_QUEUE

    // Define the prototypes for the hardware implementation of this driver. The contents of these routines are
    // driver-specific.
    static UINT _nx_driver_hardware_initialize(NX_IP_DRIVER *driver_req_ptr);
static UINT _nx_driver_hardware_enable(NX_IP_DRIVER *driver_req_ptr);
static UINT _nx_driver_hardware_disable(NX_IP_DRIVER *driver_req_ptr);
static UINT _nx_driver_hardware_packet_send(NX_PACKET *packet_ptr);
static UINT _nx_driver_hardware_multicast_join(NX_IP_DRIVER *driver_req_ptr);
static UINT _nx_driver_hardware_multicast_leave(NX_IP_DRIVER *driver_req_ptr);
static UINT _nx_driver_hardware_get_status(NX_IP_DRIVER *driver_req_ptr);
static VOID _nx_driver_hardware_packet_transmitted(VOID);
static VOID _nx_driver_hardware_packet_received(struct event_source *event);
// static NX_PACKET *_nx_driver_alloc_receivedpacket(uint16_t length);

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
static UINT _nx_driver_hardware_capability_set(NX_IP_DRIVER *driver_req_ptr);
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    nx_chibios_eth_driver                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This is the entry point of the NetX Ethernet Driver. This driver    */
/*    function is responsible for initializing the Ethernet controller,   */
/*    enabling or disabling the controller as need, preparing             */
/*    a packet for transmission, and getting status information.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        The driver request from the   */
/*                                            IP layer.                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_interface_attach           Process attach request        */
/*    _nx_driver_initialize                 Process initialize request    */
/*    _nx_driver_enable                     Process link enable request   */
/*    _nx_driver_disable                    Process link disable request  */
/*    _nx_driver_packet_send                Process send packet requests  */
/*    _nx_driver_multicast_join             Process multicast join request*/
/*    _nx_driver_multicast_leave            Process multicast leave req   */
/*    _nx_driver_get_status                 Process get status request    */
/*    _nx_driver_deferred_processing        Drive deferred processing     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    IP layer                                                            */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
/****** DRIVER SPECIFIC ****** Start of part/vendor specific global driver entry function name.  */
VOID nx_chibios_eth_driver(NX_IP_DRIVER *driver_req_ptr)
/****** DRIVER SPECIFIC ****** End of part/vendor specific global driver entry function name.  */
{
    /* Default to successful return.  */
    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;

    /* Process according to the driver request type in the IP control
    block.  */
    switch (driver_req_ptr->nx_ip_driver_command)
    {

        case NX_LINK_INTERFACE_ATTACH:

            /* Process link interface attach requests.  */
            _nx_driver_interface_attach(driver_req_ptr);
            break;

        case NX_LINK_INITIALIZE:
        {

            /* Process link initialize requests.  */
            _nx_driver_initialize(driver_req_ptr);
            break;
        }

        case NX_LINK_ENABLE:
        {

            /* Process link enable requests.  */
            _nx_driver_enable(driver_req_ptr);
            break;
        }

        case NX_LINK_DISABLE:
        {

            /* Process link disable requests.  */
            _nx_driver_disable(driver_req_ptr);
            break;
        }

        case NX_LINK_ARP_SEND:
        case NX_LINK_ARP_RESPONSE_SEND:
        case NX_LINK_PACKET_BROADCAST:
        case NX_LINK_RARP_SEND:
        case NX_LINK_PACKET_SEND:
        {

            /* Process packet send requests.  */
            _nx_driver_packet_send(driver_req_ptr);
            break;
        }

        case NX_LINK_MULTICAST_JOIN:
        {

            /* Process multicast join requests.  */
            _nx_driver_multicast_join(driver_req_ptr);
            break;
        }

        case NX_LINK_MULTICAST_LEAVE:
        {

            /* Process multicast leave requests.  */
            _nx_driver_multicast_leave(driver_req_ptr);
            break;
        }

        case NX_LINK_GET_STATUS:
        {

            /* Process get status requests.  */
            _nx_driver_get_status(driver_req_ptr);
            break;
        }

        case NX_LINK_DEFERRED_PROCESSING:
        {

            /* Process driver deferred requests.  */

            /* Process a device driver function on behave of the IP thread. */
            _nx_driver_deferred_processing(driver_req_ptr);

            break;
        }

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
        case NX_INTERFACE_CAPABILITY_GET:
        {

            /* Process get capability requests.  */
            _nx_driver_capability_get(driver_req_ptr);
            break;
        }

        case NX_INTERFACE_CAPABILITY_SET:
        {

            /* Process set capability requests.  */
            _nx_driver_capability_set(driver_req_ptr);
            break;
        }
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

        default:

            /* Invalid driver request.  */

            /* Return the unhandled command status.  */
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

            /* Default to successful return.  */
            driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_interface_attach                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the interface attach request.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_interface_attach(NX_IP_DRIVER *driver_req_ptr)
{

    /* Setup the driver's interface.  This example is for a simple one-interface
    Ethernet driver. Additional logic is necessary for multiple port devices.  */
    nx_driver_information.nx_driver_information_interface = driver_req_ptr->nx_ip_driver_interface;

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
    driver_req_ptr->nx_ip_driver_interface->nx_interface_capability_flag = NX_DRIVER_CAPABILITY;
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

    /* Return successful status.  */
    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_initialize                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the initialize request.  The processing    */
/*    in this function is generic. All ethernet controller logic is to    */
/*    be placed in _nx_driver_hardware_initialize.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_initialize        Process initialize request    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_initialize(NX_IP_DRIVER *driver_req_ptr)
{

    NX_IP *ip_ptr;
    NX_INTERFACE *interface_ptr;
    UINT status;

    /* Setup the IP pointer from the driver request.  */
    ip_ptr = driver_req_ptr->nx_ip_driver_ptr;

    /* Setup interface pointer.  */
    interface_ptr = driver_req_ptr->nx_ip_driver_interface;

    /* Initialize the driver's information structure.  */

    /* Default IP pointer to NULL.  */
    nx_driver_information.nx_driver_information_ip_ptr = NX_NULL;

    /* Setup the driver state to not initialized.  */
    nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_NOT_INITIALIZED;

    /* Setup the default packet pool for the driver's received packets.  */
    nx_driver_information.nx_driver_information_packet_pool_ptr = ip_ptr->nx_ip_default_packet_pool;

    /* Clear the deferred events for the driver.  */
    nx_driver_information.nx_driver_information_deferred_events = 0;

#ifdef NX_DRIVER_INTERNAL_TRANSMIT_QUEUE

    /* Clear the transmit queue count and head pointer.  */
    nx_driver_information.nx_driver_transmit_packets_queued = 0;
    nx_driver_information.nx_driver_transmit_queue_head = NX_NULL;
    nx_driver_information.nx_driver_transmit_queue_tail = NX_NULL;
#endif /* NX_DRIVER_INTERNAL_TRANSMIT_QUEUE */

    /* Call the hardware-specific ethernet controller initialization.  */
    status = _nx_driver_hardware_initialize(driver_req_ptr);

    /* Determine if the request was successful.  */
    if (status == NX_SUCCESS)
    {

        /* Successful hardware initialization.  */

        /* Setup driver information to point to IP pointer.  */
        nx_driver_information.nx_driver_information_ip_ptr = driver_req_ptr->nx_ip_driver_ptr;

        /* Setup the link maximum transfer unit. */
        interface_ptr->nx_interface_ip_mtu_size = NX_DRIVER_ETHERNET_MTU - NX_DRIVER_ETHERNET_FRAME_SIZE;

        // /* Setup the physical address of this IP instance.  Increment the
        // physical address lsw to simulate multiple nodes hanging on the
        // ethernet.  */
        // interface_ptr -> nx_interface_physical_address_msw =
        //   (ULONG)(( eth_handle.Init.MACAddr[0] << 8) | ( eth_handle.Init.MACAddr[1]));
        // interface_ptr -> nx_interface_physical_address_lsw =
        //   (ULONG)(( eth_handle.Init.MACAddr[2] << 24) | ( eth_handle.Init.MACAddr[3] << 16) |
        //           ( eth_handle.Init.MACAddr[4] << 8) | ( eth_handle.Init.MACAddr[5]));

        /* Indicate to the IP software that IP to physical mapping
        is required.  */
        interface_ptr->nx_interface_address_mapping_needed = NX_TRUE;

        /* Move the driver's state to initialized.  */
        nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_INITIALIZED;

        /* Indicate successful initialize.  */
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
    }
    else
    {

        /* Initialization failed.  Indicate that the request failed.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_enable                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the initialize request. The processing     */
/*    in this function is generic. All ethernet controller logic is to    */
/*    be placed in _nx_driver_hardware_enable.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_enable            Process enable request        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_enable(NX_IP_DRIVER *driver_req_ptr)
{

    NX_IP *ip_ptr;
    UINT status;

    /* Setup the IP pointer from the driver request.  */
    ip_ptr = driver_req_ptr->nx_ip_driver_ptr;

    /* See if we can honor the NX_LINK_ENABLE request.  */
    if (nx_driver_information.nx_driver_information_state < NX_DRIVER_STATE_INITIALIZED)
    {

        /* Mark the request as not successful.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
        return;
    }

    /* Check if it is enabled by someone already */
    if (nx_driver_information.nx_driver_information_state >= NX_DRIVER_STATE_LINK_ENABLED)
    {

        /* Yes, the request has already been made.  */
        driver_req_ptr->nx_ip_driver_status = NX_ALREADY_ENABLED;
        return;
    }

    /* Call hardware specific enable.  */
    status = _nx_driver_hardware_enable(driver_req_ptr);

    /* Was the hardware enable successful?  */
    if (status == NX_SUCCESS)
    {

        /* Update the driver state to link enabled.  */
        nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_LINK_ENABLED;

        /* Mark request as successful.  */
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;

        /* Mark the IP instance as link up.  */
        ip_ptr->nx_ip_driver_link_up = NX_TRUE;
    }
    else
    {

        /* Enable failed.  Indicate that the request failed.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_disable                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the disable request. The processing        */
/*    in this function is generic. All ethernet controller logic is to    */
/*    be placed in _nx_driver_hardware_disable.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_disable           Process disable request       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_disable(NX_IP_DRIVER *driver_req_ptr)
{

    NX_IP *ip_ptr;
    UINT status;

    /* Setup the IP pointer from the driver request.  */
    ip_ptr = driver_req_ptr->nx_ip_driver_ptr;

    /* Check if the link is enabled.  */
    if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED)
    {

        /* The link is not enabled, so just return an error.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
        return;
    }

    /* Call hardware specific disable.  */
    status = _nx_driver_hardware_disable(driver_req_ptr);

    /* Was the hardware disable successful?  */
    if (status == NX_SUCCESS)
    {

        /* Mark the IP instance as link down.  */
        ip_ptr->nx_ip_driver_link_up = NX_FALSE;

        /* Update the driver state back to initialized.  */
        nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_INITIALIZED;

        /* Mark request as successful.  */
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
    }
    else
    {

        /* Disable failed, return an error.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_packet_send                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the packet send request. The processing    */
/*    in this function is generic. All ethernet controller packet send    */
/*    logic is to be placed in _nx_driver_hardware_packet_send.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_packet_send       Process packet send request   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_packet_send(NX_IP_DRIVER *driver_req_ptr)
{
    ULONG *ethernet_frame_ptr;
    UINT status;

    // Setup the IP pointer from the driver request
    NX_IP *ip_ptr = driver_req_ptr->nx_ip_driver_ptr;

    /* Check to make sure the link is up.  */
    if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED)
    {

        // report an unsuccessful packet send
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;

        // Link is not up, simply free the packet.
        nx_packet_transmit_release(driver_req_ptr->nx_ip_driver_packet);
        return;
    }

    /* Process driver send packet.  */

    /* Place the ethernet frame at the front of the packet.  */
    NX_PACKET *packet_ptr = driver_req_ptr->nx_ip_driver_packet;

    /* Adjust the prepend pointer.  */
    packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr - NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Adjust the packet length.  */
    packet_ptr->nx_packet_length = packet_ptr->nx_packet_length + NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Setup the ethernet frame pointer to build the ethernet frame.  Backup another 2
     * bytes to get 32-bit word alignment.  */
    ethernet_frame_ptr = (ULONG *)(packet_ptr->nx_packet_prepend_ptr - 2);

    /* Set up the hardware addresses in the Ethernet header. */
    *ethernet_frame_ptr = driver_req_ptr->nx_ip_driver_physical_address_msw;
    *(ethernet_frame_ptr + 1) = driver_req_ptr->nx_ip_driver_physical_address_lsw;

    *(ethernet_frame_ptr + 2) =
        (ip_ptr->nx_ip_arp_physical_address_msw << 16) | (ip_ptr->nx_ip_arp_physical_address_lsw >> 16);
    *(ethernet_frame_ptr + 3) = (ip_ptr->nx_ip_arp_physical_address_lsw << 16);

    /* Set up the frame type field in the Ethernet harder. */
    if ((driver_req_ptr->nx_ip_driver_command == NX_LINK_ARP_SEND) ||
        (driver_req_ptr->nx_ip_driver_command == NX_LINK_ARP_RESPONSE_SEND))
    {

        *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_ARP;
    }
    else if (driver_req_ptr->nx_ip_driver_command == NX_LINK_RARP_SEND)
    {

        *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_RARP;
    }

#ifdef FEATURE_NX_IPV6
    else if (packet_ptr->nx_packet_ip_version == NX_IP_VERSION_V6)
    {

        *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IPV6;
    }
#endif /* FEATURE_NX_IPV6 */

    else
    {

        *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IP;
    }

    /* Endian swapping if NX_LITTLE_ENDIAN is defined.  */
    NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr));
    NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 1));
    NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 2));
    NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 3));

    /* Determine if the packet exceeds the driver's MTU.  */
    if (packet_ptr->nx_packet_length > NX_DRIVER_ETHERNET_MTU)
    {
        // This packet exceeds the size of the driver's MTU. Simply throw it away!

        // Remove the Ethernet header. 
        NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);

        // Indicate an unsuccessful packet send. 
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;

        // Simply free the packet
        nx_packet_transmit_release(packet_ptr);
        return;
    }

    // Transmit the packet through the Ethernet controller low level access routine.
    status = _nx_driver_hardware_packet_send(packet_ptr);

    // Determine if there was an error.
    if (status != NX_SUCCESS)
    {
        // Driver's hardware send packet routine failed to send the packet.

        // Remove the Ethernet header. 
        NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);

        // Indicate an unsuccessful packet send
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;

        // Simply free the packet
        nx_packet_transmit_release(packet_ptr);
    }
    else
    {
        // Set the status of the request
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_multicast_join                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the multicast join request. The processing */
/*    in this function is generic. All ethernet controller multicast join */
/*    logic is to be placed in _nx_driver_hardware_multicast_join.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_multicast_join    Process multicast join request*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_multicast_join(NX_IP_DRIVER *driver_req_ptr)
{

    UINT status;

    /* Call hardware specific multicast join function. */
    status = _nx_driver_hardware_multicast_join(driver_req_ptr);

    /* Determine if there was an error.  */
    if (status != NX_SUCCESS)
    {

        /* Indicate an unsuccessful request.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
    else
    {

        /* Indicate the request was successful.   */
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_multicast_leave                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the multicast leave request. The           */
/*    processing in this function is generic. All ethernet controller     */
/*    multicast leave logic is to be placed in                            */
/*    _nx_driver_hardware_multicast_leave.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_multicast_leave   Process multicast leave req   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_multicast_leave(NX_IP_DRIVER *driver_req_ptr)
{

    UINT status;

    /* Call hardware specific multicast leave function. */
    status = _nx_driver_hardware_multicast_leave(driver_req_ptr);

    /* Determine if there was an error.  */
    if (status != NX_SUCCESS)
    {

        /* Indicate an unsuccessful request.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
    else
    {

        /* Indicate the request was successful.   */
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_get_status                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the get status request. The processing     */
/*    in this function is generic. All ethernet controller get status     */
/*    logic is to be placed in _nx_driver_hardware_get_status.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_get_status        Process get status request    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_get_status(NX_IP_DRIVER *driver_req_ptr)
{

    UINT status;

    /* Call hardware specific get status function. */
    status = _nx_driver_hardware_get_status(driver_req_ptr);

    /* Determine if there was an error.  */
    if (status != NX_SUCCESS)
    {

        /* Indicate an unsuccessful request.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
    else
    {

        /* Indicate the request was successful.   */
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
    }
}

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_capability_get                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the get capability request.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_capability_get(NX_IP_DRIVER *driver_req_ptr)
{

    /* Return the capability of the Ethernet controller.  */
    *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_DRIVER_CAPABILITY;

    /* Return the success status.  */
    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_capability_set                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the set capability request.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_capability_set(NX_IP_DRIVER *driver_req_ptr)
{

    UINT status;

    /* Call hardware specific get status function. */
    status = _nx_driver_hardware_capability_set(driver_req_ptr);

    /* Determine if there was an error.  */
    if (status != NX_SUCCESS)
    {

        /* Indicate an unsuccessful request.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
    else
    {

        /* Indicate the request was successful.   */
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
    }
}
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_deferred_processing                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    XC, Microsoft Corporation                                           */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the deferred ISR action within the context */
/*    of the IP thread.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_packet_transmitted        Clean up after transmission    */
/*    _nx_driver_packet_received           Process a received packet      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_deferred_processing(NX_IP_DRIVER *driver_req_ptr)
{

    TX_INTERRUPT_SAVE_AREA

    ULONG deferred_events;

    /* Disable interrupts.  */
    TX_DISABLE

    /* Pickup deferred events.  */
    deferred_events = nx_driver_information.nx_driver_information_deferred_events;
    nx_driver_information.nx_driver_information_deferred_events = 0;

    /* Restore interrupts.  */
    TX_RESTORE
    /* Check for a transmit complete event.  */
    if (deferred_events & NX_DRIVER_DEFERRED_PACKET_TRANSMITTED)
    {

        /* Process transmitted packet(s).  */
        _nx_driver_hardware_packet_transmitted();
    }
    /* Check for received packet.  */
    if (deferred_events & NX_DRIVER_DEFERRED_PACKET_RECEIVED)
    {

        /* Process received packet(s).  */
        _nx_driver_hardware_packet_received(NULL);
    }

    /* Mark request as successful.  */
    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_transfer_to_netx                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing incoming packets.  This routine would      */
/*    be called from the driver-specific receive packet processing        */
/*    function _nx_driver_hardware.                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ip_ptr                                Pointer to IP protocol block  */
/*    packet_ptr                            Packet pointer                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Error indication                                                    */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_ip_packet_receive                 NetX IP packet receive        */
/*    _nx_ip_packet_deferred_receive        NetX IP packet receive        */
/*    _nx_arp_packet_deferred_receive       NetX ARP packet receive       */
/*    _nx_rarp_packet_deferred_receive      NetX RARP packet receive      */
/*    _nx_packet_release                    Release packet                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_hardware_packet_received   Driver packet receive function*/
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr)
{
    USHORT packet_type;

    /* Set the interface for the incoming packet.  */
    packet_ptr->nx_packet_ip_interface = nx_driver_information.nx_driver_information_interface;

    /* Pickup the packet header to determine where the packet needs to be
    sent.  */
    packet_type = (USHORT)(((UINT)(*(packet_ptr->nx_packet_prepend_ptr + 12))) << 8) |
                  ((UINT)(*(packet_ptr->nx_packet_prepend_ptr + 13)));

    /* Route the incoming packet according to its ethernet type.  */
    if (packet_type == NX_DRIVER_ETHERNET_IP || packet_type == NX_DRIVER_ETHERNET_IPV6)
    {
        /* Note:  The length reported by some Ethernet hardware includes
        bytes after the packet as well as the Ethernet header.  In some
        cases, the actual packet length after the Ethernet header should
        be derived from the length in the IP header (lower 16 bits of
        the first 32-bit word).  */

        /* Clean off the Ethernet header.  */
        packet_ptr->nx_packet_prepend_ptr += NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Adjust the packet length.  */
        packet_ptr->nx_packet_length -= NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Route to the ip receive function.  */
        _nx_ip_packet_deferred_receive(ip_ptr, packet_ptr);
    }
    else if (packet_type == NX_DRIVER_ETHERNET_ARP)
    {

        /* Clean off the Ethernet header.  */
        packet_ptr->nx_packet_prepend_ptr += NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Adjust the packet length.  */
        packet_ptr->nx_packet_length -= NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Route to the ARP receive function.  */
        _nx_arp_packet_deferred_receive(ip_ptr, packet_ptr);
    }
    else if (packet_type == NX_DRIVER_ETHERNET_RARP)
    {

        /* Clean off the Ethernet header.  */
        packet_ptr->nx_packet_prepend_ptr += NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Adjust the packet length.  */
        packet_ptr->nx_packet_length -= NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Route to the RARP receive function.  */
        _nx_rarp_packet_deferred_receive(ip_ptr, packet_ptr);
    }
    else
    {
        /* Invalid ethernet header... release the packet.  */
        nx_packet_release(packet_ptr);
    }
}

#ifdef NX_DRIVER_INTERNAL_TRANSMIT_QUEUE
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_transmit_packet_enqueue                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function queues a transmit packet when the hardware transmit   */
/*    queue does not have the resources (buffer descriptors, etc.) to     */
/*    send the packet.  The queue is maintained as a singularly linked-   */
/*    list with head and tail pointers. The maximum number of packets on  */
/*    the transmit queue is regulated by the constant                     */
/*    NX_DRIVER_MAX_TRANSMIT_QUEUE_DEPTH. When this number is exceeded,   */
/*    the oldest packet is discarded after the new packet is queued.      */
/*                                                                        */
/*    Note: that it is assumed further driver interrupts are locked out   */
/*    during the call to this driver utility.                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    packet_ptr                            Packet pointer                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_packet_transmit_release           Release packet                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_hardware_packet_send       Driver packet send function   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_transmit_packet_enqueue(NX_PACKET *packet_ptr)
{

    /* Determine if there is anything on the queue.  */
    if (nx_driver_information.nx_driver_transmit_queue_tail)
    {

        /* Yes, something is on the transmit queue. Simply add the new packet to the
        tail.  */
        nx_driver_information.nx_driver_transmit_queue_tail->nx_packet_queue_next = packet_ptr;

        /* Update the tail pointer.  */
        nx_driver_information.nx_driver_transmit_queue_tail = packet_ptr;
    }
    else
    {

        /* First packet on the transmit queue.  */

        /* Setup head pointers.  */
        nx_driver_information.nx_driver_transmit_queue_head = packet_ptr;
        nx_driver_information.nx_driver_transmit_queue_tail = packet_ptr;

        /* Set the packet's next pointer to NULL.  */
        packet_ptr->nx_packet_queue_next = NX_NULL;
    }

    /* Increment the total packets queued.  */
    nx_driver_information.nx_driver_transmit_packets_queued++;

    /* Determine if the total packet queued exceeds the driver's maximum transmit
    queue depth.  */
    if (nx_driver_information.nx_driver_transmit_packets_queued > NX_DRIVER_MAX_TRANSMIT_QUEUE_DEPTH)
    {

        /* Yes, remove the head packet (oldest) packet in the transmit queue and release it.  */
        packet_ptr = nx_driver_information.nx_driver_transmit_queue_head;

        /* Adjust the head pointer to the next packet.  */
        nx_driver_information.nx_driver_transmit_queue_head = packet_ptr->nx_packet_queue_next;

        /* Decrement the transmit packet queued count.  */
        nx_driver_information.nx_driver_transmit_packets_queued--;

        /* Remove the ethernet header.  */
        NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);

        /* Release the packet.  */
        nx_packet_transmit_release(packet_ptr);
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_transmit_packet_dequeue                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function removes the oldest transmit packet when the hardware  */
/*    transmit queue has new resources (usually after a transmit complete */
/*    interrupt) to send the packet. If there are no packets in the       */
/*    transmit queue, a NULL is returned.                                 */
/*                                                                        */
/*    Note: that it is assumed further driver interrupts are locked out   */
/*    during the call to this driver utility.                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    packet_ptr                            Packet pointer                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_hardware_packet_send       Driver packet send function   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static NX_PACKET *_nx_driver_transmit_packet_dequeue(VOID)
{

    NX_PACKET *packet_ptr;

    /* Pickup the head pointer of the tranmit packet queue.  */
    packet_ptr = nx_driver_information.nx_driver_transmit_queue_head;

    /* Determine if there is anything on the queue.  */
    if (packet_ptr)
    {

        /* Yes, something is on the transmit queue. Simply the packet from the head of the queue.  */

        /* Update the head pointer.  */
        nx_driver_information.nx_driver_transmit_queue_head = packet_ptr->nx_packet_queue_next;

        /* Clear the next pointer in the packet.  */
        packet_ptr->nx_packet_queue_next = NX_NULL;

        /* Decrement the transmit packet queued count.  */
        nx_driver_information.nx_driver_transmit_packets_queued--;
    }

    /* Return the packet pointer - NULL if there are no packets queued.  */
    return (packet_ptr);
}

#endif /* NX_DRIVER_INTERNAL_TRANSMIT_QUEUE */

/****** DRIVER SPECIFIC ****** Start of part/vendor specific internal driver functions.  */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_initialize                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific initialization.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    ETH_BSP_Config                        Configure Ethernet            */
/*    ETH_MACAddressConfig                  Setup MAC address             */
/*    ETH_DMARxDescReceiveITConfig          Enable receive descriptors    */
/*    nx_packet_allocate                    Allocate receive packet(s)    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_initialize                 Driver initialize processing  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_hardware_initialize(NX_IP_DRIVER *driver_req_ptr)
{
    // NX_PACKET *packet_ptr;
    // UINT i;
    //   ETH_MACConfigTypeDef MACConf;
    // INT PHYLinkState;
    // UINT duplex, speed = 0;
    // MACConfig mac_config = {MacAddress};

    /* Default to successful return.  */
    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;

    /* Setup indices.  */
    nx_driver_information.nx_driver_information_receive_current_index = 0;
    nx_driver_information.nx_driver_information_transmit_current_index = 0;
    nx_driver_information.nx_driver_information_transmit_release_index = 0;

    /* Clear the number of buffers in use counter.  */
    nx_driver_information.nx_driver_information_number_of_transmit_buffers_in_use = 0;

    /* Make sure there are receive packets... otherwise, return an error.  */
    if (nx_driver_information.nx_driver_information_packet_pool_ptr == NULL)
    {

        /* There must be receive packets. If not, return an error!  */
        return (NX_DRIVER_ERROR);
    }

    // TODO NETWORK
    // already handled by ChibiOS HAL??
    // nx_eth_init();
    // macStart(&ETHD1, &mac_config);
    dataReceived = macGetReceiveEventSource(&ETHD1);
    osalEventSetCallback(dataReceived, _nx_driver_hardware_packet_received, NULL);

    //   ETH_DMAConfigTypeDef dmaDefaultConf;
    //   dmaDefaultConf.AddressAlignedBeats = ENABLE;
    //   dmaDefaultConf.BurstMode = ETH_BURSTLENGTH_FIXED;
    //   dmaDefaultConf.DMAArbitration = ETH_DMAARBITRATION_RX1_TX1;
    //   dmaDefaultConf.FlushRxPacket = DISABLE;
    //   dmaDefaultConf.PBLx8Mode = DISABLE;
    //   dmaDefaultConf.RebuildINCRxBurst = DISABLE;
    //   dmaDefaultConf.RxDMABurstLength = ETH_RXDMABURSTLENGTH_32BEAT;
    //   dmaDefaultConf.SecondPacketOperate = ENABLE;
    //   dmaDefaultConf.TxDMABurstLength = ETH_TXDMABURSTLENGTH_32BEAT;
    //   dmaDefaultConf.TCPSegmentation = DISABLE;
    //   dmaDefaultConf.MaximumSegmentSize = 536;
    //   /* enable OSF bit to enhance throughput */
    //   HAL_ETH_SetDMAConfig(&eth_handle, &dmaDefaultConf);

    //   FilterConfig.PromiscuousMode = DISABLE;
    //   FilterConfig.HashUnicast = DISABLE;
    //   FilterConfig.HashMulticast = DISABLE;
    //   FilterConfig.DestAddrInverseFiltering = DISABLE;
    //   FilterConfig.PassAllMulticast = DISABLE;
    //   FilterConfig.BroadcastFilter = ENABLE;
    //   FilterConfig.SrcAddrInverseFiltering = DISABLE;
    //   FilterConfig.SrcAddrFiltering = DISABLE;
    //   FilterConfig.HachOrPerfectFilter = DISABLE;
    //   FilterConfig.ReceiveAllMode = DISABLE;
    //   FilterConfig.ControlPacketsFilter = 0x00;

    //   /* Set Tx packet config common parameters */
    //   memset(&TxPacketCfg, 0, sizeof(ETH_TxPacketConfig));
    //   TxPacketCfg.Attributes = ETH_TX_PACKETS_FEATURES_CSUM ;
    //   TxPacketCfg.CRCPadCtrl = ETH_CRC_PAD_DISABLE;

    // if (nx_eth_phy_init() != ETH_PHY_STATUS_OK)
    // {
    //     return (NX_DRIVER_ERROR);
    // }

    // PHYLinkState = nx_eth_phy_get_link_state();

    // /* Get link state */
    // if (!macPollLinkStatus(&ETHD1))
    // {

    //     while (!nx_eth_phy_get_link_state())
    //         ;
    //     ;
    // }
    // else
    // {
    // }

    //   for(i = 0; i < NX_DRIVER_TX_DESCRIPTORS; i++)
    //   {
    //     DMARxDscrTab[i].DESC0 = 0;
    //     DMARxDscrTab[i].DESC1 = 0;
    //     DMATxDscrTab[i].DESC2 = (uint32_t)1<<31;
    //     DMATxDscrTab[i].DESC3 = 0;
    //   }

    //  for (i = 0; i < STM32_MAC_RECEIVE_BUFFERS; i++)
    //  {

    //     /* Allocate a packet for the receive buffers.  */
    //     if (nx_packet_allocate(
    //             nx_driver_information.nx_driver_information_packet_pool_ptr,
    //             &packet_ptr,
    //             NX_RECEIVE_PACKET,
    //             NX_NO_WAIT) == NX_SUCCESS)
    //     {

    //         /* Adjust the packet.  */
    //         packet_ptr->nx_packet_prepend_ptr += 2;

    //         /* Remember the receive packet pointer.  */
    //         nx_driver_information.nx_driver_information_receive_packets[i] = packet_ptr;
    //     }
    //     else
    //     {
    //         /* Cannot allocate packets from the packet pool. */
    //         return (NX_DRIVER_ERROR);
    //     }
    // }

    // /* Save the size of one rx buffer.  */
    // nx_driver_information.nx_driver_information_rx_buffer_size =
    //     packet_ptr->nx_packet_data_end - packet_ptr->nx_packet_data_start;

    /* Clear the number of buffers in use counter.  */
    nx_driver_information.nx_driver_information_multicast_count = 0;

    /* Return success!  */
    return (NX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_enable                                          */
/*                                                            6.1         */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific link enable requests.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    ETH_Start                             Start Ethernet operation      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_enable                     Driver link enable processing */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_hardware_enable(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;
    // TODO NETWORK
    //   /* Call STM32 library to start Ethernet operation.  */
    //   HAL_ETH_Start_IT(&eth_handle);

    /* Return success!  */
    return (NX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_disable                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific link disable requests.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    ETH_MACTransmissionCmd                Disable transmit              */
/*    ETH_FlushTransmitFIFO                 Flush transmit FIFO           */
/*    ETH_MACReceptionCmd                   Disable receive               */
/*    ETH_DMATransmissionCmd                Stop DMA transmission         */
/*    ETH_DMAReceptionCmd                   Stop DMA reception            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_disable                    Driver link disable processing*/
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_hardware_disable(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;
    // TODO NETWORK
    // HAL_ETH_Stop(&eth_handle);

    /* Return success!  */
    return (NX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_packet_send                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific packet send requests.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    packet_ptr                            Pointer to packet to send     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [_nx_driver_transmit_packet_enqueue]  Optional internal transmit    */
/*                                            packet queue routine        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_packet_send                Driver packet send processing */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_hardware_packet_send(NX_PACKET *packet_ptr)
{
    NX_PACKET *pktIdx;
    // UINT buffLen = 0;
    MACTransmitDescriptor td;

    int i = 0;

    for (pktIdx = packet_ptr; pktIdx != NX_NULL; pktIdx = pktIdx->nx_packet_next)
    {
        // TODO NETWORK add define LWIP_SEND_TIMEOUT
        if (macWaitTransmitDescriptor(&ETHD1, &td, OSAL_MS2I(50)) != MSG_OK)
        {
            return NX_DRIVER_ERROR;
        }

        macWriteTransmitDescriptor(
            &td,
            (uint8_t *)pktIdx->nx_packet_prepend_ptr,
            (size_t)(pktIdx->nx_packet_append_ptr - pktIdx->nx_packet_prepend_ptr));

        i++;
    }

    macReleaseTransmitDescriptor(&td);

    return (NX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_multicast_join                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific multicast join requests.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_multicast_join             Driver multicast join         */
/*                                            processing                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_hardware_multicast_join(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;
    // TODO NETWORK

    //   /* Increase the multicast count.  */
    //   nx_driver_information.nx_driver_information_multicast_count++;

    //   /* Enable multicast frame reception.  */
    //   FilterConfig.PassAllMulticast = ENABLE;
    //   HAL_ETH_SetMACFilterConfig(&eth_handle, &FilterConfig);

    /* Return success.  */
    return (NX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_multicast_leave                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific multicast leave requests. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]*/
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_multicast_leave            Driver multicast leave        */
/*                                            processing                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_hardware_multicast_leave(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;
    // TODO NETWORK
    //   /* Decrease the multicast count.  */
    //   nx_driver_information.nx_driver_information_multicast_count--;

    //   /* If multicast count reaches zero, disable multicast frame reception.  */
    //   if (nx_driver_information.nx_driver_information_multicast_count == 0)
    //   {

    //     /* Disable multicast frame reception.  */
    //     FilterConfig.PassAllMulticast = DISABLE;
    //     HAL_ETH_SetMACFilterConfig(&eth_handle, &FilterConfig);
    //   }

    /* Return success.  */
    return (NX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_get_status                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific get status requests.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]*/
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_get_status                 Driver get status processing  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_hardware_get_status(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;
    /* Return success.  */
    return (NX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_packet_transmitted                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes packets transmitted by the ethernet         */
/*    controller.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    nx_packet_transmit_release            Release transmitted packet    */
/*    [_nx_driver_transmit_packet_dequeue]  Optional transmit packet      */
/*                                            dequeue                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_deferred_processing        Deferred driver processing    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_hardware_packet_transmitted()
{
    // NX_PACKET * release_packet;
    //   ETH_TxDescListTypeDef *dmatxdesclist = &heth->TxDescList;
    //   ULONG numOfBuf =  dmatxdesclist->BuffersInUse;
    // ULONG idx =       nx_driver_information.nx_driver_information_transmit_release_index;

    // /* Loop through buffers in use.  */
    // while (numOfBuf--)
    // {
    //   /* If no packet, just examine the next packet.  */
    //   if (dmatxdesclist->PacketAddress[idx] == NX_NULL)
    //   {

    //     /* No packet in use, skip to next.  */
    //     idx = (idx + 1) & (NX_DRIVER_TX_DESCRIPTORS - 1);
    //     continue;
    //   }

    //   /* Determine if the packet has been transmitted.  */
    //   if ((DMATxDscrTab[idx].DESC3 & ETH_DMATXNDESCRF_OWN) == 0)
    //   {

    //     /* Yes, packet has been transmitted.  */
    //     release_packet = (NX_PACKET *)dmatxdesclist->PacketAddress[idx];

    //     /* Remove the Ethernet header and release the packet.  */
    //     NX_DRIVER_ETHERNET_HEADER_REMOVE(release_packet);

    //     /* Release the packet.  */
    //     nx_packet_transmit_release(release_packet);

    //     /* Clear the entry in the in-use array.  */
    //     dmatxdesclist->PacketAddress[idx] = NX_NULL;

    //     /* Update the transmit relesae index and number of buffers in use.  */
    //     idx = (idx + 1) & (NX_DRIVER_TX_DESCRIPTORS - 1);
    //     dmatxdesclist->BuffersInUse = numOfBuf;
    //     nx_driver_information.nx_driver_information_transmit_release_index = idx;
    //   }
    //   else
    //   {
    //     /* Get out of the loop!  */
    //     break;
    //   }
    // }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_packet_received                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes packets received by the ethernet            */
/*    controller. This driver assumes NX_PACKET to be MTU size            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_transfer_to_netx           Transfer packet to NetX       */
/*    nx_packet_allocate                    Allocate receive packets      */
/*    _nx_packet_release                    Release receive packets       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_deferred_processing        Deferred driver processing    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_hardware_packet_received(struct event_source *event)
{
    (void)event;

    MACReceiveDescriptor rd;
    uint16_t framelength = 0;
    uint16_t lenghToRead;

    //NX_PACKET *pktIdx;
    NX_PACKET *packet_ptr = NULL;
    NX_PACKET *received_packet_ptr;

    //ULONG first_idx = nx_driver_information.nx_driver_information_receive_current_index;
    //received_packet_ptr = nx_driver_information.nx_driver_information_receive_packets[first_idx];

    // int i = 0;

    if (macWaitReceiveDescriptor(&ETHD1, &rd, TIME_IMMEDIATE) != MSG_OK)
    {
        return;
    }

    framelength = (uint16_t)rd.size;
    (void)framelength;
    uint16_t remainingLength = framelength;

    // try memory allocation to receive buffer
    // packet_ptr = _nx_driver_alloc_receivedpacket(framelength);

    if (nx_packet_allocate(
            nx_driver_information.nx_driver_information_packet_pool_ptr,
            &received_packet_ptr,
            NX_RECEIVE_PACKET,
            NX_NO_WAIT) == NX_SUCCESS)
    {

        /* Adjust the packet.  */
        //received_packet_ptr->nx_packet_prepend_ptr += 2;

        /* Remember the receive packet pointer.  */
        // nx_driver_information.nx_driver_information_receive_packets[i] = packet_ptr;
    }
    else
    {
        // Cannot allocate packets from the packet pool
        // drop packet
        macReleaseReceiveDescriptor(&rd);

        // done here
        return;
    }

    while (remainingLength > 0)
    {
        lenghToRead = remainingLength;

        // if (nx_packet_allocate(
        //         nx_driver_information.nx_driver_information_packet_pool_ptr,
        //         &received_packet_ptr,
        //         NX_RECEIVE_PACKET,
        //         NX_NO_WAIT) == NX_SUCCESS)
        // {

        //     /* Adjust the packet.  */
        //     received_packet_ptr->nx_packet_prepend_ptr += 2;

        //     /* Remember the receive packet pointer.  */
        //     // nx_driver_information.nx_driver_information_receive_packets[i] = packet_ptr;
        // }
        // else
        // {
        //     /* Cannot allocate packets from the packet pool. */
        //     // drop packet
        //     macReleaseReceiveDescriptor(&rd);
        //     return;
        // }

        // save pointer for the 1st packet
        if (packet_ptr == NULL)
        {
            packet_ptr = received_packet_ptr;
        }

        // memset(
        //     received_packet_ptr->nx_packet_prepend_ptr,
        //     0,
        //     (received_packet_ptr->nx_packet_append_ptr - received_packet_ptr->nx_packet_prepend_ptr));

        if(remainingLength > NX_DRIVER_ETHERNET_MTU)
        {
            lenghToRead = NX_DRIVER_ETHERNET_MTU;
        }

        remainingLength -= macReadReceiveDescriptor(
            &rd,
            (uint8_t *)received_packet_ptr->nx_packet_prepend_ptr,
            (size_t)lenghToRead);

        received_packet_ptr->nx_packet_append_ptr = received_packet_ptr->nx_packet_prepend_ptr + lenghToRead;
        received_packet_ptr->nx_packet_length = framelength;
        received_packet_ptr->nx_packet_next = NULL;

        if(remainingLength > 0)
        {
            __NOP();
        }
    }

    macReleaseReceiveDescriptor(&rd);

        /* Transfer the packet to NetX.  */
    _nx_driver_transfer_to_netx(nx_driver_information.nx_driver_information_ip_ptr, packet_ptr);


    // if (packet_ptr != NULL)
    // {
    //     for (pktIdx = packet_ptr; pktIdx != NX_NULL; pktIdx = pktIdx->nx_packet_next)
    //     {

    //         i++;
    //     }

    //     macReleaseReceiveDescriptor(&rd);

    //   INT            i;
    //   uint32_t framelength = 0;
    //   static ETH_BufferTypeDef RxBuff[NX_DRIVER_RX_DESCRIPTORS];
    //   memset(RxBuff, 0 , NX_DRIVER_RX_DESCRIPTORS*sizeof(ETH_BufferTypeDef));
    //   for(i = 0; i < NX_DRIVER_RX_DESCRIPTORS -1; i++)
    //   {
    //     RxBuff[i].next=&RxBuff[i+1];
    //   }

    //   while (HAL_ETH_GetRxDataBuffer(&eth_handle, RxBuff) == HAL_OK)
    //   {
    //     HAL_ETH_GetRxDataLength(&eth_handle, &framelength);
    //     ETH_RxDescListTypeDef *dmarxdesclist = &eth_handle.RxDescList;
    //     uint32_t FirstAppDesc = dmarxdesclist->FirstAppDesc;
    // uint32_t FirstAppDesc = 00000;

    /* This driver assumes the received packet size is 1536 bytes */
    // received_packet_ptr = nx_driver_information.nx_driver_information_receive_packets[FirstAppDesc];
    //     received_packet_ptr->nx_packet_append_ptr = received_packet_ptr->nx_packet_prepend_ptr + framelength;
    //     received_packet_ptr->nx_packet_length = framelength;
    //     received_packet_ptr->nx_packet_next = NULL;
    //     if (nx_packet_allocate(nx_driver_information.nx_driver_information_packet_pool_ptr, &packet_ptr,
    //                            NX_RECEIVE_PACKET, NX_NO_WAIT) == NX_SUCCESS)
    //     {
    //       /* Adjust the packet.  */
    //       packet_ptr -> nx_packet_prepend_ptr += 2;
    // #if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    //       SCB_InvalidateDCache_by_Addr((uint32_t*)packet_ptr -> nx_packet_data_start, packet_ptr ->
    //       nx_packet_data_end - packet_ptr -> nx_packet_data_start);
    // #endif
    //       HAL_ETH_DescAssignMemory(&eth_handle, FirstAppDesc, packet_ptr -> nx_packet_prepend_ptr, NULL);
    //       nx_driver_information.nx_driver_information_receive_packets[FirstAppDesc] = packet_ptr;

    //       /* Build Rx descriptor to be ready for next data reception */
    //       HAL_ETH_BuildRxDescriptors(&eth_handle);

    /* Transfer the packet to NetX.  */
    //_nx_driver_transfer_to_netx(nx_driver_information.nx_driver_information_ip_ptr, received_packet_ptr);
    //     }
    //     else
    //     {
    //       HAL_ETH_BuildRxDescriptors(&eth_handle);
    //     }
    //   }
    // }
    // else
    // {
    //     // drop packet
    //     macReleaseReceiveDescriptor(&rd);
    // }
}

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_capability_set                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific capability set requests.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                         Driver request pointer       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_capability_set             Capability set processing     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_hardware_capability_set(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;
    return NX_SUCCESS;
}
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

// static NX_PACKET *_nx_driver_alloc_packet(uint16_t length)
// {
//     NX_PACKET *packet = NULL;
//     // NX_PACKET *last = NULL;
//     uint16_t remainingLength = length;

//     do
//     {
//         //uint16_t qlen;

//         if (nx_packet_allocate(
//                 nx_driver_information.nx_driver_information_packet_pool_ptr,
//                 &packet,
//                 NX_RECEIVE_PACKET,
//                 NX_NO_WAIT) == NX_SUCCESS)
//         {
//             //nx_packet_free(packet);
//             return NULL;
//         }

//     } while (remainingLength > 0);

//     return NULL;
// }

// void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
// {

//   ULONG deffered_events;
//   deffered_events = nx_driver_information.nx_driver_information_deferred_events;

//   nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_RECEIVED;

//   if (!deffered_events)
//   {
//     /* Call NetX deferred driver processing.  */
//     _nx_ip_driver_deferred_processing(nx_driver_information.nx_driver_information_ip_ptr);
//   }
// }

// void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
// {
//   ULONG deffered_events;
//   deffered_events = nx_driver_information.nx_driver_information_deferred_events;

//   nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_TRANSMITTED;

//   if (!deffered_events)
//   {
//     /* Call NetX deferred driver processing.  */
//     _nx_ip_driver_deferred_processing(nx_driver_information.nx_driver_information_ip_ptr);
//   }
// }

/****** DRIVER SPECIFIC ****** Start of part/vendor specific internal driver functions.  */
