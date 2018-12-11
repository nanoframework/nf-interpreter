//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include <openssl/ssl.h>
#include <openssl.h> 


// #include <tinyclr/ssl_functions.h>
// #include <openssl/ssl.h>
// #include <openssl.h>
// #include <PKCS12\PKCS12.h>
// #include <PKCS11\Tokens\OpenSSL\OpenSSL_pkcs11.h>

//extern CK_RV Cryptoki_GetSlotIDFromSession(CK_SESSION_HANDLE session, CK_SLOT_ID_PTR pSlotID, CryptokiSession** ppSession);

bool ssl_add_cert_auth_internal( int sslContextHandle, const char* certificate, 
	int certLength, const char* certPassword )
{
    SSL *ssl = NULL;
    int ret = FALSE;
    X509 *tempCertificate=NULL;

    if((sslContextHandle >= ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0))
    {
        goto error;
    }

    ssl = (SSL*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
    if (ssl == NULL)
    {
        goto error;
    }

    // if(certLength == sizeof(INT32))
    // {
    //     CryptokiSession* pSession;
    //     CK_SLOT_ID  slotID;
    //     OBJECT_DATA* pObj;
    //     CERT_DATA* pCert;
    //     CK_SESSION_HANDLE sessCtx;

    //     if(certPassword == NULL) return FALSE;

    //     sessCtx = *(INT32*)certPassword;
        
    //     if(CKR_OK != Cryptoki_GetSlotIDFromSession(sessCtx, &slotID, &pSession)) return FALSE;

    //     pObj = PKCS11_Objects_OpenSSL::GetObjectFromHandle(&pSession->Context, *(INT32*)certificate);

    //     if(pObj == NULL || pObj->Type != CertificateType) return FALSE;

    //     pCert = (CERT_DATA*)pObj->Data;

    //     certificate = pCert->cert;
    // }
    // else
    {
        tempCertificate = ssl_parse_certificate((void*)certificate, certLength, certPassword, NULL);
    }

    if(tempCertificate != NULL)
    {
        SSL_CTX* pCtx = SSL_get_SSL_CTX(ssl);

        if(pCtx == NULL) 
        {
            X509_free(tempCertificate);
                
            return FALSE;
        }

        ret = X509_STORE_add_cert(SSL_CTX_get_cert_store(pCtx), tempCertificate);

        X509_free(tempCertificate);
    }

error:
	return ret;
}
