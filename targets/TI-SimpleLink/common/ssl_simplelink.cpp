//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHal.h>
#include <ssl.h>
#include <targetHAL_ConfigurationManager_CC32xx.h>
#include <ssl_simplelink.h>
#include <targetSimpleLinkCC32xx_Threads.h>

#include <ti/net/slneterr.h>
#include <ti/net/slnetutils.h>
#include <ti/drivers/net/wifi/sl_socket.h>

//--//

// TODO
bool ssl_parse_certificate_internal(void* buf, size_t size, void* pwd, void* x509 ){(void)buf;(void)size;(void)pwd;(void)x509;}
int ssl_accept_internal( int socket, int sslContextHandle ){(void)socket;(void)sslContextHandle;}
bool ssl_add_cert_auth_internal( int sslContextHandle, const char* certificate, int certLength, const char* certPassword ){(void)sslContextHandle;(void)certificate;(void)certLength;(void)certPassword;}

// declared at sockets_simplelink
extern int socketErrorCode;

extern "C"
{
void ssl_rand_seed(const void *seed, int length)
{
    (void)seed;
    (void)length;
}
}

bool ssl_initialize_internal()
{
    memset(&g_SSL_Driver, 0, sizeof(g_SSL_Driver));

    return true;
}

bool ssl_generic_init_internal( int sslMode, int sslVerify, const char* certificate, 
    int certLength, const char* certPassword, int& sslContextHandle, bool isServer )
{
    (void)certPassword;

    int sslContexIndex = -1;

    SlSSL_Context* context;
    SlNetSockSecAttrib_e attribName;
    int32_t status;
    uint32_t dummyVal = 1;
    uint8_t  securityMethod;
    uint32_t securityCypher = SLNETSOCK_SEC_CIPHER_FULL_LIST;

    // we only have one CA root bundle, so this is fixed to 0
    uint32_t configIndex = 0;

    ///////////////////////
    for(uint32_t i=0; i<ARRAYSIZE(g_SSL_Driver.m_sslContextArray); i++)
    { 
        if(g_SSL_Driver.m_sslContextArray[i].SslContext == NULL)
        {
            sslContexIndex = i;           
            break;
        }
    }
    
    if(sslContexIndex == -1) return false;

    // create and init nanoFramework Simple Link context
    // this needs to be freed in ssl_exit_context_internal
    context = (SlSSL_Context*)platform_malloc(sizeof(SlSSL_Context));
    if(context == NULL)
    {
        goto error;
    }  


    // create security attribute
    // this is the equivalent of SSL context in mbedTls and OpenSSL
    // it needs to be freed in ssl_exit_context_internal
    context->SecurityAttributes = SlNetSock_secAttribCreate();
    if (context->SecurityAttributes == NULL)
    {
        goto error;
    }

    context->IsServer = isServer;

    // configure protocol
    switch((SslProtocols)sslMode)
    {
        case SslProtocols_SSLv3:
            securityMethod = SLNETSOCK_SEC_METHOD_SSLV3;
            break;

        case SslProtocols_TLSv1:
            securityMethod = SLNETSOCK_SEC_METHOD_TLSV1;
            break;

        case SslProtocols_TLSv11:
            securityMethod = SLNETSOCK_SEC_METHOD_TLSV1_1;
            break;

        case SslProtocols_TLSv12:
            securityMethod = SLNETSOCK_SEC_METHOD_TLSV1_2;
            break;

        default:
            // shouldn't reach here!
            goto error;
    }
    status = SlNetSock_secAttribSet(context->SecurityAttributes, SLNETSOCK_SEC_ATTRIB_METHOD, (void *)&(securityMethod), sizeof(securityMethod));
    if (status < 0)
    {
        goto error;
    }    

    // enable all cyphers (this is Simple Link default, so nothing to do about this)

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

    //     // if( mbedtls_pk_parse_key( &pkey, (const unsigned char *) mbedtls_test_cli_key, mbedtls_test_cli_key_len, NULL, 0 ) != 0)
    //     // {
    //     //     // failed parsing the 
    //     // }

    //     // if( mbedtls_ssl_conf_own_cert( &conf, &clicert, &pkey ) != 0 )
    //     // {
    //     //     // configuring own certificate failed
    //     //     goto error;
    //     // }
    // }

    // set certificate verification
    // the current options provided by Simple Link API are only verify or don't verify
    if((SslVerification)sslVerify == SslVerification_NoVerification)
    {
        status = SlNetSock_secAttribSet(context->SecurityAttributes, SLNETSOCK_SEC_ATTRIB_DISABLE_CERT_STORE, (void *)&dummyVal, sizeof(dummyVal));
        if (status < 0)
        {
            goto error;
        }
    }

    //////////////////////////////////////

    // the equivalent of SSL contex in Simple Link is the Security Attribute that we've been building
    g_SSL_Driver.m_sslContextArray[sslContexIndex].SslContext = context;
    g_SSL_Driver.m_sslContextCount++;

    sslContextHandle = sslContexIndex;

    return true;

  error:

    // check for any memory allocation that needs to be freed before exiting
    if(context != NULL)
    {
        platform_free(context);
    }

    if(context->SecurityAttributes != NULL)
    {
        SlNetSock_secAttribDelete(context->SecurityAttributes);
    }

    return false;
}

