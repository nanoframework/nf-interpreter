/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

// https://docs.microsoft.com/azure/iot-hub/iot-hub-mqtt-support

#include "sas_token.h"

#include <stdio.h>
#include <string.h>

#include "hmac_sha256.h"

// Expire in one year minus one day
#define SAS_EXPIRATION_SECS     (364 * 24 * 60 * 60)
#define SAS_DPS_EXPIRATION_SECS (60 * 60)

static bool base64_encode(char* src, size_t src_len, char* out)
{
    char* o = out;
    char* p = src;

    const char* b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    for (; p < (src + src_len - 2); p += 3)
    {
        *o++ = b[(p[0] >> 2) & 0x3f];
        *o++ = b[((p[0] & 0x03) << 4) | ((p[1] & 0xf0) >> 4)];
        *o++ = b[((p[1] & 0x0f) << 2) | ((p[2] & 0xc0) >> 6)];
        *o++ = b[p[2] & 0x3f];
    }

    if (p < src + src_len)
    {
        *o++ = b[(p[0] >> 2) & 0x3f];

        if (p == src + src_len - 1)
        {
            *o++ = b[((p[0] & 0x03) << 4)];
            *o++ = '=';
        }

        else
        {
            *o++ = b[((p[0] & 0x03) << 4) | ((p[1] & 0xf0) >> 4)];
            *o++ = b[(p[1] & 0x0f) << 2];
        }

        *o++ = '=';
    }

    *o = 0;

    return true;
}

static size_t base64_decode(char* src, size_t len, char* out)
{
    unsigned char* o = (unsigned char*)out;
    unsigned char* p = (unsigned char*)src;

    const unsigned char b[256] = {64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        62,
        64,
        64,
        64,
        63,
        52,
        53,
        54,
        55,
        56,
        57,
        58,
        59,
        60,
        61,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        64,
        64,
        64,
        64,
        64,
        64,
        26,
        27,
        28,
        29,
        30,
        31,
        32,
        33,
        34,
        35,
        36,
        37,
        38,
        39,
        40,
        41,
        42,
        43,
        44,
        45,
        46,
        47,
        48,
        49,
        50,
        51,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64,
        64};

    while (len > 4)
    {
        *o++ = b[p[0]] << 2 | b[p[1]] >> 4;
        *o++ = b[p[1]] << 4 | b[p[2]] >> 2;
        *o++ = b[p[2]] << 6 | b[p[3]];

        p += 4;
        len -= 4;
    }

    if (len > 1)
        *o++ = b[p[0]] << 2 | b[p[1]] >> 4;
    if (len > 2)
        *o++ = b[p[1]] << 4 | b[p[2]] >> 2;
    if (len > 3)
        *o++ = b[p[2]] << 6 | b[p[3]];

    *o++ = 0;

    return (o - (unsigned char*)out);
}

static size_t base64_decode_length(char* str, size_t len)
{
    size_t outlen = (len * 6) / 8;

    for (char* p = str + len - 1; *p == '='; p--)
    {
        outlen--;
    }

    return outlen;
}

static int url_encode(char* dest, char* msg)
{
    const char* hex = "0123456789abcdef";
    char* startPtr  = dest;

    while (*msg != 0)
    {
        if (('a' <= *msg && *msg <= 'z') || ('A' <= *msg && *msg <= 'Z') || ('0' <= *msg && *msg <= '9'))
        {
            *dest++ = *msg;
        }
        else
        {
            *dest++ = '%';
            *dest++ = hex[*msg >> 4];
            *dest++ = hex[*msg & 15];
        }
        msg++;
    }
    *dest = 0;
    return dest - startPtr;
}

bool create_sas_token(char* key,
    unsigned int key_size,
    char* hostname,
    char* device_id,
    unsigned long valid_until,
    char* output,
    unsigned int output_size)
{
    char buffer[128];
    char key_binary[96];
    int key_binary_size;
    char hash[32];
    char encoded_hash[44 + 1];

    char* output_end = output + output_size;

    valid_until += SAS_EXPIRATION_SECS;
    snprintf(buffer, sizeof(buffer), "%s%%2Fdevices%%2F%s\n%lu", hostname, device_id, valid_until);

    base64_decode(key, key_size, key_binary);
    key_binary_size = base64_decode_length(key, key_size);

    hmac_sha256(
        (unsigned char*)hash, (unsigned char*)buffer, strlen(buffer), (unsigned char*)key_binary, key_binary_size);

    base64_encode(hash, sizeof(hash), encoded_hash);

    // Create the output SAS token
    output +=
        snprintf(output, output_end - output, "SharedAccessSignature sr=%s%%2Fdevices%%2F%s&sig=", hostname, device_id);
    output += url_encode(output, encoded_hash);
    output += snprintf(output, output_end - output, "&se=%lu", valid_until);

    if ((output_end - output) < 2)
    {
        return false;
    }

    return true;
}

bool create_dps_sas_token(char* key,
    unsigned int key_size,
    char* id_scope,
    char* registration_id,
    unsigned long valid_until,
    char* output,
    unsigned int output_size)
{
    char buffer[128];
    char key_binary[96];
    int key_binary_size;
    char hash[32];
    char encoded_hash[44 + 1];

    char* output_end = output + output_size;

    valid_until += SAS_DPS_EXPIRATION_SECS;
    snprintf(buffer, sizeof(buffer), "%s%%2Fregistrations%%2F%s\n%lu", id_scope, registration_id, valid_until);

    base64_decode(key, key_size, key_binary);
    key_binary_size = base64_decode_length(key, key_size);

    hmac_sha256(
        (unsigned char*)hash, (unsigned char*)buffer, strlen(buffer), (unsigned char*)key_binary, key_binary_size);

    base64_encode(hash, sizeof(hash), encoded_hash);

    // Create the output SAS token
    output += snprintf(output,
        output_end - output,
        "SharedAccessSignature sr=%s%%2Fregistrations%%2F%s&sig=",
        id_scope,
        registration_id);
    output += url_encode(output, encoded_hash);
    output += snprintf(output, output_end - output, "&se=%lu&skn=registration", valid_until);

    if ((output_end - output) < 2)
    {
        return false;
    }

    return true;
}
