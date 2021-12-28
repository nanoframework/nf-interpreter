//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2001-2004 Swedish Institute of Computer Science, All Rights Reserved
// See LICENSE file in the project root for full license information.
//
/**
 * @file
 *
 * lwIP Options Configuration
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef LWIPOPTS_H
#define LWIPOPTS_H

#define _REENT_ONLY
#define set_errno(err)
#include <sys/errno.h>
#include <target_board.h>
#include <stdlib.h>

// empty on purpose
#define LWIP_TCPIP_THREAD_ALIVE()

/*
   ------------------------------------
   ---------- Memory options ----------
   ------------------------------------
*/
/**
 * MEM_LIBC_MALLOC==1: Use malloc/free/realloc provided by your C-library
 * instead of the lwip internal allocator. Can save code size if you
 * already use it.
 */
#ifndef MEM_LIBC_MALLOC
#define MEM_LIBC_MALLOC 1
#endif

/**
 * MEMP_MEM_MALLOC==1: Use mem_malloc/mem_free instead of the lwip pool allocator.
 * Especially useful with MEM_LIBC_MALLOC but handle with care regarding execution
 * speed (heap alloc can be much slower than pool alloc) and usage from interrupts
 * (especially if your netif driver allocates PBUF_POOL pbufs for received frames
 * from interrupt)!
 * ATTENTION: Currently, this uses the heap for ALL pools (also for private pools,
 * not only for internal pools defined in memp_std.h)!
 */
#ifndef MEMP_MEM_MALLOC
#define MEMP_MEM_MALLOC 1
#endif

/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#ifndef MEM_ALIGNMENT
#define MEM_ALIGNMENT 4
#endif

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 */
#ifndef MEM_SIZE
#define MEM_SIZE (16 * 1024)
#endif
/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/
/**
 * @defgroup lwip_opts_checksum Checksum
 * @ingroup lwip_opts_infrastructure
 * @{
 */
/**
 * LWIP_CHECKSUM_CTRL_PER_NETIF==1: Checksum generation/check can be enabled/disabled
 * per netif.
 * ATTENTION: if enabled, the CHECKSUM_GEN_* and CHECKSUM_CHECK_* defines must be enabled!
 */
#if !defined LWIP_CHECKSUM_CTRL_PER_NETIF
#define LWIP_CHECKSUM_CTRL_PER_NETIF 0
#endif

/**
 * CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.
 */
#if !defined CHECKSUM_GEN_IP
#define CHECKSUM_GEN_IP 0
#endif

/**
 * CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.
 */
#if !defined CHECKSUM_GEN_UDP
#define CHECKSUM_GEN_UDP 0
#endif

/**
 * CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.
 */
#if !defined CHECKSUM_GEN_TCP
#define CHECKSUM_GEN_TCP 0
#endif

/**
 * CHECKSUM_GEN_ICMP==1: Generate checksums in software for outgoing ICMP packets.
 */
#if !defined CHECKSUM_GEN_ICMP
#define CHECKSUM_GEN_ICMP 0
#endif

/**
 * CHECKSUM_GEN_ICMP6==1: Generate checksums in software for outgoing ICMP6 packets.
 */
#if !defined CHECKSUM_GEN_ICMP6
#define CHECKSUM_GEN_ICMP6 0
#endif

/**
 * CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.
 */
#if !defined CHECKSUM_CHECK_IP
#define CHECKSUM_CHECK_IP 0
#endif

/**
 * CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.
 */
#if !defined CHECKSUM_CHECK_UDP
#define CHECKSUM_CHECK_UDP 0
#endif

/**
 * CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.
 */
#if !defined CHECKSUM_CHECK_TCP
#define CHECKSUM_CHECK_TCP 0
#endif

/**
 * CHECKSUM_CHECK_ICMP==1: Check checksums in software for incoming ICMP packets.
 */
#if !defined CHECKSUM_CHECK_ICMP
#define CHECKSUM_CHECK_ICMP 0
#endif

