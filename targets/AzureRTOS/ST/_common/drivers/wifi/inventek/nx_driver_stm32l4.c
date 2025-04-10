/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** NetX Component                                                        */
/**                                                                       */
/**   Ethernet driver for STM32L4 family of microprocessors               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* Indicate that driver source is being compiled.  */

#include "nx_driver_stm32l4.h"

#ifndef NX_ENABLE_TCPIP_OFFLOAD
#error "NX_ENABLE_TCPIP_OFFLOAD must be defined to use this driver"
#endif /* NX_ENABLE_TCPIP_OFFLOAD */

#ifndef NX_DRIVER_IP_MTU
#define NX_DRIVER_IP_MTU                        1200
#endif /* NX_DRIVER_IP_MTU */

#ifndef NX_DRIVER_RECEIVE_QUEUE_SIZE
#define NX_DRIVER_RECEIVE_QUEUE_SIZE            10
#endif /* NX_DRIVER_RECEIVE_QUEUE_SIZE */

#ifndef NX_DRIVER_STACK_SIZE
#define NX_DRIVER_STACK_SIZE                    1024
#endif /* NX_DRIVER_STACK_SIZE  */

/* Interval to receive packets when there is no packet. The default value is 100 ticks which is 1s.  */
#ifndef NX_DRIVER_THREAD_INTERVAL
#define NX_DRIVER_THREAD_INTERVAL               NX_IP_PERIODIC_RATE
#endif /* NX_DRIVER_THREAD_INTERVAL */

/* Define the maximum sockets at the same time. This is limited by hardware TCP/IP on STM32L4.  */
#define NX_DRIVER_SOCKETS_MAXIMUM               4

/* Define maximum server pending connections.  */
#ifndef NX_DRIVER_SERVER_LISTEN_COUNT
#define NX_DRIVER_SERVER_LISTEN_COUNT           1
#endif

/* Define the maximum wait timeout in ms for socket send. This is limited by hardware TCP/IP on STM32L4.  */
#define NX_DRIVER_SOCKET_SEND_TIMEOUT_MAXIMUM   3000

#define NX_DRIVER_CAPABILITY                    (NX_INTERFACE_CAPABILITY_TCPIP_OFFLOAD)


/* Define basic netword driver information typedef.  */

typedef struct NX_DRIVER_INFORMATION_STRUCT
{

    /* NetX IP instance that this driver is attached to.  */
    NX_IP               *nx_driver_information_ip_ptr;            

    /* Driver's current states.  */
    ULONG               nx_driver_information_states;  

    /* Packet pool used for receiving packets. */
    NX_PACKET_POOL      *nx_driver_information_packet_pool_ptr;        

    /* Define the driver interface association.  */
    NX_INTERFACE        *nx_driver_information_interface;
} NX_DRIVER_INFORMATION;


/* Define socket structure for hardware TCP/IP.  */

typedef struct NX_DRIVER_SOCKET_STRUCT
{
    VOID                *socket_ptr;
    UINT                 protocol;
    ULONG                local_ip;
    ULONG                remote_ip;
    USHORT               local_port;
    USHORT               remote_port;
    UCHAR                tcp_connected;
    UCHAR                is_client;
    USHORT               reseverd;
} NX_DRIVER_SOCKET;

static NX_DRIVER_INFORMATION nx_driver_information;
static NX_DRIVER_SOCKET nx_driver_sockets[NX_DRIVER_SOCKETS_MAXIMUM];
static TX_THREAD nx_driver_thread;
static UCHAR nx_driver_thread_stack[NX_DRIVER_STACK_SIZE];

/* Define the routines for processing each driver entry request.  The contents of these routines will change with
   each driver. However, the main driver entry function will not change, except for the entry function name.  */
   
