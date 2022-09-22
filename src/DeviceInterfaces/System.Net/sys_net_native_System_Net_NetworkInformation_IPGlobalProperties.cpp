//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"
#include <nanoPAL_Sockets.h>

HRESULT Library_sys_net_native_System_Net_NetworkInformation_IPGlobalProperties::
    GetIPAddress___STATIC__SystemNetIPAddress(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    HAL_Configuration_NetworkInterface config;

    CLR_RT_TypeDef_Index ipAddressTypeDef;
    CLR_RT_HeapBlock *ipAddressHbObj;
    CLR_RT_HeapBlock ipAddress;
    CLR_INT64 *pRes;
    bool ipAddressValid = false;

    CLR_RT_HeapBlock &top = stack.PushValue();

    // find <IPAddress> type, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef("IPAddress", "System.Net", ipAddressTypeDef);

    // create an instance of <IPAddress>
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(ipAddress, ipAddressTypeDef));

    // dereference the object in order to reach its fields
    ipAddressHbObj = ipAddress.Dereference();

    {
        // IPAddress _address field
        // CLR_INT64 fields need to be accessed by pointer
        CLR_RT_HeapBlock &addressFieldRef = ipAddressHbObj[Library_sys_net_native_System_Net_IPAddress::FIELD__Address];
        pRes = (CLR_INT64 *)&addressFieldRef.NumericByRef().s8;

        // IPAddress _family field
        // IP v4: AddressFamily.InterNetwork
        ipAddressHbObj[Library_sys_net_native_System_Net_IPAddress::FIELD___family].NumericByRef().s4 = SOCK_AF_INET;
    }

    // loop through all the network interface and check if any is up
    for (int interfaceIndex = 0; interfaceIndex < g_TargetConfiguration.NetworkInterfaceConfigs->Count;
         interfaceIndex++)
    {
        NANOCLR_CLEAR(config);

        // load network interface configuration from storage
        if (!ConfigurationManager_GetConfigurationBlock(
                (void *)&config,
                DeviceConfigurationOption_Network,
                interfaceIndex))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        _ASSERTE(config.StartupAddressMode > 0);

        // load adapter configuration on top of the stored config
        NANOCLR_CHECK_HRESULT(SOCK_CONFIGURATION_LoadConfiguration(&config, interfaceIndex));

        // has IP v4?
        if (config.IPv4Address > 0)
        {
            *pRes = config.IPv4Address;

            // we have an IP...
            ipAddressValid = true;

            // .. no need to check any other
            break;
        }
    }

    if (ipAddressValid)
    {
        // set address field with IPAddress heap block object
        top.SetObjectReference(ipAddressHbObj);
    }
    else
    {
        // default to IP Any Address
        top.SetObjectReference(g_CLR_RT_TypeSystem.m_assemblies[ipAddressTypeDef.Assembly() - 1]
                                   ->GetStaticField(Library_sys_net_native_System_Net_IPAddress::FIELD_STATIC__Any)
                                   ->Dereference());
    }

    NANOCLR_NOCLEANUP();
}
