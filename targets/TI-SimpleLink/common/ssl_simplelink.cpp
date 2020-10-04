//
// Copyright (c) .NET Foundation and Contributors
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

// need to declare this here because the SimpleLink header doesn't have the appropriate C++ wrappers
extern "C" 
{
    int32_t ClockSync_convert(uint32_t gmTime, struct tm *localTime);
}

// TODO
bool ssl_parse_certificate_internal(
    void* buf, 
    size_t size, 
    void* pwd, 
    void* x509 )
{
    (void)buf;
    (void)size;
    (void)pwd;
    (void)x509;
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

    return 0;    
}

int ssl_accept_internal( 
    int socket, 
    int contextHandle )
{
    (void)socket;
    (void)contextHandle;
}

bool ssl_add_cert_auth_internal( 
    int contextHandle, 
    const char* certificate, 
    int certLength,
    const char* certPassword )
{
    (void)contextHandle;
    (void)certificate;
    (void)certLength;
    (void)certPassword;

}

// declared at sockets_simplelink
extern int socketErrorCode;

extern "C"
{
void ssl_rand_seed(
    const void *seed, 
    int length)
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

bool ssl_generic_init_internal( 
    int sslMode, 
    int sslVerify, 
    const char* certificate, 
    int certLength, 
    const uint8_t* privateKey,
    int privateKeyLength,
    const char* password,
    int passwordLength,
    int& contextHandle, 
    bool isServer )
{
    (void)password;
    (void)passwordLength;
    (void)privateKey;
    (void)privateKeyLength;

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
    for(uint32_t i=0; i<ARRAYSIZE(g_SSL_Driver.ContextArray); i++)
    { 
        if(g_SSL_Driver.ContextArray[i].Context == NULL)
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
    // this is the equivalent of SSL context in mbedTLS
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

    //     // if( mbedtls_tls_conf_own_cert( &conf, &clicert, &pkey ) != 0 )
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
    g_SSL_Driver.ContextArray[sslContexIndex].Context = context;
    g_SSL_Driver.ContextCount++;

    contextHandle = sslContexIndex;

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

bool ssl_exit_context_internal(int contextHandle)
{
    SlSSL_Context* context = NULL;

    // Check contextHandle range
    if((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0) || (g_SSL_Driver.ContextArray[contextHandle].Context == NULL))
    {
        return false;
    }

    context = (SlSSL_Context*)g_SSL_Driver.ContextArray[contextHandle].Context;
    if (context == NULL)
    {
        return false;
    }

    SlNetSock_secAttribDelete(context->SecurityAttributes);

    platform_free(context);

    memset(&g_SSL_Driver.ContextArray[contextHandle], 0, sizeof(g_SSL_Driver.ContextArray[contextHandle]));

    g_SSL_Driver.ContextCount --;

    return true;
}

int ssl_connect_internal(
    int sd, 
    const char* szTargetHost, 
    int contextHandle)
{
    SlSSL_Context* context;
    int32_t status;
    struct timespec ts;
    struct tm rtcTime;

    // Check contextHandle range
    if((contextHandle >= (int)ARRAYSIZE(g_SSL_Driver.ContextArray)) || (contextHandle < 0))
    {
        return SOCK_SOCKET_ERROR;
    }
    
    // Retrieve SSL context from g_SSL_Driver    
    // sd should already have been created
    // Now do the SSL negotiation
    context = (SlSSL_Context*)g_SSL_Driver.ContextArray[contextHandle].Context;
    if (context == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    // set socket in network context
    context->SocketFd = sd;

    if(szTargetHost != NULL && szTargetHost[0] != 0)
    {
        status = SlNetSock_secAttribSet(
            context->SecurityAttributes, 
            SLNETSOCK_SEC_ATTRIB_DOMAIN_NAME, 
            (void *)szTargetHost, 
            hal_strlen_s(szTargetHost));
        if (status < 0)
        {
            // hostname_failed
            return status;
        }
    }

    // in order to validate certificates, the device has to have it's date/time set
    // get current time
    clock_gettime(CLOCK_REALTIME, &ts);

    // need to convert between structs
    ClockSync_convert(ts.tv_sec, &rtcTime);
    
    SlDateTime_t dateTime;
    dateTime.tm_sec = rtcTime.tm_sec;
    dateTime.tm_min = rtcTime.tm_min;
    dateTime.tm_hour = rtcTime.tm_hour;
    dateTime.tm_day = rtcTime.tm_mday;
    // tm_mon starts month 0
    dateTime.tm_mon = rtcTime.tm_mon + 1;
    // tm_year starts in 1970
    dateTime.tm_year = rtcTime.tm_year + 1970;

    sl_DeviceSet(
        SL_DEVICE_GENERAL, 
        SL_DEVICE_GENERAL_DATE_TIME,
        sizeof(SlDateTime_t), 
        (uint8_t *)(&dateTime));

    // DON'T setup socket for blocking operation

    // start security context on socket
    status = SlNetSock_startSec(
        context->SocketFd, 
        context->SecurityAttributes, context->IsServer ?
        (SLNETSOCK_SEC_START_SECURITY_SESSION_ONLY | SLNETSOCK_SEC_IS_SERVER) :
        (SLNETSOCK_SEC_START_SECURITY_SESSION_ONLY | SLNETSOCK_SEC_BIND_CONTEXT_ONLY));

    if ( (status < 0) && 
        (status != SLNETERR_ESEC_UNKNOWN_ROOT_CA) &&
        (status != SLNETERR_ESEC_HAND_SHAKE_TIMED_OUT) &&
        (status != SLNETERR_ESEC_DATE_ERROR) &&
        (status != SLNETERR_ESEC_SNO_VERIFY))
    {
        return status;
    }

    // got here, handshake is completed 

    return 0;
}

int ssl_pending_internal( int sd )
{
    // Simple Link doesn't have an API to check for pending data
    // just return 0
    return 0;
}

int  ssl_write_internal( 
    int sd, 
    const char* data, 
    size_t req_len)
{
    int32_t status;

    socketErrorCode = SlNetSock_send(
        sd, 
        (const void*)data,
        req_len, 
        0);

    // anything below 0 is considered an error, so we have to report that no bytes were sent
    if (socketErrorCode < 0)
    {
        return 0;
    } 

    return req_len;
}

int  ssl_read_internal( 
    int sd, 
    char* data, 
    size_t size )
{
    socketErrorCode = SlNetSock_recv(
        sd, 
        (unsigned char *)(data), 
        size, 
        0);

    return socketErrorCode;
}

int  ssl_close_socket_internal( int sd )
{
    // Simple Link takes care of everything for us, just call close socket
    SOCK_close( sd );

    return true;
}
