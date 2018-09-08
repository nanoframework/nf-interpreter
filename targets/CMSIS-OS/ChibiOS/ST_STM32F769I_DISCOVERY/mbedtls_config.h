//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// See LICENSE file in the project root for full license information.
//

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#include <target_common.h>
#include <time.h>

// need to declare this as external to be picked up by mbed TLS platform_time
extern time_t nf_get_unix_epoch();

/* System support */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_HAVE_TIME_DATE
// need to define this as the alternative to standard time function
#define MBEDTLS_PLATFORM_TIME_MACRO nf_get_unix_epoch

/* mbed TLS feature support */
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_CIPHER_MODE_CFB
#define MBEDTLS_CIPHER_MODE_CTR
#define MBEDTLS_CIPHER_PADDING_PKCS7
#define MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
#define MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
#define MBEDTLS_CIPHER_PADDING_ZEROS
#define MBEDTLS_REMOVE_ARC4_CIPHERSUITES

#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_ECP_DP_SECP192K1_ENABLED
#define MBEDTLS_ECP_DP_SECP224K1_ENABLED
#define MBEDTLS_ECP_DP_SECP256K1_ENABLED
#define MBEDTLS_ECP_DP_BP256R1_ENABLED
#define MBEDTLS_ECP_DP_BP384R1_ENABLED
#define MBEDTLS_ECP_DP_BP512R1_ENABLED
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED

#define MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_ECDSA_DETERMINISTIC
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
// #define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
// #define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED
#define MBEDTLS_PK_PARSE_EC_EXTENDED
#define MBEDTLS_GENPRIME
#define MBEDTLS_PK_RSA_ALT_SUPPORT


#define MBEDTLS_PKCS1_V15
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_SSL_ENCRYPT_THEN_MAC
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#define MBEDTLS_SSL_FALLBACK_SCSV
#define MBEDTLS_SSL_CBC_RECORD_SPLITTING
#define MBEDTLS_SSL_RENEGOTIATION


#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#define MBEDTLS_SSL_PROTO_TLS1
#define MBEDTLS_SSL_PROTO_TLS1_1
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_ALPN
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY
#define MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#define MBEDTLS_SSL_DTLS_BADMAC_LIMIT
#define MBEDTLS_SSL_EXPORT_KEYS
#define MBEDTLS_SSL_TRUNCATED_HMAC
#define MBEDTLS_X509_CHECK_KEY_USAGE
#define MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE
#define MBEDTLS_X509_RSASSA_PSS_SUPPORT

/* mbed TLS modules */
#define MBEDTLS_AESNI_C
#define MBEDTLS_AES_C
#define MBEDTLS_ARC4_C
#define MBEDTLS_ASN1_PARSE_C
// #define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_BLOWFISH_C
#define MBEDTLS_CAMELLIA_C
#define MBEDTLS_CCM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_DES_C
#define MBEDTLS_DHM_C
#define MBEDTLS_ECDH_C
// #define MBEDTLS_ECDSA_C
#define MBEDTLS_ECP_C
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_GCM_C
#define MBEDTLS_HMAC_DRBG_C
#define MBEDTLS_MD_C
#define MBEDTLS_MD5_C

#define MBEDTLS_OID_C
#define MBEDTLS_PADLOCK_C

#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C

#define MBEDTLS_PKCS5_C
#define MBEDTLS_PKCS12_C

#define MBEDTLS_PLATFORM_C  //// check this


#define MBEDTLS_RIPEMD160_C


#define MBEDTLS_RSA_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA512_C
#define MBEDTLS_SSL_CACHE_C
#define MBEDTLS_SSL_COOKIE_C
#define MBEDTLS_SSL_TICKET_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TLS_C

#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_CRL_PARSE_C
#define MBEDTLS_XTEA_C

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

// #define SSL_DEBUG_BUF MBEDTLS_SSL_DEBUG_BUF
// #define SSL_DEBUG_CRT MBEDTLS_SSL_DEBUG_CRT
// #define SSL_DEBUG_ECP MBEDTLS_SSL_DEBUG_ECP
// #define SSL_DEBUG_MPI MBEDTLS_SSL_DEBUG_MPI
// #define SSL_DEBUG_MSG MBEDTLS_SSL_DEBUG_MSG
// #define SSL_DEBUG_RET MBEDTLS_SSL_DEBUG_RET

// #define MBEDTLS_SSL_ALL_ALERT_MESSAGES
// #define MBEDTLS_SSL_DEBUG_ALL
// #define MBEDTLS_VERSION_FEATURES
// #define MBEDTLS_CERTS_C
// #define MBEDTLS_ERROR_C
// #define MBEDTLS_VERSION_C


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
