//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include <nanoPAL.h>
#include "mbedtls.h"

int ssl_pending_internal( int sd )
{
    (void)sd;
    int avail = 0;

    //mbedTLS_NFContext* context= (mbedTLS_NFContext*)SOCKET_DRIVER.GetSocketSslData(sd);
    //mbedtls_ssl_context *ssl = context->ssl;

    // if(ssl == NULL || ssl == (void*)SSL_SOCKET_ATTEMPTED_CONNECT)
    // {
    //     return SOCK_SOCKET_ERROR;
    // }
    
    //avail = SSL_pending(ssl);  /* send SSL/TLS close_notify */

    return avail;
}
