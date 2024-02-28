//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_sec_cryptography.h"

HRESULT Library_nf_sys_sec_cryptography_System_Security_Cryptography_Aes::EncryptAesEcb___SZARRAY_U1__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    mbedtls_aes_context encodeContext;

    CLR_RT_HeapBlock_Array *keyArray;
    CLR_RT_HeapBlock_Array *plainTextArray;
    CLR_RT_HeapBlock_Array *cipherTextArray;
    CLR_RT_HeapBlock *pThis;

    pThis = (CLR_RT_HeapBlock *)stack.This();
    FAULT_ON_NULL(pThis);

    // grab key and check for NULL
    keyArray = pThis[FIELD___key].DereferenceArray();
    if (keyArray == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // grab plain text and check for NULL
    plainTextArray = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL_ARG(plainTextArray);

    // data has to be multiple of 16
    if (plainTextArray->m_numOfElements % 16 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // create the return array (same length as the input)
    stack.PushValueAndClear();
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
        stack.TopValue(),
        plainTextArray->m_numOfElements,
        g_CLR_RT_WellKnownTypes.m_UInt8));
    cipherTextArray = stack.TopValue().DereferenceArray();

    // init mbedtls context
    mbedtls_aes_init(&encodeContext);

    if (mbedtls_aes_setkey_enc(&encodeContext, keyArray->GetFirstElement(), keyArray->m_numOfElements * 8) ==
        MBEDTLS_ERR_AES_INVALID_KEY_LENGTH)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (mbedtls_aes_crypt_ecb(
            &encodeContext,
            MBEDTLS_AES_ENCRYPT,
            plainTextArray->GetFirstElement(),
            cipherTextArray->GetFirstElement()) != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_CLEANUP();

    // make sure nothing is left in memory
    mbedtls_aes_free(&encodeContext);

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_sec_cryptography_System_Security_Cryptography_Aes::DecryptAesEcb___SZARRAY_U1__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    mbedtls_aes_context decodeContext;

    CLR_RT_HeapBlock_Array *keyArray;
    CLR_RT_HeapBlock_Array *plainTextArray;
    CLR_RT_HeapBlock_Array *cipherTextArray;
    CLR_RT_HeapBlock *pThis;

    pThis = (CLR_RT_HeapBlock *)stack.This();
    FAULT_ON_NULL(pThis);

    // grab key and check for NULL
    keyArray = pThis[FIELD___key].DereferenceArray();
    if (keyArray == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // grab cipher text and check for NULL
    cipherTextArray = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL_ARG(cipherTextArray);

    // data has to be multiple of 16
    if (cipherTextArray->m_numOfElements % 16 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // create the return array (same length as the input)
    stack.PushValueAndClear();
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
        stack.TopValue(),
        cipherTextArray->m_numOfElements,
        g_CLR_RT_WellKnownTypes.m_UInt8));
    plainTextArray = stack.TopValue().DereferenceArray();

    // init mbedtls context
    mbedtls_aes_init(&decodeContext);

    if (mbedtls_aes_setkey_dec(&decodeContext, keyArray->GetFirstElement(), keyArray->m_numOfElements * 8) ==
        MBEDTLS_ERR_AES_INVALID_KEY_LENGTH)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (mbedtls_aes_crypt_ecb(
            &decodeContext,
            MBEDTLS_AES_DECRYPT,
            cipherTextArray->GetFirstElement(),
            plainTextArray->GetFirstElement()) != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_CLEANUP();

    // make sure nothing is left in memory
    mbedtls_aes_free(&decodeContext);

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_sec_cryptography_System_Security_Cryptography_Aes::EncryptAesCbc___SZARRAY_U1__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    mbedtls_aes_context encodeContext;

    CLR_RT_HeapBlock_Array *keyArray;
    CLR_RT_HeapBlock_Array *ivArray;
    CLR_RT_HeapBlock_Array *plainTextArray;
    CLR_RT_HeapBlock_Array *cipherTextArray;
    CLR_RT_HeapBlock *pThis;

    pThis = (CLR_RT_HeapBlock *)stack.This();
    FAULT_ON_NULL(pThis);

    // grab key and check for NULL
    keyArray = pThis[FIELD___key].DereferenceArray();
    if (keyArray == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // grab IV
    // (expecting this to be filled with the IV from managed code)
    ivArray = pThis[FIELD___key].DereferenceArray();

    // grab plain text and check for NULL
    plainTextArray = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL_ARG(plainTextArray);

    // data has to be multiple of 16
    if (plainTextArray->m_numOfElements % 16 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // create the return array (same length as the input)
    stack.PushValueAndClear();
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
        stack.TopValue(),
        plainTextArray->m_numOfElements,
        g_CLR_RT_WellKnownTypes.m_UInt8));
    cipherTextArray = stack.TopValue().DereferenceArray();

    // init mbedtls context
    mbedtls_aes_init(&encodeContext);

    if (mbedtls_aes_setkey_enc(&encodeContext, keyArray->GetFirstElement(), keyArray->m_numOfElements * 8) ==
        MBEDTLS_ERR_AES_INVALID_KEY_LENGTH)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (mbedtls_aes_crypt_cbc(
            &encodeContext,
            MBEDTLS_AES_ENCRYPT,
            plainTextArray->m_numOfElements,
            ivArray->GetFirstElement(),
            plainTextArray->GetFirstElement(),
            cipherTextArray->GetFirstElement()) != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_CLEANUP();

    // make sure nothing is left in memory
    mbedtls_aes_free(&encodeContext);

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_sec_cryptography_System_Security_Cryptography_Aes::DecryptAesCbc___SZARRAY_U1__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    mbedtls_aes_context decodeContext;

    CLR_RT_HeapBlock_Array *keyArray;
    CLR_RT_HeapBlock_Array *ivArray;
    CLR_RT_HeapBlock_Array *plainTextArray;
    CLR_RT_HeapBlock_Array *cipherTextArray;
    CLR_RT_HeapBlock *pThis;

    pThis = (CLR_RT_HeapBlock *)stack.This();
    FAULT_ON_NULL(pThis);

    // grab key and check for NULL
    keyArray = pThis[FIELD___key].DereferenceArray();
    if (keyArray == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // grab cipher text and check for NULL
    cipherTextArray = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL_ARG(cipherTextArray);

    // data has to be multiple of 16
    if (cipherTextArray->m_numOfElements % 16 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // grab IV
    // (expecting this to be filled with the IV from managed code)
    ivArray = pThis[FIELD___key].DereferenceArray();

    // create the return array (same length as the input)
    stack.PushValueAndClear();
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
        stack.TopValue(),
        cipherTextArray->m_numOfElements,
        g_CLR_RT_WellKnownTypes.m_UInt8));
    plainTextArray = stack.TopValue().DereferenceArray();

    // init mbedtls context
    mbedtls_aes_init(&decodeContext);

    if (mbedtls_aes_setkey_dec(&decodeContext, keyArray->GetFirstElement(), keyArray->m_numOfElements * 8) ==
        MBEDTLS_ERR_AES_INVALID_KEY_LENGTH)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (mbedtls_aes_crypt_cbc(
            &decodeContext,
            MBEDTLS_AES_DECRYPT,
            plainTextArray->m_numOfElements,
            ivArray->GetFirstElement(),
            plainTextArray->GetFirstElement(),
            cipherTextArray->GetFirstElement()) != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_CLEANUP();

    // make sure nothing is left in memory
    mbedtls_aes_free(&decodeContext);

    NANOCLR_CLEANUP_END();
}
