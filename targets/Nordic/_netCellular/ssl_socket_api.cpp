//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoHAL.h"

#include <zephyr.h>
#include <stdlib.h>
#ifndef restrict
#define restrict
#endif

// #include <string.h>
#include <net/socket.h>
#include <net/tls_credentials.h>
#include <modem/modem_key_mgmt.h>
#include <fcntl.h>
#include <nrf_socket.h>

#include <nrf9160_lte.h>
#include <ssl.h>
#include <sockets_nrf9160.h>
#include <ssl_sockets_nrf9160.h>

#define SSL_TAG_NO_SEC -1
//--//

// TODO
// bool ssl_parse_certificate_internal(void *buf, size_t size, void *pwd, void *x509)
bool ssl_parse_certificate_internal(void *buf, size_t size, void *x509)
{
    (void)buf;
    (void)size;
    (void)x509;
    NATIVE_PROFILE_PAL_NETWORK();

    return true;
}

int ssl_decode_private_key_internal(
    const unsigned char *key,
    size_t keyLength,
    const unsigned char *password,
    size_t passwordLength)
{
    (void)key;
    (void)keyLength;
    (void)password;
    (void)passwordLength;
    NATIVE_PROFILE_PAL_NETWORK();

    return 0;
}

int ssl_accept_internal(int socket, int contextHandle)
{
    (void)socket;
    (void)contextHandle;
    NATIVE_PROFILE_PAL_NETWORK();
    return 0;
}

// bool ssl_add_cert_auth_internal(int contextHandle, const char *certificate, int certLength, const char *certPassword)
bool ssl_add_cert_auth_internal(int contextHandle, const char *certificate, int certLength)
{
    (void)contextHandle;
    (void)certificate;
    (void)certLength;
    NATIVE_PROFILE_PAL_NETWORK();
    return true;
}

bool ssl_initialize_internal()
{
    NATIVE_PROFILE_PAL_NETWORK();
    memset(&g_SSL_Driver, 0, sizeof(g_SSL_Driver));

    return true;
}

bool ssl_generic_init_internal(
    int sslMode,
    int sslVerify,
    const char *certificate,
    int certLength,
    const uint8_t *privateKey,
    int privateKeyLength,
    const char *password,
    int passwordLength,
    int &contextHandle,
    bool useDeviceCertificate,
    bool isServer)
{
    (void)password;
    (void)passwordLength;
    (void)privateKey;
    (void)privateKeyLength;
    NATIVE_PROFILE_PAL_NETWORK();

    int sslContexIndex = -1;

    struct nrf91ssl_Context *context;

    ///////////////////////
    for (uint32_t i = 0; i < ARRAYSIZE(g_SSL_Driver.ContextArray); i++)
    {
        if (g_SSL_Driver.ContextArray[i].Context == NULL)
        {
            sslContexIndex = i;
            break;
        }
    }

    if (sslContexIndex == -1)
        return false;

    // create and init nanoFramework Simple Link context
    // this needs to be freed in ssl_exit_context_internal
    context = (struct nrf91ssl_Context *)platform_malloc(sizeof(struct nrf91ssl_Context));
    if (context == NULL)
    {
        goto error;
    }

    // // create security attribute
    // // this is the equivalent of SSL context in mbedTLS
    // // it needs to be freed in ssl_exit_context_internal
    // context->SecurityAttributes = SlNetSock_secAttribCreate();
    // if (context->SecurityAttributes == NULL)
    // {
    //     goto error;
    // }

    context->IsServer = isServer;

    // configure protocol
    switch ((SslProtocols)sslMode)
    {
        case SslProtocols_TLSv1:
            context->securityMethod = IPPROTO_TLS_1_0;
            break;

        case SslProtocols_TLSv11:
            context->securityMethod = IPPROTO_TLS_1_1;
            break;

        case SslProtocols_TLSv12:
            context->securityMethod = IPPROTO_TLS_1_2;
            break;

        default:
            // shouldn't reach here!
            goto error;
    }
    // default to no security
    // context->sec_tag = SSL_TAG_NO_SEC;
    context->sec_tag = TLS_SEC_TAG;

    // CA root certs are taken from Simple Link trusted root-certificate catalog, so nothing to do here

    // // parse "own" certificate if passed
    // if(certificate != NULL && certLength > 0)
    // {
    //     // TODO
    //     // this isn't required for client authentication

    //     // mbedtls_x509_crt_init( &clicert );

    //     // /////////////////////////////////////////////////////////////////////////////////////////////////
    //     // // developer notes:                                                                            //
    //     // // this call parses certificates in both string and binary formats                             //
    //     // // when the formart is a string it has to include the terminator otherwise the parse will fail //
    //     // /////////////////////////////////////////////////////////////////////////////////////////////////
    //     // if(mbedtls_x509_crt_parse( &clicert, (const unsigned char*)certificate, certLength ) != 0)
    //     // {
    //     //     // x509_crt_parse_failed
    //     //     goto error;
    //     // }

    //     // if( mbedtls_pk_parse_key( &pkey, (const unsigned char *) mbedtls_test_cli_key, mbedtls_test_cli_key_len,
    //     NULL, 0 ) != 0)
    //     // {
    //     //     // failed parsing the
    //     // }

    //     // if( mbedtls_tls_conf_own_cert( &conf, &clicert, &pkey ) != 0 )
    //     // {
    //     //     // configuring own certificate failed
    //     //     goto error;
    //     // }
    // }

    switch (sslVerify)
    {
        case SslVerification_NoVerification:
            context->peer_verify = TLS_PEER_VERIFY_NONE;
            break;

        case SslVerification_VerifyPeer:
            context->peer_verify = TLS_PEER_VERIFY_REQUIRED;
            break;

        // case SslVerification_NoVerification:
        default:
            context->peer_verify = TLS_PEER_VERIFY_OPTIONAL;
            break;
    }
    // // set certificate verification
    // // the current options provided by Simple Link API are only verify or don't verify
    // if ((SslVerification)sslVerify == SslVerification_NoVerification)
    // {
    //     status = SlNetSock_secAttribSet(
    //         context->SecurityAttributes,
    //         SLNETSOCK_SEC_ATTRIB_DISABLE_CERT_STORE,
    //         (void *)&dummyVal,
    //         sizeof(dummyVal));
    //     if (status < 0)
    //     {
    //         goto error;
    //     }
    // }

    //////////////////////////////////////

    // the equivalent of SSL contex in Simple Link is the Security Attribute that we've been building
    g_SSL_Driver.ContextArray[sslContexIndex].Context = context;
    g_SSL_Driver.ContextCount++;

    contextHandle = sslContexIndex;

    return true;

error:

    // check for any memory allocation that needs to be freed before exiting
    if (context != NULL)
    {
        platform_free(context);
    }

    // if (context->SecurityAttributes != NULL)
    // {
    //     SlNetSock_secAttribDelete(context->SecurityAttributes);
    // }

    return false;
}

