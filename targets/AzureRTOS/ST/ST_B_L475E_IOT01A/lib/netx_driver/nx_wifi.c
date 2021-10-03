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
/**   NetX Porting layer for STM32L4XX                                    */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tx_api.h"
#include "nx_api.h"
#include "wifi.h"
#include "nx_ip.h"
#include "nx_tcp.h"
#include "tx_thread.h"
#include "nx_wifi.h"

#define WIFI_WRITE_TIMEOUT          100
#define WIFI_READ_TIMEOUT           100

#ifndef WIFI_THREAD_PERIOD
#define WIFI_THREAD_PERIOD          100
#endif /* WIFI_THREAD_PERIOD  */

/* Define the default thread priority, stack size, etc. The user can override this 
   via -D command line option or via project settings.  */

#ifndef NX_WIFI_STACK_SIZE
#define NX_WIFI_STACK_SIZE          (1024)
#endif /* NX_WIFI_STACK_SIZE  */

#ifndef NX_WIFI_THREAD_PRIORITY
#define NX_WIFI_THREAD_PRIORITY     (1)
#endif /* NX_WIFI_THREAD_PRIORITY  */

/* Define the stack for X-WARE WIFI.  */
static UCHAR                        nx_wifi_thread_stack[NX_WIFI_STACK_SIZE];

/* Define the prototypes for X-WARE.  */
static TX_THREAD                    nx_wifi_thread;
static NX_PACKET_POOL               *nx_wifi_pool;
static NX_IP                        *nx_wifi_ip;

/* Define the socket type, TCP socket or UDP socket.  */
#define NX_WIFI_TCP_SOCKET          0
#define NX_WIFI_UDP_SOCKET          1

/* Reserve some packets for applications, such as HTTP, etc.  */
#ifndef NX_WIFI_PACKET_RESERVED
#define NX_WIFI_PACKET_RESERVED     1
#endif /* NX_WIFI_PACKET_RESERVED  */

/* Define the WIFI socket structure.  */
typedef struct NX_WIFI_SOCKET_STRUCT
{

    /* Define socket pointer.  */
    VOID       *nx_wifi_socket_ptr;
    
    /* Define socket flag.  */
    CHAR        nx_wifi_socket_valid;
    
    /* Define socket type.  TCP or UDP.  */
    CHAR        nx_wifi_socket_type;
    
    /* Define the connected flag.  */
    CHAR        nx_wifi_socket_connected;

    /* Reserved.  */
    CHAR        reserved;
    
    /* Define the deferred packet processing queue.  */
    NX_PACKET   *nx_wifi_received_packet_head,
                *nx_wifi_received_packet_tail;

#ifdef NX_ENABLE_IP_PACKET_FILTER

    /* Define the UDP connected IP and port.  */
    ULONG       nx_wifi_udp_socket_connect_ip;
    UINT        nx_wifi_udp_socket_connect_port;
#endif /* NX_ENABLE_IP_PACKET_FILTER */
    
}NX_WIFI_SOCKET;

#ifndef NX_WIFI_SOCKET_COUNTER
#define NX_WIFI_SOCKET_COUNTER          8
#endif /* NX_WIFI_SOCKET_COUNTER  */

/* Define the TCP socket and UDP socket.  */
static NX_WIFI_SOCKET               nx_wifi_socket[NX_WIFI_SOCKET_COUNTER];

/* Define the SOCKET ID.  */
static CHAR                         nx_wifi_socket_counter;

/* Define the buffer to receive data from wifi.  */
static CHAR                         nx_wifi_buffer[ES_WIFI_PAYLOAD_SIZE];

#ifdef NX_ENABLE_IP_PACKET_FILTER

/* Define the wifi IP address.  */
static ULONG                        nx_wifi_ip_address = 0;

/* Define the buffer to build fake IP header and TCP/UDP header.
   IP Header: 20 bytes, TCP header: 20 bytes.  */
static CHAR                         nx_wifi_ip_buffer[40];

/* Define the ip packet filter.  */
static UINT  nx_wifi_ip_packet_filter(ULONG source_ip, ULONG destination_ip, 
                                      ULONG source_port, ULONG destination_port,
                                      ULONG protocol, ULONG packet_length, UINT direction);
#endif /* NX_ENABLE_IP_PACKET_FILTER */

/* Define the wifi thread.  */
static void    nx_wifi_thread_entry(ULONG thread_input);

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_initialize                                  PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function initializes the NetX Wifi.                            */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    ip_ptr                                Pointer to IP control block   */
/*    packet_pool                           Packet pool pointer           */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT nx_wifi_initialize(NX_IP *ip_ptr, NX_PACKET_POOL *packet_pool)
{

UINT    status;

    
    /* Set the IP.  */
    nx_wifi_ip = ip_ptr;
    
    /* Set the pool.  */
    nx_wifi_pool = packet_pool;
    
    /* Initialize the wifi.  */
    memset(nx_wifi_socket, 0, (NX_WIFI_SOCKET_COUNTER * sizeof(NX_WIFI_SOCKET)));
    
    /* Initialize the socket id.  */
    nx_wifi_socket_counter = 0;

    /* Create the wifi thread.  */
    status = tx_thread_create(&nx_wifi_thread, "Wifi Thread", nx_wifi_thread_entry, 0,  
                              nx_wifi_thread_stack, NX_WIFI_STACK_SIZE, 
                              NX_WIFI_THREAD_PRIORITY, NX_WIFI_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START); 
     
    /* Check for thread create errors.  */
    if (status)
        return(status);
    
    return(NX_SUCCESS);
}
 
