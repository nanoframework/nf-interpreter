//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include "mbedtls.h"

// TODO if and when implementing store certificate
// extern volatile int ssl_x509_store_ctx_idx;

bool ssl_uninitialize_internal()
{
    bool result = true;
    
    for(uint32_t i = 0; i<ARRAYSIZE(g_SSL_Driver.ContextArray); i++)
    {
        if(g_SSL_Driver.ContextArray[i].Context != NULL)
        {
            ssl_exit_context_internal( i );
        }
    }
      
    g_SSL_Driver.ContextCount = 0;

    return result;
}
