//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NANO_SSL_H
#define NANO_SSL_H

#include <ssl_types.h>
#include <ssl_functions.h>

struct SSL_Driver
{
    static const int c_MaxSslContexts = SOCKETS_MAX_COUNT;
 
    //--//

    struct SSL_Context
    {
        void * Context;
    };

   SSL_Context    ContextArray[c_MaxSslContexts];
   int32_t        ContextCount; 
};

extern SSL_Driver g_SSL_Driver;

#endif // NANO_SSL_H
