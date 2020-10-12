//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) https://www.mycplus.com. All Rights Reserved.
// See LICENSE file in the project root for full license information.
//

//////////////////////////////////////////////////////////////////////////////////////////////
// we are using the function declarations matching the mbedTLS ones BUT with weak attribute //
// if the image includes the mbedTLS, these will be replaced by the strong ones from there  //
// thus there will be no duplicate code                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////

#include "base64.h"

void build_decoding_table(char *decoding_table ) 
{
    if(decoding_table != NULL)
    {
        for (int i = 0; i < 64; i++)
            decoding_table[(unsigned char) base64_enc_map[i]] = i;
    }
}

__nfweak int mbedtls_base64_encode( 
    unsigned char *dst, 
    size_t dlen, 
    size_t *olen,
    const unsigned char *src, size_t slen )
{
    // parameter not used
    (void)dlen;

    for (unsigned int i = 0, j = 0; i < slen;) 
    {
        int octet_a = i < slen ? (unsigned char)src[i++] : 0;
        int octet_b = i < slen ? (unsigned char)src[i++] : 0;
        int octet_c = i < slen ? (unsigned char)src[i++] : 0;

        int triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        dst[j++] = base64_enc_map[(triple >> 3 * 6) & 0x3F];
        dst[j++] = base64_enc_map[(triple >> 2 * 6) & 0x3F];
        dst[j++] = base64_enc_map[(triple >> 1 * 6) & 0x3F];
        dst[j++] = base64_enc_map[(triple >> 0 * 6) & 0x3F];
    }

    for (unsigned int i = 0; i < mod_table[slen % 3]; i++)
        dst[*olen - 1 - i] = '=';

    // add terminator
    dst[*olen] = 0;

    return 0;
}

__nfweak int mbedtls_base64_decode( 
    unsigned char *dst, 
    size_t dlen, 
    size_t *olen,
    const unsigned char *src, size_t slen )
{
    // parameter not used
    (void)dlen;

    char *decoding_table = (char*)platform_malloc(256);

    if (decoding_table == NULL)
    {
        // OK to return anything but 0
        // we are forced to use mbedTLS behaviour anyway
         return -1;
    }

    build_decoding_table(decoding_table);

    if (slen % 4 != 0)
    {
        // OK to return anything but 0
        // we are forced to use mbedTLS behaviour anyway
         return -1;
    }

    for (unsigned int i = 0, j = 0; i < slen;) {

        int sextet_a = src[i] == '=' ? 0 & i++ : decoding_table[src[i++]];
        int sextet_b = src[i] == '=' ? 0 & i++ : decoding_table[src[i++]];
        int sextet_c = src[i] == '=' ? 0 & i++ : decoding_table[src[i++]];
        int sextet_d = src[i] == '=' ? 0 & i++ : decoding_table[src[i++]];

        int triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *olen) dst[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *olen) dst[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *olen) dst[j++] = (triple >> 0 * 8) & 0xFF;
    }

    // free table
    platform_free(decoding_table);

    return( 0 );
}
