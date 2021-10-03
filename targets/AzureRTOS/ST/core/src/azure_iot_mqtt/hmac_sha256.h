/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
   
#ifndef _HMAC_SHA256_H
#define _HMAC_SHA256_H

#include <stdint.h>
#include <stddef.h>

#define HMAC_SHA256_DIGEST_SIZE 32

void hmac_sha256(
    uint8_t out[HMAC_SHA256_DIGEST_SIZE],
    const uint8_t* data, size_t data_len,
    const uint8_t* key, size_t key_len);

#endif // _HMAC_SHA256_H
