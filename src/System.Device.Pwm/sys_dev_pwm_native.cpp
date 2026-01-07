//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_pwm_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeInit___VOID,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetDesiredFrequency___VOID__I4,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetActiveDutyCyclePercentage___VOID__R8,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStart___VOID,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStop___VOID,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::DisposeNative___VOID,
    nullptr,
    nullptr,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::GetChannel___STATIC__I4__I4__I4,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Pwm =
{
    "System.Device.Pwm",
    0x916EE980,
    method_lookup,
    { 100, 2, 0, 0 }
};

// clang-format on