/**
 * CHECKSUM_CHECK_ICMP6==1: Check checksums in software for incoming ICMPv6 packets
 */
#if !defined CHECKSUM_CHECK_ICMP6
#define CHECKSUM_CHECK_ICMP6 0
#endif

/**
 * LWIP_CHECKSUM_ON_COPY==1: Calculate checksum when copying data from
 * application buffers to pbufs.
 */
#if !defined LWIP_CHECKSUM_ON_COPY
#define LWIP_CHECKSUM_ON_COPY 0
#endif
/**
 * @}
 */

// /*
//    ---------------------------------------
//    ---------- IPv6 options ---------------
//    ---------------------------------------
// */
// /**
//  * LWIP_IPV6==1: Enable IPv6
//  */
// #define LWIP_IPV6 0

/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/
/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high.
 */
#ifndef MEMP_NUM_PBUF
#define MEMP_NUM_PBUF 16
#endif

/**
 * MEMP_NUM_RAW_PCB: Number of raw connection PCBs
 * (requires the LWIP_RAW option)
 */
#ifndef MEMP_NUM_RAW_PCB
#define MEMP_NUM_RAW_PCB 4
#endif

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#ifndef MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB 4
#endif

/**
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#ifndef MEMP_NUM_TCP_PCB
#define MEMP_NUM_TCP_PCB 5
#endif

/**
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 * (requires the LWIP_TCP option)
 */
#ifndef MEMP_NUM_TCP_PCB_LISTEN
#define MEMP_NUM_TCP_PCB_LISTEN 8
#endif

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 */
#ifndef MEMP_NUM_TCP_SEG
#define MEMP_NUM_TCP_SEG 16
#endif

/**
 * MEMP_NUM_REASSDATA: the number of IP packets simultaneously queued for
 * reassembly (whole packets, not fragments!)
 */
#ifndef MEMP_NUM_REASSDATA
#define MEMP_NUM_REASSDATA 5
#endif

/**
 * MEMP_NUM_FRAG_PBUF: the number of IP fragments simultaneously sent
 * (fragments, not whole packets!).
 * This is only used with IP_FRAG_USES_STATIC_BUF==0 and
 * LWIP_NETIF_TX_SINGLE_PBUF==0 and only has to be > 1 with DMA-enabled MACs
 * where the packet is not yet sent when netif->output returns.
 */
#ifndef MEMP_NUM_FRAG_PBUF
#define MEMP_NUM_FRAG_PBUF 15
#endif

/**
 * MEMP_NUM_ARP_QUEUE: the number of simulateously queued outgoing
 * packets (pbufs) that are waiting for an ARP request (to resolve
 * their destination address) to finish.
 * (requires the ARP_QUEUEING option)
 */
#ifndef MEMP_NUM_ARP_QUEUE
#define MEMP_NUM_ARP_QUEUE 30
#endif

/**
 * MEMP_NUM_IGMP_GROUP: The number of multicast groups whose network interfaces
 * can be members et the same time (one per netif - allsystems group -, plus one
 * per netif membership).
 * (requires the LWIP_IGMP option)
 */
#ifndef MEMP_NUM_IGMP_GROUP
#define MEMP_NUM_IGMP_GROUP 8
#endif

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simulateously active timeouts.
 * (requires NO_SYS==0)
 * The default number of timeouts is calculated here for all enabled modules.
 * The formula expects settings to be either '0' or '1'.
 */
#ifndef MEMP_NUM_SYS_TIMEOUT
#define MEMP_NUM_SYS_TIMEOUT                                                                                           \
    (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2 * LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + PPP_SUPPORT +        \
     (LWIP_IPV6 ? (1 + LWIP_IPV6_REASS + LWIP_IPV6_MLD) : 0))
#endif

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#ifndef MEMP_NUM_NETBUF
#define MEMP_NUM_NETBUF 2
#endif

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#ifndef MEMP_NUM_NETCONN
#define MEMP_NUM_NETCONN 4
#endif

