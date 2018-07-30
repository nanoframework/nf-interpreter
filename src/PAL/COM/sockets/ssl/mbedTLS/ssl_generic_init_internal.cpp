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
    mbedtls_x509_crt ca;
    mbedtls_entropy_context* entropy = NULL;
    mbedtls_ctr_drbg_context* ctr_drbg = NULL;
    mbedtls_ssl_context* ssl = NULL;
    mbedtls_ssl_config* conf = NULL;

    ///////////////////////

    for(uint32_t i=0; i<ARRAYSIZE(g_SSL_Driver.m_sslContextArray); i++)
    { 
        if(g_SSL_Driver.m_sslContextArray[i].SslContext == NULL)
        {
            sslContexIndex = i;           
            break;
        }
    }
    
    if(sslContexIndex == -1) return FALSE;

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
    ctr_drbg = (mbedtls_ctr_drbg_context*)platform_malloc(sizeof(mbedtls_ctr_drbg_context));
    if(ctr_drbg == NULL)
    {
        goto error;
    }    
    mbedtls_ctr_drbg_init( ctr_drbg );

    // create and init SSL context
    // this needs to be freed in ssl_exit_context_internal
    ssl = (mbedtls_ssl_context*)platform_malloc(sizeof(mbedtls_ssl_context));
    if(ssl == NULL)
    {
        goto error;
    }
    mbedtls_ssl_init(ssl);

    // create and init SSL configuration
    // this needs to be freed in ssl_exit_context_internal
    conf = (mbedtls_ssl_config*)platform_malloc(sizeof(mbedtls_ssl_config));
    if(conf == NULL)
    {
        goto error;
    }

    mbedtls_ssl_config_init( conf );
    mbedtls_x509_crt_init( &ca );

    // create and init entropy context
    // this needs to be freed in ssl_exit_context_internal
    entropy = (mbedtls_entropy_context*)platform_malloc(sizeof(mbedtls_entropy_context));
    if(entropy == NULL)
    {
        goto error;
    }
    mbedtls_entropy_init( entropy );

    // TODO: review if we can add some instance-unique data to the custom argument bellow
    if( mbedtls_ctr_drbg_seed( ctr_drbg, mbedtls_entropy_func, entropy, NULL, 0 ) != 0 )
    {
        // ctr_drbg_seed_failed
        goto error;
    }

    if( mbedtls_ssl_config_defaults( conf,
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
            conf->min_major_ver = 3;
            conf->min_minor_ver = 0;
            conf->max_major_ver = 3;
            conf->max_minor_ver = 0;
            break;

        case SslProtocols_TLSv1:
            conf->min_major_ver = 1;
            conf->min_minor_ver = 0;
            conf->max_major_ver = 1;
            conf->max_minor_ver = 0;
            break;

        case SslProtocols_TLSv11:
            conf->min_major_ver = 1;
            conf->min_minor_ver = 1;
            conf->max_major_ver = 1;
            conf->max_minor_ver = 1;
            break;

        case SslProtocols_TLSv12:
            conf->min_major_ver = 1;
            conf->min_minor_ver = 2;
            conf->max_major_ver = 1;
            conf->max_minor_ver = 2;
            break;

        default:
            // shouldn't reach here!
            return FALSE;
    }


    mbedtls_ssl_conf_rng( conf, mbedtls_ctr_drbg_random, ctr_drbg );

    // parse certificate if passed
    if(certificate != NULL && certLength > 0)
    {
        if(mbedtls_x509_crt_parse( &ca, (const unsigned char*)certificate, certLength ) != 0)
        {
            // x509_crt_parse_failed
            goto error;
        }

        mbedtls_ssl_conf_ca_chain( conf, &ca, NULL );
        mbedtls_ssl_conf_authmode( conf, MBEDTLS_SSL_VERIFY_REQUIRED );
    }

    // set certificate verification
    // the current options provided by mbed TLS are only verify or don't verify
    if((SslVerification)sslVerify == SslVerification_CertificateRequired)
    {
        authMode = MBEDTLS_SSL_VERIFY_REQUIRED;
    }
    mbedtls_ssl_conf_authmode( conf, authMode );

    // set bellow the threshold level for debug messages
    // check mbed TLS mbedtls/debug.h header for details.
    // Debug levels:
    // 0 No debug
    // 1 Error
    // 2 State change
    // 3 Informational
    // 4 Verbose
    mbedtls_debug_set_threshold( 2 );
    mbedtls_ssl_conf_dbg( conf, nf_debug, stdout );

    if( mbedtls_ssl_setup( ssl, conf ) != 0 )
    {
        // ssl_setup_failed
        goto error;
    }

    //////////////////////////////////////

    g_SSL_Driver.m_sslContextArray[sslContexIndex].SslContext = ssl;
    g_SSL_Driver.m_sslContextCount++;

    sslContextHandle = sslContexIndex;

    return TRUE;

error:

    // check for any memory allocation that needs to be freed before exiting
    if(ssl != NULL) platform_free(ssl);
    if(conf != NULL) platform_free(conf);
    if(entropy != NULL) platform_free(entropy);
    if(ctr_drbg != NULL) platform_free(ctr_drbg);

    return FALSE;
}
