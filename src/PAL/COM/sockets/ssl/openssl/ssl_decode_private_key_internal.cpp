//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include <openssl/ssl.h>
#include <openssl.h> 


// TODO FIXME
// not implemented
int ssl_decode_private_key_internal(
    const unsigned char *key, 
    size_t keyLength, 
    const unsigned char *password, 
    size_t passwordLength)
{
    (void)key;
    (void)keyLength;
    (void)password;
    (void)passwordLength;

    return -1;    
}