/**
 * MEMP_NUM_TCPIP_MSG_API: the number of struct tcpip_msg, which are used
 * for callback/timeout API communication.
 * (only needed if you use tcpip.c)
 */
#ifndef MEMP_NUM_TCPIP_MSG_API
#define MEMP_NUM_TCPIP_MSG_API 8
#endif

/**
 * MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used
 * for incoming packets.
 * (only needed if you use tcpip.c)
 */
#ifndef MEMP_NUM_TCPIP_MSG_INPKT
#define MEMP_NUM_TCPIP_MSG_INPKT 8
#endif

/**
 * MEMP_NUM_SNMP_NODE: the number of leafs in the SNMP tree.
 */
#ifndef MEMP_NUM_SNMP_NODE
#define MEMP_NUM_SNMP_NODE 50
#endif

/**
 * MEMP_NUM_SNMP_ROOTNODE: the number of branches in the SNMP tree.
 * Every branch has one leaf (MEMP_NUM_SNMP_NODE) at least!
 */
#ifndef MEMP_NUM_SNMP_ROOTNODE
#define MEMP_NUM_SNMP_ROOTNODE 30
#endif

/**
 * MEMP_NUM_SNMP_VARBIND: the number of concurrent requests (does not have to
 * be changed normally) - 2 of these are used per request (1 for input,
 * 1 for output)
 */
#ifndef MEMP_NUM_SNMP_VARBIND
#define MEMP_NUM_SNMP_VARBIND 2
#endif

/**
 * MEMP_NUM_SNMP_VALUE: the number of OID or values concurrently used
 * (does not have to be changed normally) - 3 of these are used per request
 * (1 for the value read and 2 for OIDs - input and output)
 */
#ifndef MEMP_NUM_SNMP_VALUE
#define MEMP_NUM_SNMP_VALUE 3
#endif

/**
 * MEMP_NUM_NETDB: the number of concurrently running lwip_addrinfo() calls
 * (before freeing the corresponding memory using lwip_freeaddrinfo()).
 */
#ifndef MEMP_NUM_NETDB
#define MEMP_NUM_NETDB 1
#endif

/**
 * MEMP_NUM_LOCALHOSTLIST: the number of host entries in the local host list
 * if DNS_LOCAL_HOSTLIST_IS_DYNAMIC==1.
 */
#ifndef MEMP_NUM_LOCALHOSTLIST
#define MEMP_NUM_LOCALHOSTLIST 1
#endif

/**
 * MEMP_NUM_PPPOE_INTERFACES: the number of concurrently active PPPoE
 * interfaces (only used with PPPOE_SUPPORT==1)
 */
#ifndef MEMP_NUM_PPPOE_INTERFACES
#define MEMP_NUM_PPPOE_INTERFACES 1
#endif

/**
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool.
 */
#ifndef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE 16
#endif

/*
   ---------------------------------
   ---------- RAW options ----------
   ---------------------------------
*/
/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#if !defined LWIP_RAW
#define LWIP_RAW 0
#endif

/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/
/**
 * LWIP_DHCP==1: Enable DHCP module.
 */
#if !defined LWIP_DHCP
#define LWIP_DHCP 1
#endif

/*
   ------------------------------------
   ---------- AUTOIP options ----------
   ------------------------------------
*/
/**
 * LWIP_AUTOIP==1: Enable AUTOIP module.
 */
#ifndef LWIP_AUTOIP
#define LWIP_AUTOIP 0
#endif

/**
 * LWIP_DHCP_AUTOIP_COOP==1: Allow DHCP and AUTOIP to be both enabled on
 * the same interface at the same time.
 */
#ifndef LWIP_DHCP_AUTOIP_COOP
#define LWIP_DHCP_AUTOIP_COOP 0
#endif

/*
   ----------------------------------
   ---------- DNS options -----------
   ----------------------------------
*/
/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#ifndef LWIP_DNS
#define LWIP_DNS 1
#endif

