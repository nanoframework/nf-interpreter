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
#ifndef _NX_WIFI_H_
#define _NX_WIFI_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "nx_api.h"
   
#define WIFI_RECEIVE_QUEUE_SIZE  10

UINT nx_wifi_initialize(NX_IP *ip_ptr, NX_PACKET_POOL *packet_pool);
UINT  nx_wifi_tcp_client_socket_connect(NX_TCP_SOCKET *socket_ptr,
                                        NXD_ADDRESS *server_ip,
                                        UINT server_port,
                                        ULONG wait_option);
UINT  nx_wifi_tcp_socket_disconnect(NX_TCP_SOCKET *socket_ptr, ULONG wait_option);
UINT  nx_wifi_tcp_socket_send(NX_TCP_SOCKET *socket_ptr, NX_PACKET *packet_ptr, ULONG wait_option);
UINT  nx_wifi_tcp_socket_receive(NX_TCP_SOCKET *socket_ptr, NX_PACKET **packet_ptr, ULONG wait_option);
UINT  nx_wifi_udp_socket_bind(NX_UDP_SOCKET *socket_ptr, UINT  port, ULONG wait_option);
UINT  nx_wifi_udp_socket_unbind(NX_UDP_SOCKET *socket_ptr);
UINT  nx_wifi_udp_socket_send(NX_UDP_SOCKET *socket_ptr, NX_PACKET *packet_ptr, 
                              NXD_ADDRESS *ip_address, UINT port);
UINT  nx_wifi_udp_socket_receive(NX_UDP_SOCKET *socket_ptr, NX_PACKET **packet_ptr, ULONG wait_option);

#ifdef __cplusplus
}
#endif

#endif /* _WIFI_H_ */


