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
#include "nx_ip.h"
#include "nx_wifi.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nxd_udp_socket_send                                PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sends a UDP packet through the specified socket       */
/*    with the input IP address and port.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    socket_ptr                            Pointer to UDP socket         */
/*    packet_ptr                            Pointer to UDP packet         */
/*    ip_address                            IP address                    */
/*    port                                  16-bit UDP port number        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*    NX_IPSEC_REJECTED                     Failed IPSec check            */
/*    NX_NOT_BOUND                          Socket not bound to a port    */
/*    NX_NO_INTERFACE_ADDRESS               Socket interface not marked   */
/*                                             valid                      */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_ip_packet_send                    Send UDP packet over IPv4     */
/*    _nx_ipv6_packet_send                  Send UDP packet over IPv6     */
/*    nx_ip_checksum_compute                Compute UDP header checksum   */
/*    tx_mutex_get                          Get protection mutex          */
/*    tx_mutex_put                          Put protection mutex          */
/*    _nxd_ipv6_interface_find              Find interface for input      */
/*                                             address in IP address table*/
/*    [_nx_packet_egress_sa_lookup]         IPsec process                 */
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
UINT  _nxd_udp_socket_send(NX_UDP_SOCKET *socket_ptr,
                           NX_PACKET     *packet_ptr,
                           NXD_ADDRESS   *ip_address,
                           UINT           port)
{
  
    /* Call wifi send.  */
    return(nx_wifi_udp_socket_send(socket_ptr, packet_ptr, ip_address, port));
}

