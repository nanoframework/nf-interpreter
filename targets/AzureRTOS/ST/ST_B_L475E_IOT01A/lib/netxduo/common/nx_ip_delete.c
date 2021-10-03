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
/**   Internet Protocol (IP) for STM32L4XX                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define NX_SOURCE_CODE


/* Include necessary system files.  */

#include "nx_api.h"
#include "tx_thread.h"
#include "nx_ip.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_ip_delete                                       PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes an Internet Protocol instance, including      */
/*    calling the associated driver with a link disable request.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ip_ptr                                Pointer to IP control block   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_ip_delete_queue_clear             Clear a packet queue          */
/*    _nx_ip_raw_packet_cleanup             Cleanup raw packet suspension */
/*    _nx_icmp_cleanup                      Cleanup for ICMP packets      */
/*    _nx_ip_fragment_disable               Disable fragment processing   */
/*    tx_mutex_delete                       Delete IP protection mutex    */
/*    tx_mutex_get                          Get protection mutex          */
/*    tx_mutex_put                          Put protection mutex          */
/*    tx_thread_terminate                   Terminate IP helper thread    */
/*    tx_event_flags_delete                 Delete IP event flags         */
/*    tx_thread_delete                      Delete IP helper thread       */
/*    _tx_thread_system_preempt_check       Check for preemption          */
/*    tx_timer_deactivate                   Deactivate IP-ARP timer       */
/*    tx_timer_delete                       Delete IP-ARP timer           */
/*    (ip_link_driver)                      User supplied link driver     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
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
UINT  _nx_ip_delete(NX_IP *ip_ptr)
{

TX_INTERRUPT_SAVE_AREA

    /* If trace is enabled, insert this event into the trace buffer.  */
    NX_TRACE_IN_LINE_INSERT(NX_TRACE_IP_DELETE, ip_ptr, 0, 0, 0, NX_TRACE_IP_EVENTS, 0, 0);

    /* Get mutex protection.  */
    tx_mutex_get(&(ip_ptr -> nx_ip_protection), TX_WAIT_FOREVER);

    /* Determine if the IP instance has any sockets bound to it.  */
    if ((ip_ptr -> nx_ip_udp_created_sockets_count) || (ip_ptr -> nx_ip_tcp_created_sockets_count))
    {

        /* Still sockets bound to this IP instance.  They must all be deleted prior
           to deleting the IP instance.  Release the mutex and return
           an error code.  */
        tx_mutex_put(&(ip_ptr -> nx_ip_protection));

        return(NX_SOCKETS_BOUND);
    }

    /* Disable interrupts.  */
    TX_DISABLE
    
    /* Remove the IP instance from the created list.  */

    /* See if the IP instance is the only one on the list.  */
    if (ip_ptr == ip_ptr -> nx_ip_created_next)
    {

        /* Only created IP instance, just set the created list to NULL.  */
        _nx_ip_created_ptr =  TX_NULL;
    }
    else
    {

        /* Otherwise, not the only created IP, link-up the neighbors.  */
        (ip_ptr -> nx_ip_created_next) -> nx_ip_created_previous =
            ip_ptr -> nx_ip_created_previous;
        (ip_ptr -> nx_ip_created_previous) -> nx_ip_created_next =
            ip_ptr -> nx_ip_created_next;

        /* See if we have to update the created list head pointer.  */
        if (_nx_ip_created_ptr == ip_ptr)
        {

            /* Yes, move the head pointer to the next link. */
            _nx_ip_created_ptr =  ip_ptr -> nx_ip_created_next;
        }
    }

    /* Decrement the IP created counter.  */
    _nx_ip_created_count--;

    /* Temporarily disable preemption.  */
    _tx_thread_preempt_disable++;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Release mutex protection.  */
    tx_mutex_put(&(ip_ptr -> nx_ip_protection));

    /* Delete the internal IP protection mutex.  */
    tx_mutex_delete(&(ip_ptr -> nx_ip_protection));

    /* Clear the IP ID to make it invalid.  */
    ip_ptr -> nx_ip_id =  0;

    /* Disable interrupts.  */
    TX_DISABLE

    /* Restore preemption.  */
    _tx_thread_preempt_disable--;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Check for preemption.  */
    _tx_thread_system_preempt_check();

    /* Return success to the caller.  */
    return(NX_SUCCESS);
}