static VOID         _nx_driver_interface_attach(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_initialize(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_enable(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_disable(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_multicast_join(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_multicast_leave(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_get_status(NX_IP_DRIVER *driver_req_ptr);
#ifdef NX_ENABLE_INTERFACE_CAPABILITY
static VOID         _nx_driver_capability_get(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_capability_set(NX_IP_DRIVER *driver_req_ptr);
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */
static VOID         _nx_driver_deferred_processing(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_thread_entry(ULONG thread_input);
static UINT         _nx_driver_tcpip_handler(struct NX_IP_STRUCT *ip_ptr,
                                             struct NX_INTERFACE_STRUCT *interface_ptr,
                                             VOID *socket_ptr, UINT operation, NX_PACKET *packet_ptr,
                                             NXD_ADDRESS *local_ip, NXD_ADDRESS *remote_ip,
                                             UINT local_port, UINT *remote_port, UINT wait_option);

/* Define the prototypes for the hardware implementation of this driver. The contents of these routines are
   driver-specific.  */

static UINT         _nx_driver_hardware_initialize(NX_IP_DRIVER *driver_req_ptr); 
static UINT         _nx_driver_hardware_enable(NX_IP_DRIVER *driver_req_ptr); 
static UINT         _nx_driver_hardware_disable(NX_IP_DRIVER *driver_req_ptr); 
static UINT         _nx_driver_hardware_get_status(NX_IP_DRIVER *driver_req_ptr);
#ifdef NX_ENABLE_INTERFACE_CAPABILITY
static UINT         _nx_driver_hardware_capability_set(NX_IP_DRIVER *driver_req_ptr);
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_driver_stm32l4                                  PORTABLE C       */ 
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This is the entry point of the NetX Driver. This driver             */ 
/*    function is responsible for initializing the network controller,    */ 
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
/*    _nx_driver_multicast_join             Process multicast join request*/ 
/*    _nx_driver_multicast_leave            Process multicast leave req   */ 
/*    _nx_driver_get_status                 Process get status request    */ 
/*    _nx_driver_deferred_processing        Drive deferred processing     */ 
/*    _nx_driver_capability_get             Get interface capability      */ 
/*    _nx_driver_capability_set             Set interface capability      */ 
/*                                                                        */
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    IP layer                                                            */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
VOID  nx_driver_stm32l4(NX_IP_DRIVER *driver_req_ptr)
{
    
    /* Default to successful return.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
    
    /* Process according to the driver request type in the IP control 
       block.  */
    switch (driver_req_ptr -> nx_ip_driver_command)
    {
        
    case NX_LINK_INTERFACE_ATTACH:
    {
        /* Process link interface attach requests.  */
        _nx_driver_interface_attach(driver_req_ptr);
        break;
    }
        
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

        /* Default to successful return.  */
        driver_req_ptr -> nx_ip_driver_status = NX_DRIVER_ERROR;
        nx_packet_transmit_release(driver_req_ptr -> nx_ip_driver_packet);
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
        driver_req_ptr -> nx_ip_driver_status =  NX_UNHANDLED_COMMAND;

        /* Default to successful return.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    }
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_interface_attach                        PORTABLE C       */ 
/*                                                           6.x          */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_interface_attach(NX_IP_DRIVER *driver_req_ptr)
{
#ifdef NX_ENABLE_INTERFACE_CAPABILITY
    driver_req_ptr -> nx_ip_driver_interface -> nx_interface_capability_flag = NX_DRIVER_CAPABILITY;
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

    /* Return successful status.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_initialize                              PORTABLE C       */ 
/*                                                           6.x          */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_initialize(NX_IP_DRIVER *driver_req_ptr)
{

NX_IP        *ip_ptr;
NX_INTERFACE *interface_ptr;
UINT          status;

    /* Setup the driver state to not initialized.  */
    nx_driver_information.nx_driver_information_states = NX_DRIVER_STATE_NOT_INITIALIZED;
            
    /* Setup the IP pointer from the driver request.  */
    ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;
    
    /* Setup interface pointer.  */
    interface_ptr = driver_req_ptr -> nx_ip_driver_interface;

    /* Initialize the driver's information structure.  */

    /* Default IP pointer to NULL.  */
    nx_driver_information.nx_driver_information_ip_ptr = NX_NULL;

    /* Setup the default packet pool for the driver's received packets.  */
    nx_driver_information.nx_driver_information_packet_pool_ptr = ip_ptr -> nx_ip_default_packet_pool;

    /* Call the hardware-specific ethernet controller initialization.  */
    status =  _nx_driver_hardware_initialize(driver_req_ptr);

    /* Determine if the request was successful.  */
    if (status == NX_SUCCESS)
    {

        /* Successful hardware initialization.  */

        /* Setup driver information to point to IP pointer.  */
        nx_driver_information.nx_driver_information_ip_ptr = ip_ptr;
        nx_driver_information.nx_driver_information_interface = interface_ptr;
            
        /* Setup the link maximum transfer unit. */
        interface_ptr -> nx_interface_ip_mtu_size = NX_DRIVER_IP_MTU;
            
        /* Setup the physical address of this IP instance.  */
        /* TODO: Use WIFI_GetMAC_Address() to set the real value.  */
        interface_ptr -> nx_interface_physical_address_msw = 0;
        interface_ptr -> nx_interface_physical_address_lsw = 0;
                
        /* Indicate to the IP software that IP to physical mapping
           is required.  */
        interface_ptr -> nx_interface_address_mapping_needed =  NX_FALSE;
            
        /* Move the driver's state to initialized.  */
        nx_driver_information.nx_driver_information_states = NX_DRIVER_STATE_INITIALIZED;

        /* Indicate successful initialize.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
    }
    else
    {
        
        /* Initialization failed.  Indicate that the request failed.  */
        driver_req_ptr -> nx_ip_driver_status =   NX_DRIVER_ERROR;
    }
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_enable                                  PORTABLE C       */ 
/*                                                           6.x          */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_enable(NX_IP_DRIVER *driver_req_ptr)
{

UINT            status;

    /* See if we can honor the NX_LINK_ENABLE request.  */
    if (nx_driver_information.nx_driver_information_states < NX_DRIVER_STATE_INITIALIZED)
    {

        /* Mark the request as not successful.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;                      
        return;
    }                
        
    /* Check if it is enabled by someone already */
    if (nx_driver_information.nx_driver_information_states >=  NX_DRIVER_STATE_LINK_ENABLED)
    {

        /* Yes, the request has already been made.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_ALREADY_ENABLED;
        return;
    }

    /* Call hardware specific enable.  */
    status =  _nx_driver_hardware_enable(driver_req_ptr);

    /* Was the hardware enable successful?  */
    if (status == NX_SUCCESS)
    {

        /* Update the driver state to link enabled.  */
        nx_driver_information.nx_driver_information_states = NX_DRIVER_STATE_LINK_ENABLED;

        /* Mark request as successful.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;

        /* Mark the IP interface as link up.  */
        driver_req_ptr -> nx_ip_driver_interface -> nx_interface_link_up =  NX_TRUE;

        /* Set TCP/IP callback function.  */
        driver_req_ptr -> nx_ip_driver_interface -> nx_interface_tcpip_offload_handler = _nx_driver_tcpip_handler;
    }
    else
    {
        
        /* Enable failed.  Indicate that the request failed.  */
        driver_req_ptr -> nx_ip_driver_status =   NX_DRIVER_ERROR;
    }
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_disable                                 PORTABLE C       */ 
/*                                                           6.x          */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_disable(NX_IP_DRIVER *driver_req_ptr)
{

UINT            status;

    /* Check if the link is enabled.  */
    if (nx_driver_information.nx_driver_information_states !=  NX_DRIVER_STATE_LINK_ENABLED)
    {

        /* The link is not enabled, so just return an error.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
        return;
    }

    /* Call hardware specific disable.  */
    status =  _nx_driver_hardware_disable(driver_req_ptr);

    /* Was the hardware disable successful?  */
    if (status == NX_SUCCESS)
    {
    
        /* Mark the IP interface as link down.  */        
        driver_req_ptr -> nx_ip_driver_interface -> nx_interface_link_up =  NX_FALSE;

        /* Update the driver state back to initialized.  */
        nx_driver_information.nx_driver_information_states =  NX_DRIVER_STATE_INITIALIZED;
        
        /* Mark request as successful.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;

        /* Clear the TCP/IP callback function.  */
        driver_req_ptr -> nx_ip_driver_interface -> nx_interface_tcpip_offload_handler = NX_NULL;
    }
    else
    {

        /* Disable failed, return an error.  */    
        driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    }
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_multicast_join                          PORTABLE C       */ 
/*                                                           6.x          */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_multicast_join(NX_IP_DRIVER *driver_req_ptr)
{

    /* Not supported.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_multicast_leave                         PORTABLE C       */ 
/*                                                           6.x          */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_multicast_leave(NX_IP_DRIVER *driver_req_ptr)
{

    /* Not supported.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_get_status                              PORTABLE C       */ 
/*                                                           6.x          */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_get_status(NX_IP_DRIVER *driver_req_ptr)
{

UINT        status;


    /* Call hardware specific get status function. */
    status =  _nx_driver_hardware_get_status(driver_req_ptr);
    
    /* Determine if there was an error.  */
    if (status != NX_SUCCESS)
    {

        /* Indicate an unsuccessful request.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    }
    else
    {

        /* Indicate the request was successful.   */    
        driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
    }
}


#ifdef NX_ENABLE_INTERFACE_CAPABILITY
/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_capability_get                          PORTABLE C       */ 
/*                                                           6.x          */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_capability_get(NX_IP_DRIVER *driver_req_ptr)
{
    
    /* Return the capability of the Ethernet controller.  */
    *(driver_req_ptr -> nx_ip_driver_return_ptr) = NX_DRIVER_CAPABILITY;
    
    /* Return the success status.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_capability_set                          PORTABLE C       */ 
/*                                                           6.x          */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_capability_set(NX_IP_DRIVER *driver_req_ptr)
{

UINT        status;


    /* Call hardware specific get status function. */
    status =  _nx_driver_hardware_capability_set(driver_req_ptr);
    
    /* Determine if there was an error.  */
    if (status != NX_SUCCESS)
    {

        /* Indicate an unsuccessful request.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    }
    else
    {

        /* Indicate the request was successful.   */    
        driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
    }
}
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_deferred_processing                     PORTABLE C       */ 
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_deferred_processing(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_thread_entry                            PORTABLE C       */ 
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function is the driver thread entry. In this thread, it        */ 
/*    performs checking for incoming TCP and UDP packets. On new packet,  */ 
/*    it will be passed to NetX.                                          */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_input                          Thread input                  */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    tx_mutex_get                          Obtain protection mutex       */
/*    tx_mutex_put                          Release protection mutex      */
/*    tx_thread_sleep                       Sleep driver thread           */
/*    nx_packet_allocate                    Allocate a packet for incoming*/
/*                                            TCP and UDP data            */
/*    _nx_tcp_socket_driver_packet_receive  Receive TCP packet            */
/*    _nx_tcp_socket_driver_establish       Establish TCP connection      */
/*    _nx_udp_socket_driver_packet_receive  Receive UDP packet            */
/*                                                                        */
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Driver entry function                                               */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_thread_entry(ULONG thread_input)
{
UINT i;
NX_PACKET *packet_ptr;
UINT packet_type;
UINT status;
NXD_ADDRESS local_ip;
NXD_ADDRESS remote_ip;
uint16_t data_length;
NX_IP *ip_ptr = nx_driver_information.nx_driver_information_ip_ptr;
NX_INTERFACE *interface_ptr = nx_driver_information.nx_driver_information_interface;
NX_PACKET_POOL *pool_ptr = nx_driver_information.nx_driver_information_packet_pool_ptr;

    NX_PARAMETER_NOT_USED(thread_input);

    for (;;)
    {
        
        /* Obtain the IP internal mutex before processing the IP event.  */
        tx_mutex_get(&(ip_ptr -> nx_ip_protection), TX_WAIT_FOREVER);

        /* Loop through TCP socket.  */
        for (i = 0; i < NX_DRIVER_SOCKETS_MAXIMUM; i++)
        {
            if (nx_driver_sockets[i].socket_ptr == NX_NULL)
            {

                /* Skip sockets not used.  */
                continue;
            }

            if ((nx_driver_sockets[i].local_port == 0) &&
                (nx_driver_sockets[i].remote_port == 0))
            {

                /* Skip sockets not listening.  */
                continue;
            }

            /* Set packet type.  */
            if (nx_driver_sockets[i].protocol == NX_PROTOCOL_TCP)
            {
                packet_type = NX_TCP_PACKET;
                if ((nx_driver_sockets[i].tcp_connected == NX_FALSE) &&
                    (nx_driver_sockets[i].is_client == NX_FALSE))
                {

                    /* TCP server. Try accept. */
                    if (_nx_tcp_socket_driver_establish(nx_driver_sockets[i].socket_ptr, interface_ptr, 0))
                    {

                        /* NetX TCP socket is not ready to accept. Just sleep to avoid starving.  */
                        tx_thread_sleep(NX_DRIVER_THREAD_INTERVAL);
                        continue;
                    }
                }
            }
            else
            {
                packet_type = NX_UDP_PACKET;
            } 

            /* Loop to receive all data on current socket.  */
            for (;;)
            {
                if (nx_packet_allocate(pool_ptr, &packet_ptr, packet_type, NX_NO_WAIT))
                {

                    /* Packet not available.  */
                    break;
                }

                /* Get available size of packet.  */
                data_length = (uint16_t)(packet_ptr -> nx_packet_data_end - packet_ptr -> nx_packet_prepend_ptr);

                /* Limit the data length to ES_WIFI_PAYLOAD_SIZE due to underlayer limitation.  */
                if (data_length > ES_WIFI_PAYLOAD_SIZE)
                {
                    data_length = ES_WIFI_PAYLOAD_SIZE;
                }

                /* Receive data without suspending.  */
                status = WIFI_ReceiveData(i, (uint8_t*)(packet_ptr -> nx_packet_prepend_ptr),
                                          data_length, &data_length, NX_NO_WAIT);

                if (status != WIFI_STATUS_OK)
                {

                    /* Connection error. Notify upper layer with Null packet.  */
                    if (nx_driver_sockets[i].protocol == NX_PROTOCOL_TCP)
                    {
                        _nx_tcp_socket_driver_packet_receive(nx_driver_sockets[i].socket_ptr, NX_NULL);
                        nx_driver_sockets[i].tcp_connected = NX_FALSE;
                    }
                    else
                    {
                        _nx_udp_socket_driver_packet_receive(nx_driver_sockets[i].socket_ptr, NX_NULL,
                                                             NX_NULL, NX_NULL, 0);
                    }
                    nx_packet_release(packet_ptr);
                    break;
                }

                if (data_length == 0)
                {

                    /* No incoming data.  */
                    nx_packet_release(packet_ptr);
                    break;
                }

                /* Set packet length.  */
                packet_ptr -> nx_packet_length = (ULONG)data_length;
                packet_ptr -> nx_packet_append_ptr = packet_ptr -> nx_packet_prepend_ptr + data_length;
                packet_ptr -> nx_packet_ip_interface = interface_ptr;

                /* Pass it to NetXDuo.  */
                if (nx_driver_sockets[i].protocol == NX_PROTOCOL_TCP)
                {
                    _nx_tcp_socket_driver_packet_receive(nx_driver_sockets[i].socket_ptr, packet_ptr);
                }
                else
                {

                    /* Convert IP version.  */
                    remote_ip.nxd_ip_version = NX_IP_VERSION_V4;
                    remote_ip.nxd_ip_address.v4 = nx_driver_sockets[i].remote_ip;
                    local_ip.nxd_ip_version = NX_IP_VERSION_V4;
                    local_ip.nxd_ip_address.v4 = nx_driver_sockets[i].local_ip;

                    _nx_udp_socket_driver_packet_receive(nx_driver_sockets[i].socket_ptr,
                                                         packet_ptr, &local_ip, &remote_ip,
                                                         nx_driver_sockets[i].remote_port);
                }
            }
        }
        
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(ip_ptr -> nx_ip_protection));
        
        /* Sleep some ticks to next loop.  */
        tx_thread_sleep(NX_DRIVER_THREAD_INTERVAL);
    }
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_tcpip_handler                           PORTABLE C       */ 
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function processing the TCP/IP request.                        */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    ip_ptr                                Pointer to IP                 */ 
/*    interface_ptr                         Pointer to interface          */ 
/*    socket_ptr                            Pointer to TCP or UDP socket  */ 
/*    operation                             Operation of TCP/IP request   */ 
/*    packet_ptr                            Pointer to packet             */ 
/*    local_ip                              Pointer to local IP address   */ 
/*    remote_ip                             Pointer to remote IP address  */ 
/*    local_port                            Local socket port             */ 
/*    remote_port                           Remote socket port            */ 
/*    wait_option                           Wait option in ticks          */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    nx_packet_transmit_release            Release transmittion packet   */
/*                                                                        */
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Driver entry function                                               */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_tcpip_handler(struct NX_IP_STRUCT *ip_ptr,
                                     struct NX_INTERFACE_STRUCT *interface_ptr,
                                     VOID *socket_ptr, UINT operation, NX_PACKET *packet_ptr,
                                     NXD_ADDRESS *local_ip, NXD_ADDRESS *remote_ip,
                                     UINT local_port, UINT *remote_port, UINT wait_option)
{
    (void)ip_ptr;
    (void)interface_ptr;

UINT status = NX_NOT_SUCCESSFUL;
UCHAR remote_ip_bytes[4];
NX_PACKET *current_packet;
ULONG packet_size;
ULONG offset;
uint16_t sent_size;
UINT i = 0;

    if (operation == NX_TCPIP_OFFLOAD_TCP_SERVER_SOCKET_LISTEN)
    {

        /* For TCP server socket, find duplicate listen requrest first.
           Only one socket can listen to each TCP port.  */
        for (i = 0; i < NX_DRIVER_SOCKETS_MAXIMUM; i++)
        {
            if ((nx_driver_sockets[i].local_port == local_port) &&
                (nx_driver_sockets[i].protocol == NX_PROTOCOL_TCP))
            {
                if (nx_driver_sockets[i].tcp_connected == NX_TRUE)
                {

                    /* Previous connection not closed. Multiple connection is not supported.  */
                    return(NX_NOT_SUPPORTED);
                }

                /* Find a duplicate listen. Just overwrite it.  */
                ((NX_TCP_SOCKET*)socket_ptr) -> nx_tcp_socket_tcpip_offload_context = (VOID*)i;
                nx_driver_sockets[i].socket_ptr = socket_ptr;
                return(NX_SUCCESS);
            }
        }
    }

    if ((operation == NX_TCPIP_OFFLOAD_TCP_CLIENT_SOCKET_CONNECT) ||
        (operation == NX_TCPIP_OFFLOAD_UDP_SOCKET_BIND) ||
        (operation == NX_TCPIP_OFFLOAD_TCP_SERVER_SOCKET_LISTEN))
    {

        /* Find a socket that is not used.  */
        for (i = 0; i < NX_DRIVER_SOCKETS_MAXIMUM; i++)
        {
            if (nx_driver_sockets[i].socket_ptr == NX_NULL)
            {

                /* Find an empty entry.  */
                nx_driver_sockets[i].socket_ptr = socket_ptr;
                break;
            }
        }

        if (i == NX_DRIVER_SOCKETS_MAXIMUM)
        {

            /* No more entries.  */
            return(NX_NO_MORE_ENTRIES);
        }
    }

    switch (operation)
    {
    case NX_TCPIP_OFFLOAD_TCP_CLIENT_SOCKET_CONNECT:

        /* Store the index of driver socket.  */
        ((NX_TCP_SOCKET *)socket_ptr) -> nx_tcp_socket_tcpip_offload_context = (VOID *)i;

        /* Convert remote IP to byte array.  */
        remote_ip_bytes[0] = (remote_ip -> nxd_ip_address.v4 >> 24) & 0xFF;
        remote_ip_bytes[1] = (remote_ip -> nxd_ip_address.v4 >> 16) & 0xFF;
        remote_ip_bytes[2] = (remote_ip -> nxd_ip_address.v4 >> 8) & 0xFF;
        remote_ip_bytes[3] = (remote_ip -> nxd_ip_address.v4) & 0xFF;

        /* Connect.  */
        status = WIFI_OpenClientConnection(i, WIFI_TCP_PROTOCOL, "",
                                           remote_ip_bytes, *remote_port, local_port);

        if (status)
        {
            return(NX_NOT_SUCCESSFUL);
        }

#ifdef NX_DEBUG
        printf("TCP client socket %u connect to: %u.%u.%u.%u:%u\r\n",
               i, remote_ip_bytes[0], remote_ip_bytes[1],
               remote_ip_bytes[2], remote_ip_bytes[3], *remote_port);
#endif

        /* Store address and port.  */
        nx_driver_sockets[i].remote_ip = remote_ip -> nxd_ip_address.v4;
        nx_driver_sockets[i].local_port = local_port;
        nx_driver_sockets[i].remote_port = *remote_port;
        nx_driver_sockets[i].protocol = NX_PROTOCOL_TCP;
        nx_driver_sockets[i].is_client = NX_TRUE;
        break;

    case NX_TCPIP_OFFLOAD_TCP_SERVER_SOCKET_LISTEN:

        /* Store the index of driver socket.  */
        ((NX_TCP_SOCKET *)socket_ptr) -> nx_tcp_socket_tcpip_offload_context = (VOID *)i;

        /* Start TCP server.  */
        status = WIFI_StartServer(i, WIFI_TCP_PROTOCOL, NX_DRIVER_SERVER_LISTEN_COUNT, "", local_port);
        if (status)
        {
            return(NX_NOT_SUCCESSFUL);
        }

#ifdef NX_DEBUG
        printf("TCP server socket %u listen to port: %u\r\n", i, local_port);
#endif

        /* Store address and port.  */
        nx_driver_sockets[i].local_port = local_port;
        nx_driver_sockets[i].remote_port = 0;
        nx_driver_sockets[i].protocol = NX_PROTOCOL_TCP;
        nx_driver_sockets[i].tcp_connected = NX_FALSE;
        nx_driver_sockets[i].is_client = NX_FALSE;
        break;

    case NX_TCPIP_OFFLOAD_TCP_SERVER_SOCKET_ACCEPT:
        i = (UINT)(((NX_TCP_SOCKET *)socket_ptr) -> nx_tcp_socket_tcpip_offload_context);

        /* Accept connection.  */
        status = WIFI_WaitServerConnection(i, 1, remote_ip_bytes, &nx_driver_sockets[i].remote_port);

        if (status)
        {
            return(NX_NOT_SUCCESSFUL);
        }

#ifdef NX_DEBUG
        printf("TCP server socket %u accept from: %u.%u.%u.%u:%u\r\n",
               i, remote_ip_bytes[0], remote_ip_bytes[1],
               remote_ip_bytes[2], remote_ip_bytes[3], nx_driver_sockets[i].remote_port);
#endif

        /* Store address and port.  */
        remote_ip -> nxd_ip_version = NX_IP_VERSION_V4;
        remote_ip -> nxd_ip_address.v4 = IP_ADDRESS(remote_ip_bytes[0],
                                                    remote_ip_bytes[1],
                                                    remote_ip_bytes[2],
                                                    remote_ip_bytes[3]);
        nx_driver_sockets[i].remote_ip = remote_ip -> nxd_ip_address.v4;
        *remote_port = (UINT)nx_driver_sockets[i].remote_port;
        nx_driver_sockets[i].tcp_connected = NX_TRUE;
        break;

    case NX_TCPIP_OFFLOAD_TCP_SERVER_SOCKET_UNLISTEN:
        for (i = 0; i < NX_DRIVER_SOCKETS_MAXIMUM; i++)
        {
            if ((nx_driver_sockets[i].local_port == local_port) &&
                (nx_driver_sockets[i].protocol == NX_PROTOCOL_TCP))
            {

#ifdef NX_DEBUG
                printf("TCP server socket %u unlisten port: %u\r\n",
                       i, local_port);
#endif
                nx_driver_sockets[i].socket_ptr = NX_NULL;
                nx_driver_sockets[i].local_port = 0;
                WIFI_StopServer(i);
                return(NX_SUCCESS);
            }
        }
        break;

    case NX_TCPIP_OFFLOAD_TCP_SOCKET_DISCONNECT:
        i = (UINT)(((NX_TCP_SOCKET *)socket_ptr) -> nx_tcp_socket_tcpip_offload_context);
        if (nx_driver_sockets[i].remote_port)
        {            

            if (nx_driver_sockets[i].is_client)
            {
#ifdef NX_DEBUG
                printf("TCP client socket %u disconnect\r\n", i);
#endif

                /* Disconnect.  */
                status = WIFI_CloseClientConnection(i);
            }
            else
            {
#ifdef NX_DEBUG
                printf("TCP server socket %u disconnect\r\n", i);
#endif

                /* Close server connection.  */
                status = WIFI_CloseServerConnection(i);
                nx_driver_sockets[i].tcp_connected = NX_FALSE;

                /* No need to free this entry as TCP server still needs to listening to port.  */
                break;
            }
        }

        /* Reset socket to free this entry.  */
        nx_driver_sockets[i].socket_ptr = NX_NULL;
        break;

    case NX_TCPIP_OFFLOAD_UDP_SOCKET_BIND:
        /* Note, send data from one port to multiple remotes are not supported.  */
        /* Store the index of driver socket.  */
        ((NX_UDP_SOCKET *)socket_ptr) -> nx_udp_socket_tcpip_offload_context = (VOID *)i;
        
        /* Reset the remote port to indicate the socket is not connected yet.  */
        nx_driver_sockets[i].remote_port = 0;

#ifdef NX_DEBUG
        printf("UDP socket %u bind to port: %u\r\n", i, local_port);
#endif

        status = NX_SUCCESS;
        break;

    case NX_TCPIP_OFFLOAD_UDP_SOCKET_UNBIND: 
        i = (UINT)(((NX_UDP_SOCKET *)socket_ptr) -> nx_udp_socket_tcpip_offload_context);

        if (nx_driver_sockets[i].remote_port)
        {
            
            /* Disconnect.  */
            status = WIFI_CloseClientConnection(i);

#ifdef NX_DEBUG
            printf("UDP socket %u unbind port: %u\r\n", i, local_port);
#endif
        }

        /* Reset socket to free this entry.  */
        nx_driver_sockets[i].socket_ptr = NX_NULL;
        break;

    case NX_TCPIP_OFFLOAD_UDP_SOCKET_SEND:
        i = (UINT)(((NX_UDP_SOCKET *)socket_ptr) -> nx_udp_socket_tcpip_offload_context);
        if (nx_driver_sockets[i].remote_port == 0)
        {

            /* Do connection once. */
            /* Convert remote IP to byte array.  */
            remote_ip_bytes[0] = (remote_ip -> nxd_ip_address.v4 >> 24) & 0xFF;
            remote_ip_bytes[1] = (remote_ip -> nxd_ip_address.v4 >> 16) & 0xFF;
            remote_ip_bytes[2] = (remote_ip -> nxd_ip_address.v4 >> 8) & 0xFF;
            remote_ip_bytes[3] = (remote_ip -> nxd_ip_address.v4) & 0xFF;

            /* Connect.  */
            status = WIFI_OpenClientConnection((uint8_t)i, WIFI_UDP_PROTOCOL, "",
                                               remote_ip_bytes, *remote_port,  local_port);
            if (status)
            {
                return(NX_NOT_SUCCESSFUL);
            }

#ifdef NX_DEBUG
            printf("UDP socket %u connect to: %u.%u.%u.%u:%u\r\n",
                   i, remote_ip_bytes[0], remote_ip_bytes[1],
                   remote_ip_bytes[2], remote_ip_bytes[3], *remote_port);
#endif

            /* Store address and port.  */
            nx_driver_sockets[i].local_ip = local_ip -> nxd_ip_address.v4;
            nx_driver_sockets[i].remote_ip = remote_ip -> nxd_ip_address.v4;
            nx_driver_sockets[i].local_port = local_port;
            nx_driver_sockets[i].remote_port = *remote_port;
            nx_driver_sockets[i].protocol = NX_PROTOCOL_UDP;
            nx_driver_sockets[i].is_client = NX_TRUE;
        }

        if ((packet_ptr -> nx_packet_length > ES_WIFI_PAYLOAD_SIZE)
#ifndef NX_DISABLE_PACKET_CHAIN
            || (packet_ptr -> nx_packet_next)
#endif /* NX_DISABLE_PACKET_CHAIN */
           )
        {

            /* Limitation in this driver. UDP packet must be in one packet.  */
            return (NX_NOT_SUCCESSFUL);
        }

        /* Convert wait option from ticks to ms.  */
        if (wait_option > (NX_DRIVER_SOCKET_SEND_TIMEOUT_MAXIMUM / 1000 * NX_IP_PERIODIC_RATE))
        {
            wait_option = NX_DRIVER_SOCKET_SEND_TIMEOUT_MAXIMUM;
        }
        else
        {
            wait_option = wait_option / NX_IP_PERIODIC_RATE * 1000;
        }

        /* Send data.  */
        status = WIFI_SendData((uint8_t)i,
                                packet_ptr -> nx_packet_prepend_ptr,
                                (uint16_t)(packet_ptr -> nx_packet_length), &sent_size,
                                wait_option); 
        
        /* Check status.  */
        if ((status != WIFI_STATUS_OK) || (sent_size != packet_ptr -> nx_packet_length))
        {
            return (NX_NOT_SUCCESSFUL);
        }

        /* Release the packet.  */
        nx_packet_transmit_release(packet_ptr);
        break;

    case NX_TCPIP_OFFLOAD_TCP_SOCKET_SEND:
        i = (UINT)(((NX_TCP_SOCKET *)socket_ptr) -> nx_tcp_socket_tcpip_offload_context);

        /* Initialize the current packet to the input packet pointer.  */
        current_packet =  packet_ptr;
        offset = 0;

        /* Convert wait option from ticks to ms.  */
        if (wait_option > (NX_DRIVER_SOCKET_SEND_TIMEOUT_MAXIMUM / 1000 * NX_IP_PERIODIC_RATE))
        {
            wait_option = NX_DRIVER_SOCKET_SEND_TIMEOUT_MAXIMUM;
        }
        else
        {
            wait_option = wait_option / NX_IP_PERIODIC_RATE * 1000;
        }

        /* Loop to send the packet.  */
        while(current_packet)
        {

            /* Calculate current packet size. */
            packet_size = (ULONG)(current_packet -> nx_packet_append_ptr - current_packet -> nx_packet_prepend_ptr);
            packet_size -= offset;

            /* Limit the data size to ES_WIFI_PAYLOAD_SIZE due to underlayer limitation.  */
            if (packet_size > ES_WIFI_PAYLOAD_SIZE)
            {
                packet_size = ES_WIFI_PAYLOAD_SIZE;
            }

            /* Send data.  */
            status = WIFI_SendData((uint8_t)i,
                                   current_packet -> nx_packet_prepend_ptr + offset,
                                   (uint16_t)packet_size, &sent_size,
                                   wait_option); 
            
            /* Check status.  */
            if ((status != WIFI_STATUS_OK) || (sent_size != packet_size))
            {
                return (NX_NOT_SUCCESSFUL);
            }

            /* Calculate current packet size. */
            packet_size = (ULONG)(current_packet -> nx_packet_append_ptr - current_packet -> nx_packet_prepend_ptr);

            if ((sent_size + offset) < packet_size)
            {

                /* Partial data sent. Increase the offset.  */
                offset += sent_size;
            }
            else
            {

                /* Data in current packet are all sent.  */
                offset = 0;
            
#ifndef NX_DISABLE_PACKET_CHAIN
                /* We have crossed the packet boundary.  Move to the next packet structure.  */
                current_packet =  current_packet -> nx_packet_next;
#else
                /* End of the loop.  */
                current_packet = NX_NULL;
#endif /* NX_DISABLE_PACKET_CHAIN */
            }
        }

        /* Release the packet.  */
        nx_packet_transmit_release(packet_ptr);
        break;

    default:
        break;
    }

    return(status);
}


/****** DRIVER SPECIFIC ****** Start of part/vendor specific internal driver functions.  */


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_hardware_initialize                     PORTABLE C       */ 
/*                                                           6.x          */
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
/*    tx_thread_info_get                    Get thread information        */ 
/*    tx_thread_create                      Create driver thread          */ 
/*                                                                        */
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _nx_driver_initialize                 Driver initialize processing  */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_initialize(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;

UINT status;
UINT priority = 0;

    /* Get priority of IP thread.  */
    tx_thread_info_get(tx_thread_identify(), NX_NULL, NX_NULL, NX_NULL, &priority,
                       NX_NULL, NX_NULL, NX_NULL, NX_NULL);

    /* Create the driver thread.  */
    /* The priority of network thread is lower than IP thread.  */
    status = tx_thread_create(&nx_driver_thread, "Driver Thread", _nx_driver_thread_entry, 0,  
                              nx_driver_thread_stack, NX_DRIVER_STACK_SIZE, 
                              priority + 1, priority + 1,
                              TX_NO_TIME_SLICE, TX_DONT_START); 

    /* Return success!  */
    return(status);
} 


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_hardware_enable                         PORTABLE C       */ 
/*                                                           6.x          */
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
/*    tx_thread_reset                       Reset driver thread           */ 
/*    tx_thread_resume                      Resume driver thread          */ 
/*                                                                        */
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _nx_driver_enable                     Driver link enable processing */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_enable(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;

    tx_thread_reset(&nx_driver_thread);
    tx_thread_resume(&nx_driver_thread);

    /* Return success!  */
    return(NX_SUCCESS);
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_hardware_disable                        PORTABLE C       */ 
/*                                                           6.x          */
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
/*    tx_thread_suspend                     Suspend driver thread         */ 
/*    tx_thread_terminate                   Terminate driver thread       */ 
/*                                                                        */
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _nx_driver_disable                    Driver link disable processing*/ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_disable(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;

UINT i;

    /* Reset all sockets.  */
    for (i = 0; i < NX_DRIVER_SOCKETS_MAXIMUM; i++)
    {
        if (nx_driver_sockets[i].socket_ptr)
        {

            /* Disconnect.  */
            WIFI_CloseClientConnection(i);
            nx_driver_sockets[i].socket_ptr = NX_NULL;
        }
    }

    tx_thread_suspend(&nx_driver_thread);
    tx_thread_terminate(&nx_driver_thread);

    /* Return success!  */
    return(NX_SUCCESS);
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_hardware_get_status                     PORTABLE C       */ 
/*                                                           6.x          */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_get_status(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;

    /* Return success.  */
    return(NX_SUCCESS);
}


#ifdef NX_ENABLE_INTERFACE_CAPABILITY
/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _nx_driver_hardware_capability_set                 PORTABLE C       */  
/*                                                           6.x          */
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
/*  xx-xx-xxxx     Yuxin Zhou               Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_hardware_capability_set(NX_IP_DRIVER *driver_req_ptr)
{
    (void)driver_req_ptr;
    
    return NX_SUCCESS;
}
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

/****** DRIVER SPECIFIC ****** Start of part/vendor specific internal driver functions.  */

