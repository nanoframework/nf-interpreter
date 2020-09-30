//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) ARM Limited. All Rights Reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef __BASE64_H__
#define __BASE64_H__

#include <stdio.h>
#include <nanoWeak.h>
#include <nanoHAL_v2.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// we are using the function declarations matching the mbedTLS ones BUT with weak attribute //
// if the image includes the mbedTLS, these will be replaced by the strong ones from there  //
// thus there will be no duplicate code                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

int mbedtls_base64_encode( unsigned char *dst, size_t dlen, size_t *olen,
                   const unsigned char *src, size_t slen );
int mbedtls_base64_decode( unsigned char *dst, size_t dlen, size_t *olen,
                   const unsigned char *src, size_t slen );

static const unsigned char base64_enc_map[64] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/'
};

static const unsigned int mod_table[3] = {0, 2, 1};

#ifdef __cplusplus
}
#endif

#endif //__BASE64_H__ 
