//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef MBEDTLS_H
#define MBEDTLS_H

#include <nanoHAL.h>

#include <sockets_lwip.h>

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

typedef struct mbedTLS_NFContext
{
    mbedtls_entropy_context* entropy;
    mbedtls_ctr_drbg_context* ctr_drbg;
    mbedtls_ssl_context* ssl;
    mbedtls_ssl_config* conf;
    mbedtls_net_context* server_fd;
}mbedTLS_NFContext;

int net_would_block( const mbedtls_net_context *ctx );
int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len );
int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len );
int mbedtls_net_recv_timeout( void *ctx, unsigned char *buf, size_t len, uint32_t timeout );
void mbedtls_net_free( mbedtls_net_context *ctx );

// debug output declaration 
void nf_debug( void *ctx, int level, const char *file, int line, const char *str );

#define SOCKET_DRIVER g_Sockets_LWIP_Driver

#endif
