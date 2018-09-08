//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include <openssl/ssl.h>
#include <openssl.h> 

// #include <tinyclr/ssl_functions.h>
// #include <openssl/ssl.h>
// #include <openssl.h>

int ssl_pending_internal( int sd )
{
    int avail = 0;
    SSL *ssl = (SSL*)SOCKET_DRIVER.GetSocketSslData(sd);

    if(ssl == NULL || ssl == (void*)SSL_SOCKET_ATTEMPTED_CONNECT)
    {
        return SOCK_SOCKET_ERROR;
    }
    
    avail = SSL_pending(ssl);  /* send SSL/TLS close_notify */

    return avail;
}


