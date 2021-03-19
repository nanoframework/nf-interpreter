//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"

HRESULT Library_sys_net_native_System_Net_IPAddress::IPv4ToString___STATIC__STRING__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // // get IP v4 address in numeric format
    // const ip4_addr_t ip4Address = {stack.Arg0().NumericByRef().u4};

    // NANOCLR_CHECK_HRESULT(stack.SetResult_String(ip4addr_ntoa(&ip4Address)));

    NANOCLR_CHECK_HRESULT(stack.SetResult_String(SOCK_IPAddressToString(stack.Arg0().NumericByRef().u4)));

    NANOCLR_NOCLEANUP();
}
