//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Host_Network.h>
#include <sys_net_native.h>

namespace
{
std::vector<HostNetworkInterface> s_interfaces;
using ManagedInterface = Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface;

HRESULT PopulateInterface(CLR_RT_HeapBlock &reference, const HostNetworkInterface &adapter)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock array;
    array.SetObjectReference(nullptr);
    CLR_RT_ProtectFromGC protectArray(array);
    CLR_RT_ProtectFromGC protectReference(reference);
    CLR_RT_HeapBlock *object = reference.Dereference();
    FAULT_ON_NULL(object);

    object[ManagedInterface::FIELD___interfaceIndex].SetInteger(adapter.index);
    object[ManagedInterface::FIELD___ipv4Address].SetInteger(adapter.address);
    object[ManagedInterface::FIELD___ipv4NetMask].SetInteger(adapter.netmask);
    object[ManagedInterface::FIELD___ipv4GatewayAddress].SetInteger(adapter.gateway);
    object[ManagedInterface::FIELD___ipv4dnsAddress1].SetInteger(adapter.dns[0]);
    object[ManagedInterface::FIELD___ipv4dnsAddress2].SetInteger(adapter.dns[1]);
    object[ManagedInterface::FIELD___networkInterfaceType].SetInteger(adapter.type);
    object[ManagedInterface::FIELD___specificConfigId].SetInteger(static_cast<CLR_UINT32>(UINT32_MAX));
    // The host owns address assignment; do not claim a DHCP/static policy we cannot discover.
    object[ManagedInterface::FIELD___startupAddressMode].SetInteger(static_cast<CLR_UINT8>(0));
    object[ManagedInterface::FIELD___automaticDns].SetBoolean(false);

    NANOCLR_CHECK_HRESULT(
        CLR_RT_HeapBlock_Array::CreateInstance(array, adapter.mac.size(), g_CLR_RT_WellKnownTypes.UInt8));
    if (!adapter.mac.empty())
    {
        memcpy(array.DereferenceArray()->GetFirstElement(), adapter.mac.data(), adapter.mac.size());
    }
    reference.Dereference()[ManagedInterface::FIELD___macAddress].SetObjectReference(array.DereferenceArray());

    for (int field :
         {ManagedInterface::FIELD___ipv6Address,
          ManagedInterface::FIELD___ipv6NetMask,
          ManagedInterface::FIELD___ipv6GatewayAddress,
          ManagedInterface::FIELD___ipv6dnsAddress1,
          ManagedInterface::FIELD___ipv6dnsAddress2})
    {
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(array, 4, g_CLR_RT_WellKnownTypes.UInt32));
        reference.Dereference()[field].SetObjectReference(array.DereferenceArray());
    }

    NANOCLR_NOCLEANUP();
}
} // namespace

