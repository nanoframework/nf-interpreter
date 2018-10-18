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
    NANOCLR_NATIVE_DECLARE(NativeEnableWakeupByTimer___STATIC__nanoFrameworkHardwareEsp32EspNativeError__U8);
    NANOCLR_NATIVE_DECLARE(NativeEnableWakeupByPin___STATIC__nanoFrameworkHardwareEsp32EspNativeError__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__I4);
    NANOCLR_NATIVE_DECLARE(NativeEnableWakeupByMultiPins___STATIC__nanoFrameworkHardwareEsp32EspNativeError__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__nanoFrameworkHardwareEsp32SleepWakeupMode);
    NANOCLR_NATIVE_DECLARE(NativeEnableWakeupByTouchPad___STATIC__nanoFrameworkHardwareEsp32EspNativeError);
    NANOCLR_NATIVE_DECLARE(NativeStartLightSleep___STATIC__nanoFrameworkHardwareEsp32EspNativeError);
    NANOCLR_NATIVE_DECLARE(NativeStartDeepSleep___STATIC__nanoFrameworkHardwareEsp32EspNativeError);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupCause___STATIC__nanoFrameworkHardwareEsp32SleepWakeupCause);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupGpioPin___STATIC__nanoFrameworkHardwareEsp32SleepWakeupGpioPin);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupTouchpad___STATIC__nanoFrameworkHardwareEsp32SleepTouchPad);

    //--//

};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResEventListener
{
    static const int FIELD__HighResTimers = 1;


    //--//

};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer
{
    static const int FIELD_STATIC__s_eventListener = 0;

    static const int FIELD___timerHandle = 1;
    static const int FIELD___disposedValue = 2;
    static const int FIELD___syncLock = 3;
    static const int FIELD__OnHighResTimerExpired = 4;

    NANOCLR_NATIVE_DECLARE(NativeEspTimerCreate___I4);
    NANOCLR_NATIVE_DECLARE(NativeEspTimerDispose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeStop___VOID);
    NANOCLR_NATIVE_DECLARE(NativeStartOneShot___VOID__U8);
    NANOCLR_NATIVE_DECLARE(NativeStartPeriodic___VOID__U8);
    NANOCLR_NATIVE_DECLARE(NativeGetCurrent___STATIC__U8);

    //--//

};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimerEvent
{
    static const int FIELD__EventType = 3;
    static const int FIELD__TimerHandle = 4;


    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32;

#endif  //_HARDWARE_ESP32_NATIVE_H_
