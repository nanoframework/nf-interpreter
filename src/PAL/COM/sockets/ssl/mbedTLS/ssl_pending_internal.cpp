//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include <nanoPAL.h>
#include "mbedtls.h"

int ssl_pending_internal( int sd )
{
    mbedTLS_NFContext* context= (mbedTLS_NFContext*)SOCKET_DRIVER.GetSocketSslData(sd);
    mbedtls_ssl_context *ssl = context->ssl;

    // sanity check
    if(ssl == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }
    
    return mbedtls_ssl_check_pending(ssl);
}
