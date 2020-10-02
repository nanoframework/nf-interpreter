//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>

SSL_Driver g_SSL_Driver;

// Flag to postpone init until after heap has been cleared
// in tinyhal.cpp
static bool s_InitDone = false;

bool SSL_Initialize()
{
    NATIVE_PROFILE_PAL_COM();

    memset(&g_SSL_Driver, 0, sizeof(g_SSL_Driver));
    
    return true;
}

bool SSL_Uninitialize()
{
    NATIVE_PROFILE_PAL_COM();

    for(uint32_t i = 0; i<ARRAYSIZE(g_SSL_Driver.ContextArray); i++)
    {
        if(g_SSL_Driver.ContextArray[i].Context != NULL)
        {
            ssl_exit_context_internal( i );
        }
    }
      
    g_SSL_Driver.ContextCount = 0;

    s_InitDone = false;
    
    return true;
}

static bool SSL_GenericInit( 
    int sslMode, 
    int sslVerify, 
    const char* certificate, 
    int certLength, 
    const uint8_t* privateKey, 
    int privateKeyLength, 
    const char* password, 
    int passwordLength, 
    int& contextHandle, 
    bool isServer )
{
    if (!s_InitDone)
    {
        s_InitDone = ssl_initialize_internal();
    }

    return ssl_generic_init_internal( 
        sslMode, 
        sslVerify, 
        certificate, 
        certLength, 
        privateKey, 
        privateKeyLength, 
        password, 
        passwordLength, 
        contextHandle, 
        isServer );     
}

bool SSL_ParseCertificate( 
    const char* certificate, 
    size_t certLength, 
    const char* password, 
    X509CertData* certData )
{
    if (!s_InitDone)
    {
        s_InitDone = ssl_initialize_internal();
    }

    NATIVE_PROFILE_PAL_COM();

    return ssl_parse_certificate_internal(
        (void *)certificate,
        certLength,
        (void*)password, 
        (void*)certData);
}

int SSL_DecodePrivateKey( 
    const unsigned char *key,
    size_t keyLength, 
    const unsigned char *pwd, 
    size_t pwdLength )
{
    if (!s_InitDone)
    {
        s_InitDone = ssl_initialize_internal();
    }
    
    NATIVE_PROFILE_PAL_COM();

    return ssl_decode_private_key_internal(
        key,
        keyLength, 
        pwd, 
        pwdLength );
}

bool SSL_ServerInit( 
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
    NATIVE_PROFILE_PAL_COM();

    return SSL_GenericInit( 
        sslMode, 
        sslVerify, 
        certificate, 
        certLength, 
        privateKey, 
        privateKeyLength, 
        password, 
        passwordLength, 
        contextHandle, 
        true );
}

bool SSL_ClientInit( 
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
    NATIVE_PROFILE_PAL_COM();

    return SSL_GenericInit( 
        sslMode, 
        sslVerify, 
        certificate, 
        certLength, 
        privateKey, 
        privateKeyLength, 
        password, 
        passwordLength, 
        contextHandle, 
        false );
}

bool SSL_AddCertificateAuthority( 
    int contextHandle, 
    const char* certificate, 
    int certLength, 
    const char* certPassword )
{
    return ssl_add_cert_auth_internal(
        contextHandle, 
        certificate, 
        certLength, 
        certPassword);    
}

bool SSL_ExitContext( int contextHandle )
{ 
    return ssl_exit_context_internal(contextHandle);
}

int SSL_Accept( 
    SOCK_SOCKET socket, 
    int contextHandle )
{ 
    NATIVE_PROFILE_PAL_COM();

    return ssl_accept_internal(
        socket, 
        contextHandle);
}

int SSL_Connect( 
    SOCK_SOCKET socket, 
    const char* szTargetHost, 
    int contextHandle )
{ 
    NATIVE_PROFILE_PAL_COM();

    return ssl_connect_internal(
        socket, 
        szTargetHost, 
        contextHandle);
}

int SSL_Write( 
    SOCK_SOCKET socket, 
    const char* data, 
    size_t size  )
{ 
    NATIVE_PROFILE_PAL_COM();

    return ssl_write_internal(
        socket,
        data, 
        size);
}

int SSL_Read( 
    SOCK_SOCKET socket, 
    char* data, 
    size_t size )
{ 
    NATIVE_PROFILE_PAL_COM();

    return ssl_read_internal(
        socket, 
        data, 
        size);
}

int SSL_CloseSocket( SOCK_SOCKET socket )
{
    return ssl_close_socket_internal(socket);
}

int SSL_DataAvailable( SOCK_SOCKET socket )
{
    return ssl_pending_internal(socket);
}
