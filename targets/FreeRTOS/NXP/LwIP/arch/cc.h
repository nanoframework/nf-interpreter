//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2001-2004 Swedish Institute of Computer Science. All rights reserved.
// See LICENSE file in the project root for full license information.
//

/*
 * from the original source code file
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

#ifndef __CC_H__
#define __CC_H__

#define LWIP_NO_INTTYPES_H 1

#if defined(LWIP_TIMEVAL_PRIVATE) && (LWIP_TIMEVAL_PRIVATE == 0)
#include <sys/time.h>
#endif

// Typedefs for the types used by lwip
#if LWIP_NO_STDINT_H
typedef unsigned char  u8_t;
typedef signed   char  s8_t;
typedef unsigned short u16_t;
typedef signed   short s16_t;
typedef unsigned int   u32_t;
typedef signed   int   s32_t;
typedef u32_t          mem_ptr_t;
#endif

// Compiler hints for packing lwip's structures
//FSL: very important at high optimization level

#if __GNUC__
#define PACK_STRUCT_BEGIN
#elif defined(__IAR_SYSTEMS_ICC__)
#define PACK_STRUCT_BEGIN _Pragma("pack(1)")
#elif defined(__arm__) && defined(__ARMCC_VERSION)
#define PACK_STRUCT_BEGIN __packed
#else
#define PACK_STRUCT_BEGIN
#endif

#if __GNUC__
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#elif defined(__IAR_SYSTEMS_ICC__)
#define PACK_STRUCT_STRUCT
#elif defined(__arm__) && defined(__ARMCC_VERSION)
#define PACK_STRUCT_STRUCT
#else
#define PACK_STRUCT_STRUCT
#endif

#if __GNUC__
#define PACK_STRUCT_END
#elif defined(__IAR_SYSTEMS_ICC__)
#define PACK_STRUCT_END _Pragma("pack()")
#elif defined(__arm__) && defined(__ARMCC_VERSION)
#define PACK_STRUCT_END
#else
#define PACK_STRUCT_END
#endif

#define PACK_STRUCT_FIELD(x) x



// non-fatal, print a message.
#define LWIP_PLATFORM_DIAG(x)                     do {PRINTF x;PRINTF("\r\n");} while(0)

/**
 * @brief   Halt the system on lwIP assert failure by default.
 */
#if !defined(LWIP_PLATFORM_ASSERT)
#define LWIP_PLATFORM_ASSERT(x)     __asm volatile("BKPT #0\n");
#endif

// Platform specific diagnostic output
#include "sys_arch.h"//FSL


#endif /* __CC_H__ */
