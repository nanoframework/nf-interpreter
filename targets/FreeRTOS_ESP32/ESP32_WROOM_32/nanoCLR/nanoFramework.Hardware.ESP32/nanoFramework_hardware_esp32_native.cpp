//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Logging::NativeSetLogLevel___STATIC__VOID__STRING__I4,
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
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakeupByTimer___STATIC__nanoFrameworkHardwareEsp32EspNativeError__U8,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakeupByPin___STATIC__nanoFrameworkHardwareEsp32EspNativeError__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__I4,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakeupByMultiPins___STATIC__nanoFrameworkHardwareEsp32EspNativeError__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__nanoFrameworkHardwareEsp32SleepWakeupMode,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakeupByTouchPad___STATIC__nanoFrameworkHardwareEsp32EspNativeError,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeStartLightSleep___STATIC__nanoFrameworkHardwareEsp32EspNativeError,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeStartDeepSleep___STATIC__nanoFrameworkHardwareEsp32EspNativeError,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeGetWakeupCause___STATIC__nanoFrameworkHardwareEsp32SleepWakeupCause,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeGetWakeupGpioPin___STATIC__nanoFrameworkHardwareEsp32SleepWakeupGpioPin,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeGetWakeupTouchpad___STATIC__nanoFrameworkHardwareEsp32SleepTouchPad,
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
    NULL,
    NULL,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeEspTimerCreate___I4,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeEspTimerDispose___VOID,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeStop___VOID,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeStartOneShot___VOID__U8,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeStartPeriodic___VOID__U8,
    NULL,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeGetCurrent___STATIC__U8,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32 =
{
    "nanoFramework.Hardware.Esp32", 
    0xF8079179, 
    method_lookup,
    { 1, 0, 7, 3 }
};
