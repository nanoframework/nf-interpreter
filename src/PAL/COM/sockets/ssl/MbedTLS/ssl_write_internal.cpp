//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"
#include <nanoCLR_Types.h>

int ssl_write_internal(int sd, const char *data, size_t req_len)
{
    int ret;

    CLR_Debug::Printf("SSL_W: enter sd=%d len=%d\r\n", sd, (int)req_len);

    mbedTLS_NFContext *context = (mbedTLS_NFContext *)SOCKET_DRIVER.GetSocketSslData(sd);
    mbedtls_ssl_context *ssl = context->ssl;

    // sanity check
    if (ssl == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    // Loop until all data has been sent or error
    size_t req_offset = 0;
    do
    {
        ret = mbedtls_ssl_write(ssl, (const unsigned char *)(data + req_offset), req_len - req_offset);
        if (ret > 0)
        {
            req_offset += static_cast<size_t>(ret);
        }
    } while (req_offset < req_len &&
             (ret > 0 || ret == MBEDTLS_ERR_SSL_WANT_WRITE || ret == MBEDTLS_ERR_SSL_WANT_READ));

    if (ret < 0)
    {
        CLR_Debug::Printf("SSL_W: fail ret=-0x%04X\r\n", (unsigned)-ret);
        return 0;
    }

    CLR_Debug::Printf("SSL_W: ok wrote=%d\r\n", (int)req_len);
    return req_len;
}
