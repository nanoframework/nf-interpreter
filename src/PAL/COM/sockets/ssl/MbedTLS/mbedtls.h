//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef MBEDTLS_H
#define MBEDTLS_H

#include <nanoHAL.h>
#include <sockets_lwip.h>

#ifdef PLATFORM_ESP32
#include <esp32_idf.h>
#include <mbedtls/esp_config.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include <mbedtls/version.h>
#include <mbedtls/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>

    typedef struct mbedTLS_NFContext
    {
        mbedtls_entropy_context *entropy;
        mbedtls_ctr_drbg_context *ctr_drbg;
        mbedtls_ssl_config *conf;
        mbedtls_ssl_context *ssl;
        mbedtls_net_context *server_fd;
        mbedtls_x509_crt *ca_cert;
        mbedtls_x509_crt *own_cert;
        mbedtls_pk_context *pk;
    } mbedTLS_NFContext;

    int net_would_block(const mbedtls_net_context *ctx);
    int mbedtls_net_recv(void *ctx, unsigned char *buf, size_t len);
    int mbedtls_net_send(void *ctx, const unsigned char *buf, size_t len);
    int mbedtls_net_recv_timeout(void *ctx, unsigned char *buf, size_t len, uint32_t timeout);
    void mbedtls_net_free(mbedtls_net_context *ctx);

    // debug output declaration
    void nf_debug(void *ctx, int level, const char *file, int line, const char *str);

    // function returning the system date and time in Unix Epoch
    time_t nf_get_unix_epoch();

#ifdef __cplusplus
}
#endif

#define SOCKET_DRIVER g_Sockets_LWIP_Driver

#endif
