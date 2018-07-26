//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "mbedtls.h"



int ssl_connect_internal(int sd, const char* szTargetHost, int sslContextHandle)
{
    (void)sd;
    (void)szTargetHost;
    (void)sslContextHandle;

    //int ret;


    // if( ( ret = mbedtls_net_connect( &server_fd, SERVER_NAME,
    //                                     SERVER_PORT, MBEDTLS_NET_PROTO_TCP ) ) != 0 )
    // {
    //     //printf( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret );
    //     goto exit;
    // }

    return 0;
}

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

// Probaly don't need to loop on write, done at higher level
//
int  ssl_write_internal( int sd, const char* Data, size_t req_len)
{
    (void)sd;
    (void)Data;
    (void)req_len;

    //mbedtls_ssl_config *pSsl = (mbedtls_ssl_config*)SOCKET_DRIVER.GetSocketSslData(sd);
    //int ret;

    // // Loop until all data has been sent or error
    // int req_offset = 0;
    // do {
    //     ret = mbedtls_ssl_write( pSsl, (const unsigned char *)(Data + req_offset), req_len - req_offset);
    //     if (ret > 0)
    //         req_offset += static_cast<size_t>(ret);
    // }
    // while( req_offset < req_len &&
    //        (ret > 0 ||
    //        ret == MBEDTLS_ERR_SSL_WANT_WRITE ||
    //        ret == MBEDTLS_ERR_SSL_WANT_READ) );

    // if (ret < 0) {
    //     mbedtls_printf("mbedtls_ssl_write() returned -0x%04X\n", -ret);
    //     return 0;
    // } 

    // return req_len;

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

// bool ssl_generic_init_internal( int sslMode, int sslVerify, const char* certificate, int certLength, const char* pwd, int& sslContextHandle, bool isServer )
// {
//     (void)sslMode;
//     (void)sslVerify;
//     (void)certificate;
//     (void)certLength;
//     (void)pwd;
//     (void)sslContextHandle;
//     (void)isServer;
    
//     return false;
// }

// bool ssl_initialize_internal()
// {
//     // mbedtls_ssl_init(&g_SSL_Driver.SslContext);
//     return true;
// }

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