bool ssl_exit_context_internal(int contextHandle)
{
    struct nrf91ssl_Context *context = NULL;
    NATIVE_PROFILE_PAL_NETWORK();

    // Check contextHandle range
    if ((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0) ||
        (g_SSL_Driver.ContextArray[contextHandle].Context == NULL))
    {
        return false;
    }

    context = (nrf91ssl_Context *)g_SSL_Driver.ContextArray[contextHandle].Context;
    if (context == NULL)
    {
        return false;
    }

    platform_free(context);

    memset(&g_SSL_Driver.ContextArray[contextHandle], 0, sizeof(g_SSL_Driver.ContextArray[contextHandle]));

    g_SSL_Driver.ContextCount--;

    return true;
}

int ssl_connect_internal(int sd, const char *szTargetHost, int contextHandle)
{
    int rc;
    // int cache;
    nrf91ssl_Context *context;

    // TODO:: expect we need to move this
    struct addrinfo *res;
    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };

    NATIVE_PROFILE_PAL_NETWORK();

    // Check contextHandle range
    if ((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0))
    {
        return SOCK_SOCKET_ERROR;
    }

    // Retrieve SSL context from g_SSL_Driver
    // sd should already have been created
    // Now do the SSL negotiation
    context = (nrf91ssl_Context *)g_SSL_Driver.ContextArray[contextHandle].Context;
    if (context == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    // set socket in network context
    context->SocketFd = sd;

    // TODO:: get this info from the getaddrinfo call
    rc = getaddrinfo(szTargetHost, NULL, &hints, &res);
    if (rc)
    {
        printk("getaddrinfo() failed, err %d\n", errno);
        return -1;
    }

    // TODO::
    ((struct sockaddr_in *)res->ai_addr)->sin_port = htons(443);

    context->peer_verify = 2;
    printk("calling setsockopt verify %d\n", context->peer_verify);
    rc = setsockopt(sd, SOL_TLS, TLS_PEER_VERIFY, &context->peer_verify, sizeof(uint32_t));
    if (rc)
    {
        printk("*** Failed to setup peer verification, error: %d\n", errno);
        return rc;
    }

    //    sec_tag_t sec_tag_list[1] = {context->sec_tag};
    //    rc = setsockopt(sd, SOL_TLS, TLS_SEC_TAG_LIST, sec_tag_list, sizeof(sec_tag_list));
    const sec_tag_t tls_sec_tag[] = {
        TLS_SEC_TAG,
    };
    rc = setsockopt(sd, SOL_TLS, TLS_SEC_TAG_LIST, tls_sec_tag, sizeof(tls_sec_tag));
    if (rc)
    {
        printk("Failed to setup TLS sec tag, error: %d\n", errno);
        return rc;
    }

    // if (IS_ENABLED(CONFIG_REST_CLIENT_SCKT_TLS_SESSION_CACHE_IN_USE))
    // {
    //     cache = TLS_SESSION_CACHE_ENABLED;
    // }
    // else
    // {
    //     cache = TLS_SESSION_CACHE_DISABLED;
    // }

    // rc = setsockopt(sd, SOL_TLS, TLS_SESSION_CACHE, &cache, sizeof(cache));
    // if (rc)
    // {
    //     printk("Unable to set session cache, errno %d\n", errno);
    //     return rc;
    // }

    if (szTargetHost != NULL && szTargetHost[0] != 0)
    {
        printk("target host  %d >%s<\n", hal_strlen_s(szTargetHost), szTargetHost);
        rc = setsockopt(sd, SOL_TLS, TLS_HOSTNAME, szTargetHost, hal_strlen_s(szTargetHost));
        if (rc)
        {
            printk("Failed to setup TLS hostname, error: %d\n", errno);
            return rc;
        }
    }
    printk("server >%s<\n", szTargetHost);

    rc = connect(sd, res->ai_addr, sizeof(struct sockaddr_in));
    printk("connect ret %d  %d\n", rc, errno);
    freeaddrinfo(res);

    return 0;
}

int ssl_available_internal(int sd)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return GetAvailBytes(sd);
}

int ssl_write_internal(int sd, const char *data, size_t req_len)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return SOCK_send(sd, data, req_len, 0);
}

int ssl_read_internal(int sd, char *data, size_t size)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return SOCK_recv(sd, data, size, 0);
}

int ssl_close_socket_internal(int sd)
{
    NATIVE_PROFILE_PAL_NETWORK();
    SOCK_close(sd);
    return true;
}
