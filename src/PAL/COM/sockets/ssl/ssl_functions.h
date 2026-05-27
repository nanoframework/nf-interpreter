//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SSL_FUNCTIONS_H
#define SSL_FUNCTIONS_H

#include <nanoHAL.h>

// Keep these in sync with SslProtocols in System.Net

///////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.Security.SslProtocols (in managed code) !!!  //
///////////////////////////////////////////////////////////////////////////////////
enum SslProtocols
{
    SslProtocols_None = 0,
    SslProtocols_Tls = 192,
    SslProtocols_Tls11 = 768,
    SslProtocols_Tls12 = 3072,
    SslProtocols_Tls13 = 12288,
};

//////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.Security.SslVerification (in managed code) !!!  //
//////////////////////////////////////////////////////////////////////////////////////
enum SslVerification
{
    SslVerification_Invalid = 0,
    SslVerification_NoVerification = 1,
    SslVerification_VerifyPeer = 2,
    SslVerification_CertificateRequired = 4,
    SslVerification_VerifyClientOnce = 8,
};

bool ssl_parse_certificate_internal(void *buf, size_t size, void *x509);
int ssl_decode_private_key_internal(
    const unsigned char *key,
    size_t keyLength,
    const unsigned char *pwd,
    size_t pwdLength);
int ssl_connect_internal(int sd, const char *szTargetHost, int contextHandle);
int ssl_accept_internal(int socket, int contextHandle);
int ssl_read_internal(int socket, char *data, size_t size);
int ssl_write_internal(int socket, const char *data, size_t size);
int ssl_close_socket_internal(int sd);
int ssl_available_internal(int sd);
bool ssl_exit_context_internal(int contextHandle);
bool ssl_generic_init_internal(
    int sslMode,
    int sslVerify,
    const char *certificate,
    int certLength,
    const uint8_t *privateKey,
    int privateKeyLength,
    const char *pkPassword,
    int pkPasswordLength,
    int &contextHandle,
    bool useDeviceCertificate,
    bool isServer);
bool ssl_initialize_internal();
bool ssl_uninitialize_internal();
bool ssl_add_cert_auth_internal(int contextHandle, const char *certificate, int certLength);

#endif // SSL_FUNCTIONS_H
