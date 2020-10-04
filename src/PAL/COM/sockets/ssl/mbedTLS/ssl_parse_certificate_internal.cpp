//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "mbedtls.h"

void SSL_GetCertDateTime_internal(
    DATE_TIME_INFO * dt, 
    mbedtls_x509_time * mt )
{
    dt->year = mt->year;
    dt->month = mt->mon;
    dt->day = mt->day;
    dt->hour = mt->hour;
    dt->minute = mt->min;
    dt->second = mt->sec;

    dt->dlsTime = 0;
    dt->tzOffset = 0;
}

bool ssl_parse_certificate_internal(
    void * certificate, 
    size_t size, 
    void* pwd, 
    void* x509CertData)
{
    (void)pwd;

    int ret;
    X509CertData* x509 = (X509CertData*)x509CertData;

    mbedtls_x509_crt cacert;
    mbedtls_x509_crt_init(&cacert);
    
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // developer notes:                                                                            //
    // this call parses certificates in both string and binary formats                             //
    // when the formart is a string it has to include the terminator otherwise the parse will fail //
    /////////////////////////////////////////////////////////////////////////////////////////////////
    ret = mbedtls_x509_crt_parse(
        &cacert, 
        (const unsigned char *)certificate, 
        size);
    if(ret < 0)
    {
        return false;
    }
 
    mbedtls_x509_dn_gets( 
        x509->Issuer, 
        sizeof(x509->Issuer)-1, 
        &cacert.issuer );

    mbedtls_x509_dn_gets( 
        x509->Subject, 
        sizeof(x509->Subject)-1, 
        &cacert.subject );

    SSL_GetCertDateTime_internal( 
        &x509->EffectiveDate,
        &cacert.valid_from );

    SSL_GetCertDateTime_internal(
        &x509->ExpirationDate,
        &cacert.valid_to );

    return true;
}
