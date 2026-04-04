//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// WiFi-specific lwIP option overrides for Pico W.
// These are included before the platform-wide lwipopts.h via target_board.h.
// The CYW43 WiFi chip does NOT have hardware checksum offload, so we need
// software checksums enabled (the platform lwipopts.h disables them for STM32 Ethernet).

#ifndef TARGET_LWIPOPTS_H
#define TARGET_LWIPOPTS_H

// Override checksum settings — WiFi requires software checksums
// (The platform default disables checksums assuming hardware offload)
#define CHECKSUM_GEN_IP     1
#define CHECKSUM_GEN_UDP    1
#define CHECKSUM_GEN_TCP    1
#define CHECKSUM_GEN_ICMP   1
#define CHECKSUM_CHECK_IP   1
#define CHECKSUM_CHECK_UDP  1
#define CHECKSUM_CHECK_TCP  1
#define CHECKSUM_CHECK_ICMP 1

// Match Pico SDK: skip gratuitous ARP probe during DHCP — the CYW43 data
// path is not yet proven to deliver responses fast enough, so the ARP check
// would time out and stall DHCP.
#define DHCP_DOES_ARP_CHECK         0
#define LWIP_DHCP_DOES_ACD_CHECK    0

// Increase lwIP thread stack for WiFi operations
#undef LWIP_THREAD_STACK_SIZE
#define LWIP_THREAD_STACK_SIZE 2048

// Increase tcpip thread stack — netconn/socket calls execute here,
// and TLS handshake callbacks need headroom on Cortex-M0+.
#define TCPIP_THREAD_STACKSIZE 2048

// WiFi interface name
#undef LWIP_IFNAME0
#undef LWIP_IFNAME1
#define LWIP_IFNAME0 'w'
#define LWIP_IFNAME1 'l'

#endif // TARGET_LWIPOPTS_H
