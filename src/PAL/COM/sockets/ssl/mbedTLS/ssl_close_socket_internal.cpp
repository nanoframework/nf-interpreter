//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

int ssl_close_socket_internal( int sd )
{
    mbedTLS_NFContext* context= (mbedTLS_NFContext*)SOCKET_DRIVER.GetSocketSslData(sd);

    // sanity check
    if(context != NULL)
    {
        mbedtls_ssl_context *ssl = context->ssl;

        // be nice and notify the peer that the connection is being closed
        mbedtls_ssl_close_notify(ssl);

        SOCKET_DRIVER.SetSocketSslData(sd, NULL);
    }

    SOCK_close( sd );

    return 0;
}
