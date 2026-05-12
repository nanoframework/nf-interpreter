//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Mynewt byte-order stub for MCUboot boot_serial.c.
// SMP/mcumgr protocol uses big-endian (network order).

#ifndef OS_ENDIAN_H
#define OS_ENDIAN_H

#include <stdint.h>

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define ntohs(x) __builtin_bswap16((uint16_t)(x))
#define htons(x) __builtin_bswap16((uint16_t)(x))
#define ntohl(x) __builtin_bswap32((uint32_t)(x))
#define htonl(x) __builtin_bswap32((uint32_t)(x))
#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define ntohs(x) ((uint16_t)(x))
#define htons(x) ((uint16_t)(x))
#define ntohl(x) ((uint32_t)(x))
#define htonl(x) ((uint32_t)(x))
#else
#error "Unsupported target byte order"
#endif
#else
#error "Compiler byte order macros are required"
#endif

#endif // OS_ENDIAN_H
