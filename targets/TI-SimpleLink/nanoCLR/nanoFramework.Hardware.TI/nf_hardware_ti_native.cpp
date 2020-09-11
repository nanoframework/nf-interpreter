//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_ti_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power::ConfigureWakeupFromGpioPin___STATIC__VOID__SZARRAY_nanoFrameworkHardwareTIPowerPinWakeupConfig,
    Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power::get_SourceOfReset___STATIC__nanoFrameworkHardwareTIPowerResetSource,
    Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power::NativeEnterShutdownMode___STATIC__VOID,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_TI =
{
    "nanoFramework.Hardware.TI",
    0xD906C9B4,
    method_lookup,
    { 100, 0, 0, 1 }
};

// clang-format on
