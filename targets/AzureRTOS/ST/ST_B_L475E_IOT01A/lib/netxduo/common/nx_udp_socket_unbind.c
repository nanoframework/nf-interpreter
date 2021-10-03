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
/**   User Datagram Protocol (UDP) for STM32L4XX                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define NX_SOURCE_CODE


/* Include necessary system files.  */

#include "nx_api.h"
#include "nx_udp.h"
#include "nx_packet.h"
#include "tx_thread.h"
#include "nx_wifi.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_udp_socket_unbind                               PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function unbinds the UDP socket structure from the previously  */
/*    bound UDP port.                                                     */
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
/*    _nx_packet_release                    Release data packet           */
/*    _nx_udp_bind_cleanup                  Remove and cleanup bind req   */
/*    tx_mutex_get                          Obtain protection mutex       */
/*    tx_mutex_put                          Release protection mutex      */
/*    _tx_thread_system_resume              Resume suspended thread       */
/*    _tx_thread_system_preempt_check       Check for preemption          */
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
UINT  _nx_udp_socket_unbind(NX_UDP_SOCKET *socket_ptr)
{

UINT           index;
UINT           port;
NX_IP         *ip_ptr;


    /* Setup the pointer to the associated IP instance.  */
    ip_ptr =  socket_ptr -> nx_udp_socket_ip_ptr;

    /* If trace is enabled, insert this event into the trace buffer.  */
    NX_TRACE_IN_LINE_INSERT(NX_TRACE_UDP_SOCKET_UNBIND, ip_ptr, socket_ptr, socket_ptr -> nx_udp_socket_port, 0, NX_TRACE_UDP_EVENTS, 0, 0);

    /* Obtain the IP mutex so we can figure out whether or not the port has already
       been bound to.  */
    tx_mutex_get(&(ip_ptr -> nx_ip_protection), TX_WAIT_FOREVER);

    /* Determine if the socket is bound to port.  */
    if (!socket_ptr -> nx_udp_socket_bound_next)
    {

        /* Release the protection mutex.  */
        tx_mutex_put(&(ip_ptr -> nx_ip_protection));

        /* Return a not bound error code.  */
        return(NX_NOT_BOUND);
    }

    /* Otherwise, the socket is bound.  We need to remove this socket from the
       port and check for any other UDP socket bind requests that are queued.  */

    /* Pickup the port number in the UDP socket structure.  */
    port =  socket_ptr -> nx_udp_socket_port;

    /* Calculate the hash index in the UDP port array of the associated IP instance.  */
    index =  (UINT)((port + (port >> 8)) & NX_UDP_PORT_TABLE_MASK);

    /* Determine if this is the only socket bound on this port list.  */
    if (socket_ptr -> nx_udp_socket_bound_next == socket_ptr)
    {

        /* Yes, this is the only socket on the port list.  */

        /* Clear the list head pointer and the next pointer in the socket.  */
        ip_ptr -> nx_ip_udp_port_table[index] =   NX_NULL;
        socket_ptr -> nx_udp_socket_bound_next =  NX_NULL;
    }
    else
    {

        /* Relink the neighbors of this UDP socket.  */

        /* Update the links of the adjacent sockets.  */
        (socket_ptr -> nx_udp_socket_bound_next) -> nx_udp_socket_bound_previous =
            socket_ptr -> nx_udp_socket_bound_previous;
        (socket_ptr -> nx_udp_socket_bound_previous) -> nx_udp_socket_bound_next =
            socket_ptr -> nx_udp_socket_bound_next;

        /* Determine if the head of the port list points to the socket being removed.
           If so, we need to move the head pointer.  */
        if (ip_ptr -> nx_ip_udp_port_table[index] == socket_ptr)
        {

            /* Yes, we need to move the port list head pointer.  */
            ip_ptr -> nx_ip_udp_port_table[index] =  socket_ptr -> nx_udp_socket_bound_next;
        }

        /* Clear the next pointer in the socket to indicate it is no longer bound.  */
        socket_ptr -> nx_udp_socket_bound_next =  NX_NULL;
    }
    
    /* Release the mutex protection.  */
    tx_mutex_put(&(ip_ptr -> nx_ip_protection));
    
    /* Call wifi unbind.  */
    nx_wifi_udp_socket_unbind(socket_ptr);

    /* Return success.  */
    return(NX_SUCCESS);
}

