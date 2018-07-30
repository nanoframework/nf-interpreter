//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include <nanoPAL.h>
#include "mbedtls.h"

#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
   
#include "netif/ethernet.h"

// this one lives in lwIPSocket.cpp
extern int errno;

static int net_would_block( const mbedtls_net_context *ctx )
{
    /*
    * Never return 'WOULD BLOCK' on a non-blocking socket
    */
    int val = 0;
    
    if( ( fcntl( ctx->fd, F_GETFL, val) & O_NONBLOCK ) != O_NONBLOCK )
        return( 0 );

    switch( errno )
    {
      #if defined EAGAIN
        case EAGAIN:
      #endif
      #if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
      #endif
        return( 1 );
    }

    return( 0 );
}

int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len )
{
    int32_t ret;
    int32_t fd = ((mbedtls_net_context *) ctx)->fd;

    if( fd < 0 )
    {
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    }
    
    ret = (int32_t) read( fd, buf, len );

    if( ret < 0 )
    {
        if(net_would_block((mbedtls_net_context *)ctx) != 0)
        {
            return MBEDTLS_ERR_SSL_WANT_READ;
        }
        
        if(errno == EPIPE || errno == ECONNRESET)
        {
            return MBEDTLS_ERR_NET_CONN_RESET;
        }

        if(errno == EINTR)
        {
            return MBEDTLS_ERR_SSL_WANT_READ;
        }

        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return ret;
}

int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    int32_t ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if( fd < 0 )
    {
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    }
    
    ret = (int32_t) write(fd, buf, len);

    if( ret < 0 )
    {
        if(net_would_block((mbedtls_net_context *)ctx) != 0)
        {
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        }
        
        if(errno == EPIPE || errno == ECONNRESET)
        {
            return MBEDTLS_ERR_NET_CONN_RESET;
        }
        
        if(errno == EINTR)
        {
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        }

        return MBEDTLS_ERR_NET_SEND_FAILED;
    }

    return ret;
}

int mbedtls_net_recv_timeout( void *ctx, unsigned char *buf,
                              size_t len, uint32_t timeout )
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    FD_ZERO( &read_fds );
    FD_SET( fd, &read_fds );

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = ( timeout % 1000 ) * 1000;

    ret = select( fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv );

    /* Zero fds ready means we timed out */
    if( ret == 0 )
        return( MBEDTLS_ERR_SSL_TIMEOUT );

    if( ret < 0 )
    {
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
        if( WSAGetLastError() == WSAEINTR )
            return( MBEDTLS_ERR_SSL_WANT_READ );
#else
        if( errno == EINTR )
            return( MBEDTLS_ERR_SSL_WANT_READ );
#endif

        return( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    /* This call will not block */
    return( mbedtls_net_recv( ctx, buf, len ) );
}

int ssl_connect_internal(int sd, const char* szTargetHost, int sslContextHandle)
{
    mbedtls_ssl_context* ssl;
    mbedtls_net_context* server_fd = NULL;

    int ret = -1;

    // Retrieve SSL struct from g_SSL_Driver    
    if((sslContextHandle >= (int)ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0))
    {
        goto error;
    }
    
    // sd should already have been created
    // Now do the SSL negotiation   
    ssl = (mbedtls_ssl_context*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    if (ssl == NULL) goto error;

    // allocate memory for net context 
    server_fd = (mbedtls_net_context*)platform_malloc(sizeof(mbedtls_net_context));
    if(server_fd == NULL)
    {
        goto error;
    }

    // set socket
    server_fd->fd = sd;

    if(szTargetHost != NULL && szTargetHost[0] != 0)
    {
        if( mbedtls_ssl_set_hostname( ssl, szTargetHost ) != 0 )
        {
            // hostname_failed
            goto error;
        }
    }

    mbedtls_ssl_set_bio( ssl, server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout );

    while( ( ret = mbedtls_ssl_handshake( ssl ) ) != 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            //mbedtls_printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
            goto error;
        }
    }

    return ret;

error:

    // check for any memory allocation that needs to be freed before exiting
    if(server_fd != NULL) platform_free(server_fd);
  
    return ret;
}
