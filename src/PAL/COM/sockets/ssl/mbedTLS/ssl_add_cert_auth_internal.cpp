//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

// TODO FIXME - no Certificate store

#ifdef NO_CERT_STORE

bool ssl_add_cert_auth_internal( int sslContextHandle, const char* certificate, int certLength, const char* certPassword )
{
    (void)sslContextHandle;
    (void)certificate;
    (void)certLength;
    (void)certPassword;
    
    return false;
}

#else

bool ssl_add_cert_auth_internal( int sslContextHandle, const char* certificate, int certLength, const char* certPassword )
{
    (void)certificate;
    (void)certLength;
    (void)certPassword;
    
    mbedTLS_NFContext* context;

    // Check sslContextHandle range
    if((sslContextHandle >= (int)ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0))
    {
        return false;
    }
    
    // Retrieve SSL struct from g_SSL_Driver    
    // sd should already have been created
    // Now do the SSL negotiation
    context = (mbedTLS_NFContext*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    if (context == NULL)
    {
        return false;
    }

    return false;
}

#endif
