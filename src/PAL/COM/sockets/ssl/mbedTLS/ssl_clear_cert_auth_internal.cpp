//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

// TODO FIXME - no Certificate store

void ssl_clear_cert_auth_internal(int sslContextHandle )
{
  #ifndef NO_CERT_STORE

    mbedTLS_NFContext* context;

    // Check sslContextHandle range
    if((sslContextHandle >= (int)ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0))
    {
        return;
    }
    
    // Retrieve SSL struct from g_SSL_Driver    
    // sd should already have been created
    // Now do the SSL negotiation
    context = (mbedTLS_NFContext*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    if (context == NULL)
    {
        return;
    }

    // // Set a NULL cert store, SSL_CTX_set_cert_store will free the existing cert store
    // SSL_CTX_set_cert_store( SSL_get_SSL_CTX(ssl), NULL );
  #else

    (void)sslContextHandle;

  #endif
}
