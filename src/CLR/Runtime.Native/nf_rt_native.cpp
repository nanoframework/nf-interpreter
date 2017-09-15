//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_rt_native.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_rt_native_nanoFramework_Runtime_Hardware_SystemInfo::GetSystemVersion___STATIC__VOID__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4,
    NULL,
    Library_nf_rt_native_nanoFramework_Runtime_Hardware_SystemInfo::get_OEMString___STATIC__STRING,
    Library_nf_rt_native_nanoFramework_Runtime_Hardware_SystemInfo::get_OEM___STATIC__U1,
    Library_nf_rt_native_nanoFramework_Runtime_Hardware_SystemInfo::get_Model___STATIC__U1,
    Library_nf_rt_native_nanoFramework_Runtime_Hardware_SystemInfo::get_SKU___STATIC__U2,
    NULL,
    Library_nf_rt_native_nanoFramework_Runtime_Native_Debug::GC___STATIC__U4__BOOLEAN,
    Library_nf_rt_native_nanoFramework_Runtime_Native_Debug::EnableGCMessages___STATIC__VOID__BOOLEAN,
    NULL,
    NULL,
    NULL,
    Library_nf_rt_native_nanoFramework_Runtime_Native_ExecutionConstraint::Install___STATIC__VOID__I4__I4,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Native =
{
    "nanoFramework.Runtime.Native", 
    0x10DD04A1,
    method_lookup
};

