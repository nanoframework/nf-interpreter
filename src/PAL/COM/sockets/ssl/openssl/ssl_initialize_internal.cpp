//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include <openssl/ssl.h>
#include <openssl.h> 

// FIXME - extern int64_t s_TimeUntil1900;

bool ssl_initialize_internal()
{
    NANOCLR_SSL_MEMSET(&g_SSL_Driver, 0, sizeof(g_SSL_Driver));

// FIXME   s_TimeUntil1900 = 0;

#ifndef NO_LIBRARY_INIT
//    SSL_COMP_init();
    SSL_library_init();
//    SSL_load_error_strings();
#endif

    return TRUE;
}

