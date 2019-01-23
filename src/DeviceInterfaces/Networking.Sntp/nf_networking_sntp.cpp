//
// Copyright (c) 2018 The nanoFramework project contributors
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
    0x733B4551,
    method_lookup,
    { 1, 0, 4, 3 }
};
