//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Mynewt byte-order stub for MCUboot boot_serial.c.
// STM32 is little-endian; SMP/mcumgr protocol uses big-endian (network order).
// ntohs/htons swap bytes on little-endian hosts.

#ifndef OS_ENDIAN_H
#define OS_ENDIAN_H

#include <stdint.h>

#define ntohs(x) __builtin_bswap16((uint16_t)(x))
#define htons(x) __builtin_bswap16((uint16_t)(x))
#define ntohl(x) __builtin_bswap32((uint32_t)(x))
#define htonl(x) __builtin_bswap32((uint32_t)(x))

#endif // OS_ENDIAN_H
