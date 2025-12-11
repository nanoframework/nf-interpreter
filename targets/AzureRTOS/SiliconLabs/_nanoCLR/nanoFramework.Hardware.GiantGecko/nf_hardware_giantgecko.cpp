//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_giantgecko.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration::SetSlewRate___STATIC__VOID__I4__U1,
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration::SetSlewRateAlternate___STATIC__VOID__I4__U1,
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration::SetDriveStrenght___STATIC__VOID__I4__nanoFrameworkHardwareGiantGeckoGpioConfigurationDriveStrenght,
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_GpioConfiguration::SetDriveStrenghtAlternate___STATIC__VOID__I4__nanoFrameworkHardwareGiantGeckoGpioConfigurationDriveStrenght,
    nullptr,
    nullptr,
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Power::NativeEnterHibernateMode___STATIC__VOID,
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Power::NativeEnterShutoffMode___STATIC__VOID,
    nullptr,
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Rtc::GetAlarm___STATIC__SystemDateTime,
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Rtc::NativeRtcSetAlarm___STATIC__VOID__I4__U1__U1__U1__U1__U1,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Utilities::NativeGetDeviceUniqueId___STATIC__VOID__SZARRAY_U1,
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Utilities::NativeGetProductionRevision___STATIC__U1,
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Utilities::NativeGetDeviceFamily___STATIC__U1,
    Library_nf_hardware_giantgecko_nanoFramework_Hardware_GiantGecko_Utilities::NativeGetDeviceNumber___STATIC__U1,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_GiantGecko =
{
    "nanoFramework.Hardware.GiantGecko",
    0x7945DFE7,
    method_lookup,
    { 100, 2, 0, 0 }
};

// clang-format on
