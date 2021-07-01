//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) The Mbed TLS Contributors. All Rights Reserved.
// See LICENSE file in the project root for full license information.
//

//////////////////////////////////////////////////////////////////////////////////////////////
// we are using the function declarations matching the mbedTLS ones BUT with weak attribute //
// if the image includes the mbedTLS, these will be replaced by the strong ones from there  //
// thus there will be no duplicate code                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Any changes here should be refelected in src\CLR\Helpers\Base64\base64.c //
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <base64.h>

static void mbedtls_base64_cond_assign_uint32(uint32_t *dest, const uint32_t src, uint32_t condition)
{
    /* MSVC has a warning about unary minus on unsigned integer types,
     * but this is well-defined and precisely what we want to do here. */
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4146)
#endif

    /* Generate bitmask from condition, mask will either be 0xFFFFFFFF or 0 */
    uint32_t mask = (condition | -condition);
    mask >>= 31;
    mask = -mask;

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

    *dest = (src & mask) | ((*dest) & ~mask);
}

static unsigned char mbedtls_base64_eq(size_t in_a, size_t in_b)
{
    size_t difference = in_a ^ in_b;

    /* MSVC has a warning about unary minus on unsigned integer types,
     * but this is well-defined and precisely what we want to do here. */
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4146)
#endif

    difference |= -difference;

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

    /* cope with the varying size of size_t per platform */
    difference >>= (sizeof(difference) * 8 - 1);

    return (unsigned char)(1 ^ difference);
}

static void mbedtls_base64_cond_assign_uchar(
    unsigned char *dest,
    const unsigned char *const src,
    unsigned char condition)
{
    /* MSVC has a warning about unary minus on unsigned integer types,
     * but this is well-defined and precisely what we want to do here. */
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4146)
#endif

    /* Generate bitmask from condition, mask will either be 0xFF or 0 */
    unsigned char mask = (condition | -condition);
    mask >>= 7;
    mask = -mask;

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

    *dest = ((*src) & mask) | ((*dest) & ~mask);
}

static unsigned char mbedtls_base64_table_lookup(
    const unsigned char *const table,
    const size_t table_size,
    const size_t table_index)
{
    size_t i;
    unsigned char result = 0;

    for (i = 0; i < table_size; ++i)
    {
        mbedtls_base64_cond_assign_uchar(&result, &table[i], mbedtls_base64_eq(i, table_index));
    }

    return result;
}

__nfweak int mbedtls_base64_encode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen)
{
    size_t i, n;
    int C1, C2, C3;
    unsigned char *p;

    if (slen == 0)
    {
        *olen = 0;
        return (0);
    }

    n = slen / 3 + (slen % 3 != 0);

    if (n > (BASE64_SIZE_T_MAX - 1) / 4)
    {
        *olen = BASE64_SIZE_T_MAX;
        return (MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL);
    }

    n *= 4;

    if ((dlen < n + 1) || (NULL == dst))
    {
        *olen = n + 1;
        return (MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL);
    }

    n = (slen / 3) * 3;

    for (i = 0, p = dst; i < n; i += 3)
    {
        C1 = *src++;
        C2 = *src++;
        C3 = *src++;

        *p++ = mbedtls_base64_table_lookup(base64_enc_map, sizeof(base64_enc_map), ((C1 >> 2) & 0x3F));

        *p++ =
            mbedtls_base64_table_lookup(base64_enc_map, sizeof(base64_enc_map), ((((C1 & 3) << 4) + (C2 >> 4)) & 0x3F));

        *p++ = mbedtls_base64_table_lookup(
            base64_enc_map,
            sizeof(base64_enc_map),
            ((((C2 & 15) << 2) + (C3 >> 6)) & 0x3F));

        *p++ = mbedtls_base64_table_lookup(base64_enc_map, sizeof(base64_enc_map), (C3 & 0x3F));
    }

    if (i < slen)
    {
        C1 = *src++;
        C2 = ((i + 1) < slen) ? *src++ : 0;

        *p++ = mbedtls_base64_table_lookup(base64_enc_map, sizeof(base64_enc_map), ((C1 >> 2) & 0x3F));

        *p++ =
            mbedtls_base64_table_lookup(base64_enc_map, sizeof(base64_enc_map), ((((C1 & 3) << 4) + (C2 >> 4)) & 0x3F));

        if ((i + 1) < slen)
        {
            *p++ = mbedtls_base64_table_lookup(base64_enc_map, sizeof(base64_enc_map), (((C2 & 15) << 2) & 0x3F));
        }
        else
        {
            *p++ = '=';
        }

        *p++ = '=';
    }

    *olen = p - dst;
    *p = 0;

    return 0;
}

__nfweak int mbedtls_base64_decode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen)
{
    size_t i, n;
    uint32_t j, x;
    unsigned char *p;
    unsigned char dec_map_lookup;

    /* First pass: check for validity and get output length */
    for (i = n = j = 0; i < slen; i++)
    {
        /* Skip spaces before checking for EOL */
        x = 0;
        while (i < slen && src[i] == ' ')
        {
            ++i;
            ++x;
        }

        /* Spaces at end of buffer are OK */
        if (i == slen)
        {
            break;
        }

        if ((slen - i) >= 2 && src[i] == '\r' && src[i + 1] == '\n')
        {
            continue;
        }

        if (src[i] == '\n')
        {
            continue;
        }

        /* Space inside a line is an error */
        if (x != 0)
        {
            return (MBEDTLS_ERR_BASE64_INVALID_CHARACTER);
        }

        if (src[i] == '=' && ++j > 2)
        {
            return (MBEDTLS_ERR_BASE64_INVALID_CHARACTER);
        }

        dec_map_lookup = mbedtls_base64_table_lookup(base64_dec_map, sizeof(base64_dec_map), src[i]);

        if (src[i] > 127 || dec_map_lookup == 127)
        {
            return (MBEDTLS_ERR_BASE64_INVALID_CHARACTER);
        }

        if (dec_map_lookup < 64 && j != 0)
        {
            return (MBEDTLS_ERR_BASE64_INVALID_CHARACTER);
        }

        n++;
    }

    if (n == 0)
    {
        *olen = 0;
        return (0);
    }

    /* The following expression is to calculate the following formula without
     * risk of integer overflow in n:
     *     n = ( ( n * 6 ) + 7 ) >> 3;
     */
    n = (6 * (n >> 3)) + ((6 * (n & 0x7) + 7) >> 3);
    n -= j;

    if (dst == NULL || dlen < n)
    {
        *olen = n;
        return (MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL);
    }

    for (j = 3, n = x = 0, p = dst; i > 0; i--, src++)
    {
        if (*src == '\r' || *src == '\n' || *src == ' ')
        {
            continue;
        }

        dec_map_lookup = mbedtls_base64_table_lookup(base64_dec_map, sizeof(base64_dec_map), *src);

        mbedtls_base64_cond_assign_uint32(&j, j - 1, mbedtls_base64_eq(dec_map_lookup, 64));
        x = (x << 6) | (dec_map_lookup & 0x3F);

        if (++n == 4)
        {
            n = 0;
            if (j > 0)
            {
                *p++ = (unsigned char)(x >> 16);
            }

            if (j > 1)
            {
                *p++ = (unsigned char)(x >> 8);
            }

            if (j > 2)
            {
                *p++ = (unsigned char)(x);
            }
        }
    }

    *olen = p - dst;

    return (0);
}
