//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_ti_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power::ConfigureWakeupFromGpioPin___STATIC__VOID__SZARRAY_nanoFrameworkHardwareTIPowerPinWakeupConfig,
    Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power::get_SourceOfReset___STATIC__nanoFrameworkHardwareTIPowerResetSource,
    Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power::NativeEnterShutdownMode___STATIC__VOID,
    Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power::NativeEnterStandbyMode___STATIC__VOID__SystemTimeSpan,
    NULL,
    NULL,
    Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Utilities::NativeGetIeeeAddress___STATIC__SZARRAY_U1,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_TI =
{
    "nanoFramework.Hardware.TI",
    0x8A9130A3,
    method_lookup,
    { 100, 0, 0, 3 }
};

// clang-format on
