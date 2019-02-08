//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_pwm_native.h"

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
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_MaxFrequency___R8,
    Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_MinFrequency___R8,
    Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_PinCount___I4,
    NULL,
    NULL,
    NULL,
    Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::NativeSetDesiredFrequency___U4__U4,
    NULL,
    NULL,
    Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::GetDeviceSelector___STATIC__STRING,
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
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeInit___VOID,
    Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeSetActiveDutyCyclePercentage___VOID__U4,
    Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeSetPolarity___VOID__U1,
    Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeStart___VOID,
    Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeStop___VOID,
    Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::DisposeNative___VOID,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Pwm =
{
    "Windows.Devices.Pwm", 
    0x75280B26,
    method_lookup,
    { 1, 1, 3, 1 }
};
