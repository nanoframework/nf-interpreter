//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#ifndef SSL_FUNCTIONS_H
#define SSL_FUNCTIONS_H

#include <nanoHal.h>

// Keep these in sync with SslProtocols in System.Net

///////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.Security.SslProtocols (in managed code) !!!  //
///////////////////////////////////////////////////////////////////////////////////
enum SslProtocols
{
        SslProtocols_None =     0x00,
        SslProtocols_TLSv1 =    0x10,
        SslProtocols_TLSv11 =   0x20,
        SslProtocols_TLSv12 =   0x40,
};

//////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.Security.SslVerification (in managed code) !!!  //
//////////////////////////////////////////////////////////////////////////////////////
enum SslVerification
{
        SslVerification_Invalid =               0,
        SslVerification_NoVerification =        1,
        SslVerification_VerifyPeer =            2,
        SslVerification_CertificateRequired =   4,
        SslVerification_VerifyClientOnce =      8,
};

bool ssl_parse_certificate_internal(
        void* buf, 
        size_t size, 
        void* pwd, 
        void* x509 );
int ssl_decode_private_key_internal( 
        const unsigned char *key, 
        size_t keyLength, 
        const unsigned char *pwd, 
        size_t pwdLength );
int ssl_connect_internal(
        int sd, 
        const char* szTargetHost, 
        int contextHandle);
int ssl_accept_internal( 
        int socket, 
        int contextHandle );
int ssl_read_internal( 
        int socket, 
        char* data, 
        size_t size );
int ssl_write_internal( 
        int socket,
        const char* data, 
        size_t size);
int ssl_close_socket_internal( int sd );
int ssl_pending_internal( int sd );
bool ssl_exit_context_internal( int contextHandle );
bool ssl_generic_init_internal( 
        int sslMode, 
        int sslVerify, 
        const char* certificate, 
        int certLength, 
        const uint8_t* privateKey, 
        int privateKeyLength, 
        const char* password, 
        int passwordLength, 
        int& contextHandle, 
        bool isServer );
bool ssl_initialize_internal();
bool ssl_uninitialize_internal();
bool ssl_add_cert_auth_internal( 
        int contextHandle, 
        const char* certificate,
        int certLength, 
        const char* certPassword );

#endif // SSL_FUNCTIONS_H
