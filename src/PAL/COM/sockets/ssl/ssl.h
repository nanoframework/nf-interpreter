//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANO_SSL_H_
#define _NANO_SSL_H_ 1

#include <ssl_types.h>
#include <ssl_functions.h>

struct SSL_Driver
{
    static const int c_MaxSslContexts = SOCKETS_MAX_COUNT;
 
    //--//

    struct SSL_Context
    {
        // TODO - FIXME
        //mbedtls_ssl_context SslContext;
        void * SslContext;
    };

   SSL_Context    m_sslContextArray[c_MaxSslContexts];
   int32_t        m_sslContextCount;   
   SSL_DATE_TIME_FUNC m_pfnGetTimeFuncPtr;
 
};

extern SSL_Driver g_SSL_Driver;


#endif