/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_thread_entry                                PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function is the entry point for NetX Wifi helper thread.  The  */
/*    Wifi helper thread is responsible for receiving packet.             */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    thread_input                          Pointer to IP control block   */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s), and      */
/*                                            optimized internal code,    */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
void nx_wifi_thread_entry(ULONG thread_input)
{

UINT            i;
UINT            socket_counter;
USHORT          size;
UINT            status; 
NX_PACKET       *packet_ptr;
NX_TCP_SOCKET   *tcp_socket;
NX_UDP_SOCKET   *udp_socket;
#ifdef NX_ENABLE_IP_PACKET_FILTER
uint8_t         ip_address[4];
#endif /* NX_ENABLE_IP_PACKET_FILTER */


#ifdef NX_ENABLE_IP_PACKET_FILTER
    if (WIFI_GetIP_Address(ip_address) == WIFI_STATUS_OK)
    {
        nx_wifi_ip_address = IP_ADDRESS(ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }
#endif /* NX_ENABLE_IP_PACKET_FILTER  */
    
    while(1)
    {
      
        /* Obtain the IP internal mutex before processing the IP event.  */
        tx_mutex_get(&(nx_wifi_ip -> nx_ip_protection), TX_WAIT_FOREVER);
        
        socket_counter = 0;
        
        /* Check if have socket.  */
        if (nx_wifi_socket_counter != 0)
        {
          
            /* Loop to receive the data from */
            for (i = 0; (i < NX_WIFI_SOCKET_COUNTER) && (socket_counter < nx_wifi_socket_counter); i++)
            {
              
                /* Check if the socket is valid and connected.  */
                if ((nx_wifi_socket[i].nx_wifi_socket_valid == 0) || (nx_wifi_socket[i].nx_wifi_socket_connected == 0))
                    continue;                                           
                
                /* Update the socket counter.  */
                socket_counter++;
                
                /* Loop to receive the data from wifi for current socket.  */
                status = WIFI_STATUS_OK;
                do
                {
                    
                    /* Make sure there is enought space to store the data before receiving data from WIFI.  */
                    if ((nx_wifi_pool -> nx_packet_pool_available * nx_wifi_pool -> nx_packet_pool_payload_size) < 
                        (ES_WIFI_PAYLOAD_SIZE + (NX_WIFI_PACKET_RESERVED * nx_wifi_pool -> nx_packet_pool_payload_size)))
                        break;
                      
                    /* Receive the data in WIFI_READ_TIMEOUT ms.  */
                    status = WIFI_ReceiveData(i, (uint8_t*)nx_wifi_buffer, ES_WIFI_PAYLOAD_SIZE, &size, WIFI_READ_TIMEOUT);
                    
                    /* Check status.  */
                    if ((status != WIFI_STATUS_OK) || (size == 0))
                        break;
                    
                    /* Allocate one packet to store the data.  */
                    if (nx_packet_allocate(nx_wifi_pool, &packet_ptr,  NX_RECEIVE_PACKET, NX_NO_WAIT))
                        break;
                      
                    /* Set the data.  */
                    if (nx_packet_data_append(packet_ptr, nx_wifi_buffer, size, nx_wifi_pool, NX_NO_WAIT))
                    {
                        nx_packet_release(packet_ptr);
                        break;
                    }
          
                    /* Check to see if the deferred processing queue is empty.  */
                    if (nx_wifi_socket[i].nx_wifi_received_packet_head)
                    {

                        /* Not empty, just place the packet at the end of the queue.  */
                        (nx_wifi_socket[i].nx_wifi_received_packet_tail) -> nx_packet_queue_next =  packet_ptr;
                        packet_ptr -> nx_packet_queue_next =  NX_NULL;
                        nx_wifi_socket[i].nx_wifi_received_packet_tail =  packet_ptr;

                    }
                    else
                    {

                        /* Empty deferred receive processing queue.  Just setup the head pointers and
                           set the event flags to ensure the IP helper thread looks at the deferred processing
                           queue.  */
                        nx_wifi_socket[i].nx_wifi_received_packet_head =  packet_ptr;
                        nx_wifi_socket[i].nx_wifi_received_packet_tail =  packet_ptr;
                        packet_ptr -> nx_packet_queue_next =             NX_NULL;
                          
                        /* Check the socket type.  */
                        if (nx_wifi_socket[i].nx_wifi_socket_type == NX_WIFI_TCP_SOCKET)
                        {
                            
                            /* Get the tcp socket.  */
                            tcp_socket = (NX_TCP_SOCKET *)nx_wifi_socket[i].nx_wifi_socket_ptr;

#ifdef NX_ENABLE_IP_PACKET_FILTER
                            nx_wifi_ip_packet_filter(tcp_socket -> nx_tcp_socket_connect_ip.nxd_ip_address.v4,
                                                     nx_wifi_ip_address,
                                                     tcp_socket -> nx_tcp_socket_connect_port,
                                                     tcp_socket -> nx_tcp_socket_port,
                                                     NX_IP_TCP, size, NX_IP_PACKET_IN);
#endif /* NX_ENABLE_IP_PACKET_FILTER */

                            /* Determine if there is a socket receive notification function specified.  */
                            if (tcp_socket -> nx_tcp_receive_callback)
                            {

                                /* Yes, notification is requested.  Call the application's receive notification
                                   function for this socket.  */
                                (tcp_socket -> nx_tcp_receive_callback)(tcp_socket);
                            }
                        }
                        else
                        {

                            /* Get the udp socket.  */
                            udp_socket = (NX_UDP_SOCKET *)nx_wifi_socket[i].nx_wifi_socket_ptr;

#ifdef NX_ENABLE_IP_PACKET_FILTER

                            /* Process packet filter.  */
                            nx_wifi_ip_packet_filter(nx_wifi_socket[i].nx_wifi_udp_socket_connect_ip,
                                                     nx_wifi_ip_address,
                                                     nx_wifi_socket[i].nx_wifi_udp_socket_connect_port,
                                                     udp_socket -> nx_udp_socket_port,
                                                     NX_IP_UDP, size, NX_IP_PACKET_IN);
#endif /* NX_ENABLE_IP_PACKET_FILTER */

                            /* Determine if there is a socket receive notification function specified.  */
                            if (udp_socket -> nx_udp_receive_callback)
                            {

                                /* Yes, notification is requested.  Call the application's receive notification
                                   function for this socket.  */
                                (udp_socket -> nx_udp_receive_callback)(udp_socket);
                            }                     
                        }
                    }
        
                    /* Queue the packet.  */
                }while (status == WIFI_STATUS_OK);  
            }
        }
        
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        
        /* Sleep some ticks to next loop.  */
        tx_thread_sleep(WIFI_THREAD_PERIOD);
    }
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_tick_convert_ms                             PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function converts the ticks to milliseconds.                   */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    tick                                  Tick value                    */
/*    millisecond                           Destination to millisecond    */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  nx_wifi_tick_convert_ms(ULONG tick, ULONG *millisecond)
{
  
UINT    factor = 1000/NX_IP_PERIODIC_RATE;
    

    /* Check the wait_option.  */
    if (tick)
    {
      
        /* Change ticks to milliseconds to ticks.  */
        if (tick >= NX_WAIT_FOREVER/factor)
            *millisecond = NX_WAIT_FOREVER;
        else
            *millisecond = (tick * factor);
    }
    else
    {
        *millisecond = 0;
    }    
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_socket_entry_find                           PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function finds an available entry.                             */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    socket_ptr                            Socket pointer                */
/*    entry_index                           Destination to entry          */
/*    entry_find                            Find flag                     */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  nx_wifi_socket_entry_find(void *socket_ptr, UCHAR *entry_index, UCHAR entry_find)
{
    
UINT    i;
UCHAR   empty_index = NX_WIFI_SOCKET_COUNTER;

    /* Loop to find an empty entry.  */
    for (i = 0; i < NX_WIFI_SOCKET_COUNTER; i++)
    {
        
        /* Check the valid flag.  */
        if (nx_wifi_socket[i].nx_wifi_socket_valid)
        {
          
            /* Check if the entry already exist.  */          
            if (nx_wifi_socket[i].nx_wifi_socket_ptr == socket_ptr)
            {
              
                /* Check if find the entry.  */
                if (entry_find)
                {                
                    *entry_index = i;
                    return(NX_SUCCESS);
                }
                else
                {
                    return(NX_NOT_SUCCESSFUL);
                }
            }
        }
        else
        {
            
            /* Set the empty index.  */
            if (empty_index > i)
                empty_index = i;
        }
    }
    
    /* Check if have empty entry.  */
    if (empty_index >= NX_WIFI_SOCKET_COUNTER)
        return (NX_NOT_SUCCESSFUL);
     
    (*entry_index) = empty_index;
    return(NX_SUCCESS);
}



/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_socket_reset                                PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function resets the entry and release the packet.              */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    socket_ptr                            Socket pointer                */
/*    entry_index                           Destination to entry          */
/*    entry_find                            Find flag                     */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static void  nx_wifi_socket_reset(UCHAR entry_index)
{
     
NX_PACKET *next_packet;
NX_PACKET *current_packet;   

    /* Check if this is an valid entry.  */
    if (nx_wifi_socket[entry_index].nx_wifi_socket_valid == 0)
        return;    

    /* Setup next packet to queue head.  */
    next_packet = nx_wifi_socket[entry_index].nx_wifi_received_packet_head;

    /* Release any packets queued up.  */
    while (next_packet)
    {

        /* Setup the current packet pointer.  */
        current_packet =  next_packet;

        /* Move to the next packet.  */
        next_packet =  next_packet -> nx_packet_queue_next;

        /* Release the current packet.  */
        nx_packet_release(current_packet);
    }
    
    /* Reset the entry.  */
    memset(&nx_wifi_socket[entry_index], 0, sizeof(NX_WIFI_SOCKET));    
    nx_wifi_socket_counter--;
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_socket_receive                              PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function receives packet for wifi socket                       */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    socket_ptr                            Socket pointer                */
/*    packet_ptr                            Pointer to received packet    */
/*    wait_option                           Suspension option             */
/*    socket_type                           Socket type                   */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s), and      */
/*                                            optimized internal code,    */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  nx_wifi_socket_receive(VOID *socket_ptr, NX_PACKET **packet_ptr, ULONG wait_option, UINT socket_type)
{

UINT    status;
UCHAR   entry_index;
ULONG   total_millisecond;
ULONG   wait_millisecond;
UINT    start_time;
ULONG   millisecond;
USHORT  size;
UINT    received_packet = NX_FALSE;
#ifdef NX_ENABLE_IP_PACKET_FILTER
NX_TCP_SOCKET *tcp_socket;
NX_UDP_SOCKET *udp_socket;
#endif /* NX_ENABLE_IP_PACKET_FILTER */

    /* Obtain the IP internal mutex before processing the IP event.  */
    tx_mutex_get(&(nx_wifi_ip -> nx_ip_protection), TX_WAIT_FOREVER);     
    
    /* Find an avaiable entry.  */
    if (nx_wifi_socket_entry_find((void *)socket_ptr, &entry_index, 1))
    {
      
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    }

    /* Check if the socket is connected.  */
    if (nx_wifi_socket[entry_index].nx_wifi_socket_connected == 0)
    {
      
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    } 
    
    /* Convert the tick to millisecond.  */
    nx_wifi_tick_convert_ms(wait_option, &total_millisecond); 
          
    /* Receive the packet from queue.  */
    if (nx_wifi_socket[entry_index].nx_wifi_received_packet_head)
    {
                
        /* Remove the first packet and process it!  */
          
        /* Pickup the first packet.  */
        *packet_ptr =  nx_wifi_socket[entry_index].nx_wifi_received_packet_head;

        /* Move the head pointer to the next packet.  */
        nx_wifi_socket[entry_index].nx_wifi_received_packet_head =  (*packet_ptr) -> nx_packet_queue_next;

        /* Check for end of deferred processing queue.  */
        if (nx_wifi_socket[entry_index].nx_wifi_received_packet_head == NX_NULL)
        {

            /* Yes, the queue is empty.  Set the tail pointer to NULL.  */
            nx_wifi_socket[entry_index].nx_wifi_received_packet_tail =  NX_NULL;
        }
        
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_SUCCESS);
    }
    else
    {
        
        /* Get the start time.  */
        start_time = tx_time_get();
        
        /* Loop to receive a packet.  */
        while(total_millisecond)
        {
            
            /* Check if exceed the max value of ES_WIFI_TIMEOUT.  */
            if (total_millisecond > ES_WIFI_TIMEOUT)
                wait_millisecond = ES_WIFI_TIMEOUT;
            else
                wait_millisecond = total_millisecond;
            
            /* Check if exceed the max value of ES_WIFI_TRANSPORT_TIMEOUT.  */
            if (wait_millisecond > ES_WIFI_TRANSPORT_TIMEOUT)
                wait_millisecond = ES_WIFI_TRANSPORT_TIMEOUT;
        
            /* Receive the data within a specified time.  */ 
            status = WIFI_ReceiveData(entry_index, (uint8_t*)nx_wifi_buffer, ES_WIFI_PAYLOAD_SIZE, &size, wait_millisecond);
                                
            /* Check status.  */
            if ((status != WIFI_STATUS_OK) || (size == 0))
            {
                
                /* Convert the tick to millisecond.  */
                nx_wifi_tick_convert_ms((tx_time_get() - start_time), &millisecond); 
                
                /* Update the remaining millisecond.  */
                if (millisecond >= total_millisecond)
                    total_millisecond = 0;
                else
                    total_millisecond -=millisecond;
                
                continue;
            }
            else
            {
                received_packet = NX_TRUE;
                break;
            }
        }    
        
        /* Check if receive a packet.  */
        if (received_packet != NX_TRUE)
        {
            
            /* Release the IP internal mutex before processing the IP event.  */
            tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
            return(NX_NO_PACKET);
        }

#ifdef NX_ENABLE_IP_PACKET_FILTER

        /* Check the socket type.  */
        if (nx_wifi_socket[entry_index].nx_wifi_socket_type == NX_WIFI_TCP_SOCKET)
        {

            /* Get the tcp socket.  */
            tcp_socket = (NX_TCP_SOCKET *)nx_wifi_socket[entry_index].nx_wifi_socket_ptr;

            /* Process packet filter.  */
            nx_wifi_ip_packet_filter(tcp_socket -> nx_tcp_socket_connect_ip.nxd_ip_address.v4,
                                     nx_wifi_ip_address,
                                     tcp_socket -> nx_tcp_socket_connect_port,
                                     tcp_socket -> nx_tcp_socket_port,
                                     NX_IP_TCP, size, NX_IP_PACKET_IN);
        }
        else
        {

            /* Get the udp socket.  */
            udp_socket = (NX_UDP_SOCKET *)nx_wifi_socket[entry_index].nx_wifi_socket_ptr;

            /* Process packet filter.  */
            nx_wifi_ip_packet_filter(nx_wifi_socket[entry_index].nx_wifi_udp_socket_connect_ip,
                                     nx_wifi_ip_address,
                                     nx_wifi_socket[entry_index].nx_wifi_udp_socket_connect_port,
                                     udp_socket -> nx_udp_socket_port,
                                     NX_IP_UDP, size, NX_IP_PACKET_IN);
        }
#endif /* NX_ENABLE_IP_PACKET_FILTER  */
        
        /* Allocate one packet to store the data.  */
        if (nx_packet_allocate(nx_wifi_pool, packet_ptr,  NX_RECEIVE_PACKET, NX_NO_WAIT))
        {
            
            /* Release the IP internal mutex before processing the IP event.  */
            tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
            return(NX_NOT_SUCCESSFUL);
        }
                      
        /* Set the data.  */
        if (nx_packet_data_append(*packet_ptr, nx_wifi_buffer, size, nx_wifi_pool, NX_NO_WAIT))
        {          
            
            /* Release the packet.  */
            nx_packet_release(*packet_ptr);
            
            /* Release the IP internal mutex before processing the IP event.  */
            tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
            return(NX_NOT_SUCCESSFUL);
        }
        
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_SUCCESS);
    }
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_tcp_client_socket_connect                   PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function sends wifi connection command                         */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    socket_ptr                            Socket pointer                */
/*    packet_ptr                            Pointer to received packet    */
/*    wait_option                           Suspension option             */
/*    socket_type                           Socket type                   */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  nx_wifi_tcp_client_socket_connect(NX_TCP_SOCKET *socket_ptr,
                                        NXD_ADDRESS *server_ip,
                                        UINT server_port,
                                        ULONG wait_option)
{
  
UINT    status ;
UCHAR   entry_index;
  
    
    /* Obtain the IP internal mutex before processing the IP event.  */
    tx_mutex_get(&(nx_wifi_ip -> nx_ip_protection), TX_WAIT_FOREVER);
    
    /* Find an avaiable entry.  */
    if (nx_wifi_socket_entry_find((void *)socket_ptr, &entry_index, 0))
    {
      
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    }
    
    /* Set the entry info.  */
    nx_wifi_socket[entry_index].nx_wifi_socket_ptr = (void *)socket_ptr;
    nx_wifi_socket[entry_index].nx_wifi_socket_valid = 1;
    nx_wifi_socket[entry_index].nx_wifi_socket_type = NX_WIFI_TCP_SOCKET; 
    nx_wifi_socket[entry_index].nx_wifi_socket_connected = 0;
    nx_wifi_socket_counter++;
    
    /* Swap the address.  */
    NX_CHANGE_ULONG_ENDIAN(server_ip -> nxd_ip_address.v4);
  
    /* Wifi connect.  */
    status= WIFI_OpenClientConnection(entry_index , WIFI_TCP_PROTOCOL, "", (unsigned char* )(&(server_ip -> nxd_ip_address.v4)), server_port, socket_ptr -> nx_tcp_socket_port) ;
    
    /* Swap the address.  */
    NX_CHANGE_ULONG_ENDIAN(server_ip -> nxd_ip_address.v4);
    
    if(status == WIFI_STATUS_OK)
    {     
            
        /* Update the connect flag.  */
        nx_wifi_socket[entry_index].nx_wifi_socket_connected = 1;   
        
        /* Update the address and port.  */
        socket_ptr -> nx_tcp_socket_connect_ip.nxd_ip_version = NX_IP_VERSION_V4;
        socket_ptr -> nx_tcp_socket_connect_ip.nxd_ip_address.v4 = server_ip -> nxd_ip_address.v4;
        socket_ptr -> nx_tcp_socket_connect_port = server_port;
        socket_ptr -> nx_tcp_socket_state =  NX_TCP_ESTABLISHED;  
        
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_SUCCESS); 
    }
    else
    {
      
        /* Reset the entry.  */
        nx_wifi_socket_reset(entry_index);
        
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    }
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_tcp_socket_disconnect                       PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function sends wifi disconnect command                         */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    socket_ptr                            Pointer to TCP client socket  */
/*    wait_option                           Suspension option             */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  nx_wifi_tcp_socket_disconnect(NX_TCP_SOCKET *socket_ptr, ULONG wait_option)
{
  
UCHAR   entry_index;


    /* Obtain the IP internal mutex before processing the IP event.  */
    tx_mutex_get(&(nx_wifi_ip -> nx_ip_protection), TX_WAIT_FOREVER);
    
    /* Check if the entry already exist.  */
    if (nx_wifi_socket_entry_find((void *)socket_ptr, &entry_index, 1))
    {
      
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    }
    
    /* Check if the socket is connected.  */
    if (nx_wifi_socket[entry_index].nx_wifi_socket_connected == 0)
    {
      
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    }
    
    /* Close connection.  */
    WIFI_CloseClientConnection(entry_index);

    /* Reset the entry.  */   
    socket_ptr -> nx_tcp_socket_state = NX_TCP_CLOSED;  
      
    /* Reset the entry.  */
    nx_wifi_socket_reset(entry_index);
        
    /* Release the IP internal mutex before processing the IP event.  */
    tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
    
    /* Return success.  */
    return(NX_SUCCESS);
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_tcp_socket_send                             PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function sends a TCP packet.                                   */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    socket_ptr                            Pointer to socket             */
/*    packet_ptr                            Pointer to packet to send     */
/*    wait_option                           Suspension option             */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  nx_wifi_tcp_socket_send(NX_TCP_SOCKET *socket_ptr, NX_PACKET *packet_ptr, ULONG wait_option)
{
  
UINT        status ;
UCHAR       entry_index;
USHORT      send_data_length;
ULONG       packet_size;
NX_PACKET   *current_packet;

    
    /* Obtain the IP internal mutex before processing the IP event.  */
    tx_mutex_get(&(nx_wifi_ip -> nx_ip_protection), TX_WAIT_FOREVER);
        
    /* Find an avaiable entry.  */
    if (nx_wifi_socket_entry_find((void *)socket_ptr, &entry_index, 1))
    {
      
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    }
    
    /* Check if the socket is connected.  */
    if (nx_wifi_socket[entry_index].nx_wifi_socket_connected == 0)
    {
      
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    }
    
    /* Initialize the current packet to the input packet pointer.  */
    current_packet =  packet_ptr;
    
    /* Loop to send the packet.  */
    while(current_packet)
    {
      
        /* Calculate current packet size. */
        packet_size = (ULONG)(current_packet -> nx_packet_append_ptr - current_packet -> nx_packet_prepend_ptr);
      
        /* Send data.  */
        status = WIFI_SendData(entry_index, current_packet -> nx_packet_prepend_ptr, packet_size, &send_data_length, WIFI_WRITE_TIMEOUT); 
        
        /* Check status.  */
        if ((status != WIFI_STATUS_OK) || (send_data_length != packet_size))
        {
          
            /* Release the IP internal mutex before processing the IP event.  */
            tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
            return (NX_NOT_SUCCESSFUL);
        }

#ifdef NX_ENABLE_IP_PACKET_FILTER

        /* Process packet filter.  */
        nx_wifi_ip_packet_filter(nx_wifi_ip_address,
                                 socket_ptr -> nx_tcp_socket_connect_ip.nxd_ip_address.v4,
                                 socket_ptr -> nx_tcp_socket_port,
                                 socket_ptr -> nx_tcp_socket_connect_port,
                                 NX_IP_TCP, packet_size, NX_IP_PACKET_OUT);
#endif /* NX_ENABLE_IP_PACKET_FILTER */

#ifndef NX_DISABLE_PACKET_CHAIN
        /* We have crossed the packet boundary.  Move to the next packet
           structure.  */
        current_packet =  current_packet -> nx_packet_next;
#else

        /* End the loop.  */
        current_packet = NX_NULL;
#endif /* NX_DISABLE_PACKET_CHAIN */
    }
     
    /* Release the packet.  */
    nx_packet_release(packet_ptr);
    
    /* Release the IP internal mutex before processing the IP event.  */
    tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
    
    return (NX_SUCCESS);      
}

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_tcp_socket_receive                          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function receives a TCP packet.                                */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    socket_ptr                            Pointer to socket             */
/*    packet_ptr                            Pointer to packet pointer     */
/*    wait_option                           Suspension option             */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  nx_wifi_tcp_socket_receive(NX_TCP_SOCKET *socket_ptr, NX_PACKET **packet_ptr, ULONG wait_option)
{
    return(nx_wifi_socket_receive((VOID*)socket_ptr, packet_ptr, wait_option, NX_WIFI_TCP_SOCKET));
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_udp_socket_bind                             PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function binds UDP socket.                                     */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    socket_ptr                            Pointer to UDP socket         */
/*    port                                  16-bit UDP port number        */
/*    wait_option                           Suspension option             */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  nx_wifi_udp_socket_bind(NX_UDP_SOCKET *socket_ptr, UINT  port, ULONG wait_option)
{
  
UCHAR       entry_index;

    /* Obtain the IP internal mutex before processing the IP event.  */
    tx_mutex_get(&(nx_wifi_ip -> nx_ip_protection), TX_WAIT_FOREVER);
    
    /* Find an avaiable entry.  */
    if (nx_wifi_socket_entry_find((void *)socket_ptr, &entry_index, 0))
    {
      
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    }
    
    /* Set the entry info.  */
    nx_wifi_socket[entry_index].nx_wifi_socket_ptr = (void *)socket_ptr;
    nx_wifi_socket[entry_index].nx_wifi_socket_valid = 1;
    nx_wifi_socket[entry_index].nx_wifi_socket_type = NX_WIFI_UDP_SOCKET;
    nx_wifi_socket[entry_index].nx_wifi_socket_connected = 0;
    nx_wifi_socket_counter++;
    
    /* Release the IP internal mutex before processing the IP event.  */
    tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        
    return(NX_SUCCESS);
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_udp_socket_unbind                           PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function unbinds UDP socket.                                   */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    socket_ptr                            Pointer to UDP socket         */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  nx_wifi_udp_socket_unbind(NX_UDP_SOCKET *socket_ptr)
{

UCHAR   entry_index;

    /* Obtain the IP internal mutex before processing the IP event.  */
    tx_mutex_get(&(nx_wifi_ip -> nx_ip_protection), TX_WAIT_FOREVER);
        
    /* Check if the entry already exist.  */
    if (nx_wifi_socket_entry_find((void *)socket_ptr, &entry_index, 1))
    {
      
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    }
    
    /* Check if the socket is connected.  */
    if (nx_wifi_socket[entry_index].nx_wifi_socket_connected == 0)
    {
      
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    }
    
    /* Close connection.  */
    WIFI_CloseClientConnection(entry_index);

    /* Reset the entry.  */
    nx_wifi_socket_reset(entry_index);
        
    /* Release the IP internal mutex before processing the IP event.  */
    tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
    
    /* Return success.  */
    return(NX_SUCCESS);
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_udp_socket_send                             PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function sends UDP packet.                                     */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    socket_ptr                            Pointer to UDP socket         */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  nx_wifi_udp_socket_send(NX_UDP_SOCKET *socket_ptr, NX_PACKET *packet_ptr, 
                              NXD_ADDRESS *ip_address, UINT port)
{

UINT        status ;
UCHAR       entry_index;
USHORT      send_data_length;
ULONG       packet_size;
NX_PACKET   *current_packet;

    /* Obtain the IP internal mutex before processing the IP event.  */
    tx_mutex_get(&(nx_wifi_ip -> nx_ip_protection), TX_WAIT_FOREVER);
    
    /* Find an avaiable entry.  */
    if (nx_wifi_socket_entry_find((void *)socket_ptr, &entry_index, 1))
    {
      
        /* Release the IP internal mutex before processing the IP event.  */
        tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
        return(NX_NOT_SUCCESSFUL);
    }
    
    /* Check if already open the connection.  */
    if (nx_wifi_socket[entry_index].nx_wifi_socket_connected == 0)
    {        
        
        /* Swap the address.  */
        NX_CHANGE_ULONG_ENDIAN(ip_address -> nxd_ip_address.v4);

        /* Open connection.  */
        status= WIFI_OpenClientConnection(entry_index , WIFI_UDP_PROTOCOL, "", (unsigned char* )(&(ip_address -> nxd_ip_address.v4)), port, socket_ptr -> nx_udp_socket_port) ;

        /* Swap the address.  */
        NX_CHANGE_ULONG_ENDIAN(ip_address -> nxd_ip_address.v4);

        /* Check status.  */
        if(status)
        {
          
            /* Reset the entry.  */
            nx_wifi_socket_reset(entry_index);
        
            /* Release the IP internal mutex before processing the IP event.  */
            tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
            return(NX_NOT_SUCCESSFUL);
        }

        /* Update the connect flag.  */
        nx_wifi_socket[entry_index].nx_wifi_socket_connected = 1;

#ifdef NX_ENABLE_IP_PACKET_FILTER

        /* Set IP and port.  */
        nx_wifi_socket[entry_index].nx_wifi_udp_socket_connect_ip = ip_address -> nxd_ip_address.v4;
        nx_wifi_socket[entry_index].nx_wifi_udp_socket_connect_port = port;
#endif /* NX_ENABLE_IP_PACKET_FILTER */
    }
        
    /* Initialize the current packet to the input packet pointer.  */
    current_packet =  packet_ptr;
    
    /* Loop to send the packet.  */
    while(current_packet)
    {
      
        /* Calculate current packet size. */
        packet_size = (ULONG)(current_packet -> nx_packet_append_ptr - current_packet -> nx_packet_prepend_ptr);
      
        /* Loop to send data.  */
        status = WIFI_SendData(entry_index, (uint8_t *)current_packet-> nx_packet_prepend_ptr, packet_size, &send_data_length, WIFI_WRITE_TIMEOUT); 

        /* Check status.  */
        if ((status != WIFI_STATUS_OK) || (send_data_length != packet_size))
        {
          
            /* Release the IP internal mutex before processing the IP event.  */
            tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
            return(NX_NOT_SUCCESSFUL);
        }

#ifdef NX_ENABLE_IP_PACKET_FILTER

        /* Process packet filter.  */
        nx_wifi_ip_packet_filter(nx_wifi_ip_address,
                                 nx_wifi_socket[entry_index].nx_wifi_udp_socket_connect_ip,
                                 socket_ptr -> nx_udp_socket_port,
                                 nx_wifi_socket[entry_index].nx_wifi_udp_socket_connect_port,
                                 NX_IP_UDP, packet_size, NX_IP_PACKET_OUT);
#endif /* NX_ENABLE_IP_PACKET_FILTER */

#ifndef NX_DISABLE_PACKET_CHAIN
        /* We have crossed the packet boundary.  Move to the next packet
           structure.  */
        current_packet =  current_packet -> nx_packet_next;
#else

        /* End the loop.  */
        current_packet = NX_NULL;
#endif /* NX_DISABLE_PACKET_CHAIN */
    }
        
    /* Release the packet.  */
    nx_packet_release(packet_ptr);
    
    /* Release the IP internal mutex before processing the IP event.  */
    tx_mutex_put(&(nx_wifi_ip -> nx_ip_protection));
    return(NX_SUCCESS);
} 


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_udp_socket_receive                          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function receives UDP packet.                                  */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    socket_ptr                            Pointer to UDP socket         */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  nx_wifi_udp_socket_receive(NX_UDP_SOCKET *socket_ptr, NX_PACKET **packet_ptr, ULONG wait_option)
{
    return(nx_wifi_socket_receive((VOID*)socket_ptr, packet_ptr, wait_option, NX_WIFI_UDP_SOCKET));
} 


#ifdef NX_ENABLE_IP_PACKET_FILTER
/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    nx_wifi_ip_packet_filter                            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function builds fake IP header and TCP/UDP header to filter.   */
/*                                                                        */ 
/*    Note: Only fill source IP, destination IP and Protocol in IP header */ 
/*    and fill source port and destination port in TCP/UDP header.        */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */
/*    source_ip                             Source IP address             */
/*    destination_ip                        Destination IP address        */
/*    source_port                           Source port                   */
/*    destination_port                      Destination port              */
/*    protocol                              Protocol: TCP/UDP             */
/*    packet_length                         Lenght of packet              */
/*    direction                             Direction: IN/OUT             */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Yuxin Zhou               Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
static UINT  nx_wifi_ip_packet_filter(ULONG source_ip, ULONG destination_ip,
                                      ULONG source_port, ULONG destination_port,
                                      ULONG protocol, ULONG packet_length, UINT direction)
{

NX_IPV4_HEADER *ip_header_ptr;
ULONG port;
UINT status;

    /* Check if the IP packet filter is set. */
    if (nx_wifi_ip -> nx_ip_packet_filter)
    {

        /* Initialize the buffer.  */
        memset(nx_wifi_ip_buffer, 0, 40);

        /* Update packet length.  */
        if (protocol == NX_IP_TCP)
        {
            packet_length += 40;
        }
        else
        {
            packet_length += 28;
        }

        /* Fill IP header.  */
        ip_header_ptr = (NX_IPV4_HEADER *) nx_wifi_ip_buffer;

        /* Build the first 32-bit word of the IP header.  */
        ip_header_ptr -> nx_ip_header_word_0 = (NX_IP_VERSION | (0xFFFF & packet_length));
                                                        
        /* Build the third 32-bit word of the IP header.  */
        ip_header_ptr -> nx_ip_header_word_2 = protocol;

        /* Place the source IP address in the IP header.  */
        ip_header_ptr -> nx_ip_header_source_ip = source_ip;

        /* Place the destination IP address in the IP header.  */
        ip_header_ptr -> nx_ip_header_destination_ip = destination_ip;

        NX_CHANGE_ULONG_ENDIAN(ip_header_ptr -> nx_ip_header_word_0);
        NX_CHANGE_ULONG_ENDIAN(ip_header_ptr -> nx_ip_header_word_2);
        NX_CHANGE_ULONG_ENDIAN(ip_header_ptr -> nx_ip_header_source_ip);
        NX_CHANGE_ULONG_ENDIAN(ip_header_ptr -> nx_ip_header_destination_ip);

        /* Fill the TCP/UDP ports.  */
        port = (source_port << NX_SHIFT_BY_16) | (destination_port);
        NX_CHANGE_ULONG_ENDIAN(port);
        memcpy(&nx_wifi_ip_buffer[20], &port, 4); /* Use case of memcpy is verified.  */

        /* Yes, call the IP packet filter routine. */
        status = nx_wifi_ip -> nx_ip_packet_filter(nx_wifi_ip_buffer, direction);

        if (status)
        {
            return (status);
        }
    }

    return(NX_SUCCESS);
}
#endif /* NX_ENABLE_IP_PACKET_FILTER */
