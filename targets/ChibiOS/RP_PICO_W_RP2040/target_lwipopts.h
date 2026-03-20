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

// Increase lwIP thread stack for WiFi operations
#undef LWIP_THREAD_STACK_SIZE
#define LWIP_THREAD_STACK_SIZE 2048

// WiFi interface name
#undef LWIP_IFNAME0
#undef LWIP_IFNAME1
#define LWIP_IFNAME0 'w'
#define LWIP_IFNAME1 'l'

#endif // TARGET_LWIPOPTS_H
