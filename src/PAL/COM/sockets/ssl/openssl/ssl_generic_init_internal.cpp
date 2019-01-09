//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include <openssl/ssl.h>
#include <openssl.h> 


// #include <NANOCLR/ssl_functions.h>
// #include <openssl/ssl.h>
// #include <openssl/err.h>
// #include <openssl.h>
// #include <PKCS11\Tokens\OpenSSL\OpenSSL_pkcs11.h>

// TODO - FIXME
//extern CK_RV Cryptoki_GetSlotIDFromSession(CK_SESSION_HANDLE session, CK_SLOT_ID_PTR pSlotID, CryptokiSession** ppSession);

// FIXME - sort out an error print function
void ERR_print_errors_fp()
{


}


// int hal_fprintf_ssl(const char* format, ... )
// {
//     va_list arg_ptr;
//     int     chars;

//     va_start( arg_ptr, format );

//     chars = hal_vprintf( format, arg_ptr );

//     va_end( arg_ptr );

//     return chars;
// }



bool ssl_generic_init_internal( int sslMode, int sslVerify, const char* certificate, 
    int certLength, const char* certPassword, int& sslContextHandle, bool isServer )
{
    SSL*                ssl = NULL;
    SSL_CTX*            ctx = NULL;
    SSL_METHOD*         meth = NULL;
    X509*               cert_x509 = NULL;
    EVP_PKEY*           pkey = NULL;

    int                 sslCtxIndex = -1;

    // we only have one CA root bundle, so this is fixed to 0
    uint32_t configIndex = 0;

    for(uint32_t i=0; i<ARRAYSIZE(g_SSL_Driver.m_sslContextArray); i++)
    { 
        if(g_SSL_Driver.m_sslContextArray[i].SslContext == NULL)
        {
            sslCtxIndex = i;           
            break;
        }
    }
    
    if(sslCtxIndex == -1) return FALSE;

    if(isServer)
    {
        // TODO:  we should be setting up the CA list
         //SSL_CTX_set_client_CA_list

        switch((SslProtocols)sslMode)
        {
            case SslProtocols_SSLv3:
                meth = (SSL_METHOD*)SSLv3_server_method();
                break;

            case SslProtocols_TLSv1:
                meth = (SSL_METHOD*)TLSv1_server_method();
                break;

            case SslProtocols_TLSv11:
                meth = (SSL_METHOD*)TLSv1_1_server_method();
                break;

            case SslProtocols_TLSv12:
                meth = (SSL_METHOD*)TLSv1_2_server_method();
                break;

            default:
                // shouldn't reach here!
                return FALSE;
        }
    }
    else
    {
        switch((SslProtocols)sslMode)
        {
            case SslProtocols_SSLv3:
                meth = (SSL_METHOD*)SSLv3_client_method();
                break;

            case SslProtocols_TLSv1:
                meth = (SSL_METHOD*)TLSv1_client_method();
                break;

            case SslProtocols_TLSv11:
                meth = (SSL_METHOD*)TLSv1_1_client_method();
                break;

            case SslProtocols_TLSv12:
                meth = (SSL_METHOD*)TLSv1_2_client_method();
                break;

            default:
                // shouldn't reach here!
                return FALSE;
        }
    }

    // SSL_CTX is freed along with SSL in ssl_exit_context_internal
    ctx = SSL_CTX_new (meth);

    if (ctx == NULL)
    {
        ERR_print_errors_fp();
        goto err;
    }

    if(certificate != NULL && certLength > 0)
    {
        // Certificate Handle passed
        if(certLength == sizeof(INT32))
        {
            // TODO - FIXME     
            goto err;
            // CryptokiSession* pSession;
            // CK_SLOT_ID  slotID;
            // OBJECT_DATA* pObj;
            // CERT_DATA* pCert;
            
            // if(CKR_OK != Cryptoki_GetSlotIDFromSession(sslContextHandle, &slotID, &pSession)) return FALSE;

            // pObj = PKCS11_Objects_OpenSSL::GetObjectFromHandle(&pSession->Context, *(INT32*)certificate);

            // if(pObj == NULL || pObj->Type != CertificateType) return FALSE;

            // pCert = (CERT_DATA*)pObj->Data;

            // cert_x509 = pCert->cert;
            // pkey = (EVP_PKEY*)pCert->privKeyData.key;

            // g_SSL_Driver.m_sslContextArray[sslCtxIndex].CryptokiSession = sslContextHandle;
        }
        else
        {
            cert_x509 = ssl_parse_certificate((void*)certificate, certLength, certPassword, &pkey);

            if (cert_x509 == NULL || pkey == NULL)
            {
                ERR_print_errors_fp();
                goto err;
            }
        }
    }

    if(cert_x509 != NULL)
    {
        if (SSL_CTX_use_certificate(ctx, cert_x509) <= 0) 
        {
            ERR_print_errors_fp();
            goto err;
        }
    }

    if(pkey != NULL)
    {
        if (SSL_CTX_use_PrivateKey(ctx, pkey) <= 0) 
        {
            ERR_print_errors_fp();
            goto err;
        }
#ifndef NO_CHECK_PRIVATE_KEY        
        if (!SSL_CTX_check_private_key(ctx)) 
        {
            NANOCLR_SSL_FPRINTF("Private key does not match the certificate public key\n");
            goto err;
        }
#endif
    }

    // create the SSL object
    ssl = SSL_new(ctx);
    if (ssl == NULL)
    {
        ERR_print_errors_fp();
        goto err;
    }
    
    if (ssl == NULL) goto err;

    // CA root certs from store, if available
    if(g_TargetConfiguration.CertificateStore->Count > 0)
    {
        cert_x509 = ssl_parse_certificate(
            (void*)g_TargetConfiguration.CertificateStore->Certificates[configIndex]->Certificate,
            g_TargetConfiguration.CertificateStore->Certificates[configIndex]->CertificateSize, NULL, NULL);

        if(cert_x509 != NULL)
        {
            X509_STORE_add_cert(SSL_CTX_get_cert_store(ctx), cert_x509);
        }
    }

    if(certLength != sizeof(INT32) || cert_x509 != NULL)
    {
        if(cert_x509 != NULL) X509_free(cert_x509);
    }

    // NANOCLR_SSL_VERIFY_XXX >> 1 == SSL_VERIFY_xxx
    ssl->verify_mode = (sslVerify >> 1);

    g_SSL_Driver.m_sslContextArray[sslCtxIndex].SslContext = ssl;
    g_SSL_Driver.m_sslContextCount++;

    sslContextHandle = sslCtxIndex;

    return (ctx != NULL);

err:
    if(ssl != NULL) SSL_free(ssl);
    if(ctx != NULL) SSL_CTX_free(ctx);
    
    if(certLength != sizeof(INT32))
    {
        X509_free(cert_x509);
        EVP_PKEY_free(pkey);
    }
    
    return FALSE;
}

