//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

int  ssl_closesocket_internal( int sd )
{
    mbedTLS_NFContext* context= (mbedTLS_NFContext*)SOCKET_DRIVER.GetSocketSslData(sd);
    mbedtls_ssl_context *ssl = context->ssl;

    // sanity check
    if(ssl == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    SOCKET_DRIVER.SetSocketSslData(sd, NULL);
    SOCKET_DRIVER.UnregisterSocket(sd);
    
    mbedtls_ssl_close_notify(ssl);

    SOCK_close( sd );

    return 0;
}
