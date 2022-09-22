//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"

HRESULT Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2::
    DecodePrivateKeyNative___STATIC__VOID__SZARRAY_U1__STRING(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *keyData = stack.Arg0().DereferenceArray();
    CLR_UINT8 *keyBuffer;
    CLR_RT_HeapBlock *passwordHb = stack.Arg1().DereferenceString();
    const char *password = NULL;
    CLR_UINT32 passwordLength = 0;

    // get key buffer
    keyBuffer = keyData->GetFirstElement();

    // get password length, if there is a password
    if (passwordHb)
    {
        password = passwordHb->StringText();
        passwordLength = hal_strlen_s(password);
    }

    if (SSL_DecodePrivateKey(
            (const unsigned char *)keyBuffer,
            keyData->m_numOfElements,
            (const unsigned char *)password,
            passwordLength) < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}
