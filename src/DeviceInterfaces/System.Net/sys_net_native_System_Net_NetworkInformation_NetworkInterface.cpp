//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"

void Network_PostEvent( unsigned int eventType, unsigned int flags, unsigned int index)
{
    PostManagedEvent( EVENT_NETWORK, eventType, flags, index );
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::InitializeNetworkInterfaceSettings___VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    HAL_Configuration_NetworkInterface config; 
    CLR_RT_HeapBlock* pConfig           = stack.Arg0().Dereference();  _ASSERTE(pConfig != NULL);
    
    CLR_UINT32 interfaceIndex           = pConfig[ FIELD___interfaceIndex ].NumericByRefConst().u4;

    NANOCLR_CLEAR(config);
    
    // load network interface configuration from storage
    if(!ConfigurationManager_GetConfigurationBlock((void*)&config, DeviceConfigurationOption_Network, interfaceIndex))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }
    _ASSERTE(config.StartupAddressMode > 0);

    // now load adapter configuration on top of the stored config 
    NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_LoadAdapterConfiguration(&config, interfaceIndex));

    pConfig[ FIELD___ipv4Address            ].SetInteger( (CLR_UINT32)config.IPv4Address);
    pConfig[ FIELD___ipv4GatewayAddress     ].SetInteger( (CLR_UINT32)config.IPv4GatewayAddress);
    pConfig[ FIELD___ipv4NetMask            ].SetInteger( (CLR_UINT32)config.IPv4NetMask);
    pConfig[ FIELD___ipv4dnsAddress1        ].SetInteger( (CLR_UINT32)config.IPv4DNSAddress1);
    pConfig[ FIELD___ipv4dnsAddress2        ].SetInteger( (CLR_UINT32)config.IPv4DNSAddress2);

    // FIXME IPV6
    // pConfig[ FIELD___ipv6Address            ].SetInteger( (CLR_UINT32)config.IPv6Address);
    // pConfig[ FIELD___ipv6GatewayAddress     ].SetInteger( (CLR_UINT32)config.IPv6GatewayAddress);
    // pConfig[ FIELD___ipv6NetMask            ].SetInteger( (CLR_UINT32)config.IPv6NetMask);
    // pConfig[ FIELD___ipv6dnsAddress1        ].SetInteger( (CLR_UINT32)config.IPv6DNSAddress1);
    // pConfig[ FIELD___ipv6dnsAddress2        ].SetInteger( (CLR_UINT32)config.IPv6DNSAddress2);

    pConfig[ FIELD___automaticDns           ].SetInteger( (CLR_UINT32)config.AutomaticDNS);
    pConfig[ FIELD___networkInterfaceType   ].SetInteger( (CLR_UINT32)config.InterfaceType);
    pConfig[ FIELD___specificConfigId       ].SetInteger( (CLR_UINT32)config.SpecificConfigId);
    pConfig[ FIELD___startupAddressMode     ].SetInteger( (CLR_UINT32)config.StartupAddressMode);

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( pConfig[ FIELD___macAddress ], NETIF_MAX_HWADDR_LEN, g_CLR_RT_WellKnownTypes.m_UInt8));
    memcpy(pConfig[ FIELD___macAddress ].DereferenceArray()->GetFirstElement(), config.MacAddress, NETIF_MAX_HWADDR_LEN);
    
    NANOCLR_NOCLEANUP();
}

