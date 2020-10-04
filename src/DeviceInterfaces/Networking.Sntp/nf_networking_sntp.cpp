//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_networking_sntp.h"

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    Library_nf_networking_sntp_nanoFramework_Networking_Sntp::Start___STATIC__VOID,
    Library_nf_networking_sntp_nanoFramework_Networking_Sntp::Stop___STATIC__VOID,
    Library_nf_networking_sntp_nanoFramework_Networking_Sntp::UpdateNow___STATIC__VOID,
    Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_IsStarted___STATIC__BOOLEAN,
    Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_Server1___STATIC__STRING,
    Library_nf_networking_sntp_nanoFramework_Networking_Sntp::set_Server1___STATIC__VOID__STRING,
    Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_Server2___STATIC__STRING,
    Library_nf_networking_sntp_nanoFramework_Networking_Sntp::set_Server2___STATIC__VOID__STRING,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Networking_Sntp =
{
    "nanoFramework.Networking.Sntp",
    0xE2D9BDED,
    method_lookup,
    { 100, 0, 4, 4 }
};
