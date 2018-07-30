//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "mbedtls.h"

#include "lwip/sockets.h"
  

// this one lives in lwIPSocket.cpp
extern int errno;

int sslRecv(void *ctx, unsigned char *buf, size_t len)
{
    (void)buf;
    (void)len;
    (void)ctx;

    //int sd = (int)ctx;

    // int ret = socket->recv(buf, len);
 
    // if (ret == NSAPI_ERROR_WOULD_BLOCK)
    //     ret = MBEDTLS_ERR_SSL_WANT_READ;
    // else if (ret < 0)
    //     mbedtls_printf("socket.recv() returned %d\n", ret);
 
    // return ret;
    return 0;
}

int ssl_accept_internal( int sd, int sslContextHandle )
{
    (void)sd;
    (void)sslContextHandle;

    // int err = SOCK_SOCKET_ERROR;
    // int nonblock = 0;
    // mbedtls_ssl_config *pSsl = NULL;

    // // Check sslContextHandle range
    // if((sslContextHandle >= ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0))
    // {
    //     goto error;
    // }

    // pSsl = (mbedtls_ssl_config*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    // if (ssl == NULL)
    // {
    //     goto error;
    // }

    // // Set socket against this SSL context 
    // mbedtls_ssl_set_bio(pSsl, static_cast<void *>(sd), sslSend, sslRecv, NULL);
 
    // // Set non blocking socket
    // SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // // Connecion is set up now do SSL handshake  
    // mbedtls_ssl_handshake()
    
    // nonblock = 1;
    // SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // // Save SSL context against socket
    // SOCKET_DRIVER.SetSocketSslData(sd, (void*)ssl);

 //error:
    //return err;

    return 0;
 }


int  ssl_read_internal( int sd, char* Data, size_t size )
{
    (void)sd;
    (void)Data;
    (void)size;

    //mbedtls_ssl_config *pSsl = (mbedtls_ssl_config*)SOCKET_DRIVER.GetSocketSslData(sd);
    
    // int ret = mbedtls_ssl_read(pSsl, (unsigned char *)(Data), size);
    // if ( ret < 0 )
    // {
    //     return 0;
    // }

    // return ret;
    return 0;
}

int  ssl_closesocket_internal( int sd )
{
    (void)sd;

    return 0;
}

void ssl_clear_cert_auth_internal(int sslContextHandle )
{
    (void)sslContextHandle;
}

bool ssl_add_cert_auth_internal( int sslContextHandle, const char* certificate, int certLength, const char* certPassword )
{
    (void)sslContextHandle;
    (void)certificate;
    (void)certLength;
    (void)certPassword;

    return false;
}

// mbed TLS requires a function with this signature, so we are wrapping the call to our debug_printf here 
void nf_debug( void *ctx, int level, const char *file, int line, const char *str )
{
    (void)level;
    (void)ctx;
    (void)file;
    (void)line;

    // the following line outputs the source code file name and line number
    // for verbose output SWO is overhelmed and output fails at some point
    //debug_printf( "%s:%04d: %s", file, line, str );

    // this is a lightheight version with just the debug messages
    debug_printf( "%s", str );
}

int net_would_block( const mbedtls_net_context *ctx )
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