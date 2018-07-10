//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include <openssl/ssl.h>
#include <openssl.h> 

// #include <tinyclr/ssl_functions.h>
// #include <openssl/ssl.h>
// #include <openssl.h>

void ssl_clear_cert_auth_internal( int sslContextHandle )
{
#ifndef NO_CERT_STORE
    SSL *ssl = NULL;
    
    if((sslContextHandle >= (int)ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0))
    {
        return;
    }

    ssl = (SSL*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    if (ssl == NULL)
    {
        return;
    }

    // Set a NULL cert store, SSL_CTX_set_cert_store will free the existing cert store
    SSL_CTX_set_cert_store( SSL_get_SSL_CTX(ssl), NULL );
#else
    (void)sslContextHandle;
#endif
}


