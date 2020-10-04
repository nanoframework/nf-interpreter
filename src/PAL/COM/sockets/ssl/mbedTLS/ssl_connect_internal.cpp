//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

int ssl_connect_internal(
    int sd, 
    const char* szTargetHost, 
    int contextHandle)
{
    mbedTLS_NFContext* context;
    int nonblock = 0;

    int ret = SOCK_SOCKET_ERROR;

    // Check contextHandle range
    if((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0))
    {
        goto error;
    }
    
    // Retrieve SSL struct from g_SSL_Driver    
    // sd should already have been created
    // Now do the SSL negotiation
    context = (mbedTLS_NFContext*)g_SSL_Driver.ContextArray[contextHandle].Context;
    if (context == NULL)
    {
        return false;
    }

    // set socket in network context
    context->server_fd->fd = sd;

    if(szTargetHost != NULL && szTargetHost[0] != 0)
    {
        if( (ret = mbedtls_ssl_set_hostname( context->ssl, szTargetHost )) != 0 )
        {
            // hostname_failed
            goto error;
        }
    }

    // setup internal SSL context and calls to transport layer send, receive and receive with timeout
    mbedtls_ssl_set_bio( context->ssl, context->server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout );

    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // perform SSL handshake
    while( ( ret = mbedtls_ssl_handshake( context->ssl ) ) != 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            // SSL handshake failed
            //mbedtls_printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
            goto error;
        }
    }

    nonblock = 1;
    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // store SSL context in sockets driver
    SOCKET_DRIVER.SetSocketSslData(sd, (void*)context);

error:
    return ret;
}
