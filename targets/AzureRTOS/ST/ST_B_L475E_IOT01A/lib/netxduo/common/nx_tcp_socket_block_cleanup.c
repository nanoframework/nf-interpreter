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
#include "nx_ipv6.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_tcp_socket_block_cleanup                        PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function cleans up the transmission control block.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    socket_ptr                            Pointer to owning socket      */
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
/*    _nx_tcp_fast_periodic_processing      Process TCP packet for socket */
/*    _nx_tcp_socket_connection_reset       Reset TCP connection          */
/*    _nx_tcp_socket_disconnect             Close TCP conenction          */
/*    _nx_tcp_socket_state_last_ack         Process data on LAST ACK state*/
/*    _nx_tcp_client_socket_unbind          Ubind the TCP client socket   */
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
VOID  _nx_tcp_socket_block_cleanup(NX_TCP_SOCKET *socket_ptr)
{

    /* Clean up the connect IP address.  */

    socket_ptr -> nx_tcp_socket_connect_ip.nxd_ip_version = 0;
    socket_ptr -> nx_tcp_socket_connect_ip.nxd_ip_address.v4 = 0;

    /* Clean up the connect port.  */
    socket_ptr -> nx_tcp_socket_connect_port = 0;

    /* Reset zero window probe flag. */
    socket_ptr -> nx_tcp_socket_zero_window_probe_has_data = NX_FALSE;

    /* Simply clear the timeout.  */
    socket_ptr -> nx_tcp_socket_timeout = 0;

    /* Reset duplicated ack received. */
    socket_ptr -> nx_tcp_socket_duplicated_ack_received = 0;

    /* Reset fast recovery stage. */
    socket_ptr -> nx_tcp_socket_fast_recovery = NX_FALSE;

    /* Connection needs to be closed down immediately.  */
    if (socket_ptr -> nx_tcp_socket_client_type)
    {

        /* If trace is enabled, insert this event into the trace buffer.  */
        NX_TRACE_IN_LINE_INSERT(NX_TRACE_INTERNAL_TCP_STATE_CHANGE, socket_ptr -> nx_tcp_socket_ip_ptr, socket_ptr, socket_ptr -> nx_tcp_socket_state, NX_TCP_CLOSED, NX_TRACE_INTERNAL_EVENTS, 0, 0);

        /* Client socket, return to a CLOSED state.  */
        socket_ptr -> nx_tcp_socket_state =  NX_TCP_CLOSED;
    }
    else
    {

        /* If trace is enabled, insert this event into the trace buffer.  */
        NX_TRACE_IN_LINE_INSERT(NX_TRACE_INTERNAL_TCP_STATE_CHANGE, socket_ptr -> nx_tcp_socket_ip_ptr, socket_ptr, socket_ptr -> nx_tcp_socket_state, NX_TCP_LISTEN_STATE, NX_TRACE_INTERNAL_EVENTS, 0, 0);

        /* Server socket, return to LISTEN state.  */
        socket_ptr -> nx_tcp_socket_state =  NX_TCP_LISTEN_STATE;
    }
}