/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
*/

/**
 * TCP_MSS: TCP Maximum segment size. (default is 536, a conservative default,
 * you might want to increase this.)
 * For the receive side, this MSS is advertised to the remote side
 * when opening a connection. For the transmit size, this MSS sets
 * an upper limit on the MSS advertised by the remote host.
 */
#ifndef TCP_MSS
#define TCP_MSS 1480
#endif

/**
 * TCP_LISTEN_BACKLOG: Enable the backlog option for tcp listen pcb.
 */
#ifndef TCP_LISTEN_BACKLOG
#define TCP_LISTEN_BACKLOG 1
#endif

/**
 * LWIP_EVENT_API and LWIP_CALLBACK_API: Only one of these should be set to 1.
 *     LWIP_EVENT_API==1: The user defines lwip_tcp_event() to receive all
 *         events (accept, sent, etc) that happen in the system.
 *     LWIP_CALLBACK_API==1: The PCB callback function is called directly
 *         for the event. This is the default.
 */
#if !defined(LWIP_EVENT_API) && !defined(LWIP_CALLBACK_API)
#define LWIP_EVENT_API    0
#define LWIP_CALLBACK_API 1
#endif

/** LWIP_ALTCP_TLS==1: enable TLS support for altcp API.
 * This needs a port of the functions in altcp_tls.h to a TLS library.
 * A port to ARM mbedtls is provided with lwIP, see apps/altcp_tls/ directory
 * and LWIP_ALTCP_TLS_MBEDTLS option.
 */
#if !defined LWIP_ALTCP_TLS
#define LWIP_ALTCP_TLS 0
#endif

/*
   ------------------------------------------------
   ---------- Network Interfaces options ----------
   ------------------------------------------------
*/

/**
 * LWIP_SINGLE_NETIF==1: use a single netif only. This is the common case for
 * small real-life targets. Some code like routing etc. can be left out.
 */
#if !defined LWIP_SINGLE_NETIF
#define LWIP_SINGLE_NETIF 1
#endif

/**
 * LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname
 * field.
 */
#ifndef LWIP_NETIF_HOSTNAME
#define LWIP_NETIF_HOSTNAME 1
#endif

/**
 * LWIP_NETIF_STATUS_CALLBACK==1: Support a callback function whenever an interface
 * changes its up/down status (i.e., due to DHCP IP acquistion)
 */
#ifndef LWIP_NETIF_STATUS_CALLBACK
#define LWIP_NETIF_STATUS_CALLBACK 1
#endif

/**
 * LWIP_NETIF_EXT_STATUS_CALLBACK==1: Support an extended callback function
 * for several netif related event that supports multiple subscribers.
 * @see netif_ext_status_callback
 */
#if !defined LWIP_NETIF_EXT_STATUS_CALLBACK
#define LWIP_NETIF_EXT_STATUS_CALLBACK 0
#endif

/**
 * LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
#ifndef LWIP_NETIF_LINK_CALLBACK
#define LWIP_NETIF_LINK_CALLBACK 1
#endif

/*
   ------------------------------------
   ---------- Thread options ----------
   ------------------------------------
*/
/**
 * TCPIP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef TCPIP_THREAD_STACKSIZE
#define TCPIP_THREAD_STACKSIZE 1024
#endif

/**
 * TCPIP_THREAD_PRIO: The priority assigned to the main tcpip thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef TCPIP_THREAD_PRIO
#define TCPIP_THREAD_PRIO (NORMALPRIO + 2)
#endif

/**
 * TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when tcpip_init is called.
 */
#ifndef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE MEMP_NUM_PBUF
#endif

/**
 * SLIP_THREAD_STACKSIZE: The stack size used by the slipif_loop thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef SLIPIF_THREAD_STACKSIZE
#define SLIPIF_THREAD_STACKSIZE 1024
#endif

/**
 * SLIPIF_THREAD_PRIO: The priority assigned to the slipif_loop thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef SLIPIF_THREAD_PRIO
#define SLIPIF_THREAD_PRIO (NORMALPRIO)
#endif

/**
 * PPP_THREAD_NAME: The name assigned to the pppInputThread.
 */
