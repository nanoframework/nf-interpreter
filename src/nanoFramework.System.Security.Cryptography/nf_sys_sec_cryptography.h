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
#include <esp32_idf.h>
#include <mbedtls/esp_config.h>
#else
#include <nf_mbedtls_config.h>
#endif

#include <mbedtls/platform.h>
#include <mbedtls/md.h>
#include <mbedtls/md_internal.h>

struct Library_nf_sys_sec_cryptography_System_Security_Cryptography_HMACSHA256
{
    static const int FIELD___disposed = 1;
    static const int FIELD___keyValue = 2;
    static const int FIELD___hashValue = 3;

    NANOCLR_NATIVE_DECLARE(HashCore___STATIC__SZARRAY_U1__SZARRAY_U1__SZARRAY_U1);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Security_Cryptography;

#endif // NF_SYS_SEC_CRYPTOGRAPHY_H
