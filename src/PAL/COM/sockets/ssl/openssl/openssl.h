//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _SOCKETS_OPENSSL_H_
#define _SOCKETS_OPENSSL_H_ 1

#include <sockets_lwip.h>

void ERR_print_errors_fp();

X509* ssl_parse_certificate(void* pCert, size_t certLen, const char* pwd, EVP_PKEY** privateKey);

#define OPENSSL_TYPE__FILE_STDERR   0

#define SOCKET_DRIVER g_Sockets_LWIP_Driver

//--//
#define SSL_SOCKET_ATTEMPTED_CONNECT -1
//--//


// Define disable functions that are not available - openssl wrapper 
#define NO_LIBRARY_INIT 
#define NO_CHECK_PRIVATE_KEY 
#endif
