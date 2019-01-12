//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#ifndef SSL_FUNCTIONS_H
#define SSL_FUNCTIONS_H

#include <nanoHal.h>
//#include <ossl_typ.h>

// Keey these in sync with SslProtocols in System.Net

///////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.Security.SslProtocols (in managed code) !!!  //
///////////////////////////////////////////////////////////////////////////////////
enum SslProtocols
{
        SslProtocols_None =     0x00,
        SslProtocols_SSLv3 =    0x08,
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


// Lifted from Apps.h
#define FORMAT_UNDEF    0
#define FORMAT_ASN1     1
#define FORMAT_TEXT     2
#define FORMAT_PEM      3
#define FORMAT_NETSCAPE 4
#define FORMAT_PKCS12   5
#define FORMAT_SMIME    6
#define FORMAT_ENGINE   7
#define FORMAT_IISSGC   8  

bool ssl_parse_certificate_internal(void* buf, size_t size, void* pwd, void* x509 );
int ssl_connect_internal(int sd, const char* szTargetHost, int sslContextHandle);
int ssl_accept_internal( int socket, int sslContextHandle );
int ssl_read_internal( int socket, char* Data, size_t size );
int ssl_write_internal( int socket, const char* Data, size_t size);
int ssl_closesocket_internal( int sd );
int ssl_pending_internal( int sd );
bool ssl_exit_context_internal(int sslContextHandle );
bool ssl_generic_init_internal( int sslMode, int sslVerify, const char* certificate, int certLength, const char* pwd, int& sslContextHandle, bool isServer );
bool ssl_initialize_internal();
bool ssl_uninitialize_internal();
bool ssl_add_cert_auth_internal( int sslContextHandle, const char* certificate, 	int certLength, const char* certPassword );

#endif
