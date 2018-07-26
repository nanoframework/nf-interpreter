//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include "mbedtls.h"

bool ssl_generic_init_internal( int sslMode, int sslVerify, const char* certificate, 
    int certLength, const char* certPassword, int& sslContextHandle, bool isServer )
{
    (void)sslMode;
    (void)sslVerify;
    (void)certificate;
    (void)certLength;
    (void)certPassword;
    (void)sslContextHandle;
    (void)isServer;
    
    return false;
}
