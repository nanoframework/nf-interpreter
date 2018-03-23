//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"



HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::UpdateConfiguration___STATIC__VOID__SystemNetNetworkInformationWireless80211__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        UNSUPPORTED_TYPE param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UNSUPPORTED_TYPE( stack, 0, param0 ) );

        char param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_CHAR( stack, 1, param1 ) );

//        Wireless80211::UpdateConfiguration( param0, param1, hr );
        hr = 0;
       NANOCLR_CHECK_HRESULT( hr );
    }    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::SaveAllConfigurations___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
//        Wireless80211::SaveAllConfigurations( hr );
        hr = 0;
        NANOCLR_CHECK_HRESULT( hr );
    }
    NANOCLR_NOCLEANUP();
}
