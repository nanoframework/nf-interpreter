//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

SslError ssl_connect_internal(int sd, const char *szTargetHost, int contextHandle, int *mbedtlsCode)
{
    mbedTLS_NFContext *context;
    int nonblock = 0;
    int ret;

    *mbedtlsCode = 0;

    // Check contextHandle range
    if ((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0))
    {
        return SslError_HandshakeBadContext;
    }

    // Retrieve SSL struct from g_SSL_Driver
    // sd should already have been created
    // Now do the SSL negotiation
    context = (mbedTLS_NFContext *)g_SSL_Driver.ContextArray[contextHandle].Context;
    if (context == NULL || context->ssl == NULL)
    {
        return SslError_HandshakeBadContext;
    }

    // set socket in network context
    context->server_fd->fd = sd;

    if (szTargetHost != NULL && szTargetHost[0] != 0)
    {
        if ((ret = mbedtls_ssl_set_hostname(context->ssl, szTargetHost)) != 0)
        {
            *mbedtlsCode = ret;
            return SslError_HandshakeSetHostname;
        }
    }

    // setup internal SSL context and calls to transport layer send, receive and receive with timeout
    mbedtls_ssl_set_bio(context->ssl, context->server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // perform SSL handshake
    while ((ret = mbedtls_ssl_handshake(context->ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            // SSL handshake failed
            // mbedtls_printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
            if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED)
            {
                // surface the verify result flags - more actionable than the raw mbedTLS code
                *mbedtlsCode = (int)mbedtls_ssl_get_verify_result(context->ssl);
                return SslError_HandshakeCertVerifyFailed;
            }

            *mbedtlsCode = ret;
            return SslError_HandshakeFailed;
        }
    }

    nonblock = 1;
    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // store SSL context in sockets driver
    SOCKET_DRIVER.SetSocketSslData(sd, (void *)context);

    return SslError_None;
}
