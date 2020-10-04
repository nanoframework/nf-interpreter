//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"


HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::GetWirelessAPConfigurationCount___STATIC__I4( CLR_RT_StackFrame& stack )
{
#ifdef PLATFORM_ESP32
	NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    // grab the count right from the structure
    stack.SetResult_I4(g_TargetConfiguration.WirelessAPConfigs->Count);    

    NANOCLR_NOCLEANUP_NOLABEL();
#else
	NANOCLR_HEADER();

	NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

	NANOCLR_NOCLEANUP();
#endif
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::GetWirelessAPConfiguration___STATIC__SystemNetNetworkInformationWirelessAPConfiguration__I4( CLR_RT_StackFrame& stack )
{
#ifdef PLATFORM_ESP32
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
    pConfig[FIELD___options].SetInteger((CLR_UINT8)config.Options);
    pConfig[FIELD___apChannel].SetInteger((CLR_UINT8)config.Channel);
    pConfig[FIELD___apMaxConnections].SetInteger((CLR_UINT8)config.MaxConnections);
 
    // the following ones are strings so a simple assignment isn't enough, need to create a managed string instance and copy over 
    // make sure the terminators are there
    config.Password[WIRELESS82011_CONFIG_MAX_PASSWORD_LEN - 1] = 0;
    config.Ssid[WIRELESS82011_CONFIG_MAX_SSID_LEN - 1] = 0;
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(pConfig[FIELD___apPassword], (const char*)config.Password));
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(pConfig[FIELD___apSsid], (const char*)config.Ssid));

    NANOCLR_NOCLEANUP();
#else
	NANOCLR_HEADER();

	NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

	NANOCLR_NOCLEANUP();
#endif
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::UpdateConfiguration___STATIC__VOID( CLR_RT_StackFrame& stack )
{
#ifdef PLATFORM_ESP32
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
    config.Options = (WirelessAPConfiguration_ConfigurationOptions)pConfig[FIELD___options].NumericByRef().u1;
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
#else
	NANOCLR_HEADER();

	NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

	NANOCLR_NOCLEANUP();
#endif
}


HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::NativeGetConnectedClients___STATIC__SZARRAY_SystemNetNetworkInformationWirelessAPStation__I4(CLR_RT_StackFrame& stack)
{
#ifdef PLATFORM_ESP32
	NANOCLR_HEADER();

	CLR_RT_TypeDef_Index    apStationTypeDef;
	CLR_RT_HeapBlock*       apStation;
	CLR_RT_HeapBlock*       hbObj;

	uint16_t                stationCount = 0;

	uint8_t  mac[6];
	uint8_t  rssi;
	uint32_t phyModes = 0;

	CLR_RT_HeapBlock& top = stack.PushValue();

	// Get index of station info required or if index == 0 then return all connected stations 
	uint16_t index = (uint16_t)stack.Arg0().NumericByRef().u4;


	// find <WirelessAPStation> type, don't bother checking the result as it exists for sure
	g_CLR_RT_TypeSystem.FindTypeDef("WirelessAPStation", "System.Net.NetworkInformation", apStationTypeDef);

	// Count stations connected
	for (int x = 0; x < Network_Interface_Max_Stations(); x++)
	{
		if (index != 0 && x != index) continue;

		if (Network_Interface_Get_Station(x, mac, &rssi, &phyModes))
		{
			stationCount++;
		}
	}

	// Create an array of <WirelessAPStation>
	NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, stationCount, apStationTypeDef));

	// get a pointer to the first object in the array (which is of type <WirelessAPStation>)
	apStation = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

	// Create Array
	if (stationCount > 0)
	{
		for (int x = 0; x < Network_Interface_Max_Stations(); x++)
		{
			if (index != 0 && x != index) continue;

			if (Network_Interface_Get_Station(x, mac, &rssi, &phyModes))
			{
				// create an instance of <WirelessAPStation>
				NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*apStation, apStationTypeDef));

				// dereference the object in order to reach its fields
				hbObj = apStation->Dereference();

				NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(hbObj[Library_sys_net_native_System_Net_NetworkInformation_WirelessAPStation::FIELD___macAddress], 6, g_CLR_RT_WellKnownTypes.m_UInt8));
				memcpy(hbObj[Library_sys_net_native_System_Net_NetworkInformation_WirelessAPStation::FIELD___macAddress].DereferenceArray()->GetFirstElement(), mac, 6);


				CLR_RT_HeapBlock& rssiFieldRef = hbObj[Library_sys_net_native_System_Net_NetworkInformation_WirelessAPStation::FIELD___rssi];
				CLR_INT8* pRes2 = (CLR_INT8*)&rssiFieldRef.NumericByRef().s1;
				*pRes2 = rssi;

				CLR_RT_HeapBlock& phyModesFieldRef = hbObj[Library_sys_net_native_System_Net_NetworkInformation_WirelessAPStation::FIELD___phyModes];
				CLR_INT32* pRes3 = (CLR_INT32*)&phyModesFieldRef.NumericByRef().s4;
				*pRes3 = phyModes;

				apStation++;
			}
		}
	}

	NANOCLR_NOCLEANUP();
#else
	NANOCLR_HEADER();

	NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

	NANOCLR_NOCLEANUP();
#endif
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::NativeDeauthStation___STATIC__STRING__I4(CLR_RT_StackFrame& stack)
{
#ifdef PLATFORM_ESP32
	NANOCLR_HEADER();

	uint16_t index = (uint16_t)stack.Arg0().NumericByRef().u4;
	
	Network_Interface_Deauth_Station(index);

	NANOCLR_NOCLEANUP_NOLABEL();
#else
	NANOCLR_HEADER();

	NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

	NANOCLR_NOCLEANUP();
#endif
}
