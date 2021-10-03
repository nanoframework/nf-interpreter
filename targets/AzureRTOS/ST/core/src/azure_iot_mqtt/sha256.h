/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
   
#ifndef _SHA256_H
#define _SHA256_H

#include <stdint.h>
#include <stddef.h>

#define SHA256_DIGEST_SIZE 32

typedef struct
{
    uint32_t state[8];
    uint64_t count;
    unsigned char buffer[64];
} sha256_t;

void sha256_init(sha256_t* p);
void sha256_update(sha256_t* p, const unsigned char* data, size_t size);
void sha256_final(sha256_t* p, unsigned char* digest);

#endif // _SHA256_H
