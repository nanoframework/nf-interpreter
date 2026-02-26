//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_SYS_SEC_CRYPTOGRAPHY_H
#define NF_SYS_SEC_CRYPTOGRAPHY_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

#ifdef PLATFORM_ESP32
#define MBEDTLS_SHA256_C
#include <esp32_idf.h>
#include <mbedtls/esp_config.h>
#else
#include <nf_mbedtls_config.h>
#endif

#include "mbedtls/version.h"
#include <mbedtls/platform.h>
#include <mbedtls/md.h>
#include <mbedtls/cipher.h>

typedef enum __nfpack CipherMode
{
    CipherMode_None = 0,
    CipherMode_CBC = 1,
    CipherMode_ECB = 2,
} CipherMode;

struct Library_nf_sys_sec_cryptography_System_Security_Cryptography_Aes
{
    static const int FIELD___mode = 1;
    static const int FIELD___key = 2;
    static const int FIELD___iv = 3;

    NANOCLR_NATIVE_DECLARE(EncryptAesEcb___SZARRAY_U1__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(DecryptAesEcb___SZARRAY_U1__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(EncryptAesCbc___SZARRAY_U1__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(DecryptAesCbc___SZARRAY_U1__SZARRAY_U1);

    //--//
    static HRESULT EncryptDecrypt(
        CLR_RT_StackFrame &stack,
        mbedtls_cipher_type_t cipher,
        mbedtls_operation_t operation);
};

struct Library_nf_sys_sec_cryptography_System_Security_Cryptography_HMACSHA256
{
    static const int FIELD___disposed = 1;
    static const int FIELD___keyValue = 2;
    static const int FIELD___hashValue = 3;

    NANOCLR_NATIVE_DECLARE(HashCore___STATIC__SZARRAY_U1__SZARRAY_U1__SZARRAY_U1);

    //--//
};

struct Library_nf_sys_sec_cryptography_System_Security_Cryptography_HMACSHA512
{
    static const int FIELD___disposed = 1;
    static const int FIELD___keyValue = 2;
    static const int FIELD___hashValue = 3;

    NANOCLR_NATIVE_DECLARE(HashCore___STATIC__SZARRAY_U1__SZARRAY_U1__SZARRAY_U1);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Security_Cryptography;

#endif // NF_SYS_SEC_CRYPTOGRAPHY_H
