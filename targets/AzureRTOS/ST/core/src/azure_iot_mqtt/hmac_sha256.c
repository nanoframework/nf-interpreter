/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
   
#include "hmac_sha256.h"

#include "sha256.h"

#define B 64
#define L (SHA256_DIGEST_SIZE)
#define K (SHA256_DIGEST_SIZE * 2)

#define I_PAD 0x36
#define O_PAD 0x5C

void hmac_sha256(
    uint8_t out[HMAC_SHA256_DIGEST_SIZE],
    const uint8_t* data, size_t data_len,
    const uint8_t* key, size_t key_len)
{
    sha256_t ss;
    uint8_t kh[SHA256_DIGEST_SIZE];

    if (key_len > B) 
    {
        sha256_init(&ss);
        sha256_update(&ss, key, key_len);
        sha256_final(&ss, kh);
        key_len = SHA256_DIGEST_SIZE;
        key = kh;
    }

    uint8_t kx[B];
    for (size_t i = 0; i < key_len; i++) kx[i] = I_PAD ^ key[i];
    for (size_t i = key_len; i < B; i++) kx[i] = I_PAD ^ 0;

    sha256_init(&ss);
    sha256_update(&ss, kx, B);
    sha256_update(&ss, data, data_len);
    sha256_final(&ss, out);

    for (size_t i = 0; i < key_len; i++) kx[i] = O_PAD ^ key[i];
    for (size_t i = key_len; i < B; i++) kx[i] = O_PAD ^ 0;

    sha256_init(&ss);
    sha256_update(&ss, kx, B);
    sha256_update(&ss, out, SHA256_DIGEST_SIZE);
    sha256_final(&ss, out);
}
