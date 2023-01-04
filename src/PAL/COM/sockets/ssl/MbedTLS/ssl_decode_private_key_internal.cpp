//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "mbedtls.h"

extern void SSL_GetCertDateTime_internal(DATE_TIME_INFO * dt, mbedtls_x509_time * mt );

int ssl_decode_private_key_internal(
    const unsigned char *key, 
    size_t keyLength, 
    const unsigned char *password, 
    size_t passwordLength)
{
    mbedtls_pk_context pkey;
    
    int retCode;

    mbedtls_pk_init( &pkey );

    /////////////////////////////////////////////////////////////////////////////////////////////////
    // developer notes:                                                                            //
    // this call parses certificates in both string and binary formats                             //
    // when the formart is a string it has to include the terminator otherwise the parse will fail //
    /////////////////////////////////////////////////////////////////////////////////////////////////
    retCode = mbedtls_pk_parse_key( 
        &pkey, 
        key,
        keyLength, 
        password, 
        passwordLength);


    // need to free this here
    mbedtls_pk_free( &pkey );

    return retCode;
}
