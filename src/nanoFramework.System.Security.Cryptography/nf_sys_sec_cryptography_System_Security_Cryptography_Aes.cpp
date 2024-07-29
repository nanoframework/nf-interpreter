//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_sec_cryptography.h"

HRESULT Library_nf_sys_sec_cryptography_System_Security_Cryptography_Aes::EncryptAesEcb___SZARRAY_U1__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(EncryptDecrypt(stack, MBEDTLS_CIPHER_AES_128_ECB, MBEDTLS_ENCRYPT));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_sec_cryptography_System_Security_Cryptography_Aes::DecryptAesEcb___SZARRAY_U1__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(EncryptDecrypt(stack, MBEDTLS_CIPHER_AES_128_ECB, MBEDTLS_DECRYPT));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_sec_cryptography_System_Security_Cryptography_Aes::EncryptAesCbc___SZARRAY_U1__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(EncryptDecrypt(stack, MBEDTLS_CIPHER_AES_128_CBC, MBEDTLS_ENCRYPT));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_sec_cryptography_System_Security_Cryptography_Aes::DecryptAesCbc___SZARRAY_U1__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(EncryptDecrypt(stack, MBEDTLS_CIPHER_AES_128_CBC, MBEDTLS_DECRYPT));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_sec_cryptography_System_Security_Cryptography_Aes::EncryptDecrypt(
    CLR_RT_StackFrame &stack,
    mbedtls_cipher_type_t cipher,
    mbedtls_operation_t operation)
{
    NANOCLR_HEADER();

    mbedtls_cipher_context_t ctx;
    mbedtls_cipher_info_t const *cipherInfo;
    size_t outputLength = 0;
    size_t ivLength = 0;
    uint16_t passCount = 0;
    uint8_t *ivCopy = NULL;
    uint8_t *workBuffer = NULL;

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
    ivArray = pThis[FIELD___iv].DereferenceArray();

    // need to check if IV is NULL (AES doesn't require an IV)
    if (ivArray != NULL)
    {
        // need a copy of the IV because it will be modified by mbedtls
        ivCopy = (uint8_t *)platform_malloc(ivArray->m_numOfElements);

        if (ivCopy == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        memcpy((void *)ivCopy, ivArray->GetFirstElement(), ivArray->m_numOfElements);

        ivLength = ivArray->m_numOfElements;
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
    // per MbedTLS requirements, the output buffer has to be at least the block size longer than the input buffer
    stack.PushValueAndClear();
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
        stack.TopValue(),
        plainTextArray->m_numOfElements,
        g_CLR_RT_WellKnownTypes.m_UInt8));

    // get a reference to the array to return
    cipherTextArray = stack.TopValue().DereferenceArray();

    // init mbedtls context
    mbedtls_cipher_init(&ctx);

    // set up the cipher
    cipherInfo = mbedtls_cipher_info_from_type(cipher);
    mbedtls_cipher_setup(&ctx, cipherInfo);

    // need a work buffer to hold the encrypted data
    workBuffer = (uint8_t *)platform_malloc(plainTextArray->m_numOfElements + cipherInfo->private_block_size);

    if (workBuffer == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // set the padding mode to none
    mbedtls_cipher_set_padding_mode(&ctx, MBEDTLS_PADDING_NONE);

    if (mbedtls_cipher_setkey(&ctx, keyArray->GetFirstElement(), keyArray->m_numOfElements * 8, operation) != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (cipher == MBEDTLS_CIPHER_AES_128_ECB && plainTextArray->m_numOfElements > 16)
    {
        // need to iterate through the plain text array, encrypting 16 bytes at a time
        while (passCount < plainTextArray->m_numOfElements / 16)
        {
            mbedtls_cipher_crypt(
                &ctx,
                ivCopy,
                ivLength,
                plainTextArray->GetElement(passCount * 16),
                16,
                &workBuffer[passCount * 16],
                &outputLength);

            passCount++;
        }
    }
    else
    {
        // encrypt the data
        mbedtls_cipher_crypt(
            &ctx,
            ivCopy,
            ivLength,
            plainTextArray->GetFirstElement(),
            plainTextArray->m_numOfElements,
            workBuffer,
            &outputLength);
    }

    // make sure nothing is left in memory
    mbedtls_cipher_free(&ctx);

    // copy the output buffer to the return array
    memcpy(cipherTextArray->GetFirstElement(), workBuffer, plainTextArray->m_numOfElements);

    NANOCLR_CLEANUP();

    if (ivCopy != NULL)
    {
        platform_free(ivCopy);
    }

    if (workBuffer != NULL)
    {
        platform_free(workBuffer);
    }

    NANOCLR_CLEANUP_END();
}
