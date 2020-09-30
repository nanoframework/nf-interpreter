//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoHal.h"

//--//

extern "C"
{
void ssl_rand_seed(const void *seed, int length)
{
    (void)seed;
    (void)length;
}
}

__nfweak bool SSL_Initialize()
{
    NATIVE_PROFILE_PAL_COM();
    return FALSE;
}

__nfweak bool SSL_Uninitialize()
{
    NATIVE_PROFILE_PAL_COM();
    return TRUE;
}

__nfweak bool SSL_ServerInit( 
    int sslMode, 
    int sslVerify, 
    const char* certificate, 
    int certLength, 
    const uint8_t* privateKey, 
    int privateKeyLength, 
    const char* password, 
    int passwordLength, 
    int& contextHandle )
{ 
    (void)sslMode; 
    (void)sslVerify; 
    (void)certificate; 
    (void)certLength; 
    (void)privateKey; 
    (void)privateKeyLength; 
    (void)password; 
    (void)passwordLength; 
    (void)contextHandle;

    NATIVE_PROFILE_PAL_COM();

    return TRUE; 
}

__nfweak bool SSL_ClientInit( 
    int sslMode, 
    int sslVerify, 
    const char* certificate, 
    int certLength, 
    const uint8_t* privateKey, 
    int privateKeyLength, 
    const char* password, 
    int passwordLength, 
    int& contextHandle )
{ 
    (void)sslMode; 
    (void)sslVerify; 
    (void)certificate; 
    (void)certLength; 
    (void)privateKey; 
    (void)privateKeyLength; 
    (void)password; 
    (void)passwordLength; 
    (void)contextHandle;

    NATIVE_PROFILE_PAL_COM();

    return TRUE; 
}

__nfweak bool SSL_AddCertificateAuthority( 
    int contextHandle, 
    const char* certificate, 
    int certLength, 
    const char* certPassword )
{
    (void)contextHandle; 
    (void)certificate; 
    (void)certLength; 
    (void)certPassword;

    NATIVE_PROFILE_PAL_COM();

    return TRUE;
}

__nfweak bool SSL_ExitContext( int contextHandle )
{ 
    (void)contextHandle;

    NATIVE_PROFILE_PAL_COM();

    return TRUE; 
}

__nfweak int SSL_Accept( 
    SOCK_SOCKET socket, 
    int contextHandle )
{ 
    (void)socket; 
    (void)contextHandle;

    NATIVE_PROFILE_PAL_COM();

    return 0; 
}

__nfweak int SSL_Connect( 
    SOCK_SOCKET socket, 
    const char* szTargetHost, 
    int contextHandle )
{ 
    (void)socket; 
    (void)szTargetHost;  
    (void)contextHandle;

    NATIVE_PROFILE_PAL_COM();

    return 0; 
}

__nfweak int SSL_Write( 
    SOCK_SOCKET socket, 
    const char* data, 
    size_t size )
{
    (void)socket; 
    (void)data;  
    (void)size;

    NATIVE_PROFILE_PAL_COM();

    return 0; 
}

__nfweak int SSL_Read( 
    SOCK_SOCKET socket, 
    char* data, 
    size_t size )
{ 
    (void)socket; 
    (void)data; 
    (void)size;

    NATIVE_PROFILE_PAL_COM();

    return 0; 
}

__nfweak int SSL_CloseSocket( SOCK_SOCKET socket )
{
    (void)socket;

    NATIVE_PROFILE_PAL_COM();

    return 0;
}

__nfweak bool SSL_ParseCertificate( 
    const char* certificate, 
    size_t certLength, 
    const char* password, 
    X509CertData* certData )
{
    (void)certificate; 
    (void)certLength; 
    (void)password;  
    (void)certData; 

    NATIVE_PROFILE_PAL_COM();

    return TRUE;
}

__nfweak int SSL_DecodePrivateKey( 
    const unsigned char *key, 
    size_t keyLength, 
    const unsigned char *pwd, 
    size_t pwdLength )
{
    (void)key;
    (void)keyLength;
    (void)pwd;
    (void)pwdLength;

    NATIVE_PROFILE_PAL_COM();

    return -1;
}

__nfweak int SSL_DataAvailable( SOCK_SOCKET socket )
{
    (void)socket;

    NATIVE_PROFILE_PAL_COM();

    return 0;
}

