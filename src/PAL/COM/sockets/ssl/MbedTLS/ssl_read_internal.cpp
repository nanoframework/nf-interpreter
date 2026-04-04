//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"
#include <nanoCLR_Types.h>

int ssl_read_internal(int sd, char *data, size_t size)
{
    CLR_Debug::Printf("SSL_R: enter sd=%d len=%d\r\n", sd, (int)size);

    mbedTLS_NFContext *context = (mbedTLS_NFContext *)SOCKET_DRIVER.GetSocketSslData(sd);
    mbedtls_ssl_context *ssl = context->ssl;

    // sanity check
    if (ssl == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    int ret = mbedtls_ssl_read(ssl, (unsigned char *)(data), size);
    CLR_Debug::Printf("SSL_R: ret=%d\r\n", ret);

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
        CLR_Debug::Printf("SSL_R: error -0x%04X\r\n", (unsigned)-ret);
        return SOCK_SOCKET_ERROR;
    }

    return ret;
}
