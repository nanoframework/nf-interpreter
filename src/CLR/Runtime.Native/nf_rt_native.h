//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_RT_NATIVE_H_
#define _NF_RT_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>

struct Library_nf_rt_native_nanoFramework_Runtime_Hardware_SystemInfo
{
    NANOCLR_NATIVE_DECLARE(GetSystemVersion___STATIC__VOID__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(get_OEMString___STATIC__STRING);
    NANOCLR_NATIVE_DECLARE(get_OEM___STATIC__U1);
    NANOCLR_NATIVE_DECLARE(get_Model___STATIC__U1);
    NANOCLR_NATIVE_DECLARE(get_SKU___STATIC__U2);

    //--//

};

struct Library_nf_rt_native_nanoFramework_Runtime_Native_Debug
{
    NANOCLR_NATIVE_DECLARE(GC___STATIC__U4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(EnableGCMessages___STATIC__VOID__BOOLEAN);

    //--//

};

struct Library_nf_rt_native_nanoFramework_Runtime_Native_ExecutionConstraint
{
    NANOCLR_NATIVE_DECLARE(Install___STATIC__VOID__I4__I4);

    //--//

};

struct Library_nf_rt_native_nanoFramework_Runtime_Native_RTC
{
    NANOCLR_NATIVE_DECLARE(Native_RTC_SetSystemTime___STATIC__BOOLEAN__I4__U1__U1__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(Native_RTC_SetAlarm___STATIC__BOOLEAN__I4__U1__U1__U1__U1__U1__U1);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Native;

#endif  //_NF_RT_NATIVE_H_
