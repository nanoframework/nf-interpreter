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

    SOCK_WirelessConfiguration config; 
    CLR_RT_HeapBlock*          pConfig         = stack.Arg0().Dereference();  _ASSERTE(pConfig != NULL);
    bool                       useEncryption   = stack.Arg1().NumericByRef().s1 != 0;
    CLR_UINT32                 interfaceIndex  = 0;//pConfig[ FIELD__Id ].NumericByRefConst().u4;
    CLR_UINT32                 authentication;
    CLR_UINT32                 encryption;
    CLR_UINT32                 radio;
    CLR_RT_HeapBlock_String*   hbPassword      = NULL;
    CLR_RT_HeapBlock_String*   hbSsId          = NULL;
    CLR_UINT32                 ssidLength;
    CLR_UINT32                 passPhraseLength;

    NANOCLR_CLEAR(config);

    authentication                = pConfig[ FIELD__Authentication ].NumericByRef().u4;
    encryption                    = pConfig[ FIELD__Encryption     ].NumericByRef().u4;
    radio                         = pConfig[ FIELD__Radio          ].NumericByRef().u4;

    config.wirelessFlags = WIRELESS_FLAG_AUTHENTICATION__set(authentication) | WIRELESS_FLAG_ENCRYPTION__set(encryption) | WIRELESS_FLAG_RADIO__set(radio);

    hbPassword = pConfig[ FIELD__Password ].DereferenceString(); FAULT_ON_NULL(hbPassword);     
    passPhraseLength = hal_strlen_s(hbPassword->StringText());
    if (passPhraseLength >= sizeof(config.passPhrase)) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);        

    hbSsId = pConfig[ FIELD__Ssid ].DereferenceString(); FAULT_ON_NULL(hbSsId);     
    ssidLength = hal_strlen_s(hbSsId->StringText());
    if (ssidLength >= sizeof(config.ssid)) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);        

    if (useEncryption)
    {
        ASSERT(FALSE); // TODO: IMPLEMENT ENCRYPTION
    }

    // Data is saved as clear text.
    config.wirelessFlags &= ~(WIRELESS_FLAG_DATA__set(WIRELESS_FLAG_DATA_ENCRYPTED));

    hal_strncpy_s( config.passPhrase, WIRELESS_PASSPHRASE_LENGTH, hbPassword->StringText(), passPhraseLength );    

    hal_strncpy_s( config.ssid, WIRELESS_SSID_LENGTH, hbSsId->StringText(), ssidLength );    

    NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_UpdateWirelessConfiguration( interfaceIndex, &config ));
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211NetworkInterface::SaveAllConfigurations___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_SaveAllWirelessConfigurations());

    NANOCLR_NOCLEANUP();
}
