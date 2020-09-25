//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"

int  ssl_read_internal( 
    int sd, 
    char* data, 
    size_t size )
{
    mbedTLS_NFContext* context= (mbedTLS_NFContext*)SOCKET_DRIVER.GetSocketSslData(sd);
    mbedtls_ssl_context *ssl = context->ssl;

    // sanity check
    if(ssl == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }
    
    int ret = mbedtls_ssl_read(ssl, (unsigned char *)(data), size);
    if ( ret < 0 )
    {
        return 0;
    }

    return ret;
}
