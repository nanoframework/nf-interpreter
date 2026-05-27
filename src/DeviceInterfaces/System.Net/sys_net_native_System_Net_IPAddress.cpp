//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"

HRESULT Library_sys_net_native_System_Net_IPAddress::IPv4ToString___STATIC__STRING__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // get IP v4 address in numeric format
    NANOCLR_CHECK_HRESULT(stack.SetResult_String(SOCK_IPV4AddressToString(stack.Arg0().NumericByRef().u4)));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_IPAddress::IPv6ToString___STATIC__STRING__SZARRAY_U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(LWIP_IPV6) && LWIP_IPV6
    // Get address of ushort array with ipv6 address
    CLR_UINT16 *addr = (CLR_UINT16 *)stack.Arg0().DereferenceArray()->GetFirstElement();

    // get IP v6 address in numeric format
    NANOCLR_CHECK_HRESULT(stack.SetResult_String(SOCK_IPV6AddressToString(addr)));
#else
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif

    NANOCLR_NOCLEANUP();
}
