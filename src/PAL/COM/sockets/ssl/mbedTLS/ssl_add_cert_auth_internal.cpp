//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

bool ssl_add_cert_auth_internal( int sslContextHandle, const char* certificate, int certLength, const char* certPassword )
{
    (void)certPassword;

    mbedTLS_NFContext* context;

    // Check sslContextHandle range
    if((sslContextHandle >= (int)ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0))
    {
        return FALSE;
    }

    // Retrieve SSL struct from g_SSL_Driver    
    // sd should already have been created
    context = (mbedTLS_NFContext*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;

    if (context != NULL)
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////
        // developer notes:                                                                            //
        // this call parses certificates in both string and binary formats                             //
        // when the formart is a string it has to include the terminator otherwise the parse will fail //
        /////////////////////////////////////////////////////////////////////////////////////////////////
        if( mbedtls_x509_crt_parse(context->x509_crt, (const unsigned char*)certificate, certLength ) == 0)
        {
            // add to CA chain
            mbedtls_ssl_conf_ca_chain( context->conf, context->x509_crt, NULL );

            // done
            return TRUE;
        }
    }

    return false;
}
