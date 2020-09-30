//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include "mbedtls.h"

int  ssl_write_internal( 
    int sd, 
    const char* data, 
    size_t req_len)
{
    int ret;

    mbedTLS_NFContext* context= (mbedTLS_NFContext*)SOCKET_DRIVER.GetSocketSslData(sd);
    mbedtls_ssl_context *ssl = context->ssl;

    // sanity check
    if(ssl == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    // Loop until all data has been sent or error
    size_t req_offset = 0;
    do 
    {
        ret = mbedtls_ssl_write( ssl, (const unsigned char *)(data + req_offset), req_len - req_offset);
        if (ret > 0)
        {
            req_offset += static_cast<size_t>(ret);
        }
    }
    while( req_offset < req_len &&
           (ret > 0 ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE ||
           ret == MBEDTLS_ERR_SSL_WANT_READ) );

    if (ret < 0)
    {
        //mbedtls_printf("mbedtls_ssl_write() returned -0x%04X\n", -ret);
        return 0;
    } 

    return req_len;
}
