//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 

#include <openssl/ssl.h>


// #include <tinyclr/ssl_functions.h>
// #include <openssl/ssl.h>
// #include <openssl.h>
// #include <Crypto/crypto.h>

bool ssl_uninitialize_internal()
{
    bool result = TRUE;
    
    for(uint32_t i = 0; i<ARRAYSIZE(g_SSL_Driver.m_sslContextArray); i++)
    {
        if(g_SSL_Driver.m_sslContextArray[i].SslContext != NULL)
        {
            ssl_exit_context_internal( i );
        }
    }
      
    g_SSL_Driver.m_sslContextCount = 0;

// TODO - FIXME
 //   CRYPTO_cleanup_all_ex_data();
 //   EVP_cleanup();

    return result;
}




