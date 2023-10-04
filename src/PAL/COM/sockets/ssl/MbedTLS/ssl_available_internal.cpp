//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include <nanoPAL.h>
#include "mbedtls.h"

int ssl_available_internal(int sd)
{
    mbedTLS_NFContext *context = (mbedTLS_NFContext *)SOCKET_DRIVER.GetSocketSslData(sd);
    mbedtls_ssl_context *ssl = context->ssl;

    // sanity check
    if (ssl == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    // Developer note
    // Ideally we should be making a call to read passing a NULL pointer and requesting 0 bytes
    // Like this: mbedtls_ssl_read(ssl, NULL, 0)
    // It won't work because we are using blockign sockets.
    // Even if we unblock it temporarily, it will still won't return until there is something to be read.
    // That call will block the execution and the watchdog will eventually kick in.
    // Bottom line: take the information provided by this call as informational.

    int availableBytes = mbedtls_ssl_check_pending(ssl);

    if (availableBytes > 0)
    {
        return mbedtls_ssl_get_bytes_avail(ssl);
    }
    else
    {
        return availableBytes;
    }
}
