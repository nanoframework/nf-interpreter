//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h>
#include "mbedtls.h"
#include "mbedtls/debug.h"
#include <mbedtls/pk_internal.h>

bool ssl_generic_init_internal(
    int sslMode,
    int sslVerify,
    const char *certificate,
    int certLength,
    const uint8_t *privateKey,
    int privateKeyLength,
    const char *pkPassword,
    int pkPasswordLength,
    int &contextHandle,
    bool useDeviceCertificate,
    bool isServer)
{
    int minVersion = MBEDTLS_SSL_MINOR_VERSION_3;
    int maxVersion = MBEDTLS_SSL_MINOR_VERSION_1;
    int sslContexIndex = -1;
    int authMode = MBEDTLS_SSL_VERIFY_NONE;
    int endpoint = 0;
    int ret = 0;

    mbedtls_x509_crt *ownCertificate = NULL;
    HAL_Configuration_X509CaRootBundle *certStore = NULL;
    HAL_Configuration_X509DeviceCertificate *deviceCert = NULL;

    ///////////////////////
    mbedTLS_NFContext *context;

    for (uint32_t i = 0; i < ARRAYSIZE(g_SSL_Driver.ContextArray); i++)
    {
        if (g_SSL_Driver.ContextArray[i].Context == NULL)
        {
            sslContexIndex = i;
            break;
        }
    }

    if (sslContexIndex == -1)
        return FALSE;

    // create and init MbedTLS nanoFramework context
    // this needs to be freed in ssl_exit_context_internal
    context = (mbedTLS_NFContext *)platform_malloc(sizeof(mbedTLS_NFContext));
    if (context == NULL)
    {
        goto error;
    }

    memset(context, 0, sizeof(mbedTLS_NFContext));

    // allocate memory for net context
    context->server_fd = (mbedtls_net_context *)platform_malloc(sizeof(mbedtls_net_context));
    if (context->server_fd == NULL)
    {
        goto error;
    }

    memset(context->server_fd, 0, sizeof(mbedtls_net_context));

    if (isServer)
    {
        endpoint = MBEDTLS_SSL_IS_SERVER;
    }
    else
    {
        endpoint = MBEDTLS_SSL_IS_CLIENT;
    }

    // create and init private key context
    // this needs to be freed in ssl_exit_context_internal
    context->pk = (mbedtls_pk_context *)platform_malloc(sizeof(mbedtls_pk_context));
    if (context->pk == NULL)
    {
        goto error;
    }

    mbedtls_pk_init(context->pk);

    // create and init CTR_DRBG
    // this needs to be freed in ssl_exit_context_internal
    context->ctr_drbg = (mbedtls_ctr_drbg_context *)platform_malloc(sizeof(mbedtls_ctr_drbg_context));
    if (context->ctr_drbg == NULL)
    {
        goto error;
    }
    mbedtls_ctr_drbg_init(context->ctr_drbg);

    // create and init SSL context
    // this needs to be freed in ssl_exit_context_internal
    context->ssl = (mbedtls_ssl_context *)platform_malloc(sizeof(mbedtls_ssl_context));
    if (context->ssl == NULL)
    {
        goto error;
    }
    mbedtls_ssl_init(context->ssl);

    // create and init SSL configuration
    // this needs to be freed in ssl_exit_context_internal
    context->conf = (mbedtls_ssl_config *)platform_malloc(sizeof(mbedtls_ssl_config));
    if (context->conf == NULL)
    {
        goto error;
    }

    mbedtls_ssl_config_init(context->conf);

    // create and init X509 CRT
    // this needs to be freed in ssl_exit_context_internal
    context->x509_crt = (mbedtls_x509_crt *)platform_malloc(sizeof(mbedtls_x509_crt));
    if (context->x509_crt == NULL)
    {
        goto error;
    }
    mbedtls_x509_crt_init(context->x509_crt);

    // create and init entropy context
    // this needs to be freed in ssl_exit_context_internal
    context->entropy = (mbedtls_entropy_context *)platform_malloc(sizeof(mbedtls_entropy_context));
    if (context->entropy == NULL)
    {
        goto error;
    }
    mbedtls_entropy_init(context->entropy);

    // TODO: review if we can add some instance-unique data to the custom argument below
    if (mbedtls_ctr_drbg_seed(context->ctr_drbg, mbedtls_entropy_func, context->entropy, NULL, 0) != 0)
    {
        // ctr_drbg_seed_failed
        goto error;
    }

    if (mbedtls_ssl_config_defaults(
            context->conf,
            endpoint,
            MBEDTLS_SSL_TRANSPORT_STREAM,
            MBEDTLS_SSL_PRESET_DEFAULT) != 0)
    {
        // ssl_config_defaults_failed
        goto error;
    }

    // figure out the min and max protocol version to support
    // sanity check for none, application has to set the supported protocols
    if ((SslProtocols)sslMode == SslProtocols_None)
    {
        goto error;
    }

    // find minimum version
    if (sslMode & (SslProtocols_TLSv11 | SslProtocols_TLSv1))
    {
        minVersion = MBEDTLS_SSL_MINOR_VERSION_2;
    }
    if (sslMode & SslProtocols_TLSv1)
    {
        minVersion = MBEDTLS_SSL_MINOR_VERSION_1;
    }
    mbedtls_ssl_conf_min_version(context->conf, MBEDTLS_SSL_MAJOR_VERSION_3, minVersion);

    // find maximum version
    if (sslMode & (SslProtocols_TLSv12 | SslProtocols_TLSv11))
    {
        maxVersion = MBEDTLS_SSL_MINOR_VERSION_2;
    }
    if (sslMode & SslProtocols_TLSv12)
    {
        maxVersion = MBEDTLS_SSL_MINOR_VERSION_3;
    }
    mbedtls_ssl_conf_max_version(context->conf, MBEDTLS_SSL_MAJOR_VERSION_3, maxVersion);

    // configure random generator
    mbedtls_ssl_conf_rng(context->conf, mbedtls_ctr_drbg_random, context->ctr_drbg);

#if defined(PLATFORM_ESP32) && defined(CONFIG_MBEDTLS_DEBUG)
    mbedtls_esp_enable_debug_log(context->conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif

    // CA root certs from store, if available
    if (g_TargetConfiguration.CertificateStore->Count > 0)
    {
        // get certificate store
        certStore = ConfigurationManager_GetCertificateStore();

        if (certStore)
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////
            // developer notes:                                                                            //
            // don't care about failure in processing the CA cert bundle                                   //
            // the outcome is that the CA certs won't be loaded into the trusted CA chain                  //
            // this call parses certificates in both string and binary formats                             //
            // when the format is a string it has to include the terminator otherwise the parse will fail //
            /////////////////////////////////////////////////////////////////////////////////////////////////
            mbedtls_x509_crt_parse(
                context->x509_crt,
                (const unsigned char *)certStore->Certificate,
                certStore->CertificateSize);

            platform_free(certStore);
        }
    }

    // check if the device certificate is to be used
    if (useDeviceCertificate)
    {
        deviceCert = ConfigurationManager_GetDeviceCertificate();

        // check if there is a device certificate stored
        if (deviceCert)
        {
            // there is!
            // fill in the parameters like if it was supplied by the caller
            // OK to override, that's the intended behaviour
            certificate = (const char *)deviceCert->Certificate;
            certLength = deviceCert->CertificateSize;

            // the private key is also part of the device certificate
            privateKey = (const uint8_t *)deviceCert->Certificate;
            privateKeyLength = deviceCert->CertificateSize;
        }
    }

    // parse "own" certificate if passed
    if (certificate != NULL && certLength > 0)
    {
        // is there a private key?
        if (privateKey != NULL && privateKeyLength > 0)
        {

            if (mbedtls_pk_parse_key(
                    context->pk,
                    privateKey,
                    privateKeyLength,
                    (const unsigned char *)pkPassword,
                    pkPasswordLength) < 0)
            {
                // private key parse failed
                goto error;
            }
        }

        // parse certificate
        ownCertificate = (mbedtls_x509_crt *)platform_malloc(sizeof(mbedtls_x509_crt));
        if (ownCertificate == NULL)
        {
            goto error;
        }

        mbedtls_x509_crt_init(ownCertificate);

        if (mbedtls_x509_crt_parse(ownCertificate, (const unsigned char *)certificate, certLength))
        {
            // failed parsing own certificate failed
            goto error;
        }

        if (mbedtls_ssl_conf_own_cert(context->conf, ownCertificate, context->pk))
        {
            // configuring own certificate failed
            goto error;
        }

        // free memory, if allocated
        if (deviceCert)
        {
            platform_free(deviceCert);
        }
    }
    else
    {
        // no PK, need to set it to NULL
        context->pk = NULL;
    }

    mbedtls_ssl_conf_ca_chain(context->conf, context->x509_crt, NULL);

    // set certificate verification
    // the current options provided by Mbed TLS are only verify or don't verify
    if ((SslVerification)sslVerify == SslVerification_CertificateRequired)
    {
        authMode = MBEDTLS_SSL_VERIFY_REQUIRED;
    }
    mbedtls_ssl_conf_authmode(context->conf, authMode);

    // setup debug stuff
    // only required if output debug is enabled in mbedtls_config.h
#if defined(MBEDTLS_DEBUG_C) && defined(MBEDTLS_DEBUG_THRESHOLD)
    mbedtls_debug_set_threshold(MBEDTLS_DEBUG_THRESHOLD);
    mbedtls_ssl_conf_dbg(context->conf, nf_debug, stdout);
#endif

    ret = mbedtls_ssl_setup(context->ssl, context->conf);
    if (ret != 0)
    {
        // ssl_setup_failed
        goto error;
    }

    //////////////////////////////////////

    g_SSL_Driver.ContextArray[sslContexIndex].Context = context;
    g_SSL_Driver.ContextCount++;

    contextHandle = sslContexIndex;

    return true;

error:

    if (context->pk)
    {
        mbedtls_pk_free(context->pk);
    }

    if (context->server_fd)
    {
        mbedtls_net_free(context->server_fd);
    }

    mbedtls_ctr_drbg_free(context->ctr_drbg);
    mbedtls_entropy_free(context->entropy);
    mbedtls_x509_crt_free(context->x509_crt);
    mbedtls_ssl_config_free(context->conf);
    mbedtls_ssl_free(context->ssl);

    // check for any memory allocation that needs to be freed before exiting
    if (context->ssl)
    {
        platform_free(context->ssl);
    }

    if (context->conf)
    {
        platform_free(context->conf);
    }

    if (context->entropy)
    {
        platform_free(context->entropy);
    }

    if (context->ctr_drbg)
    {
        platform_free(context->ctr_drbg);
    }

    if (context->server_fd)
    {
        platform_free(context->server_fd);
    }

    if (context->x509_crt)
    {
        platform_free(context->x509_crt);
    }

    if (context->pk)
    {
        platform_free(context->pk);
    }

    if (ownCertificate)
    {
        mbedtls_x509_crt_free(ownCertificate);
        platform_free(ownCertificate);
    }
    if (context)
    {
        platform_free(context);
    }

    if (certStore)
    {
        platform_free(certStore);
    }

    if (deviceCert)
    {
        platform_free(deviceCert);
    }

    return false;
}
