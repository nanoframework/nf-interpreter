//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) The Mbed TLS Contributors. All Rights Reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef BASE64_H
#define BASE64_H

#include <nanoHAL_v2.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// we are using the function declarations matching the mbedTLS ones BUT with weak attribute //
// if the image includes the mbedTLS, these will be replaced by the strong ones from there  //
// thus there will be no duplicate code                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////

#define BASE64_SIZE_T_MAX                    ((size_t)-1) /* SIZE_T_MAX is not standard */
#define MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL  -0x002A      /**< Output buffer too small. */
#define MBEDTLS_ERR_BASE64_INVALID_CHARACTER -0x002C      /**< Invalid character in input. */

#ifdef __cplusplus
extern "C"
{
#endif

    int mbedtls_base64_encode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen);
    int mbedtls_base64_decode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen);

    static const unsigned char base64_enc_map[64] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
        's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

    static const unsigned char base64_dec_map[128] = {
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 62,
        127, 127, 127, 63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  127, 127, 127, 64,  127, 127, 127, 0,
        1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,
        23,  24,  25,  127, 127, 127, 127, 127, 127, 26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
        39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  127, 127, 127, 127, 127};

#ifdef __cplusplus
}
#endif

#endif //BASE64_H
