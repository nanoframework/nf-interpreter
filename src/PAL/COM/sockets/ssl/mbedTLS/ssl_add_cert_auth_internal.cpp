//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

bool ssl_add_cert_auth_internal( 
    int contextHandle, 
    const char* certificate, 
    int certLength, 
    const char* certPassword )
{
    (void)certPassword;

    mbedTLS_NFContext* context;

    // Check contextHandle range
    if((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0))
    {
        return false;
    }

    // Retrieve SSL struct from g_SSL_Driver    
    // sd should already have been created
    context = (mbedTLS_NFContext*)g_SSL_Driver.ContextArray[contextHandle].Context;

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
            mbedtls_ssl_conf_ca_chain( 
                context->conf, 
                context->x509_crt, 
                NULL );

            // done
            return true;
        }
    }

    return false;
}
