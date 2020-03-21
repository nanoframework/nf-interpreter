//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"

HRESULT Library_sys_net_native_System_Net_Security_CertificateManager::AddCaCertificateBundle___STATIC__BOOLEAN__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* arrayCA      = stack.Arg0().DereferenceArray(); 

    CLR_UINT8*  certificateBinary;
    uint32_t    certificateSize;
    
    // we only have one CA root bundle, so this is fixed to 0
    uint32_t configIndex = 0;

    // check for empty array
    FAULT_ON_NULL(arrayCA);

    certificateSize = (int)arrayCA->m_numOfElements;

    // get a pointer to the the binary data for the certificate
    certificateBinary = arrayCA->GetFirstElement();

    if(g_TargetConfiguration.CertificateStore->Count == 0)
    {
        // not found, add the certificate bundle
        if(ConfigurationManager_StoreConfigurationBlock(certificateBinary, DeviceConfigurationOption_X509CaRootBundle, configIndex, certificateSize, 0, false) != TRUE)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }
    else
    {
        // update the configuration block
        if(ConfigurationManager_UpdateConfigurationBlock(certificateBinary, DeviceConfigurationOption_X509CaRootBundle, configIndex) != TRUE)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }

    // reach here, we should be OK
    stack.SetResult_Boolean(TRUE);

    NANOCLR_NOCLEANUP();
}
