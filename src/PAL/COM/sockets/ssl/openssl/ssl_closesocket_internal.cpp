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

int ssl_closesocket_internal( int sd )
{
    int err = 0;
    SSL *ssl = (SSL*)SOCKET_DRIVER.GetSocketSslData(sd);

    if(ssl == NULL || ssl == (void*)SSL_SOCKET_ATTEMPTED_CONNECT)
    {
        return SOCK_SOCKET_ERROR;
    }

    SOCKET_DRIVER.SetSocketSslData(sd, NULL);
    SOCKET_DRIVER.UnregisterSocket(sd);
    
    SSL_shutdown (ssl);  /* send SSL/TLS close_notify */

    SOCK_close( sd );

    return err;
}

