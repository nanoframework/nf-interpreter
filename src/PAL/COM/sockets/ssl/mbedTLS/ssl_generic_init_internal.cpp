//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include "mbedtls.h"
#include "mbedtls/debug.h"

bool ssl_generic_init_internal( int sslMode, int sslVerify, const char* certificate, 
    int certLength, const char* certPassword, int& sslContextHandle, bool isServer )
{
    (void)sslMode;
    (void)certPassword;

    int sslContexIndex = -1;
    int authMode = MBEDTLS_SSL_VERIFY_NONE;
    int endpoint = 0;

    ///////////////////////
    mbedTLS_NFContext* context;

    for(uint32_t i=0; i<ARRAYSIZE(g_SSL_Driver.m_sslContextArray); i++)
    { 
        if(g_SSL_Driver.m_sslContextArray[i].SslContext == NULL)
        {
            sslContexIndex = i;           
            break;
        }
    }
    
    if(sslContexIndex == -1) return FALSE;

    // create and init mbedTLS nanoFramework context
    // this needs to be freed in ssl_exit_context_internal
    context = (mbedTLS_NFContext*)platform_malloc(sizeof(mbedTLS_NFContext));
    if(context == NULL)
    {
        goto error;
    }  

    // allocate memory for net context 
    context->server_fd = (mbedtls_net_context*)platform_malloc(sizeof(mbedtls_net_context));
    if(context->server_fd == NULL)
    {
        goto error;
    }

    if(isServer)
    {
        endpoint = MBEDTLS_SSL_IS_SERVER;

        // TODO NOT IMPLEMENTED
        // // TODO:  we should be setting up the CA list
    }
    else
    {
        endpoint = MBEDTLS_SSL_IS_CLIENT;
    }

    // create and init CTR_DRBG
    // this needs to be freed in ssl_exit_context_internal
    context->ctr_drbg = (mbedtls_ctr_drbg_context*)platform_malloc(sizeof(mbedtls_ctr_drbg_context));
    if(context->ctr_drbg == NULL)
    {
        goto error;
    }    
    mbedtls_ctr_drbg_init( context->ctr_drbg );

    // create and init SSL context
    // this needs to be freed in ssl_exit_context_internal
    context->ssl = (mbedtls_ssl_context*)platform_malloc(sizeof(mbedtls_ssl_context));
    if(context->ssl == NULL)
    {
        goto error;
    }
    mbedtls_ssl_init(context->ssl);

    // create and init SSL configuration
    // this needs to be freed in ssl_exit_context_internal
    context->conf = (mbedtls_ssl_config*)platform_malloc(sizeof(mbedtls_ssl_config));
    if(context->conf == NULL)
    {
        goto error;
    }

    mbedtls_ssl_config_init( context->conf );

    // create and init X509 CRT
    // this needs to be freed in ssl_exit_context_internal
    context->x509_crt = (mbedtls_x509_crt*)platform_malloc(sizeof(mbedtls_x509_crt));
    if(context->x509_crt == NULL)
    {
        goto error;
    }
    mbedtls_x509_crt_init( context->x509_crt );

    // create and init entropy context
    // this needs to be freed in ssl_exit_context_internal
    context->entropy = (mbedtls_entropy_context*)platform_malloc(sizeof(mbedtls_entropy_context));
    if(context->entropy == NULL)
    {
        goto error;
    }
    mbedtls_entropy_init( context->entropy );

    // TODO: review if we can add some instance-unique data to the custom argument bellow
    if( mbedtls_ctr_drbg_seed( context->ctr_drbg, mbedtls_entropy_func, context->entropy, NULL, 0 ) != 0 )
    {
        // ctr_drbg_seed_failed
        goto error;
    }

    if( mbedtls_ssl_config_defaults( context->conf,
                endpoint,
                MBEDTLS_SSL_TRANSPORT_STREAM,
                MBEDTLS_SSL_PRESET_DEFAULT ) != 0 )
    {
        // ssl_config_defaults_failed
        goto error;
    }

    // configure protocol
    switch((SslProtocols)sslMode)
    {
        case SslProtocols_SSLv3:
            context->conf->min_major_ver = 3;
            context->conf->min_minor_ver = 0;
            context->conf->max_major_ver = 3;
            context->conf->max_minor_ver = 0;
            break;

        case SslProtocols_TLSv1:
            context->conf->min_major_ver = 1;
            context->conf->min_minor_ver = 0;
            context->conf->max_major_ver = 1;
            context->conf->max_minor_ver = 0;
            break;

        case SslProtocols_TLSv11:
            context->conf->min_major_ver = 1;
            context->conf->min_minor_ver = 1;
            context->conf->max_major_ver = 1;
            context->conf->max_minor_ver = 1;
            break;

        case SslProtocols_TLSv12:
            context->conf->min_major_ver = 1;
            context->conf->min_minor_ver = 2;
            context->conf->max_major_ver = 1;
            context->conf->max_minor_ver = 2;
            break;

        default:
            // shouldn't reach here!
            return FALSE;
    }


    mbedtls_ssl_conf_rng( context->conf, mbedtls_ctr_drbg_random, context->ctr_drbg );

    // parse certificate if passed
    if(certificate != NULL && certLength > 0)
    {
        // need to add an extra byte here because the mbed TLS API is expecting 
        // the buffer length INCLUDING the terminator
        if(mbedtls_x509_crt_parse( context->x509_crt, (const unsigned char*)certificate, certLength + 1 ) != 0)
        {
            // x509_crt_parse_failed
            goto error;
        }

        mbedtls_ssl_conf_ca_chain( context->conf, context->x509_crt, NULL );
    }

    // set certificate verification
    // the current options provided by mbed TLS are only verify or don't verify
    if((SslVerification)sslVerify == SslVerification_CertificateRequired)
    {
        authMode = MBEDTLS_SSL_VERIFY_REQUIRED;
    }
    mbedtls_ssl_conf_authmode( context->conf, authMode );

    // set bellow the threshold level for debug messages
    // check mbed TLS mbedtls/debug.h header for details.
    // Debug levels:
    // 0 No debug
    // 1 Error
    // 2 State change
    // 3 Informational
    // 4 Verbose
    mbedtls_debug_set_threshold( 2 );
    mbedtls_ssl_conf_dbg( context->conf, nf_debug, stdout );

    if( mbedtls_ssl_setup( context->ssl, context->conf ) != 0 )
    {
        // ssl_setup_failed
        goto error;
    }

    //////////////////////////////////////

    g_SSL_Driver.m_sslContextArray[sslContexIndex].SslContext = context;
    g_SSL_Driver.m_sslContextCount++;

    sslContextHandle = sslContexIndex;

    return TRUE;

error:

    // check for any memory allocation that needs to be freed before exiting
    if(context->ssl != NULL) platform_free(context->ssl);
    if(context->conf != NULL) platform_free(context->conf);
    if(context->entropy != NULL) platform_free(context->entropy);
    if(context->ctr_drbg != NULL) platform_free(context->ctr_drbg);
    if(context->server_fd != NULL) platform_free(context->server_fd);
    if(context->x509_crt != NULL) platform_free(context->x509_crt);

    return FALSE;
}
