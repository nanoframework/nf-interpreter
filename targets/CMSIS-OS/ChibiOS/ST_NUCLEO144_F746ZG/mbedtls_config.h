//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// See LICENSE file in the project root for full license information.
//

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#include <target_common.h>
//#include <time.h>

// need to declare this as external to be picked up by mbed TLS platform_time
//extern time_t nf_get_unix_epoch();

/* System support */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_HAVE_TIME_DATE
// need to define this as the alternative to standard time function
//#define MBEDTLS_PLATFORM_TIME_MACRO nf_get_unix_epoch

/* mbed TLS feature support */
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#define MBEDTLS_SSL_PROTO_TLS1_1

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_DES_C
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_MD_C
#define MBEDTLS_MD5_C
// #define MBEDTLS_NET_C
#define MBEDTLS_OID_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_RSA_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_USE_C
#define MBEDTLS_DEBUG_C

#define MBEDTLS_NO_PLATFORM_ENTROPY

////////////////////////////////////////////////////////////////////////////
// This define depends on the platform having a hardware random generator.
// Requires that a function mbedtls_hardware_poll() exits as explained in mbed TLS documentation.
// Because it's target dependent, the define PLATFORM_HAS_RNG belongs in the target_common header
#if (PLATFORM_HAS_RNG == TRUE)
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#endif
//////////////////////////////////////////////////////////////////////////

/* For test certificates */
#define MBEDTLS_BASE64_C
#define MBEDTLS_CERTS_C
#define MBEDTLS_PEM_PARSE_C

#define SSL_DEBUG_BUF MBEDTLS_SSL_DEBUG_BUF
#define SSL_DEBUG_CRT MBEDTLS_SSL_DEBUG_CRT
#define SSL_DEBUG_ECP MBEDTLS_SSL_DEBUG_ECP
#define SSL_DEBUG_MPI MBEDTLS_SSL_DEBUG_MPI
#define SSL_DEBUG_MSG MBEDTLS_SSL_DEBUG_MSG
#define SSL_DEBUG_RET MBEDTLS_SSL_DEBUG_RET

/* uncomment the defines below to enable static memory allocation feature */
#if 0 
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_DEBUG_C
#endif

#ifdef USE_LCD
#include "lcd_log.h"
#define MBEDTLS_PLATFORM_PRINTF_MACRO LCD_UsrLog
#endif

#include <mbedtls/check_config.h>

#endif // MBEDTLS_CONFIG_H
