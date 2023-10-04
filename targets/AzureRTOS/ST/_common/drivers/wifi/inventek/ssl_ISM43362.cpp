//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHal.h>
#include <ssl.h>
#include "ssl_ISM43362.h"
#include "wifi.h"

//--//

// TODO
bool ssl_parse_certificate_internal(void *buf, size_t size, void *pwd, void *x509)
{
    (void)buf;
    (void)size;
    (void)pwd;
    (void)x509;

    // can't really do anything here, so just return true
    return true;
}

bool ssl_get_public_key_raw_internal(void *certificate, size_t size, void *x509RawData)
{
    (void)certificate;
    (void)size;
    (void)x509RawData;

    // can't really do anything here, so just return false
    return false;
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

    // can't really do anything here, so just return
    return 0;
}

int ssl_accept_internal(int socket, int contextHandle)
{
    (void)socket;
    (void)contextHandle;

    return 0;
}

bool ssl_add_cert_auth_internal(int contextHandle, const char *certificate, int certLength, const char *certPassword)
{
    (void)contextHandle;
    (void)certificate;
    (void)certLength;
    (void)certPassword;

    // always return TRUE because we can't really add a certificate to the chain
    return true;
}

// declared at sockets driver
extern int socketErrorCode;

bool ssl_initialize_internal()
{
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
    (void)certificate;
    (void)certLength;
    (void)useDeviceCertificate;
    (void)sslMode;
    (void)sslVerify;
    (void)isServer;

    SSL_Context *context;
    int sslContexIndex = -1;

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
    {
        return false;
    }

    // create and init context
    // this needs to be freed in ssl_exit_context_internal
    context = (SSL_Context *)platform_malloc(sizeof(SSL_Context));
    if (context == NULL)
    {
        goto error;
    }

    // reset socket index
    context->SocketIndex = -1;

    //////////////////////////////////////

    // the equivalent of SSL contex in ISM43362 API is the socket number
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

    return false;
}

bool ssl_exit_context_internal(int contextHandle)
{
    // Check contextHandle range
    if ((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0) ||
        (g_SSL_Driver.ContextArray[contextHandle].Context == NULL))
    {
        return false;
    }

    memset(&g_SSL_Driver.ContextArray[contextHandle], 0, sizeof(g_SSL_Driver.ContextArray[contextHandle]));

    g_SSL_Driver.ContextCount--;

    return true;
}

int ssl_connect_internal(int sd, const char *szTargetHost, int contextHandle)
{
    (void)sd;
    (void)szTargetHost;
    (void)contextHandle;

    SSL_Context *context;
    // int32_t status;
    // struct timespec ts;
    // struct tm rtcTime;

    // Check contextHandle range
    if ((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0))
    {
        return SOCK_SOCKET_ERROR;
    }

    // Retrieve SSL context from g_SSL_Driver
    // sd should already have been created
    // Now do the SSL negotiation
    context = (SSL_Context *)g_SSL_Driver.ContextArray[contextHandle].Context;
    if (context == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    // set socket in network context
    context->SocketIndex = sd;

    // at this point the socket must have been connected

    //////////////////////////////////////////////////////////////////////
    // current firmware in ISM43362 does not support secure connections //
    // so we are faking it as if it would work                          //
    // SOCK_UpgradeToSsl(sd);
    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    return 0;
}

int ssl_available_internal(int sd)
{
    (void)sd;

    // ISM43362 API doesn't have an API to check for available data
    // just return 0
    return 0;
}

int ssl_write_internal(int sd, const char *data, size_t req_len)
{
    (void)sd;
    (void)data;
    (void)req_len;

    // ISM43362 takes care of everything for us, just call the send API
    return SOCK_send(sd, data, req_len, 0);
}

int ssl_read_internal(int sd, char *data, size_t size)
{
    (void)sd;
    (void)data;
    (void)size; // SSL_RESULT__WOULD_BLOCK

    // ISM43362 takes care of everything for us, just call the recv API
    return SOCK_recv(sd, data, size, 0);
}

int ssl_close_socket_internal(int sd)
{
    // ISM43362 takes care of everything for us, just call close socket
    SOCK_close(sd);

    return true;
}
