//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_pwm_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeInit___VOID,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetDesiredFrequency___U4__U4,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetActiveDutyCyclePercentage___VOID__U4,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStart___VOID,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStop___VOID,
    Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::DisposeNative___VOID,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Pwm =
{
    "System.Device.Pwm",
    0x3D216AC0,
    method_lookup,
    { 100, 1, 0, 4 }
};

// clang-format on