#ifndef PPP_THREAD_NAME
#define PPP_THREAD_NAME "pppInputThread"
#endif

/**
 * PPP_THREAD_STACKSIZE: The stack size used by the pppInputThread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef PPP_THREAD_STACKSIZE
#define PPP_THREAD_STACKSIZE 1024
#endif

/**
 * PPP_THREAD_PRIO: The priority assigned to the pppInputThread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef PPP_THREAD_PRIO
#define PPP_THREAD_PRIO (NORMALPRIO)
#endif

/**
 * DEFAULT_THREAD_NAME: The name assigned to any other lwIP thread.
 */
#ifndef DEFAULT_THREAD_NAME
#define DEFAULT_THREAD_NAME "lwIP"
#endif

/**
 * DEFAULT_THREAD_STACKSIZE: The stack size used by any other lwIP thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef DEFAULT_THREAD_STACKSIZE
#define DEFAULT_THREAD_STACKSIZE 1024
#endif

/**
 * DEFAULT_THREAD_PRIO: The priority assigned to any other lwIP thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef DEFAULT_THREAD_PRIO
#define DEFAULT_THREAD_PRIO (NORMALPRIO)
#endif

/**
 * DEFAULT_RAW_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_RAW. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#ifndef DEFAULT_RAW_RECVMBOX_SIZE
#define DEFAULT_RAW_RECVMBOX_SIZE 4
#endif

/**
 * DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_UDP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#ifndef DEFAULT_UDP_RECVMBOX_SIZE
#define DEFAULT_UDP_RECVMBOX_SIZE 4
#endif

/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#ifndef DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE 40
#endif

/**
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#ifndef DEFAULT_ACCEPTMBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE 4
#endif

/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/

/** LWIP_TCPIP_TIMEOUT==1: Enable tcpip_timeout/tcpip_untimeout tod create
 * timers running in tcpip_thread from another thread.
 */
#ifndef LWIP_TCPIP_TIMEOUT
#define LWIP_TCPIP_TIMEOUT 1
#endif

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/

/**
 * LWIP_TCP_KEEPALIVE==1: Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT
 * options processing. Note that TCP_KEEPIDLE and TCP_KEEPINTVL have to be set
 * in seconds. (does not require sockets.c, and will affect tcp.c)
 */
#ifndef LWIP_TCP_KEEPALIVE
#define LWIP_TCP_KEEPALIVE 0
#endif

/**
 * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
 * SO_SNDTIMEO processing.
 */
#ifndef LWIP_SO_SNDTIMEO
#define LWIP_SO_SNDTIMEO 1
#endif

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#ifndef LWIP_SO_RCVTIMEO
#define LWIP_SO_RCVTIMEO 1
#endif

/**
 * LWIP_SO_LINGER==1: Enable linger option for sockets/netconns
 */
#define LWIP_SO_LINGER 1

/**
 * LWIP_SO_RCVBUF==1: Enable SO_RCVBUF processing.
 */
#ifndef LWIP_SO_RCVBUF
#define LWIP_SO_RCVBUF 1
#endif

/**
 * SO_REUSE==1: Enable SO_REUSEADDR option.
 */
#ifndef SO_REUSE
#define SO_REUSE 1
#endif

/**
 * SO_REUSE_RXTOALL==1: Pass a copy of incoming broadcast/multicast packets
 * to all local matches if SO_REUSEADDR is turned on.
 * WARNING: Adds a memcpy for every packet if passing to more than one pcb!
 */
#ifndef SO_REUSE_RXTOALL
#define SO_REUSE_RXTOALL 1
#endif

/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/
/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#ifndef LWIP_STATS
#define LWIP_STATS 0
#endif

