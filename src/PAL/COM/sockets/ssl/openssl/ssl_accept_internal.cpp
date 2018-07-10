//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include <openssl/ssl.h>
#include <openssl.h> 


// #include <e_os.h>
// #include <tinyclr/ssl_functions.h>
// #include <openssl/ssl.h>
// #include <openssl.h>

int ssl_accept_internal( int sd, int sslContextHandle )
{
    int err = SOCK_SOCKET_ERROR;
    SSL *ssl = NULL;
    int nonblock = 0;
    
    if((sslContextHandle >= (int)ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0))
    {
        goto error;
    }

    ssl = (SSL*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    if (ssl == NULL)
    {
        goto error;
    }

    if (!SSL_set_fd(ssl, sd))
    {
        goto error;
    }

    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    err = SSL_accept (ssl);

    nonblock = 1;
    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    err = SSL_get_error(ssl,err);
    
    switch(err)
    {
        case SSL_ERROR_WANT_READ:
            err = SOCK_EWOULDBLOCK;
#if !defined(TCPIP_LWIP) && !defined(TCPIP_LWIP_OS)
//FIXME           SOCKET_DRIVER.ClearStatusBitsForSocket( sd, FALSE );        
#endif
            break;

        case SSL_ERROR_WANT_WRITE:
            err = SOCK_TRY_AGAIN;
#if !defined(TCPIP_LWIP) && !defined(TCPIP_LWIP_OS)
//FIXME           SOCKET_DRIVER.ClearStatusBitsForSocket( sd, TRUE );        
#endif
            break;

        case SSL_ERROR_NONE:
#if !defined(TCPIP_LWIP) && !defined(TCPIP_LWIP_OS)
//FIXME          SOCKET_DRIVER.ClearStatusBitsForSocket( sd, FALSE ); 
#endif
            break;
    }

    SOCKET_DRIVER.SetSocketSslData(sd, (void*)ssl);

error:
    return err;
}

