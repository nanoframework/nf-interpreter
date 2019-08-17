//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"


HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::GetWirelessAPConfigurationCount___STATIC__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    // grab the count right from the structure
    stack.SetResult_I4(g_TargetConfiguration.WirelessAPConfigs->Count);    

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::GetWirelessAPConfiguration___STATIC__SystemNetNetworkInformationWirelessAPConfiguration__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    HAL_Configuration_WirelessAP config; 
    CLR_RT_HeapBlock*         pConfig;
    CLR_UINT32                configurationIndex = stack.Arg0().NumericByRef().u4;
    CLR_RT_HeapBlock&         top            = stack.PushValueAndClear();

    NANOCLR_CLEAR(config);
    
    // load wireless AP configuration from the storage
    if(!ConfigurationManager_GetConfigurationBlock((void*)&config, DeviceConfigurationOption_WirelessNetworkAP, configurationIndex))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // create new object for configuration
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_WirelessAPConfiguration));
    
    // load from stack
    pConfig = top.Dereference(); FAULT_ON_NULL(pConfig);

    // fill in fields from config struct
    pConfig[FIELD___apId].SetInteger((CLR_UINT32)config.Id);
    pConfig[FIELD___apAuthentication].SetInteger((CLR_UINT32)config.Authentication);
    pConfig[FIELD___apEncryption].SetInteger((CLR_UINT32)config.Encryption);
    pConfig[FIELD___apRadio].SetInteger((CLR_UINT32)config.Radio);
    pConfig[FIELD___apFlags].SetInteger((CLR_UINT8)config.Flags);
    pConfig[FIELD___apChannel].SetInteger((CLR_UINT8)config.Channel);
    pConfig[FIELD___apMaxConnections].SetInteger((CLR_UINT8)config.MaxConnections);
 
    // the following ones are strings so a simple assignment isn't enough, need to create a managed string instance and copy over 
    // make sure the terminators are there
    config.Password[WIRELESS82011_CONFIG_MAX_PASSWORD_LEN - 1] = 0;
    config.Ssid[WIRELESS82011_CONFIG_MAX_SSID_LEN - 1] = 0;
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(pConfig[FIELD___apPassword], (const char*)config.Password));
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(pConfig[FIELD___apSsid], (const char*)config.Ssid));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::UpdateConfiguration___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    HAL_Configuration_WirelessAP config; 
    CLR_RT_HeapBlock* pConfig = stack.Arg0().Dereference();  _ASSERTE(pConfig != NULL);
    
    CLR_UINT32 configurationIndex = pConfig[FIELD___apConfigurationIndex].NumericByRefConst().u4;

    CLR_RT_HeapBlock_String* hbPassword = NULL;
    CLR_RT_HeapBlock_String* hbSsid = NULL;
    CLR_UINT32 ssidLength;
    CLR_UINT32 passwordLength;

    NANOCLR_CLEAR(config);

    config.Id = (CLR_UINT32)pConfig[FIELD___apId].NumericByRef().u4;
    config.Authentication = (AuthenticationType)pConfig[FIELD___apAuthentication].NumericByRef().u4;
    config.Encryption = (EncryptionType)pConfig[FIELD___apEncryption].NumericByRef().u4;
    config.Radio = (RadioType)pConfig[FIELD___apRadio].NumericByRef().u4;
    config.Flags = (uint8_t)pConfig[FIELD___apFlags].NumericByRef().u1;
    config.Channel = (uint8_t)pConfig[FIELD___apChannel].NumericByRef().u1;
    config.MaxConnections = (uint8_t)pConfig[FIELD___apMaxConnections].NumericByRef().u1;

    // the following ones are strings
    // make sure the terminators are there
    hbPassword = pConfig[FIELD___apPassword].DereferenceString(); FAULT_ON_NULL(hbPassword);     
    passwordLength = hal_strlen_s(hbPassword->StringText());
    if (passwordLength >= sizeof(config.Password)) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);        
    hal_strncpy_s((char*)config.Password, WIRELESS82011_CONFIG_MAX_PASSWORD_LEN, hbPassword->StringText(), passwordLength);    

    hbSsid = pConfig[FIELD___apSsid].DereferenceString(); FAULT_ON_NULL(hbSsid);     
    ssidLength = hal_strlen_s(hbSsid->StringText());
    if (ssidLength >= sizeof(config.Ssid)) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);        
    hal_strncpy_s((char*)config.Ssid, WIRELESS82011_CONFIG_MAX_SSID_LEN, hbSsid->StringText(), ssidLength);    

    // store configuration
    if(ConfigurationManager_UpdateConfigurationBlock(&config, DeviceConfigurationOption_WirelessNetworkAP, configurationIndex) != TRUE)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_NOCLEANUP();

}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::NativeGetConnectedClients___STATIC__STRING(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

	NANOCLR_NOCLEANUP();
}