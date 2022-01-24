//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NF_RT_NATIVE_H
#define NF_RT_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>

typedef enum __nfpack SystemInfo_FloatingPoint
{
    SystemInfo_FloatingPoint_None = 0,
    SystemInfo_FloatingPoint_SinglePrecisionSoftware = 1,
    SystemInfo_FloatingPoint_SinglePrecisionHardware = 2,
    SystemInfo_FloatingPoint_DoublePrecisionSoftware = 3,
    SystemInfo_FloatingPoint_DoublePrecisionHardware = 4,
} SystemInfo_FloatingPoint;

struct Library_nf_rt_native_nanoFramework_Runtime_Native_ExecutionConstraint
{
    NANOCLR_NATIVE_DECLARE(Install___STATIC__VOID__I4__I4);

    //--//
};

struct Library_nf_rt_native_nanoFramework_Runtime_Native_GC
{
    NANOCLR_NATIVE_DECLARE(Run___STATIC__U4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(EnableGCMessages___STATIC__VOID__BOOLEAN);

    //--//
};

struct Library_nf_rt_native_nanoFramework_Runtime_Native_Power
{
    static const int FIELD_STATIC__OnRebootEvent = 0;

    NANOCLR_NATIVE_DECLARE(NativeReboot___STATIC__VOID);

    //--//
};

struct Library_nf_rt_native_nanoFramework_Runtime_Native_Rtc
{
    NANOCLR_NATIVE_DECLARE(Native_RTC_SetSystemTime___STATIC__BOOLEAN__I4__U1__U1__U1__U1__U1__U1);

    //--//
};

struct Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo
{
    NANOCLR_NATIVE_DECLARE(GetSystemVersion___STATIC__VOID__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(get_OEMString___STATIC__STRING);
    NANOCLR_NATIVE_DECLARE(get_OEM___STATIC__U1);
    NANOCLR_NATIVE_DECLARE(get_Model___STATIC__U1);
    NANOCLR_NATIVE_DECLARE(get_SKU___STATIC__U2);
    NANOCLR_NATIVE_DECLARE(get_TargetName___STATIC__STRING);
    NANOCLR_NATIVE_DECLARE(get_Platform___STATIC__STRING);
    NANOCLR_NATIVE_DECLARE(GetNativeFloatingPointSupport___STATIC__U1);

    //--//
};

struct Library_nf_rt_native_System_Environment
{
    NANOCLR_NATIVE_DECLARE(get_TickCount64___STATIC__I8);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Native;

#endif //_NF_RT_NATIVE_H_
