//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Provides Mynewt-compatible signatures

#ifndef BASE64_BASE64_H
#define BASE64_BASE64_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Worst-case output size for base64 encoding (no newlines, with NUL).
// Matches the Mynewt definition used by boot_serial.c.
#define BASE64_ENCODE_SIZE(__size) (((((__size) - 1) / 3) * 4) + 4)

    // Encode src_len raw bytes from data into the base64 string at dst.
    // should_pad: 1 = append '=' padding, 0 = no padding.
    // Returns the number of ASCII characters written (excluding NUL).
    // dst must be at least BASE64_ENCODE_SIZE(src_len) + 1 bytes.
    int base64_encode(const void *data, int src_len, char *dst, uint8_t should_pad);

    // Decode the null-terminated base64 string src into raw bytes at dst.
    // dst must be large enough for the decoded data (use base64_decode_len() first).
    // Returns the number of bytes written to dst, or a negative value on error.
    int base64_decode(const char *src, void *dst);

    // Return the number of raw bytes that a base64 string would decode to.
    int base64_decode_len(const char *src);

#ifdef __cplusplus
}
#endif

#endif // BASE64_BASE64_H
