//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>

SSL_Driver g_SSL_Driver;

// Flag to postpone init until after heap has been cleared
// in tinyhal.cpp
static bool s_init_done = false;


bool SSL_Initialize()
{
    NATIVE_PROFILE_PAL_COM();

    memset(&g_SSL_Driver, 0, sizeof(g_SSL_Driver));
    
    return true;
}

bool SSL_Uninitialize()
{
    NATIVE_PROFILE_PAL_COM();
    bool retVal = true;

    retVal = ssl_uninitialize_internal();

    s_init_done = FALSE;
    
    return retVal;
}

static bool SSL_GenericInit( int sslMode, int sslVerify, const char* certificate, int certLength, const char* pwd, int& sslContextHandle, bool isServer )
{
    if (!s_init_done) s_init_done=ssl_initialize_internal();
    return ssl_generic_init_internal( sslMode, sslVerify, certificate, certLength, pwd, sslContextHandle, isServer );     
}

bool SSL_ParseCertificate( const char* certificate, size_t certLength, const char* password, X509CertData* certData )
{
    if (!s_init_done) s_init_done=ssl_initialize_internal();
    NATIVE_PROFILE_PAL_COM();
    return ssl_parse_certificate_internal((void *)certificate,
                                          certLength,
                                          (void*)password, (void*)certData);
}

void SSL_RegisterTimeCallback(SSL_DATE_TIME_FUNC pfn)
{
    NATIVE_PROFILE_PAL_COM();
    g_SSL_Driver.m_pfnGetTimeFuncPtr = pfn;
}

bool SSL_ServerInit( int sslMode, int sslVerify, const char* certificate, int certLength, const char* certPassword, int& sslContextHandle )
{
    NATIVE_PROFILE_PAL_COM();
    return SSL_GenericInit( sslMode, sslVerify, certificate, certLength, certPassword, sslContextHandle, TRUE );
}

bool SSL_ClientInit( int sslMode, int sslVerify, const char* certificate, int certLength, const char* certPassword, int& sslContextHandle )
{ 
    NATIVE_PROFILE_PAL_COM();
    return SSL_GenericInit( sslMode, sslVerify, certificate, certLength, certPassword, sslContextHandle, FALSE );
}

bool SSL_AddCertificateAuthority( int sslContextHandle, const char* certificate, int certLength, const char* certPassword )
{
    return ssl_add_cert_auth_internal(sslContextHandle, certificate, certLength, certPassword);    
}

bool SSL_ExitContext( int sslContextHandle )
{ 
    return ssl_exit_context_internal(sslContextHandle);
}

int SSL_Accept( SOCK_SOCKET socket, int sslContextHandle )
{ 
    NATIVE_PROFILE_PAL_COM();
    return ssl_accept_internal(socket, sslContextHandle);
}

int SSL_Connect( SOCK_SOCKET socket, const char* szTargetHost, int sslContextHandle )
{ 
    NATIVE_PROFILE_PAL_COM();
    return ssl_connect_internal(socket, szTargetHost, sslContextHandle);
}


int SSL_Write( SOCK_SOCKET socket, const char* Data, size_t size  )
{ 
    NATIVE_PROFILE_PAL_COM();
    return ssl_write_internal(socket, Data, size);
}

int SSL_Read( SOCK_SOCKET socket, char* Data, size_t size )
{ 
    NATIVE_PROFILE_PAL_COM();
    return ssl_read_internal(socket, Data, size);
}

int SSL_CloseSocket( SOCK_SOCKET socket )
{
    return ssl_closesocket_internal(socket);
}

int SSL_DataAvailable( SOCK_SOCKET socket )
{
    return ssl_pending_internal(socket);
}
