//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NANOFRAMEWORK_HARDWARE_ESP32_NATIVE_H
#define NANOFRAMEWORK_HARDWARE_ESP32_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <corlib_native.h>

typedef enum __nfpack NativeMemory_MemoryType
{
    NativeMemory_MemoryType_All = 0,
    NativeMemory_MemoryType_Internal = 1,
    NativeMemory_MemoryType_SpiRam = 2,
} NativeMemory_MemoryType;

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Configuration
{
    NANOCLR_NATIVE_DECLARE(NativeSetPinFunction___STATIC__VOID__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetPinFunction___STATIC__I4__I4);

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

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Logging
{
    NANOCLR_NATIVE_DECLARE(NativeSetLogLevel___STATIC__VOID__STRING__I4);

    //--//
};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_NativeMemory
{
    NANOCLR_NATIVE_DECLARE(NativeGetMemoryTotalSize___STATIC__U4__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetMemoryTotalFreeSize___STATIC__U4__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetMemoryLargestFreeBlock___STATIC__U4__I4);

    //--//
};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep
{
    NANOCLR_NATIVE_DECLARE(NativeEnableWakeupByTimer___STATIC__nanoFrameworkHardwareEsp32EspNativeError__U8);
    NANOCLR_NATIVE_DECLARE(
        NativeEnableWakeupByPin___STATIC__nanoFrameworkHardwareEsp32EspNativeError__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__I4);
    NANOCLR_NATIVE_DECLARE(
        NativeEnableWakeupByMultiPins___STATIC__nanoFrameworkHardwareEsp32EspNativeError__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__nanoFrameworkHardwareEsp32SleepWakeupMode);
    NANOCLR_NATIVE_DECLARE(NativeEnableWakeupByTouchPad___STATIC__nanoFrameworkHardwareEsp32EspNativeError);
    NANOCLR_NATIVE_DECLARE(NativeStartLightSleep___STATIC__nanoFrameworkHardwareEsp32EspNativeError);
    NANOCLR_NATIVE_DECLARE(NativeStartDeepSleep___STATIC__nanoFrameworkHardwareEsp32EspNativeError);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupCause___STATIC__nanoFrameworkHardwareEsp32SleepWakeupCause);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupGpioPin___STATIC__nanoFrameworkHardwareEsp32SleepWakeupGpioPin);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupTouchpad___STATIC__nanoFrameworkHardwareEsp32SleepTouchPad);

    //--//
};

typedef enum __nfpack IFilterSetting_FilterType
{
    IFilterSetting_FilterType_Esp32 = 0,
    IFilterSetting_FilterType_S2_S3 = 1,
} IFilterSetting_FilterType;

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_Esp32FilterSetting
{
    static const int FIELD___period = 1;
    static const int FIELD___type = 2;

    //--//
};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_S2S3FilterSetting
{
    static const int FIELD___periodeSettingMode = 1;
    static const int FIELD___filterSettingDebounce = 2;
    static const int FIELD___filterSettingNoiseThreshold = 3;
    static const int FIELD___type = 4;
    static const int FIELD___jitterSize = 5;
    static const int FIELD___filterSettingSmoothMode = 6;

    //--//
};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad
{
    static const int FIELD_STATIC___touchHighVoltage = 1;
    static const int FIELD_STATIC___touchLowVoltage = 2;
    static const int FIELD_STATIC___touchHighVoltageAttenuation = 3;
    static const int FIELD_STATIC___isFilterOn = 4;
    static const int FIELD_STATIC___touchPadEventHandler = 5;

    static const int FIELD___calibrationData = 1;
    static const int FIELD___callbacks = 2;
    static const int FIELD___syncLock = 3;
    static const int FIELD___disposedValue = 4;
    static const int FIELD___touchPadNumber = 5;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDeinit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeRead___U4);
    NANOCLR_NATIVE_DECLARE(NativeGetThreshold___U4);
    NANOCLR_NATIVE_DECLARE(NativeSetThreshold___VOID__U4);
    NANOCLR_NATIVE_DECLARE(NativeGetTouchSpeed___VOID__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(NativeSetChargeSpeed___VOID__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeCalibrate___I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetGpioNumberFromTouchNumber___STATIC__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetTriggerMode___STATIC__nanoFrameworkHardwareEsp32TouchTouchTriggerMode);
    NANOCLR_NATIVE_DECLARE(NativeSetTriggerMode___STATIC__VOID__nanoFrameworkHardwareEsp32TouchTouchTriggerMode);
    NANOCLR_NATIVE_DECLARE(NativeGetTriggerSource___STATIC__nanoFrameworkHardwareEsp32TouchWakeUpSource);
    NANOCLR_NATIVE_DECLARE(NativeSetTriggerSource___STATIC__VOID__nanoFrameworkHardwareEsp32TouchWakeUpSource);
    NANOCLR_NATIVE_DECLARE(NativeGetMeasurementMode___STATIC__nanoFrameworkHardwareEsp32TouchMeasurementMode);
    NANOCLR_NATIVE_DECLARE(NativeSetMeasurementMode___STATIC__VOID__nanoFrameworkHardwareEsp32TouchMeasurementMode);
    NANOCLR_NATIVE_DECLARE(NativeSetVoltage___STATIC__VOID__nanoFrameworkHardwareEsp32TouchTouchHighVoltage__nanoFrameworkHardwareEsp32TouchTouchLowVoltage__nanoFrameworkHardwareEsp32TouchTouchHighVoltageAttenuation);
    NANOCLR_NATIVE_DECLARE(NativeGetVoltage___STATIC__VOID__BYREF_nanoFrameworkHardwareEsp32TouchTouchHighVoltage__BYREF_nanoFrameworkHardwareEsp32TouchTouchLowVoltage__BYREF_nanoFrameworkHardwareEsp32TouchTouchHighVoltageAttenuation);
    NANOCLR_NATIVE_DECLARE(NativeStartFilter___STATIC__VOID__nanoFrameworkHardwareEsp32TouchIFilterSetting);
    NANOCLR_NATIVE_DECLARE(NativeStopFilter___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(NativeSetMeasurementTime___STATIC__VOID__U2__U2);
    NANOCLR_NATIVE_DECLARE(NativeGetMeasurementTime___STATIC__VOID__BYREF_U2__BYREF_U2);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32;

#endif // NANOFRAMEWORK_HARDWARE_ESP32_NATIVE_H
