//
// Copyright (c) 2017 The nanoFramework project contributors
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

__nfweak bool SSL_ServerInit( int sslMode, int sslVerify, const char* certificate, int certLength, const char* certPassword, int& sslContextHandle )
{ 
    (void)sslMode; (void)sslVerify; (void)certificate; (void)certLength; (void)certPassword;  (void)sslContextHandle;
    NATIVE_PROFILE_PAL_COM();
    return TRUE; 
}

__nfweak bool SSL_ClientInit( int sslMode, int sslVerify, const char* certificate, int certLength, const char* certPassword, int& sslContextHandle )
{ 
    (void)sslMode; (void)sslVerify; (void)certificate; (void)certLength; (void)certPassword;  (void)sslContextHandle;
    NATIVE_PROFILE_PAL_COM();
    return TRUE; 
}

__nfweak bool SSL_AddCertificateAuthority( int sslContextHandle, const char* certificate, int certLength, const char* certPassword )
{
    (void)sslContextHandle; (void)certificate; (void)certLength; (void)certPassword; 
    NATIVE_PROFILE_PAL_COM();
    return TRUE;
}

__nfweak bool SSL_ExitContext( int sslContextHandle )
{ 
    (void)sslContextHandle; 
    NATIVE_PROFILE_PAL_COM();
    return TRUE; 
}

__nfweak int SSL_Accept( SOCK_SOCKET socket, int sslContextHandle )
{ 
    (void)socket; (void)sslContextHandle; 
    NATIVE_PROFILE_PAL_COM();
    return 0; 
}

__nfweak int SSL_Connect( SOCK_SOCKET socket, const char* szTargetHost, int sslContextHandle )
{ 
    (void)socket; (void)szTargetHost;  (void)sslContextHandle;
    NATIVE_PROFILE_PAL_COM();
    return 0; 
}

__nfweak int SSL_Write( SOCK_SOCKET socket, const char* Data, size_t size )
{ 
    (void)socket; (void)Data;  (void)size;
    NATIVE_PROFILE_PAL_COM();
    return 0; 
}

__nfweak int SSL_Read( SOCK_SOCKET socket, char* Data, size_t size )
{ 
    (void)socket; (void)Data;  (void)size;
    NATIVE_PROFILE_PAL_COM();
    return 0; 
}

__nfweak int SSL_CloseSocket( SOCK_SOCKET socket )
{
    (void)socket; 
    NATIVE_PROFILE_PAL_COM();
    return 0;
}

__nfweak void SSL_GetTime(DATE_TIME_INFO* pdt)
{
    (void)pdt; 
    NATIVE_PROFILE_PAL_COM();
}

__nfweak void SSL_RegisterTimeCallback(SSL_DATE_TIME_FUNC pfn)
{
    (void)pfn; 
    NATIVE_PROFILE_PAL_COM();
}

__nfweak bool SSL_ParseCertificate( const char* certificate, size_t certLength, const char* password, X509CertData* certData )
{
    (void)certificate; (void)certLength; (void)password;  (void)certData; 

    NATIVE_PROFILE_PAL_COM();
    return TRUE;
}

__nfweak int SSL_DataAvailable( SOCK_SOCKET socket )
{
    (void)socket;
    NATIVE_PROFILE_PAL_COM();
    return 0;
}

