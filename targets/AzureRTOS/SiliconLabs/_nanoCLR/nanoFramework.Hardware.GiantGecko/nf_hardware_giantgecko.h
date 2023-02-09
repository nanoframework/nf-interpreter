//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_HARDWARE_GIANTGECKO_H
#define NF_HARDWARE_GIANTGECKO_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

struct Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Power
{
    NANOCLR_NATIVE_DECLARE(NativeEnterHibernateMode___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(NativeEnterShutoffMode___STATIC__VOID);

    //--//
};

struct Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Rtc
{
    NANOCLR_NATIVE_DECLARE(GetAlarm___STATIC__SystemDateTime);
    NANOCLR_NATIVE_DECLARE(NativeRtcSetAlarm___STATIC__VOID__I4__U1__U1__U1__U1__U1);

    //--//
};

struct Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Utilities
{
    static const int FIELD_STATIC___deviceUniqueId = 0;
    static const int FIELD_STATIC___productionRevision = 1;
    static const int FIELD_STATIC___deviceFamily = 2;
    static const int FIELD_STATIC___deviceNumber = 3;

    NANOCLR_NATIVE_DECLARE(NativeGetDeviceUniqueId___STATIC__VOID__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeGetProductionRevision___STATIC__U1);
    NANOCLR_NATIVE_DECLARE(NativeGetDeviceFamily___STATIC__U1);
    NANOCLR_NATIVE_DECLARE(NativeGetDeviceNumber___STATIC__U1);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_GiantGecko;

#endif // NF_HARDWARE_GIANTGECKO_H