bool ssl_exit_context_internal(int sslContextHandle)
{
    SlSSL_Context* context = NULL;

    // Check sslContextHandle range
    if((sslContextHandle >= (int)ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0) || (g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext == NULL))
    {
        return false;
    }

    context = (SlSSL_Context*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    if (context == NULL)
    {
        return false;
    }

    SlNetSock_secAttribDelete(context->SecurityAttributes);

    platform_free(context);

    memset(&g_SSL_Driver.m_sslContextArray[sslContextHandle], 0, sizeof(g_SSL_Driver.m_sslContextArray[sslContextHandle]));

    g_SSL_Driver.m_sslContextCount --;

    return true;
}

int ssl_connect_internal(int sd, const char* szTargetHost, int sslContextHandle)
{
    SlSSL_Context* context;
    int32_t status;
    int nonblock = 0;
    int32_t dummyVar;
    nFSlSocketAsyncEvent_t handShakeResult;
    struct timespec ts;

    // Check sslContextHandle range
    if((sslContextHandle >= (int)ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0))
    {
        return SOCK_SOCKET_ERROR;
    }
    
    // Retrieve SSL context from g_SSL_Driver    
    // sd should already have been created
    // Now do the SSL negotiation
    context = (SlSSL_Context*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    if (context == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    // set socket in network context
    context->SocketFd = sd;

    if(szTargetHost != NULL && szTargetHost[0] != 0)
    {
        status = SlNetSock_secAttribSet(context->SecurityAttributes, SLNETSOCK_SEC_ATTRIB_DOMAIN_NAME, (void *)szTargetHost, hal_strlen_s(szTargetHost));
        if (status < 0)
        {
            // hostname_failed
            return status;
        }
    }

    // setup socket for blocking operation
    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // start security context on socket
    status = SlNetSock_startSec(context->SocketFd, context->SecurityAttributes, context->IsServer ?
            (SLNETSOCK_SEC_BIND_CONTEXT_ONLY | SLNETSOCK_SEC_IS_SERVER) :
            (SLNETSOCK_SEC_BIND_CONTEXT_ONLY));
    if (status < 0)
    {
        return status;
    }

    // perform SSL handshake
    sl_SetSockOpt(context->SocketFd, SL_SOL_SOCKET, SL_SO_STARTTLS, &dummyVar, sizeof(dummyVar));

    // wait 2 seconds for TLS handshake to complete
    // TODO: evaluate this to use CLR events
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 2;

    while(true)
    {
        if(mq_timedreceive(nF_ControlBlock.socketAsyncEvent,
            (char*)&handShakeResult, sizeof(nFSlSocketAsyncEvent_t), NULL, &ts) < 0)
        {
            // timeout occurred without receiving any message
            return SOCK_SOCKET_ERROR;
        }
        else
        {
            // this event matches the socket we are waiting for
            if(handShakeResult.Sd == sd)
            {
                break;
            }
        }
    }

    // setup socket to non blocking operation
    nonblock = 1;
    SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

    // go here, handshake is completed 
    // return handshake operation result
    status = (handShakeResult.Type == SL_SSL_NOTIFICATION_CONNECTED_SECURED) ? 0 : SOCK_SOCKET_ERROR;

    return status;
}

int ssl_pending_internal( int sd )
{
    // Simple Link doesn't have an API to check for pending data
    // just return 0
    return 0;
}

int  ssl_write_internal( int sd, const char* data, size_t req_len)
{
    int32_t status;

    socketErrorCode = SlNetSock_send(sd, (const void*)data, req_len, 0);

    // anything bellow 0 is considered an error, so we have to report that no bytes were sent
    if (socketErrorCode < 0)
    {
        return 0;
    } 

    return req_len;
}

int  ssl_read_internal( int sd, char* data, size_t size )
{
    socketErrorCode = SlNetSock_recv(sd, (unsigned char *)(data), size, 0);

    return socketErrorCode;
}

int  ssl_closesocket_internal( int sd )
{
    // Simple Link takes care of everything for us, just call close socket
    SOCK_close( sd );

    return 0;
}

bool ssl_uninitialize_internal()
{
    for(uint32_t i = 0; i<ARRAYSIZE(g_SSL_Driver.m_sslContextArray); i++)
    {
        if(g_SSL_Driver.m_sslContextArray[i].SslContext != NULL)
        {
            ssl_exit_context_internal( i );
        }
    }

    g_SSL_Driver.m_sslContextCount = 0;

    return true;
}
