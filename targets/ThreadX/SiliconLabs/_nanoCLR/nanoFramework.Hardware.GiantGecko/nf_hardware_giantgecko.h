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

typedef enum __nfpack GpioConfiguration_DriveStrenght
{
    GpioConfiguration_DriveStrenght_Strong = 0,
    GpioConfiguration_DriveStrenght_Weak = 1,
} GpioConfiguration_DriveStrenght;

struct Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration
{
    NANOCLR_NATIVE_DECLARE(SetSlewRate___STATIC__VOID__I4__U1);
    NANOCLR_NATIVE_DECLARE(SetSlewRateAlternate___STATIC__VOID__I4__U1);
    NANOCLR_NATIVE_DECLARE(
        SetDriveStrenght___STATIC__VOID__I4__nanoFrameworkHardwareGiantGeckoGpioConfigurationDriveStrenght);
    NANOCLR_NATIVE_DECLARE(
        SetDriveStrenghtAlternate___STATIC__VOID__I4__nanoFrameworkHardwareGiantGeckoGpioConfigurationDriveStrenght);

    //--//
    static HRESULT SetSlewRate(CLR_RT_StackFrame &stack, bool isAlternate);
    static HRESULT SetDriveStrenght(CLR_RT_StackFrame &stack, bool isAlternate);
};

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
