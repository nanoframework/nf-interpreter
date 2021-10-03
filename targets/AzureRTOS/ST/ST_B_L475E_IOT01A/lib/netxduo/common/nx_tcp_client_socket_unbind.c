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
/**   Transmission Control Protocol (TCP) for STM32L4XX                   */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define NX_SOURCE_CODE


/* Include necessary system files.  */

#include "nx_api.h"
#include "nx_tcp.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_tcp_client_socket_unbind                        PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function unbinds the TCP client socket structure from the      */
/*    previously bound TCP port.                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    socket_ptr                            Pointer to TCP client socket  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_tcp_client_bind_cleanup           Remove and cleanup bind req   */
/*    _nx_tcp_socket_thread_resume          Resume thread suspended on    */
/*                                            port                        */
/*    _nx_tcp_socket_block_cleanup          Cleanup the socket block      */
/*    tx_mutex_get                          Obtain protection mutex       */
/*    tx_mutex_put                          Release protection mutex      */
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
UINT  _nx_tcp_client_socket_unbind(NX_TCP_SOCKET *socket_ptr)
{

TX_INTERRUPT_SAVE_AREA

UINT           index;
UINT           port;
NX_IP         *ip_ptr;


    /* Setup the pointer to the associated IP instance.  */
    ip_ptr =  socket_ptr -> nx_tcp_socket_ip_ptr;

    /* If trace is enabled, insert this event into the trace buffer.  */
    NX_TRACE_IN_LINE_INSERT(NX_TRACE_TCP_CLIENT_SOCKET_UNBIND, ip_ptr, socket_ptr, 0, 0, NX_TRACE_TCP_EVENTS, 0, 0);

    /* Obtain the IP mutex so we can figure out whether or not the port has already
       been bound to.  */
    tx_mutex_get(&(ip_ptr -> nx_ip_protection), TX_WAIT_FOREVER);

    /* Cleanup the transmission control block.  */
    _nx_tcp_socket_block_cleanup(socket_ptr);

    /* Determine if the socket is still in the closed state.  */
    if (socket_ptr -> nx_tcp_socket_state != NX_TCP_CLOSED)
    {

        /* No, release the IP protection.  */
        tx_mutex_put(&(ip_ptr -> nx_ip_protection));

        /* Return an error code.  */
        return(NX_NOT_CLOSED);
    }

    /* Determine if the socket is bound to port.  */
    if (!socket_ptr -> nx_tcp_socket_bound_next)
    {

        /* Determine if there is a special condition for the socket not being in
           a bound condition...  i.e. the socket is in a pending-to-be-bound condition
           in a call from a different thread.  */
        if (socket_ptr -> nx_tcp_socket_bind_in_progress)
        {

            /* Release the protection mutex.  */
            tx_mutex_put(&(ip_ptr -> nx_ip_protection));

            /* Return success.  */
            return(NX_SUCCESS);
        }
        else
        {

            /* Release the protection mutex.  */
            tx_mutex_put(&(ip_ptr -> nx_ip_protection));

            /* Return a not bound error code.  */
            return(NX_NOT_BOUND);
        }
    }

    /* Otherwise, the socket is bound.  We need to remove this socket from the
       port and check for any other TCP socket bind requests that are queued.  */

    /* Pickup the port number in the TCP socket structure.  */
    port =  socket_ptr -> nx_tcp_socket_port;

    /* Calculate the hash index in the TCP port array of the associated IP instance.  */
    index =  (UINT)((port + (port >> 8)) & NX_TCP_PORT_TABLE_MASK);

    /* Disable interrupts while we unlink the current socket.  */
    TX_DISABLE

    /* Determine if this is the only socket bound on this port list.  */
    if (socket_ptr -> nx_tcp_socket_bound_next == socket_ptr)
    {

        /* Yes, this is the only socket on the port list.  */

        /* Clear the list head pointer and the next pointer in the socket.  */
        ip_ptr -> nx_ip_tcp_port_table[index] =   NX_NULL;
        socket_ptr -> nx_tcp_socket_bound_next =  NX_NULL;
    }
    else
    {

        /* Relink the neighbors of this TCP socket.  */

        /* Update the links of the adjacent sockets.  */
        (socket_ptr -> nx_tcp_socket_bound_next) -> nx_tcp_socket_bound_previous =
            socket_ptr -> nx_tcp_socket_bound_previous;
        (socket_ptr -> nx_tcp_socket_bound_previous) -> nx_tcp_socket_bound_next =
            socket_ptr -> nx_tcp_socket_bound_next;

        /* Determine if the head of the port list points to the socket being removed.
           If so, we need to move the head pointer.  */
        if (ip_ptr -> nx_ip_tcp_port_table[index] == socket_ptr)
        {

            /* Yes, we need to move the port list head pointer.  */
            ip_ptr -> nx_ip_tcp_port_table[index] =  socket_ptr -> nx_tcp_socket_bound_next;
        }

        /* Clear the next pointer in the socket to indicate it is no longer bound.  */
        socket_ptr -> nx_tcp_socket_bound_next =  NX_NULL;
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Release the protection mutex.  */
    tx_mutex_put(&(ip_ptr -> nx_ip_protection));

    /* Return success.  */
    return(NX_SUCCESS);  
}

