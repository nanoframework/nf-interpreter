//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

int ssl_connect_internal(int sd, const char* szTargetHost, int sslContextHandle)
{
    mbedTLS_NFContext* context;

    int ret = -1;

    // Retrieve SSL struct from g_SSL_Driver    
    if((sslContextHandle >= (int)ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0))
    {
        goto error;
    }
    
    // sd should already have been created
    // Now do the SSL negotiation
    context = (mbedTLS_NFContext*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    if (context == NULL) goto error;

    // set socket
    context->server_fd->fd = sd;

    if(szTargetHost != NULL && szTargetHost[0] != 0)
    {
        if( (ret = mbedtls_ssl_set_hostname( context->ssl, szTargetHost )) != 0 )
        {
            // hostname_failed
            goto error;
        }
    }

    mbedtls_ssl_set_bio( context->ssl, context->server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout );

    while( ( ret = mbedtls_ssl_handshake( context->ssl ) ) != 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            // SSL handshake failed
            //mbedtls_printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
            goto error;
        }
    }

error:
    return ret;
}
