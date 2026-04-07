//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

bool ssl_exit_context_internal(int contextHandle)
{
    mbedTLS_NFContext *context;

    // Check contextHandle range
    if ((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0) ||
        (g_SSL_Driver.ContextArray[contextHandle].Context == NULL))
    {
        return false;
    }

    context = (mbedTLS_NFContext *)g_SSL_Driver.ContextArray[contextHandle].Context;
    if (context == NULL)
    {
        return false;
    }

    mbedtls_pk_free(context->pk);
    mbedtls_net_free(context->server_fd);
    mbedtls_ctr_drbg_free(context->ctr_drbg);
    mbedtls_entropy_free(context->entropy);
    mbedtls_ssl_config_free(context->conf);
    mbedtls_x509_crt_free(context->ca_cert);
    mbedtls_x509_crt_free(context->own_cert);
    mbedtls_ssl_free(context->ssl);

    // zero memory to wipe any security critical info in RAM
    memset(context->ssl, 0, sizeof(mbedtls_ssl_context));

    // free memory
    if (context->pk)
    {
        platform_free(context->pk);
    }

    if (context->server_fd)
    {
        platform_free(context->server_fd);
    }

    if (context->entropy)
    {
        platform_free(context->entropy);
    }

    if (context->ctr_drbg)
    {
        platform_free(context->ctr_drbg);
    }

    if (context->conf)
    {
        platform_free(context->conf);
    }

    if (context->ca_cert)
    {
        platform_free(context->ca_cert);
    }

    if (context->own_cert)
    {
        platform_free(context->own_cert);
    }

    if (context->ssl)
    {
        platform_free(context->ssl);
    }

    if (context)
    {
        platform_free(context);
    }

    memset(&g_SSL_Driver.ContextArray[contextHandle], 0, sizeof(g_SSL_Driver.ContextArray[contextHandle]));

    g_SSL_Driver.ContextCount--;

    return true;
}
