//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

int ssl_read_internal(int sd, char *data, size_t size)
{
    mbedTLS_NFContext *context = (mbedTLS_NFContext *)SOCKET_DRIVER.GetSocketSslData(sd);
    mbedtls_ssl_context *ssl = context->ssl;

    // sanity check
    if (ssl == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    int ret = mbedtls_ssl_read(ssl, (unsigned char *)(data), size);

    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
    {
        // Non-blocking socket has no data yet — tell caller to retry via select/poll
        return SSL_RESULT__WOULD_BLOCK;
    }

    if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY || ret == 0)
    {
        // Peer closed the connection — signal end-of-stream
        return 0;
    }

    if (ret < 0)
    {
        // Any other error
        return SOCK_SOCKET_ERROR;
    }

    return ret;
}
