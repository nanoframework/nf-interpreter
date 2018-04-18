//
// Copyright (c) 2018 The nanoFramework project contributors
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

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::GetWireless82011Configuration___STATIC__SystemNetNetworkInformationWireless80211Configuration__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    HAL_Configuration_Wireless80211 config; 
    CLR_RT_HeapBlock* pConfig = stack.Arg0().Dereference();  _ASSERTE(pConfig != NULL);
    
    CLR_UINT32 configurationIndex = pConfig[ FIELD___configurationIndex ].NumericByRefConst().u4;

    NANOCLR_CLEAR(config);
    
    // load wireless 802.11 configuration from the storage
    memcpy(&config, g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex], sizeof(HAL_Configuration_Wireless80211));

    pConfig[ FIELD___id ].SetInteger( (CLR_UINT32)config.Id);
    pConfig[ FIELD___authentication ].SetInteger( (CLR_UINT32)config.Authentication);
    pConfig[ FIELD___encryption ].SetInteger( (CLR_UINT32)config.Encryption);
    pConfig[ FIELD___radio ].SetInteger( (CLR_UINT32)config.Radio);

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( pConfig[ FIELD___password ], WIRELESS82011_CONFIG_MAX_PASSWORD_LEN, g_CLR_RT_WellKnownTypes.m_UInt8 ));   
    memcpy( pConfig[ FIELD___password ].DereferenceArray()->GetFirstElement(), config.Password, WIRELESS82011_CONFIG_MAX_PASSWORD_LEN );

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( pConfig[ FIELD___ssid ], WIRELESS82011_CONFIG_MAX_SSID_LEN, g_CLR_RT_WellKnownTypes.m_UInt8 ));   
    memcpy( pConfig[ FIELD___ssid ].DereferenceArray()->GetFirstElement(), config.Ssid, WIRELESS82011_CONFIG_MAX_SSID_LEN );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::UpdateConfiguration___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    HAL_Configuration_Wireless80211 config; 
    CLR_RT_HeapBlock* pConfig = stack.Arg0().Dereference();  _ASSERTE(pConfig != NULL);
    
    CLR_UINT32 configurationIndex = pConfig[ FIELD___configurationIndex ].NumericByRefConst().u4;
    CLR_RT_HeapBlock_Array* pPassword = pConfig[ FIELD___password ].DereferenceArray();
    CLR_RT_HeapBlock_Array* pSsid = pConfig[ FIELD___ssid ].DereferenceArray();

    NANOCLR_CLEAR(config);

    config.Id = (CLR_UINT32)pConfig[ FIELD___id ].NumericByRef().u4;
    config.Authentication = (AuthenticationType)pConfig[ FIELD___authentication ].NumericByRef().u4;
    config.Encryption = (EncryptionType)pConfig[ FIELD___encryption ].NumericByRef().u4;
    config.Radio = (RadioType)pConfig[ FIELD___radio ].NumericByRef().u4;

    memcpy( &config.Password, pPassword->GetFirstElement(), WIRELESS82011_CONFIG_MAX_PASSWORD_LEN ); 
    memcpy( &config.Ssid, pSsid->GetFirstElement(), WIRELESS82011_CONFIG_MAX_SSID_LEN ); 

    NANOCLR_NOCLEANUP();
}
