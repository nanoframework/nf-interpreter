//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"

void Network_PostEvent( unsigned int eventType, unsigned int flags )
{
    PostManagedEvent( EVENT_NETWORK, eventType, flags, 0 );
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::InitializeNetworkInterfaceSettings___VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    SOCK_NetworkConfiguration config; 
    CLR_RT_HeapBlock* pConfig           = stack.Arg0().Dereference();  _ASSERTE(pConfig != NULL);
    CLR_UINT32 interfaceIndex           = pConfig[ FIELD___interfaceIndex ].NumericByRefConst().u4;

    NANOCLR_CLEAR(config);
    NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_LoadAdapterConfiguration( interfaceIndex, &config ));

    pConfig[ FIELD___flags                ].SetInteger( config.flags      );
    pConfig[ FIELD___ipAddress            ].SetInteger( config.ipaddr     );
    pConfig[ FIELD___gatewayAddress       ].SetInteger( config.gateway    );
    pConfig[ FIELD___subnetMask           ].SetInteger( config.subnetmask );
    pConfig[ FIELD___dnsAddress1          ].SetInteger( config.dnsServer1 );
    pConfig[ FIELD___dnsAddress2          ].SetInteger( config.dnsServer2 );    
    pConfig[ FIELD___networkInterfaceType ].SetInteger( config.networkInterfaceType );

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( pConfig[ FIELD___macAddress ], config.macAddressLen, g_CLR_RT_WellKnownTypes.m_UInt8 ));   

    memcpy( pConfig[ FIELD___macAddress ].DereferenceArray()->GetFirstElement(), config.macAddressBuffer, config.macAddressLen );
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::UpdateConfiguration___VOID__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    SOCK_NetworkConfiguration config; 
    CLR_RT_HeapBlock* pConfig           = stack.Arg0().Dereference();  _ASSERTE(pConfig != NULL);
    CLR_UINT32 interfaceIndex           = pConfig[ FIELD___interfaceIndex ].NumericByRefConst().u4;
    CLR_UINT32 updateFlags              = stack.Arg1().NumericByRef().u4;
    CLR_RT_HeapBlock_Array* pMACAddress = pConfig[ FIELD___macAddress ].DereferenceArray();

    NANOCLR_CLEAR(config);

    config.flags                = pConfig[ FIELD___flags                ].NumericByRef().u4;
    config.ipaddr               = pConfig[ FIELD___ipAddress            ].NumericByRef().u4;
    config.gateway              = pConfig[ FIELD___gatewayAddress       ].NumericByRef().u4;
    config.subnetmask           = pConfig[ FIELD___subnetMask           ].NumericByRef().u4;
    config.dnsServer1           = pConfig[ FIELD___dnsAddress1          ].NumericByRef().u4;
    config.dnsServer2           = pConfig[ FIELD___dnsAddress2          ].NumericByRef().u4;
    config.networkInterfaceType = pConfig[ FIELD___networkInterfaceType ].NumericByRef().u4;
    
    if(pMACAddress != NULL)
    {
        config.macAddressLen = pMACAddress->m_numOfElements;
        
        if(config.macAddressLen > sizeof(config.macAddressBuffer)) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        
        memcpy( &config.macAddressBuffer, pMACAddress->GetFirstElement(), config.macAddressLen ); 
    }

    NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_UpdateAdapterConfiguration( interfaceIndex, updateFlags, &config ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::GetNetworkInterfaceCount___STATIC__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    stack.SetResult_I4( SOCK_CONFIGURATION_GetAdapterCount() );    

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::GetNetworkInterface___STATIC__SystemNetNetworkInformationNetworkInterface__U4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    SOCK_NetworkConfiguration config; 
    CLR_UINT32                type = 0;
    CLR_RT_HeapBlock*         pConfig;
    CLR_UINT32                interfaceIndex = stack.Arg0().NumericByRef().u4;
    CLR_RT_HeapBlock&         top            = stack.PushValueAndClear();
    //RSAKey*                   key            = NULL;

    NANOCLR_CLEAR(config);

    NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_LoadConfiguration( interfaceIndex, &config ));    
        
    type = SOCK_NETWORKCONFIGURATION_FLAGS_TYPE__value(config.flags);
    
    switch(type)
    {
    case SOCK_NETWORKCONFIGURATION_FLAGS_NETWORK_INTERFACE:
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex( top, g_CLR_RT_WellKnownTypes.m_NetworkInterface ));
            break;
        }
    case SOCK_NETWORKCONFIGURATION_FLAGS_WIRELESS:
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex( top, g_CLR_RT_WellKnownTypes.m_Wireless80211 ));
            break;
        }
    default:
        {
            /// Unknown type, not supported.
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            break;
        }
    }
    
    pConfig = top.Dereference(); FAULT_ON_NULL(pConfig);
    
    pConfig[ FIELD___flags                ].SetInteger( config.flags      );
    pConfig[ FIELD___ipAddress            ].SetInteger( config.ipaddr     );
    pConfig[ FIELD___gatewayAddress       ].SetInteger( config.gateway    );
    pConfig[ FIELD___subnetMask           ].SetInteger( config.subnetmask );
    pConfig[ FIELD___dnsAddress1          ].SetInteger( config.dnsServer1 );
    pConfig[ FIELD___dnsAddress2          ].SetInteger( config.dnsServer2 );    
    pConfig[ FIELD___networkInterfaceType ].SetInteger( config.networkInterfaceType );
    pConfig[ FIELD___interfaceIndex       ].SetInteger( interfaceIndex    );

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( pConfig[ FIELD___macAddress ], config.macAddressLen, g_CLR_RT_WellKnownTypes.m_UInt8 ));   

    memcpy( pConfig[ FIELD___macAddress ].DereferenceArray()->GetFirstElement(), config.macAddressBuffer, config.macAddressLen );

    /// Type specific load.
    switch(type)
    {
    case SOCK_NETWORKCONFIGURATION_FLAGS_WIRELESS:
        {
            CLR_UINT32 wirelessIndex = SOCK_NETWORKCONFIGURATION_FLAGS_SUBINDEX__value(config.flags);
            SOCK_WirelessConfiguration wirelessConfig;
            
            NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_LoadWirelessConfiguration( wirelessIndex, &wirelessConfig ));

            pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__Authentication ].SetInteger( WIRELESS_FLAG_AUTHENTICATION__value(wirelessConfig.wirelessFlags) );
            pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__Encryption     ].SetInteger( WIRELESS_FLAG_ENCRYPTION__value    (wirelessConfig.wirelessFlags) );
            pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__Radio          ].SetInteger( WIRELESS_FLAG_RADIO__value         (wirelessConfig.wirelessFlags) );

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__NetworkKey ], wirelessConfig.networkKeyLength, g_CLR_RT_WellKnownTypes.m_UInt8 )); 
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__ReKeyInternal ], wirelessConfig.reKeyLength, g_CLR_RT_WellKnownTypes.m_UInt8 )); 

            uint32_t dataFlag = WIRELESS_FLAG_DATA__value(wirelessConfig.wirelessFlags);
            if (dataFlag & WIRELESS_FLAG_DATA_ENCRYPTED)
            {
                ASSERT(FALSE);  // TODO: ADD SUPPORT FOR ENCRYPTION
                //key = RetrieveWirelessEncryptionKey();
            }

            //if (key == NULL)
            {   
                wirelessConfig.passPhrase[ WIRELESS_PASSPHRASE_LENGTH - 1 ] = 0;
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__PassPhrase ], wirelessConfig.passPhrase ));                
                if(wirelessConfig.networkKeyLength > 0)
                {
                    memcpy( pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__NetworkKey ].DereferenceArray()->GetFirstElement(), wirelessConfig.networkKey, wirelessConfig.networkKeyLength );                
                }
                if(wirelessConfig.reKeyLength > 0)
                {
                    memcpy( pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__ReKeyInternal ].DereferenceArray()->GetFirstElement(), wirelessConfig.reKeyInternal, wirelessConfig.reKeyLength );
                }
            }
            /*
                    else
                    {   
                        char passPhrase[ WIRELESS_PASSPHRASE_LENGTH ];
                        
                        Decrypt( (BYTE *)key, (BYTE *)wirelessConfig.passPhrase, WIRELESS_PASSPHRASE_LENGTH - 1, (BYTE *)passPhrase, WIRELESS_PASSPHRASE_LENGTH - 1 );
                        passPhrase[ WIRELESS_PASSPHRASE_LENGTH - 1 ] = 0;
                        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__PassPhrase ], passPhrase ));

                        if(wirelessConfig.networkKeyLength > 0)
                        {
                            Decrypt( (BYTE *)key, (BYTE *)wirelessConfig.networkKey, wirelessConfig.networkKeyLength, pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__NetworkKey ].DereferenceArray()->GetFirstElement(), wirelessConfig.networkKeyLength );
                        }

                        if(wirelessConfig.reKeyLength > 0)
                        {
                            Decrypt( (BYTE *)key, (BYTE *)wirelessConfig.reKeyInternal, wirelessConfig.reKeyLength, pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__ReKeyInternal ].DereferenceArray()->GetFirstElement(), wirelessConfig.reKeyLength );                
                        }
                    }
                    */

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__Ssid ], wirelessConfig.ssid ));                

            pConfig[Library_sys_net_native_System_Net_NetworkInformation_Wireless80211::FIELD__Id ].SetInteger( wirelessIndex );

            break;
        }
    default:
        {
            break;
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::IPAddressFromString___STATIC__U4__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    LPCSTR szName = stack.Arg0().RecoverString();
    struct SOCK_addrinfo hints;
    struct SOCK_addrinfo* addr = NULL;     
    struct SOCK_sockaddr_in* addr_in;
    int ret;
    
    memset( &hints, 0, sizeof(hints) );

    ret = SOCK_getaddrinfo( szName, NULL, &hints, &addr );

    // getaddrinfo returns a winsock error code rather than SOCK_SOCKET_ERROR, so pass this on to the exception handling
    if(ret != 0 || addr == NULL || addr->ai_family != SOCK_AF_INET)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    _ASSERTE(addr->ai_addr != NULL);
    _ASSERTE(addr->ai_addrlen >= sizeof(SOCK_sockaddr_in));

    addr_in = (struct SOCK_sockaddr_in*)addr->ai_addr;

    stack.PushValue().SetInteger( (CLR_UINT32)addr_in->sin_addr.S_un.S_addr );

    NANOCLR_CLEANUP();

    if( addr ) SOCK_freeaddrinfo( addr );

    NANOCLR_CLEANUP_END();
}
