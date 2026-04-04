//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"
#include <nanoCLR_Types.h>

int ssl_connect_internal(int sd, const char *szTargetHost, int contextHandle)
{
    mbedTLS_NFContext *context;
    int nonblock = 0;
    int hsIter = 0;

    int ret = SOCK_SOCKET_ERROR;

    // Check contextHandle range
    if ((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0))
    {
        goto error;
    }

    // Retrieve SSL struct from g_SSL_Driver
    // sd should already have been created
    // Now do the SSL negotiation
    context = (mbedTLS_NFContext *)g_SSL_Driver.ContextArray[contextHandle].Context;
    if (context == NULL)
    {
        return false;
    }

    // set socket in network context
    context->server_fd->fd = sd;

    if (szTargetHost != NULL && szTargetHost[0] != 0)
    {
        if ((ret = mbedtls_ssl_set_hostname(context->ssl, szTargetHost)) != 0)
        {
            // hostname_failed
            goto error;
        }
    }

    // setup internal SSL context and calls to transport layer send, receive and receive with timeout
    mbedtls_ssl_set_bio(context->ssl, context->server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // Print current system time for SNTP sync diagnostics
    {
        extern uint64_t g_HAL_Time_UtcOffset;
        uint64_t rawTicks = HAL_Time_CurrentSysTicks();
        uint64_t halTime = HAL_Time_CurrentTime();
        uint64_t dateTime = HAL_Time_CurrentDateTime(false);
        time_t now = nf_get_unix_epoch();
        CLR_Debug::Printf("SSL: ticks=%u/%u hal=%u/%u dt=%u/%u epoch=%d off=%u/%u\r\n",
            (unsigned)(rawTicks >> 32), (unsigned)(rawTicks & 0xFFFFFFFF),
            (unsigned)(halTime >> 32), (unsigned)(halTime & 0xFFFFFFFF),
            (unsigned)(dateTime >> 32), (unsigned)(dateTime & 0xFFFFFFFF),
            (int)now,
            (unsigned)(g_HAL_Time_UtcOffset >> 32), (unsigned)(g_HAL_Time_UtcOffset & 0xFFFFFFFF));
    }

    // perform SSL handshake
    CLR_Debug::Printf("SSL: starting TLS 1.2 handshake\r\n");
    while ((ret = mbedtls_ssl_handshake(context->ssl)) != 0)
    {
        hsIter++;
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            // SSL handshake failed — print error for diagnostics
            CLR_Debug::Printf("SSL: handshake failed -0x%04X\r\n", (unsigned)-ret);
            if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED)
            {
                uint32_t flags = mbedtls_ssl_get_verify_result(context->ssl);
                CLR_Debug::Printf("SSL: verify flags 0x%08X\r\n", flags);
                if (flags & MBEDTLS_X509_BADCERT_EXPIRED)
                    CLR_Debug::Printf("SSL:  - cert expired\r\n");
                if (flags & MBEDTLS_X509_BADCERT_NOT_TRUSTED)
                    CLR_Debug::Printf("SSL:  - cert not trusted\r\n");
                if (flags & 0x02)
                    CLR_Debug::Printf("SSL:  - cert not yet valid\r\n");
                if (flags & MBEDTLS_X509_BADCERT_CN_MISMATCH)
                    CLR_Debug::Printf("SSL:  - CN mismatch\r\n");
            }
            goto error;
        }
    }

    CLR_Debug::Printf("SSL: handshake OK (%d iterations)\r\n", hsIter);

    nonblock = 1;
    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // store SSL context in sockets driver
    SOCKET_DRIVER.SetSocketSslData(sd, (void *)context);

error:
    return ret;
}