inline bool operator!=(const HAL_Configuration_NetworkInterface &lhs, const HAL_Configuration_NetworkInterface &rhs) {
  if (lhs.IPv4Address != rhs.IPv4Address) {
    return true;
  }

  if (lhs.IPv4GatewayAddress != rhs.IPv4GatewayAddress) {
    return true;
  }

  if (lhs.IPv4NetMask != rhs.IPv4NetMask) {
    return true;
  }

  if (lhs.IPv4DNSAddress1 != rhs.IPv4DNSAddress1) {
    return true;
  }

  if (lhs.IPv4DNSAddress2 != rhs.IPv4DNSAddress2) {
    return true;
  }

  if (lhs.AutomaticDNS != rhs.AutomaticDNS) {
    return true;
  }

  if (lhs.InterfaceType != rhs.InterfaceType) {
    return true;
  }

  if (lhs.SpecificConfigId != rhs.SpecificConfigId) {
    return true;
  }

  if (lhs.StartupAddressMode != rhs.StartupAddressMode) {
    return true;
  }

  if (memcmp(lhs.MacAddress, rhs.MacAddress, sizeof(rhs.MacAddress))) {
    return true;
  }

  if (memcmp(lhs.IPv6Address, rhs.IPv6Address, sizeof(rhs.IPv6Address))) {
    return true;
  }

  if (memcmp(lhs.IPv6NetMask, rhs.IPv6NetMask, sizeof(rhs.IPv6NetMask))) {
    return true;
  }

  if (memcmp(lhs.IPv6GatewayAddress, rhs.IPv6GatewayAddress, sizeof(rhs.IPv6GatewayAddress))) {
    return true;
  }

  if (memcmp(lhs.IPv6DNSAddress1, rhs.IPv6DNSAddress1, sizeof(rhs.IPv6DNSAddress1))) {
    return true;
  }

  if (memcmp(lhs.IPv6DNSAddress2, rhs.IPv6DNSAddress2, sizeof(rhs.IPv6DNSAddress2))) {
    return true;
  }

  return false;
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::UpdateConfiguration___VOID__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    HAL_Configuration_NetworkInterface config, storageConfig;
    bool configChanged;
    CLR_RT_HeapBlock* pConfig           = stack.Arg0().Dereference();  _ASSERTE(pConfig != NULL);
    CLR_UINT32 interfaceIndex           = pConfig[ FIELD___interfaceIndex ].NumericByRefConst().u4;
    CLR_UINT32 updateFlags              = stack.Arg1().NumericByRef().u4;
    CLR_RT_HeapBlock_Array* pMACAddress = pConfig[ FIELD___macAddress ].DereferenceArray();

    NANOCLR_CLEAR(config);
    NANOCLR_CLEAR(storageConfig);

    config.IPv4Address           = pConfig[ FIELD___ipv4Address        ].NumericByRef().u4;
    config.IPv4GatewayAddress    = pConfig[ FIELD___ipv4GatewayAddress ].NumericByRef().u4;
    config.IPv4NetMask           = pConfig[ FIELD___ipv4NetMask        ].NumericByRef().u4;
    config.IPv4DNSAddress1       = pConfig[ FIELD___ipv4dnsAddress1    ].NumericByRef().u4;
    config.IPv4DNSAddress2       = pConfig[ FIELD___ipv4dnsAddress2    ].NumericByRef().u4;

    // FIXME IPV6
    // config.IPv6Address           = pConfig[ FIELD___ipv6Address        ].NumericByRef().u4;
    // config.IPv6GatewayAddress    = pConfig[ FIELD___ipv6GatewayAddress ].NumericByRef().u4;
    // config.IPv6NetMask           = pConfig[ FIELD___ipv6NetMask        ].NumericByRef().u4;
    // config.IPv6DNSAddress1       = pConfig[ FIELD___ipv6dnsAddress1    ].NumericByRef().u4;
    // config.IPv6DNSAddress2       = pConfig[ FIELD___ipv6dnsAddress2    ].NumericByRef().u4;
    
    config.AutomaticDNS         =                           pConfig[ FIELD___automaticDns         ].NumericByRef().u4;
    config.InterfaceType        = (NetworkInterfaceType)    pConfig[ FIELD___networkInterfaceType ].NumericByRef().u4;
    config.SpecificConfigId     = (CLR_UINT32)              pConfig[ FIELD___specificConfigId     ].NumericByRef().u4;
    config.StartupAddressMode   = (AddressMode)             pConfig[ FIELD___startupAddressMode   ].NumericByRef().u4;

    if(pMACAddress != NULL)
    {
        memcpy( &config.MacAddress, pMACAddress->GetFirstElement(), NETIF_MAX_HWADDR_LEN ); 
    }

    // load network interface configuration from storage
    if(!ConfigurationManager_GetConfigurationBlock((void*)&storageConfig, DeviceConfigurationOption_Network, interfaceIndex))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // save the configuration only if there has been a change
    configChanged = (config != storageConfig);

    // store configuration, updating the configuration block
    if(configChanged && ConfigurationManager_UpdateConfigurationBlock(&config, DeviceConfigurationOption_Network, interfaceIndex) != TRUE)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }
    
    NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_UpdateAdapterConfiguration(&config, interfaceIndex, updateFlags));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::GetNetworkInterfaceCount___STATIC__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    // grab the count right from the structure
    stack.SetResult_I4( g_TargetConfiguration.NetworkInterfaceConfigs->Count );    

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::GetNetworkInterface___STATIC__SystemNetNetworkInformationNetworkInterface__U4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    HAL_Configuration_NetworkInterface config; 
    CLR_RT_HeapBlock*         pConfig;
    CLR_UINT32                interfaceIndex = stack.Arg0().NumericByRef().u4;
    CLR_RT_HeapBlock&         top            = stack.PushValueAndClear();

    NANOCLR_CLEAR(config);
    
    // load network interface configuration from storage
    if(!ConfigurationManager_GetConfigurationBlock((void*)&config, DeviceConfigurationOption_Network, interfaceIndex))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }
    _ASSERTE(config.StartupAddressMode > 0);

    // now load adapter configuration on top of the stored config 
    NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_LoadConfiguration(&config, interfaceIndex));

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex( top, g_CLR_RT_WellKnownTypes.m_NetworkInterface ));
    
    pConfig = top.Dereference(); FAULT_ON_NULL(pConfig);

    pConfig[ FIELD___ipv4Address            ].SetInteger( (CLR_UINT32)config.IPv4Address);
    pConfig[ FIELD___ipv4GatewayAddress     ].SetInteger( (CLR_UINT32)config.IPv4GatewayAddress);
    pConfig[ FIELD___ipv4NetMask            ].SetInteger( (CLR_UINT32)config.IPv4NetMask);
    pConfig[ FIELD___ipv4dnsAddress1        ].SetInteger( (CLR_UINT32)config.IPv4DNSAddress1);
    pConfig[ FIELD___ipv4dnsAddress2        ].SetInteger( (CLR_UINT32)config.IPv4DNSAddress2);

    // FIXME IPV6
    // pConfig[ FIELD___ipv6Address            ].SetInteger( (CLR_UINT32)config.IPv6Address);
    // pConfig[ FIELD___ipv6GatewayAddress     ].SetInteger( (CLR_UINT32)config.IPv6GatewayAddress);
    // pConfig[ FIELD___ipv6NetMask            ].SetInteger( (CLR_UINT32)config.IPv6NetMask);
    // pConfig[ FIELD___ipv6dnsAddress1        ].SetInteger( (CLR_UINT32)config.IPv6DNSAddress1);
    // pConfig[ FIELD___ipv6dnsAddress2        ].SetInteger( (CLR_UINT32)config.IPv6DNSAddress2);

    pConfig[ FIELD___automaticDns           ].SetInteger( (CLR_UINT32)config.AutomaticDNS);
    pConfig[ FIELD___networkInterfaceType   ].SetInteger( (CLR_UINT32)config.InterfaceType);
    pConfig[ FIELD___specificConfigId       ].SetInteger( (CLR_UINT32)config.SpecificConfigId);
    pConfig[ FIELD___startupAddressMode     ].SetInteger( (CLR_UINT32)config.StartupAddressMode);
    pConfig[ FIELD___interfaceIndex         ].SetInteger( (CLR_UINT32)interfaceIndex);

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( pConfig[ FIELD___macAddress ], NETIF_MAX_HWADDR_LEN, g_CLR_RT_WellKnownTypes.m_UInt8 ));   
    memcpy( pConfig[ FIELD___macAddress ].DereferenceArray()->GetFirstElement(), config.MacAddress, NETIF_MAX_HWADDR_LEN );

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