/*
   ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/
/**
 * LWIP_DBG_MIN_LEVEL: After masking, the value of the debug is
 * compared against this value. If it is smaller, then debugging
 * messages are written.
 */
#ifndef LWIP_DBG_MIN_LEVEL
#define LWIP_DBG_MIN_LEVEL LWIP_DBG_LEVEL_ALL
#endif

/**
 * LWIP_DBG_TYPES_ON: A mask that can be used to globally enable/disable
 * debug messages of certain types.
 */
#ifndef LWIP_DBG_TYPES_ON
#define LWIP_DBG_TYPES_ON LWIP_DBG_OFF
#endif

/**
 * ETHARP_DEBUG: Enable debugging in etharp.c.
 */
#ifndef ETHARP_DEBUG
#define ETHARP_DEBUG LWIP_DBG_OFF
#endif

/**
 * NETIF_DEBUG: Enable debugging in netif.c.
 */
#ifndef NETIF_DEBUG
#define NETIF_DEBUG LWIP_DBG_OFF
#endif

/**
 * PBUF_DEBUG: Enable debugging in pbuf.c.
 */
#ifndef PBUF_DEBUG
#define PBUF_DEBUG LWIP_DBG_OFF
#endif

/**
 * API_LIB_DEBUG: Enable debugging in api_lib.c.
 */
#ifndef API_LIB_DEBUG
#define API_LIB_DEBUG LWIP_DBG_OFF
#endif

/**
 * API_MSG_DEBUG: Enable debugging in api_msg.c.
 */
#ifndef API_MSG_DEBUG
#define API_MSG_DEBUG LWIP_DBG_OFF
#endif

/**
 * SOCKETS_DEBUG: Enable debugging in sockets.c.
 */
#ifndef SOCKETS_DEBUG
#define SOCKETS_DEBUG LWIP_DBG_OFF
#endif

/**
 * ICMP_DEBUG: Enable debugging in icmp.c.
 */
#ifndef ICMP_DEBUG
#define ICMP_DEBUG LWIP_DBG_OFF
#endif

/**
 * INET_DEBUG: Enable debugging in inet.c.
 */
#ifndef INET_DEBUG
#define INET_DEBUG LWIP_DBG_OFF
#endif

/**
 * IP_DEBUG: Enable debugging for IP.
 */
#ifndef IP_DEBUG
#define IP_DEBUG LWIP_DBG_OFF
#endif

/**
 * IP_REASS_DEBUG: Enable debugging in ip_frag.c for both frag & reass.
 */
#ifndef IP_REASS_DEBUG
#define IP_REASS_DEBUG LWIP_DBG_OFF
#endif

/**
 * RAW_DEBUG: Enable debugging in raw.c.
 */
#ifndef RAW_DEBUG
#define RAW_DEBUG LWIP_DBG_OFF
#endif

/**
 * MEM_DEBUG: Enable debugging in mem.c.
 */
#ifndef MEM_DEBUG
#define MEM_DEBUG LWIP_DBG_OFF
#endif

/**
 * SYS_DEBUG: Enable debugging in sys.c.
 */
#ifndef SYS_DEBUG
#define SYS_DEBUG LWIP_DBG_OFF
#endif

/**
 * TIMERS_DEBUG: Enable debugging in timers.c.
 */
#ifndef TIMERS_DEBUG
#define TIMERS_DEBUG LWIP_DBG_OFF
#endif

/**
 * TCP_DEBUG: Enable debugging for TCP.
 */
#ifndef TCP_DEBUG
#define TCP_DEBUG LWIP_DBG_OFF
#endif

/**
 * TCP_INPUT_DEBUG: Enable debugging in tcp_in.c for incoming debug.
 */
#ifndef TCP_INPUT_DEBUG
#define TCP_INPUT_DEBUG LWIP_DBG_OFF
#endif

/**
 * TCP_FR_DEBUG: Enable debugging in tcp_in.c for fast retransmit.
 */
