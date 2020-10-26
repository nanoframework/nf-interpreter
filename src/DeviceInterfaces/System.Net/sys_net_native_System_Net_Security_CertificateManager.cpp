//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"

HRESULT Library_sys_net_native_System_Net_Security_CertificateManager::
    AddCaCertificateBundle___STATIC__BOOLEAN__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    uint32_t certificateSize;
    uint32_t allocationSize;
    HAL_Configuration_X509CaRootBundle *caBundle = NULL;

    CLR_RT_HeapBlock_Array *arrayCA = stack.Arg0().DereferenceArray();

    // check for empty array
    FAULT_ON_NULL(arrayCA);

    certificateSize = (int)arrayCA->m_numOfElements;

    // build a HAL_Configuration_X509CaRootBundle
    // because certificate length is variable need to compute required memory
    // header
    allocationSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
    // certificate
    allocationSize += certificateSize;

    // allocate memory
    caBundle = (HAL_Configuration_X509CaRootBundle *)platform_malloc(allocationSize);
    // sanity check
    FAULT_ON_NULL(caBundle);

    // fill in struct
    caBundle->CertificateSize = certificateSize;

    // copy from get a pointer to the the binary data for the certificate
    memcpy(caBundle->Certificate, arrayCA->GetFirstElement(), certificateSize);

    if (g_TargetConfiguration.CertificateStore->Count == 0)
    {
        // not found, add the certificate bundle
        // we only support one CA root bundle, so this is fixed to 0
        // block size doesn't matter
        // offset is 0 and done flag is true because this is being stored in a single chunk
        if (ConfigurationManager_StoreConfigurationBlock(
                caBundle,
                DeviceConfigurationOption_X509CaRootBundle,
                0,
                certificateSize,
                0,
                true) != TRUE)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }
    else
    {
        // update the configuration block
        // we only support one CA root bundle, so this is fixed to 0
        if (ConfigurationManager_UpdateConfigurationBlock(caBundle, DeviceConfigurationOption_X509CaRootBundle, 0) !=
            TRUE)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }

    // reach here, we should be OK
    stack.SetResult_Boolean(TRUE);

    NANOCLR_CLEANUP();

    if (caBundle != NULL)
    {
        platform_free(caBundle);
    }

    NANOCLR_CLEANUP_END();
}
