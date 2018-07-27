//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include "mbedtls.h"

bool ssl_generic_init_internal( int sslMode, int sslVerify, const char* certificate, 
    int certLength, const char* certPassword, int& sslContextHandle, bool isServer )
{
    (void)sslMode;
    (void)sslVerify;
    (void)certPassword;

    //SSL*                ssl = NULL;
    //SSL_CTX*            ctx = NULL;
    //SSL_METHOD*         meth = NULL;
    //X509*               cert_x509 = NULL;
    //EVP_PKEY*           pkey = NULL;

    int                 sslContexIndex = -1;


    ///////////////////////
    //mbedtls_net_context server_fd;
    mbedtls_x509_crt ca;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context* ssl;
    mbedtls_ssl_config conf;

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
        // TODO NOT IMPLEMENTED
        // // TODO:  we should be setting up the CA list
        //  //SSL_CTX_set_client_CA_list

        // if (sslMode & NANOCLR_SSL_MODE_TLS12 )
        // {
        //     meth = (SSL_METHOD*)TLSv1_2_server_method();
        // }
        // else
        // if (sslMode & NANOCLR_SSL_MODE_TLS11 )
        // {
        //     meth = (SSL_METHOD*)TLSv1_1_server_method();
        // }
        // else
        // if(sslMode & NANOCLR_SSL_MODE_TLS1)
        // {
        //     meth = (SSL_METHOD*)TLSv1_server_method();
        // }
        // else
        // {
        //     meth = (SSL_METHOD*)SSLv3_server_method();  
        // }
    }
    else
    {
        // if ((sslMode & NANOCLR_SSL_MODE_TLS12) == NANOCLR_SSL_MODE_TLS12)
        // {
        //     meth = (SSL_METHOD*)TLSv1_2_client_method();
        // }
        // else if ((sslMode & NANOCLR_SSL_MODE_TLS11 ) == NANOCLR_SSL_MODE_TLS11)
        // {
        //     meth = (SSL_METHOD*)TLSv1_1_client_method();
        // }
        // else if((sslMode & NANOCLR_SSL_MODE_TLS1) == NANOCLR_SSL_MODE_TLS1)
        // {
        //     meth = (SSL_METHOD*)TLSv1_client_method();
        // }
        // else
        // {
        //     meth = (SSL_METHOD*)SSLv3_client_method();  
        // }
    }

    //////////////////////////////////////
    mbedtls_ctr_drbg_init( &ctr_drbg );

    //mbedtls_net_init( &server_fd );

    // this needs to be freed in ssl_exit_context_internal
    ssl = (mbedtls_ssl_context*)platform_malloc(sizeof(mbedtls_ssl_context));
    if(ssl == NULL)
    {
        goto error;
    }
    mbedtls_ssl_init(ssl);

    mbedtls_ssl_config_init( &conf );
    mbedtls_x509_crt_init( &ca );

    mbedtls_entropy_init( &entropy );

    // TODO: review if we can add some instance-unique data to the custom argument bellow
    if( mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0 ) != 0 )
    {
        // ctr_drbg_seed_failed
        goto error;
    }

    if( mbedtls_ssl_config_defaults( &conf,
                MBEDTLS_SSL_IS_CLIENT,
                MBEDTLS_SSL_TRANSPORT_STREAM,
                MBEDTLS_SSL_PRESET_DEFAULT ) != 0 )
    {
        // ssl_config_defaults_failed
        goto error;
    }

    mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctr_drbg );

// #if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
//     mbedtls_ssl_conf_psk( &conf, psk, sizeof( psk ),
//                 (const unsigned char *) psk_id, sizeof( psk_id ) - 1 );
// #endif

    // parse certificate if passed
    if(certificate != NULL && certLength > 0)
    {
        if(mbedtls_x509_crt_parse_der( &ca, (const unsigned char*)certificate, certLength ) != 0)
        {
            // x509_crt_parse_failed
            goto error;
        }

        mbedtls_ssl_conf_ca_chain( &conf, &ca, NULL );
        mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_REQUIRED );
    }

    if( mbedtls_ssl_setup( ssl, &conf ) != 0 )
    {
        // ssl_setup_failed
        goto error;
    }

    // if( mbedtls_ssl_set_hostname( &ssl, HOSTNAME ) != 0 )
    // {
    //     // hostname_failed
    //     goto error;
    // }


    //////////////////////////////////////

    g_SSL_Driver.m_sslContextArray[sslContexIndex].SslContext = &ssl;
    g_SSL_Driver.m_sslContextCount++;

    sslContextHandle = sslContexIndex;

    return TRUE;

error:

    // check for any memory allocation that needs to be freed before exiting
    if(ssl != NULL) platform_free(ssl);
    
    return FALSE;
}
