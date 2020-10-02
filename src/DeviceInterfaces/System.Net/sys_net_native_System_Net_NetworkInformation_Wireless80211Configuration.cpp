//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"

HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::GetWireless82011ConfigurationCount___STATIC__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    // grab the count right from the structure
    stack.SetResult_I4(g_TargetConfiguration.Wireless80211Configs->Count);    

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::GetWireless82011Configuration___STATIC__SystemNetNetworkInformationWireless80211Configuration__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    HAL_Configuration_Wireless80211 config; 
    CLR_RT_HeapBlock*         pConfig;
    CLR_UINT32                configurationIndex = stack.Arg0().NumericByRef().u4;
    CLR_RT_HeapBlock&         top            = stack.PushValueAndClear();

    NANOCLR_CLEAR(config);
    
    // load wireless 802.11 configuration from the storage
    if(!ConfigurationManager_GetConfigurationBlock((void*)&config, DeviceConfigurationOption_Wireless80211Network, configurationIndex))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // create new object for configuration
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_Wireless80211Configuration));
    
    // load from stack
    pConfig = top.Dereference(); FAULT_ON_NULL(pConfig);

    // fill in fields from config struct
    pConfig[FIELD___id].SetInteger((CLR_UINT32)config.Id);
    pConfig[FIELD___authentication].SetInteger((CLR_UINT32)config.Authentication);
    pConfig[FIELD___encryption].SetInteger((CLR_UINT32)config.Encryption);
    pConfig[FIELD___radio].SetInteger((CLR_UINT32)config.Radio);
    pConfig[FIELD___options].SetInteger((CLR_UINT8)config.Options);

    // the following ones are strings so a simple assignment isn't enough, need to create a managed string instance and copy over 
    // make sure the terminators are there
    config.Password[WIRELESS82011_CONFIG_MAX_PASSWORD_LEN - 1] = 0;
    config.Ssid[WIRELESS82011_CONFIG_MAX_SSID_LEN - 1] = 0;
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(pConfig[FIELD___password], (const char*)config.Password));
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(pConfig[FIELD___ssid], (const char*)config.Ssid));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::UpdateConfiguration___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    HAL_Configuration_Wireless80211 config; 
    CLR_RT_HeapBlock* pConfig = stack.Arg0().Dereference();  _ASSERTE(pConfig != NULL);
    
    CLR_UINT32 configurationIndex = pConfig[FIELD___configurationIndex].NumericByRefConst().u4;

    CLR_RT_HeapBlock_String* hbPassword = NULL;
    CLR_RT_HeapBlock_String* hbSsid = NULL;
    CLR_UINT32 ssidLength;
    CLR_UINT32 passwordLength;

    NANOCLR_CLEAR(config);

    config.Id = (CLR_UINT32)pConfig[FIELD___id].NumericByRef().u4;
    config.Authentication = (AuthenticationType)pConfig[FIELD___authentication].NumericByRef().u4;
    config.Encryption = (EncryptionType)pConfig[FIELD___encryption].NumericByRef().u4;
    config.Radio = (RadioType)pConfig[FIELD___radio].NumericByRef().u4;
    config.Options = (Wireless80211Configuration_ConfigurationOptions)pConfig[FIELD___options].NumericByRef().u1;
 
    // the following ones are strings
    // make sure the terminators are there
    hbPassword = pConfig[FIELD___password].DereferenceString(); FAULT_ON_NULL(hbPassword);     
    passwordLength = hal_strlen_s(hbPassword->StringText());
    if (passwordLength >= sizeof(config.Password)) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);        
    hal_strncpy_s((char*)config.Password, WIRELESS82011_CONFIG_MAX_PASSWORD_LEN, hbPassword->StringText(), passwordLength);    

    hbSsid = pConfig[FIELD___ssid].DereferenceString(); FAULT_ON_NULL(hbSsid);     
    ssidLength = hal_strlen_s(hbSsid->StringText());
    if (ssidLength >= sizeof(config.Ssid)) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);        
    hal_strncpy_s((char*)config.Ssid, WIRELESS82011_CONFIG_MAX_SSID_LEN, hbSsid->StringText(), ssidLength);    

    // store configuration
    if(ConfigurationManager_UpdateConfigurationBlock(&config, DeviceConfigurationOption_Wireless80211Network, configurationIndex) != TRUE)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_NOCLEANUP();
}