HRESULT HostNetwork_Resolve(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    SOCK_addrinfo *addresses = nullptr;
    int request = 0;
    int result;
    bool signalled = true;
    CLR_INT64 *timeout;
    CLR_RT_HeapBlock timeoutValue;
    const char *name = stack.Arg0().RecoverString();

    FAULT_ON_NULL_ARG(name);
    timeoutValue.SetInteger(static_cast<CLR_INT64>(30000) * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(timeoutValue, timeout));

    if (stack.m_customState == 1)
    {
        request = HostNetwork_StartResolve(name);
        if (request < 0)
        {
            Library_sys_net_native_System_Net_Sockets_NativeSocket::ThrowError(stack, SOCK_getlasterror());
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }
        stack.PushValueI4(request);
        stack.m_customState = 2;
    }
    request = stack.m_evalStack[1].NumericByRef().s4;

    for (;;)
    {
        if (!signalled)
        {
            Library_sys_net_native_System_Net_Sockets_NativeSocket::ThrowError(stack, SOCK_ETIMEDOUT);
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }
        result = HostNetwork_CompleteResolve(request, &addresses);
        if (result == 0)
        {
            break;
        }
        int error = SOCK_getlasterror();
        if (error != SOCK_EWOULDBLOCK)
        {
            Library_sys_net_native_System_Net_Sockets_NativeSocket::ThrowError(stack, error);
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, signalled));
    }

    {
        CLR_UINT32 count = 0;
        for (auto address = addresses; address != nullptr; address = address->ai_next)
        {
            ++count;
        }
        CLR_RT_HeapBlock canonicalName;
        canonicalName.SetObjectReference(nullptr);
        CLR_RT_ProtectFromGC protectName(canonicalName);
        CLR_RT_HeapBlock addressArray;
        addressArray.SetObjectReference(nullptr);
        CLR_RT_ProtectFromGC protectAddresses(addressArray);
        CLR_RT_HeapBlock addressBytes;
        addressBytes.SetObjectReference(nullptr);
        CLR_RT_ProtectFromGC protectBytes(addressBytes);

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(canonicalName, addresses->ai_canonname));
        NANOCLR_CHECK_HRESULT(canonicalName.StoreToReference(stack.Arg1(), 0));

        CLR_RT_ReflectionDef_Index index;
        index.kind = REFLECTION_TYPE;
        index.levels = 2;
        index.data.type.data = g_CLR_RT_WellKnownTypes.UInt8.data;
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(addressArray, count, index));

        count = 0;
        for (auto address = addresses; address != nullptr; address = address->ai_next)
        {
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                addressBytes,
                address->ai_addrlen,
                g_CLR_RT_WellKnownTypes.UInt8));
            memcpy(addressBytes.DereferenceArray()->GetFirstElement(), address->ai_addr, address->ai_addrlen);
            auto element = reinterpret_cast<CLR_RT_HeapBlock *>(addressArray.DereferenceArray()->GetElement(count++));
            element->SetObjectReference(addressBytes.DereferenceArray());
        }
        NANOCLR_CHECK_HRESULT(addressArray.StoreToReference(stack.Arg2(), 0));
    }
    stack.PopValue();
    stack.PopValue();

    NANOCLR_CLEANUP();

    SOCK_freeaddrinfo(addresses);
    if (request > 0 && hr != CLR_E_THREAD_WAITING && hr != CLR_E_RESCHEDULE)
    {
        HostNetwork_CancelResolve(request);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::GetIsNetworkAvailable___STATIC__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    int available = HostNetwork_IsAvailable();
    if (available < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }
    stack.SetResult_Boolean(available != 0);
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::
    IPV4AddressFromString___STATIC__I8__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    uint64_t address;
    NANOCLR_CHECK_HRESULT(SOCK_IPV4AddressFromString(stack.Arg0().RecoverString(), &address));
    stack.PushValue().SetInteger(static_cast<CLR_UINT64>(address));
    NANOCLR_NOCLEANUP();
}

HRESULT ManagedInterface::GetNetworkInterfaceCount___STATIC__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    if (HostNetwork_ListInterfaces(s_interfaces) < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }
    stack.SetResult_I4(s_interfaces.size());
    NANOCLR_NOCLEANUP();
}

HRESULT ManagedInterface::GetNetworkInterface___STATIC__SystemNetNetworkInformationNetworkInterface__U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    CLR_UINT32 index = stack.Arg0().NumericByRef().u4;
    CLR_RT_TypeDef_Index type;
    if (index >= s_interfaces.size())
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }
    if (!g_CLR_RT_TypeSystem.FindTypeDef("NetworkInterface", "System.Net.NetworkInformation", type))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TYPE_UNAVAILABLE);
    }
    {
        CLR_RT_HeapBlock &result = stack.PushValueAndClear();
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(result, type));
        NANOCLR_CHECK_HRESULT(PopulateInterface(result, s_interfaces[index]));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT ManagedInterface::InitializeNetworkInterfaceSettings___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock *object = stack.This();
    CLR_UINT32 index;
    std::vector<HostNetworkInterface> interfaces;
    FAULT_ON_NULL(object);
    index = object[FIELD___interfaceIndex].NumericByRef().u4;
    if (HostNetwork_ListInterfaces(interfaces) < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }
    for (const auto &adapter : interfaces)
    {
        if (adapter.index == index)
        {
            NANOCLR_CHECK_HRESULT(PopulateInterface(stack.Arg0(), adapter));
            NANOCLR_SET_AND_LEAVE(S_OK);
        }
    }
    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    NANOCLR_NOCLEANUP();
}

#define HOST_UNSUPPORTED_NETWORK_METHOD(name)                                                                          \
    HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::name(CLR_RT_StackFrame &stack)      \
    {                                                                                                                  \
        (void)stack;                                                                                                   \
        return CLR_E_NOT_SUPPORTED;                                                                                    \
    }

HOST_UNSUPPORTED_NETWORK_METHOD(UpdateConfiguration___VOID__I4)
HOST_UNSUPPORTED_NETWORK_METHOD(IPV6AddressFromString___STATIC__SZARRAY_U2__STRING)
