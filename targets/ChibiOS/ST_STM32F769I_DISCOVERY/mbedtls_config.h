//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// See LICENSE file in the project root for full license information.
//

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/* For test certificates */
// #define MBEDTLS_CERTS_C
// #define MBEDTLS_PEM_PARSE_C

// #define SSL_DEBUG_BUF MBEDTLS_SSL_DEBUG_BUF
// #define SSL_DEBUG_CRT MBEDTLS_SSL_DEBUG_CRT
// #define SSL_DEBUG_ECP MBEDTLS_SSL_DEBUG_ECP
// #define SSL_DEBUG_MPI MBEDTLS_SSL_DEBUG_MPI
// #define SSL_DEBUG_MSG MBEDTLS_SSL_DEBUG_MSG
// #define SSL_DEBUG_RET MBEDTLS_SSL_DEBUG_RET

// #define MBEDTLS_SSL_ALL_ALERT_MESSAGES
// #define MBEDTLS_VERSION_FEATURES
// #define MBEDTLS_CERTS_C
// #define MBEDTLS_ERROR_C
// #define MBEDTLS_VERSION_C

// uncomment the defines below to enable static memory allocation feature
#if 0 
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C
#define MBEDTLS_PLATFORM_MEMORY
#endif

#ifdef USE_LCD
#include "lcd_log.h"
#define MBEDTLS_PLATFORM_PRINTF_MACRO LCD_UsrLog
#endif

// uncomment the defines bellow to generate debug output 
// set below the threshold level for debug messages
// check mbed TLS mbedtls/debug.h header for details.
// Debug levels:
// 0 No debug
// 1 Error
// 2 State change
// 3 Informational
// 4 Verbose

// #define MBEDTLS_DEBUG_C
// #define MBEDTLS_SSL_DEBUG_ALL
// #define MBEDTLS_SSL_ALL_ALERT_MESSAGES
// #define MBEDTLS_DEBUG_THRESHOLD     2

#endif // MBEDTLS_CONFIG_H
