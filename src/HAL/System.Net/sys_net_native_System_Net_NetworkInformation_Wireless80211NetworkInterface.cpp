//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"

HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211NetworkInterface::UpdateConfiguration___STATIC__VOID__SystemNetNetworkInformationWireless80211NetworkInterface__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    HAL_Configuration_Wireless80211NetworkInterface config; 
    CLR_RT_HeapBlock*          pConfig         = stack.Arg0().Dereference();  _ASSERTE(pConfig != NULL);
    bool                       useEncryption   = stack.Arg1().NumericByRef().s1 != 0;
    CLR_UINT32                 interfaceIndex  = pConfig[ Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::FIELD___interfaceIndex ].NumericByRefConst().u4;
    AuthenticationType         authentication;
    EncryptionType             encryption;
    RadioType                  radio;
    CLR_RT_HeapBlock_String*   hbPassword      = NULL;
    CLR_RT_HeapBlock_String*   hbSsId          = NULL;
    CLR_UINT32                 ssidLength;
    CLR_UINT32                 passwordLength;

    NANOCLR_CLEAR(config);

    config.Authentication = (AuthenticationType)pConfig[ FIELD__Authentication ].NumericByRef().u4;
    config.Encryption = (EncryptionType)pConfig[ FIELD__Encryption ].NumericByRef().u4;
    config.Radio =  (RadioType)pConfig[ FIELD__Radio ].NumericByRef().u4;

    hbPassword = pConfig[ FIELD__Password ].DereferenceString(); FAULT_ON_NULL(hbPassword);     
    passwordLength = hal_strlen_s(hbPassword->StringText());
    if (passwordLength >= sizeof(config.Password)) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);        
    hal_strncpy_s( (char*)config.Password, sizeof(config.Password), hbPassword->StringText(), passwordLength );    

    hbSsId = pConfig[ FIELD__Ssid ].DereferenceString(); FAULT_ON_NULL(hbSsId);     
    ssidLength = hal_strlen_s(hbSsId->StringText());
    if (ssidLength >= sizeof(config.Ssid)) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);        
    hal_strncpy_s( (char*)config.Ssid, sizeof(config.Ssid), hbSsId->StringText(), ssidLength );    

    NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_UpdateWirelessConfiguration(&config, interfaceIndex));
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211NetworkInterface::SaveAllConfigurations___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_SaveAllWirelessConfigurations());

    NANOCLR_NOCLEANUP();
}
