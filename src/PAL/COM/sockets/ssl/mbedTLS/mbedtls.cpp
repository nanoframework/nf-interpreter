//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "mbedtls.h"

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

int  ssl_pending_internal( int sd )
{
    (void)sd;

    return 0;
}

bool ssl_exit_context_internal(int sslContextHandle )
{
    (void)sslContextHandle;

    return false;
}

bool ssl_uninitialize_internal()
{
    return false;
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
