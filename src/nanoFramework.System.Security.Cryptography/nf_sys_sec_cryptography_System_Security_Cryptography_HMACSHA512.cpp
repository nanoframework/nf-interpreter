//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_sec_cryptography.h"

// SHA-512 digest size
#define HASHED_KEY_BUFFER_SIZE 64

HRESULT Library_nf_sys_sec_cryptography_System_Security_Cryptography_HMACSHA512::
    HashCore___STATIC__SZARRAY_U1__SZARRAY_U1__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if MBEDTLS_VERSION_MAJOR >= 3
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA512);
#else
    const mbedtls_md_info_t *md_info = &mbedtls_sha512_info;
#endif

    mbedtls_md_context_t ctx;

    CLR_RT_HeapBlock_Array *keyArray;
    CLR_RT_HeapBlock_Array *sourceArray;
    CLR_RT_HeapBlock_Array *outputArray;

    uint8_t *keyBuffer;
    size_t keyLength;
    uint8_t hashedKey[HASHED_KEY_BUFFER_SIZE]; // SHA-512 digest size
    bool keyWasHashed = false;

    // grab pointers to the arguments and validate
    keyArray = stack.Arg0().DereferenceArray();
    FAULT_ON_NULL_ARG(keyArray);

    sourceArray = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL_ARG(sourceArray);

    // HMAC-SHA512 block size is 128 bytes
    // If key is longer than block size, hash it with SHA-512 to derive a 64-byte key
    if (keyArray->m_numOfElements > 128)
    {
        if (mbedtls_md(md_info, keyArray->GetFirstElement(), keyArray->m_numOfElements, hashedKey) != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        keyBuffer = hashedKey;
        keyLength = HASHED_KEY_BUFFER_SIZE;
        keyWasHashed = true;
    }
    else
    {
        keyBuffer = keyArray->GetFirstElement();
        keyLength = keyArray->m_numOfElements;
    }

    // create the return array (64 bytes length)
    stack.PushValueAndClear();
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
        stack.TopValue(),
        HASHED_KEY_BUFFER_SIZE,
        g_CLR_RT_WellKnownTypes.m_UInt8));
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
    if (mbedtls_md_hmac_starts(&ctx, keyBuffer, keyLength) != 0)
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

    // clear sensitive key material if it was hashed
    if (keyWasHashed)
    {
        memset(hashedKey, 0, sizeof(hashedKey));
    }

    NANOCLR_CLEANUP_END();
}
