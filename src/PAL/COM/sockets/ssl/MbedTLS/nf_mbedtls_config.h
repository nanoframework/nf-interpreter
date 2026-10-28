//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// See LICENSE file in the project root for full license information.
//

#ifndef NF_MBEDTLS_CONFIG_H
#define NF_MBEDTLS_CONFIG_H

#include <nanoCLR_Headers.h>
#include <target_common.h>
#include <time.h>

// need to declare this as external to be picked up by Mbed TLS platform_time
#ifdef __cplusplus
extern "C"
{
#endif

    time_t nf_get_unix_epoch();

#ifdef __cplusplus
}
#endif

/* System support */
#define MBEDTLS_HAVE_ASM

#define MBEDTLS_HAVE_TIME
#define MBEDTLS_PLATFORM_MS_TIME_ALT

#define MBEDTLS_HAVE_TIME_DATE

#define MBEDTLS_DEPRECATED_REMOVED

// need to define this as the alternative to standard time function
#define MBEDTLS_PLATFORM_TIME_MACRO nf_get_unix_epoch

/* Mbed TLS feature support */
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_CIPHER_PADDING_PKCS7

// requirements for MBEDTLS_ECDSA_C
// requirements for MBEDTLS_ECP_C
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#define MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_ECDSA_DETERMINISTIC
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_PK_PARSE_EC_EXTENDED
#define MBEDTLS_ERROR_STRERROR_DUMMY
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_FORCE_SHA256
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_SSL_ENCRYPT_THEN_MAC
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#define MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_PROTO_TLS1_3
#define MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
#define MBEDTLS_SSL_ALPN
#define MBEDTLS_SSL_SERVER_NAME_INDICATION
#define MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH
#define MBEDTLS_X509_RSASSA_PSS_SUPPORT

/* Mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C

#define MBEDTLS_BASE64_C

#define MBEDTLS_BIGNUM_C

#define MBEDTLS_CIPHER_C

#define MBEDTLS_CTR_DRBG_C

#define MBEDTLS_ECDH_C
#define MBEDTLS_ECDSA_C

#define MBEDTLS_ECP_C

#define MBEDTLS_ENTROPY_C

#define MBEDTLS_GCM_C
#define MBEDTLS_HKDF_C
#define MBEDTLS_HMAC_DRBG_C

#define MBEDTLS_MD_C

#define MBEDTLS_MD5_C

#define MBEDTLS_OID_C

#define MBEDTLS_PEM_PARSE_C

#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C

#define MBEDTLS_PKCS5_C

#define MBEDTLS_PLATFORM_C

#define MBEDTLS_RSA_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA224_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA512_C
#define MBEDTLS_SHA384_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TLS_C

#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C

#define MBEDTLS_AES_ROM_TABLES

#ifndef MBEDTLS_SSL_OUT_CONTENT_LEN
#define MBEDTLS_SSL_OUT_CONTENT_LEN 4096
#endif

#define MBEDTLS_USE_PSA_CRYPTO
#define MBEDTLS_PSA_CRYPTO_C
#define MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG

////////////////////////////////////////////////////////////////////////////
// This define depends on the platform having a hardware random generator.
// Requires that a function mbedtls_hardware_poll() exits as explained in Mbed TLS documentation.
// Because it's target dependent, the define PLATFORM_HAS_RNG belongs in the target_common header
#if (PLATFORM_HAS_RNG == TRUE)
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#endif
//////////////////////////////////////////////////////////////////////////

#endif // NF_MBEDTLS_CONFIG_H
