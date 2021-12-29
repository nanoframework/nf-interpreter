//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2001-2004 Swedish Institute of Computer Science.  All rights reserved.
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef CC_H
#define CC_H

#include <hal.h>

// need to declare this here instead of adding an include because debug_printf is declared in nanoHAL.h which is a C++
// header
extern void debug_printf(const char *format, ...);

/* Use errno provided by system. */
#define LWIP_ERRNO_INCLUDE <errno.h>

/**
 * @brief   Use system provided struct timeval by default.
 */
#ifndef LWIP_TIMEVAL_PRIVATE
#define LWIP_TIMEVAL_PRIVATE 0
#include <sys/time.h>
#endif

/**
 * @brief   Use a no-op diagnostic output macro by default.
 */
#if !defined(LWIP_PLATFORM_DIAG)
#define LWIP_PLATFORM_DIAG(x)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        debug_printf x;                                                                                                \
    } while (0)
#endif

/**
 * @brief   Halt the system on lwIP assert failure by default.
 */
#if !defined(LWIP_PLATFORM_ASSERT)
#define LWIP_PLATFORM_ASSERT(x) __asm volatile("BKPT #0\n");
#endif

/**
 * @brief   The NETIF API is required by lwipthread.
 */
#ifdef LWIP_NETIF_API
#undef LWIP_NETIF_API
#endif
#define LWIP_NETIF_API 1

// disable byte order functions
#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS
#define htons(x) __builtin_bswap16(x)
#define ntohs(x) __builtin_bswap16(x)
#define htonl(x) __builtin_bswap32(x)
#define ntohl(x) __builtin_bswap32(x)

#endif // CC_H
