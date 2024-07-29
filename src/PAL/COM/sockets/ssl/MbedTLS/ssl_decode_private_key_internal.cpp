//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "mbedtls.h"

extern void SSL_GetCertDateTime_internal(DATE_TIME_INFO *dt, mbedtls_x509_time *mt);

#if defined(PLATFORM_ESP32)
int random(void *ctx, unsigned char *buf, size_t len)
{
    return mbedtls_ctr_drbg_random(ctx, buf, len);
}
#else

#include <hal.h>
#include <hal_nf_community.h>

int random(void *context, unsigned char *output, size_t output_size)
{
    (void)context;

    // start random generator
    rngStart();

    for (size_t i = 0; i < output_size; i++)
    {
        // our generator returns 32bits numbers
        *output = (uint8_t)rngGenerateRandomNumber();

        output++;
    }

    return 0;
}
#endif

int ssl_decode_private_key_internal(
    const unsigned char *key,
    size_t keyLength,
    const unsigned char *password,
    size_t passwordLength)
{
    mbedtls_pk_context pkey;

    int retCode;

    mbedtls_pk_init(&pkey);

    /////////////////////////////////////////////////////////////////////////////////////////////////
    // developer notes:                                                                            //
    // this call parses certificates in both string and binary formats                             //
    // when the formart is a string it has to include the terminator otherwise the parse will fail //
    /////////////////////////////////////////////////////////////////////////////////////////////////
#if MBEDTLS_VERSION_MAJOR < 3
    retCode = mbedtls_pk_parse_key(&pkey, key, keyLength, password, passwordLength);
#else
    retCode = mbedtls_pk_parse_key(&pkey, key, keyLength, password, passwordLength, random, NULL);
#endif

    // need to free this here
    mbedtls_pk_free(&pkey);

    return retCode;
}
