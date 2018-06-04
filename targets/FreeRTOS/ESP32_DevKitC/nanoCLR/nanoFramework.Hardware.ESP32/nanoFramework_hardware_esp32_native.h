//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _HARDWARE_ESP32_NATIVE_H_
#define _HARDWARE_ESP32_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <corlib_native.h>

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Logging
{
    NANOCLR_NATIVE_DECLARE(NativeSetLogLevel___STATIC__VOID__STRING__I4);

    //--//

};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep
{
    NANOCLR_NATIVE_DECLARE(NativeEnableWakupByTimer___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp__U8);
    NANOCLR_NATIVE_DECLARE(NativeEnableWakupByPin___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__I4);
    NANOCLR_NATIVE_DECLARE(NativeEnableWakeupByMultiPins___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__nanoFrameworkHardwareEsp32SleepWakupMode);
    NANOCLR_NATIVE_DECLARE(NativeEnableWakeupByTouchPad___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp);
    NANOCLR_NATIVE_DECLARE(NativeStartLightSleep___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp);
    NANOCLR_NATIVE_DECLARE(NativeStartDeepSleep___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupCause___STATIC__nanoFrameworkHardwareEsp32SleepWakeupCause);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupGpioPin___STATIC__nanoFrameworkHardwareEsp32SleepWakeupGpioPin);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupTouchpad___STATIC__nanoFrameworkHardwareEsp32SleepTouchPad);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32;

#endif  //_HARDWARE_ESP32_NATIVE_H_
