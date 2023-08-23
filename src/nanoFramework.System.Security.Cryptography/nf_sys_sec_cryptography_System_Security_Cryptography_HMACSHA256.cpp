//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_sec_cryptography.h"

HRESULT Library_nf_sys_sec_cryptography_System_Security_Cryptography_HMACSHA256::
    HashCore___STATIC__SZARRAY_U1__SZARRAY_U1__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if MBEDTLS_VERSION_MAJOR >= 3
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
#else
    const mbedtls_md_info_t *md_info = &mbedtls_sha256_info;
#endif

    mbedtls_md_context_t ctx;

    CLR_RT_HeapBlock_Array *keyArray;
    CLR_RT_HeapBlock_Array *sourceArray;
    CLR_RT_HeapBlock_Array *outputArray;

    // grab pointers to the arguments
    keyArray = stack.Arg0().DereferenceArray();
    FAULT_ON_NULL_ARG(keyArray);

    sourceArray = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL_ARG(sourceArray);

    // create the return array (32 bytes length)
    stack.PushValueAndClear();
    NANOCLR_CHECK_HRESULT(
        CLR_RT_HeapBlock_Array::CreateInstance(stack.TopValue(), 32, g_CLR_RT_WellKnownTypes.m_UInt8));
    outputArray = stack.TopValue().DereferenceArray();

    // better clear memory
    memset(outputArray->GetFirstElement(), 0, outputArray->m_numOfElements);

    // need to initialize MbedTLS stuff
    mbedtls_md_init(&ctx);
    if (mbedtls_md_setup(&ctx, md_info, 1) != 0)
    {
        // failed to initialize
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // perform calculation
    if (mbedtls_md_hmac_starts(&ctx, keyArray->GetFirstElement(), keyArray->m_numOfElements) != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    if (mbedtls_md_hmac_update(&ctx, sourceArray->GetFirstElement(), sourceArray->m_numOfElements) != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    if (mbedtls_md_hmac_finish(&ctx, outputArray->GetFirstElement()) != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // need to clean up MbedTLS stuff
    NANOCLR_CLEANUP();

    mbedtls_md_free(&ctx);

    NANOCLR_CLEANUP_END();
}
