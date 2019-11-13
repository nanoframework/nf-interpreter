//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

int ssl_accept_internal( 
    int sd, 
    int contextHandle )
{
    mbedTLS_NFContext* context;
    mbedtls_ssl_context *ssl;
    int nonblock = 0;
    int ret = SOCK_SOCKET_ERROR;

    // Check contextHandle range
    if((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0))
    {
        goto error;
    }

    context = (mbedTLS_NFContext*)g_SSL_Driver.ContextArray[contextHandle].Context;
    ssl = context->ssl;

    // sanity check
    if(ssl == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    // set socket in network context
    context->server_fd->fd = sd;

    // setup internal SSL context and calls to transport layer send, receive and receive with timeout
    mbedtls_ssl_set_bio( context->ssl, context->server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout );

    // Set non blocking socket
    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // connection is set up now proceed to SSL handshake  

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

    // Save SSL context against socket
    SOCKET_DRIVER.SetSocketSslData(sd, (void*)context);

error:
    
    return ret;
}