#ifndef TCP_FR_DEBUG
#define TCP_FR_DEBUG LWIP_DBG_OFF
#endif

/**
 * TCP_RTO_DEBUG: Enable debugging in TCP for retransmit
 * timeout.
 */
#ifndef TCP_RTO_DEBUG
#define TCP_RTO_DEBUG LWIP_DBG_OFF
#endif

/**
 * TCP_CWND_DEBUG: Enable debugging for TCP congestion window.
 */
#ifndef TCP_CWND_DEBUG
#define TCP_CWND_DEBUG LWIP_DBG_OFF
#endif

/**
 * TCP_WND_DEBUG: Enable debugging in tcp_in.c for window updating.
 */
#ifndef TCP_WND_DEBUG
#define TCP_WND_DEBUG LWIP_DBG_OFF
#endif

/**
 * TCP_OUTPUT_DEBUG: Enable debugging in tcp_out.c output functions.
 */
#ifndef TCP_OUTPUT_DEBUG
#define TCP_OUTPUT_DEBUG LWIP_DBG_OFF
#endif

/**
 * TCP_RST_DEBUG: Enable debugging for TCP with the RST message.
 */
#ifndef TCP_RST_DEBUG
#define TCP_RST_DEBUG LWIP_DBG_OFF
#endif

/**
 * TCP_QLEN_DEBUG: Enable debugging for TCP queue lengths.
 */
#ifndef TCP_QLEN_DEBUG
#define TCP_QLEN_DEBUG LWIP_DBG_OFF
#endif

/**
 * UDP_DEBUG: Enable debugging in UDP.
 */
#ifndef UDP_DEBUG
#define UDP_DEBUG LWIP_DBG_OFF
#endif

/**
 * TCPIP_DEBUG: Enable debugging in tcpip.c.
 */
#ifndef TCPIP_DEBUG
#define TCPIP_DEBUG LWIP_DBG_OFF
#endif

/**
 * SLIP_DEBUG: Enable debugging in slipif.c.
 */
#ifndef SLIP_DEBUG
#define SLIP_DEBUG LWIP_DBG_OFF
#endif

/**
 * DHCP_DEBUG: Enable debugging in dhcp.c.
 */
#ifndef DHCP_DEBUG
#define DHCP_DEBUG LWIP_DBG_OFF
#endif

/**
 * AUTOIP_DEBUG: Enable debugging in autoip.c.
 */
#ifndef AUTOIP_DEBUG
#define AUTOIP_DEBUG LWIP_DBG_OFF
#endif

/**
 * DNS_DEBUG: Enable debugging for DNS.
 */
#if !defined DNS_DEBUG
#define DNS_DEBUG LWIP_DBG_OFF
#endif

/**
 * IP6_DEBUG: Enable debugging for IPv6.
 */
#if !defined IP6_DEBUG
#define IP6_DEBUG LWIP_DBG_OFF
#endif

/**
 * DHCP6_DEBUG: Enable debugging in dhcp6.c.
 */
#if !defined DHCP6_DEBUG
#define DHCP6_DEBUG LWIP_DBG_OFF
#endif
/**
 * @}
 */

/**
 * LWIP_TESTMODE: Changes to make unit test possible
 */
#if !defined LWIP_TESTMODE
#define LWIP_TESTMODE 0
#endif

// in order to enable the lwIP debug output the below has to be uncomment and
// the options above changed to LWIP_DBG_ON for the features that are to output debug information
// #define LWIP_DEBUG 1

// enable full duplex comms on multiple threads
// this requires adding an extra field in ChibiOS thread_t (see comment on platform_sys_arch.c)
//#define LWIP_NETCONN_FULLDUPLEX         1

#ifndef LWIP_RAND
#define LWIP_RAND() ((u32_t)rand())
#endif

// empty define to keep the compiler when enabling lwIP debug
#define sntp_format_time

// required as we are defining priority-aware mutexes
#define LWIP_COMPAT_MUTEX_ALLOWED

#endif // LWIPOPTS_H
