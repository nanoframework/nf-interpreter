//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include "mbedtls.h"


bool ssl_exit_context_internal(int sslContextHandle )
{
    mbedTLS_NFContext* context = NULL;

    if((sslContextHandle >= (int)ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0) || (g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext == NULL))
    {
        return FALSE;
    }

    context = (mbedTLS_NFContext*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    
    // zero memory to wipe any security critical info in RAM
    memset(context->ssl, 0, sizeof(mbedtls_ssl_context));

    // free memory
    platform_free(context->server_fd);
    platform_free(context->entropy);
    platform_free(context->ctr_drbg);
    platform_free(context->conf);
    platform_free(context->ssl);
    platform_free(context);

    NANOCLR_SSL_MEMSET(&g_SSL_Driver.m_sslContextArray[sslContextHandle], 0, sizeof(g_SSL_Driver.m_sslContextArray[sslContextHandle]));

    g_SSL_Driver.m_sslContextCount --;

    return TRUE;
}
