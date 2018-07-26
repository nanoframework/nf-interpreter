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
#define NANOCLR_SSL_MODE_TLS1  0x10
#define NANOCLR_SSL_MODE_TLS11 0x20
#define NANOCLR_SSL_MODE_TLS12 0x40
#define NANOCLR_SSL_MODE_SSL3  0x08

#define NANOCLR_SSL_VERIFY_NONE         0x01
#define NANOCLR_SSL_VERIFY_PEER         0x02
#define NANOCLR_SSL_VERIFY_CERT_REQ     0x04
#define NANOCLR_SSL_VERIFY_CLIENT_ONCE  0x08


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
void ssl_clear_cert_auth_internal(int sslContextHandle );
bool ssl_add_cert_auth_internal( int sslContextHandle, const char* certificate, 	int certLength, const char* certPassword );

#endif
