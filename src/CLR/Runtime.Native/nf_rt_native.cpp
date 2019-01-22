//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_rt_native.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::GetSystemVersion___STATIC__VOID__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4,
    NULL,
    Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::get_OEMString___STATIC__STRING,
    Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::get_OEM___STATIC__U1,
    Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::get_Model___STATIC__U1,
    Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::get_SKU___STATIC__U2,
    NULL,
    Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::GetNativeFloatingPointSupport___STATIC__U1,
    NULL,
    NULL,
    Library_nf_rt_native_nanoFramework_Runtime_Native_Debug::GC___STATIC__U4__BOOLEAN,
    Library_nf_rt_native_nanoFramework_Runtime_Native_Debug::EnableGCMessages___STATIC__VOID__BOOLEAN,
    NULL,
    NULL,
    NULL,
    Library_nf_rt_native_nanoFramework_Runtime_Native_ExecutionConstraint::Install___STATIC__VOID__I4__I4,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_rt_native_nanoFramework_Runtime_Native_Power::NativeReboot___STATIC__VOID,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_rt_native_nanoFramework_Runtime_Native_ResourceUtility::GetObject___STATIC__OBJECT__mscorlibSystemResourcesResourceManager__mscorlibSystemEnum,
    Library_nf_rt_native_nanoFramework_Runtime_Native_ResourceUtility::GetObject___STATIC__OBJECT__mscorlibSystemResourcesResourceManager__mscorlibSystemEnum__I4__I4,
    NULL,
    NULL,
    NULL,
    Library_nf_rt_native_nanoFramework_Runtime_Native_Rtc::Native_RTC_SetSystemTime___STATIC__BOOLEAN__I4__U1__U1__U1__U1__U1__U1,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Native =
{
    "nanoFramework.Runtime.Native", 
    0xFE14C7A7,
    method_lookup,
    { 1, 0, 4, 4 }
};
