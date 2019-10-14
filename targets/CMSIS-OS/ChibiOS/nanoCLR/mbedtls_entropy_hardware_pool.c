//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_nf_community.h>

// Get len bytes of entropy from the hardware RNG.
int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen )
{
    (void)data;

    // start random generator
    rngStart();

    for(size_t i = 0; i < len; i++)
    {
        // our generator returns 32bits numbers
        *output = rngGenerateRandomNumber();

        output++;
    }

    // callers require this to be set
    *olen = len;

    // stop random generator
    rngStop();

    return 0;
}
