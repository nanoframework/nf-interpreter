//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Base64 encode/decode for MCUboot serial recovery.
//
// Implements the Mynewt-compatible base64 functions used by boot_serial.c in the
// non-Zephyr, non-ESP32 (Mynewt) code path.

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "base64/base64.h"

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_encode(const void *data, int src_len, char *dst, uint8_t should_pad)
{
    const uint8_t *src = (const uint8_t *)data;
    char *p = dst;
    int i = 0;
    char *last = NULL;

    while (i < src_len)
    {
        uint32_t c = (uint32_t)src[i++] << 16;

        if (i < src_len)
        {
            c |= (uint32_t)src[i] << 8;
        }

        i++;

        if (i < src_len)
        {
            c |= (uint32_t)src[i];
        }

        i++;

        last = p;
        p[0] = b64_table[(c >> 18) & 0x3FU];
        p[1] = b64_table[(c >> 12) & 0x3FU];
        p[2] = b64_table[(c >> 6) & 0x3FU];
        p[3] = b64_table[(c) & 0x3FU];
        p += 4;
    }

    if (last != NULL)
    {
        // number of padding bytes needed (0, 1, or 2)
        int diff = i - src_len;

        if (diff > 0)
        {
            if (should_pad)
            {
                // Fill padding positions with '='
                for (int k = 0; k < diff; k++)
                {
                    last[4 - diff + k] = '=';
                }
            }
            else
            {
                // Trim the unused output characters
                p = last + (4 - diff);
            }
        }
    }

    *p = '\0';

    return (int)(p - dst);
}

// Decode a single base64 character; returns the 6-bit value or -1 on error.
static int b64_char_value(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - 'A';
    }

    if (c >= 'a' && c <= 'z')
    {
        return c - 'a' + 26;
    }

    if (c >= '0' && c <= '9')
    {
        return c - '0' + 52;
    }

    if (c == '+')
    {
        return 62;
    }

    if (c == '/')
    {
        return 63;
    }

    // invalid (including '=')
    return -1;
}

int base64_decode_len(const char *src)
{
    int raw_len = (int)strlen(src);

    // Strip trailing whitespace (\n, \r) before counting padding
    while (raw_len > 0 && (src[raw_len - 1] == '\n' || src[raw_len - 1] == '\r'))
    {
        raw_len--;
    }

    // Strip trailing padding
    int len = raw_len;

    while (len > 0 && src[len - 1] == '=')
    {
        len--;
    }

    // Each 4 base64 chars encode 3 bytes; account for partial groups
    return (len * 3) / 4;
}

int base64_decode(const char *src, void *dst)
{
    uint8_t *out = (uint8_t *)dst;
    int out_len = 0;
    int src_len = (int)strlen(src);

    for (int i = 0; i < src_len;)
    {
        // Skip whitespace (\n, \r) between groups — NLIP frames end with \n
        while (i < src_len && (src[i] == '\n' || src[i] == '\r'))
        {
            i++;
        }
        if (i >= src_len)
        {
            break;
        }

        // Consume up to 4 characters
        int vals[4] = {0, 0, 0, 0};
        int padding = 0;
        int j;

        for (j = 0; j < 4; j++)
        {
            if (i >= src_len || src[i] == '\0')
            {
                // Short last group — treat as error (NLIP frames are padded)
                if (j < 2)
                {
                    return -1;
                }

                vals[j] = 0;
                padding++;
            }
            else if (src[i] == '=')
            {
                vals[j] = 0;
                padding++;
                i++;
            }
            else
            {
                int v = b64_char_value(src[i]);
                if (v < 0)
                {
                    return -1;
                }
                vals[j] = v;
                i++;
            }
        }

        // Reconstruct up to 3 bytes from the 4 6-bit values
        uint32_t combined = (uint32_t)((vals[0] << 18) | (vals[1] << 12) | (vals[2] << 6) | vals[3]);
        out[out_len++] = (uint8_t)((combined >> 16) & 0xFFU);
        if (padding < 2)
        {
            out[out_len++] = (uint8_t)((combined >> 8) & 0xFFU);
        }
        if (padding < 1)
        {
            out[out_len++] = (uint8_t)(combined & 0xFFU);
        }
    }

    return out_len;
}
