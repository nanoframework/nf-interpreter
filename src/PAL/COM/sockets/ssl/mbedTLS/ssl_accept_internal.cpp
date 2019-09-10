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

    // TODO check how to handle server certificates and certificate chain parsing

    // if( ( ret = mbedtls_net_bind( context->server_fd, NULL, "4433", MBEDTLS_NET_PROTO_TCP ) ) != 0 )
    // {
    //     goto error;
    // }

    // // Set socket against this SSL context 
    // mbedtls_ssl_set_bio(pSsl, static_cast<void *>(sd), sslSend, sslRecv, NULL);
 
    // Set non blocking socket
    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // // Connecion is set up now do SSL handshake  
    // mbedtls_ssl_handshake()
    
    // nonblock = 1;
    // SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // Save SSL context against socket
    SOCKET_DRIVER.SetSocketSslData(sd, (void*)context);

error:
    
    return ret;
}
