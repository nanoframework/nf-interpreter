//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
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
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakupByTimer___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp__U8,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakupByPin___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__I4,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakeupByMultiPins___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__nanoFrameworkHardwareEsp32SleepWakupMode,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakeupByTouchPad___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeStartLightSleep___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeStartDeepSleep___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeGetWakeupCause___STATIC__nanoFrameworkHardwareEsp32SleepWakeupCause,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeGetWakeupGpioPin___STATIC__nanoFrameworkHardwareEsp32SleepWakeupGpioPin,
    Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeGetWakeupTouchpad___STATIC__nanoFrameworkHardwareEsp32SleepTouchPad,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32 =
{
    "nanoFramework.Hardware.Esp32", 
    0x33CA620D,
    method_lookup,
    { 1, 0, 0, 0}
};




