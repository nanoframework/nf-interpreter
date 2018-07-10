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

bool ssl_exit_context_internal(int sslContextHandle )
{
    if((sslContextHandle >= (int)ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0) || (g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext == NULL))
    {
        return FALSE;
    }

    SSL *ssl = (SSL*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    
    SSL_free(ssl);

    NANOCLR_SSL_MEMSET(&g_SSL_Driver.m_sslContextArray[sslContextHandle], 0, sizeof(g_SSL_Driver.m_sslContextArray[sslContextHandle]));

    g_SSL_Driver.m_sslContextCount --;

    return TRUE;
}


