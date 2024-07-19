//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NANOFRAMEWORK_HARDWARE_ESP32_NATIVE_H
#define NANOFRAMEWORK_HARDWARE_ESP32_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack EspNativeError
{
    EspNativeError_OK = 0,
    EspNativeError_FAIL = -1,
    EspNativeError_NO_MEM = 257,
    EspNativeError_INVALID_ARG = 258,
    EspNativeError_INVALID_STATE = 259,
    EspNativeError_INVALID_SIZE = 260,
    EspNativeError_NOT_FOUND = 261,
    EspNativeError_NOT_SUPPORTED = 262,
    EspNativeError_TIMEOUT = 263,
    EspNativeError_INVALID_RESPONSE = 264,
    EspNativeError_INVALID_CRC = 265,
    EspNativeError_INVALID_VERSION = 266,
    EspNativeError_INVALID_MAC = 267,
    EspNativeError_WIFI_BASE = 12288,
} EspNativeError;

typedef enum __nfpack HighResTimerEventType
{
    HighResTimerEventType_TimerExpired = 101,
} HighResTimerEventType;

typedef enum __nfpack Logging_LogLevel
{
    Logging_LogLevel_LOG_LEVEL_NONE = 0,
    Logging_LogLevel_LOG_LEVEL_ERROR = 1,
    Logging_LogLevel_LOG_LEVEL_WARN = 2,
    Logging_LogLevel_LOG_LEVEL_INFO = 3,
    Logging_LogLevel_LOG_LEVEL_DEBUG = 4,
    Logging_LogLevel_LOG_LEVEL_VERBOSE = 5,
} Logging_LogLevel;

typedef enum __nfpack NativeMemory_MemoryType
{
    NativeMemory_MemoryType_All = 0,
    NativeMemory_MemoryType_Internal = 1,
    NativeMemory_MemoryType_SpiRam = 2,
} NativeMemory_MemoryType;

typedef enum __nfpack Sleep_WakeupCause
{
    Sleep_WakeupCause_Undefined = 0,
    Sleep_WakeupCause_Ext0 = 2,
    Sleep_WakeupCause_Ext1 = 3,
    Sleep_WakeupCause_Timer = 4,
    Sleep_WakeupCause_TouchPad = 5,
    Sleep_WakeupCause_Ulp = 6,
    Sleep_WakeupCause_Gpio = 7,
    Sleep_WakeupCause_Uart = 8,
} Sleep_WakeupCause;

typedef enum __nfpack Sleep_WakeupGpioPin
{
    Sleep_WakeupGpioPin_None = 0,
    Sleep_WakeupGpioPin_Pin0 = 1,
    Sleep_WakeupGpioPin_Pin1 = 2,
    Sleep_WakeupGpioPin_Pin2 = 4,
    Sleep_WakeupGpioPin_Pin3 = 8,
    Sleep_WakeupGpioPin_Pin4 = 16,
    Sleep_WakeupGpioPin_Pin5 = 32,
    Sleep_WakeupGpioPin_Pin6 = 64,
    Sleep_WakeupGpioPin_Pin7 = 128,
    Sleep_WakeupGpioPin_Pin8 = 256,
    Sleep_WakeupGpioPin_Pin9 = 512,
    Sleep_WakeupGpioPin_Pin10 = 1024,
    Sleep_WakeupGpioPin_Pin11 = 2048,
    Sleep_WakeupGpioPin_Pin12 = 4096,
    Sleep_WakeupGpioPin_Pin13 = 8192,
    Sleep_WakeupGpioPin_Pin14 = 16384,
    Sleep_WakeupGpioPin_Pin15 = 32768,
    Sleep_WakeupGpioPin_Pin16 = 65536,
    Sleep_WakeupGpioPin_Pin17 = 131072,
    Sleep_WakeupGpioPin_Pin18 = 262144,
    Sleep_WakeupGpioPin_Pin19 = 524288,
    Sleep_WakeupGpioPin_Pin20 = 1048576,
    Sleep_WakeupGpioPin_Pin21 = 2097152,
    Sleep_WakeupGpioPin_Pin25 = 33554432,
    Sleep_WakeupGpioPin_Pin26 = 67108864,
    Sleep_WakeupGpioPin_Pin27 = 134217728,
    Sleep_WakeupGpioPin_Pin32 = 4294967296,
    Sleep_WakeupGpioPin_Pin33 = 8589934592,
    Sleep_WakeupGpioPin_Pin34 = 17179869184,
    Sleep_WakeupGpioPin_Pin35 = 34359738368,
    Sleep_WakeupGpioPin_Pin36 = 68719476736,
    Sleep_WakeupGpioPin_Pin37 = 137438953472,
    Sleep_WakeupGpioPin_Pin38 = 274877906944,
    Sleep_WakeupGpioPin_Pin39 = 549755813888,
} Sleep_WakeupGpioPin;

typedef enum __nfpack Sleep_WakeupMode
{
    Sleep_WakeupMode_AllLow = 0,
    Sleep_WakeupMode_AnyHigh = 1,
} Sleep_WakeupMode;

typedef enum __nfpack Sleep_WakeUpPort
{
    Sleep_WakeUpPort_COM1 = 0,
    Sleep_WakeUpPort_COM2 = 1,
} Sleep_WakeUpPort;

typedef enum __nfpack DenoiseCapacitance
{
    DenoiseCapacitance_Cap5pf = 0,
    DenoiseCapacitance_Cap6pf4 = 1,
    DenoiseCapacitance_Cap7pf8 = 2,
    DenoiseCapacitance_Cap9pf2 = 3,
    DenoiseCapacitance_Cap10pf6 = 4,
    DenoiseCapacitance_Cap12pf0 = 5,
    DenoiseCapacitance_Cap13pf4 = 6,
    DenoiseCapacitance_Cap14pf8 = 7,
} DenoiseCapacitance;

typedef enum __nfpack DenoiseRange
{
    DenoiseRange_Bit12 = 0,
    DenoiseRange_Bit10 = 1,
    DenoiseRange_Bit8 = 2,
    DenoiseRange_Bit4 = 3,
} DenoiseRange;

typedef enum __nfpack FilterSettingDebounce
{
    FilterSettingDebounce_NoDebounce = 0,
    FilterSettingDebounce_One = 1,
    FilterSettingDebounce_Two = 2,
    FilterSettingDebounce_Three = 3,
    FilterSettingDebounce_Four = 4,
    FilterSettingDebounce_Five = 5,
    FilterSettingDebounce_Six = 6,
    FilterSettingDebounce_Seven = 7,
} FilterSettingDebounce;

typedef enum __nfpack FilterSettingMode
{
    FilterSettingMode_Iir4 = 0,
    FilterSettingMode_Iir8 = 1,
    FilterSettingMode_Iir16 = 2,
    FilterSettingMode_Iir32 = 3,
    FilterSettingMode_Iir64 = 4,
    FilterSettingMode_Iir128 = 5,
    FilterSettingMode_Iir256 = 6,
    FilterSettingMode_IirJitter = 7,
} FilterSettingMode;

typedef enum __nfpack FilterSettingNoiseThreshold
{
    FilterSettingNoiseThreshold_Low = 0,
    FilterSettingNoiseThreshold_Mediumlow = 1,
    FilterSettingNoiseThreshold_MediumHigh = 2,
    FilterSettingNoiseThreshold_High = 3,
} FilterSettingNoiseThreshold;

typedef enum __nfpack FilterSettingSmoothMode
{
    FilterSettingSmoothMode_Off = 0,
    FilterSettingSmoothMode_Iir2 = 1,
    FilterSettingSmoothMode_Iir4 = 2,
    FilterSettingSmoothMode_Iir8 = 3,
} FilterSettingSmoothMode;

typedef enum __nfpack MeasurementMode
{
    MeasurementMode_Timer = 0,
    MeasurementMode_Software = 1,
} MeasurementMode;

typedef enum __nfpack TouchChargeSpeed_ChargeSpeed
{
    TouchChargeSpeed_ChargeSpeed_Zero = 0,
    TouchChargeSpeed_ChargeSpeed_Slowest = 1,
    TouchChargeSpeed_ChargeSpeed_Speed2 = 2,
    TouchChargeSpeed_ChargeSpeed_Speed3 = 3,
    TouchChargeSpeed_ChargeSpeed_Speed4 = 4,
    TouchChargeSpeed_ChargeSpeed_Speed5 = 5,
    TouchChargeSpeed_ChargeSpeed_Speed6 = 6,
    TouchChargeSpeed_ChargeSpeed_Fastest = 7,
} TouchChargeSpeed_ChargeSpeed;

typedef enum __nfpack TouchChargeSpeed_InitialCharge
{
    TouchChargeSpeed_InitialCharge_Low = 0,
    TouchChargeSpeed_InitialCharge_High = 1,
} TouchChargeSpeed_InitialCharge;

typedef enum __nfpack TouchHighVoltage
{
    TouchHighVoltage_Volt2V4 = 0,
    TouchHighVoltage_Volt2V5 = 1,
    TouchHighVoltage_Volt2V6 = 2,
    TouchHighVoltage_Volt2V7 = 3,
} TouchHighVoltage;

typedef enum __nfpack TouchHighVoltageAttenuation
{
    TouchHighVoltageAttenuation_Volt1V5 = 0,
    TouchHighVoltageAttenuation_Volt1V0 = 1,
    TouchHighVoltageAttenuation_Volt0V5 = 2,
    TouchHighVoltageAttenuation_Volt0V0 = 3,
} TouchHighVoltageAttenuation;

typedef enum __nfpack TouchLowVoltage
{
    TouchLowVoltage_Volt0V5 = 0,
    TouchLowVoltage_Volt0V6 = 1,
    TouchLowVoltage_Volt0V7 = 2,
    TouchLowVoltage_Volt0V8 = 3,
} TouchLowVoltage;

typedef enum __nfpack TouchTriggerMode
{
    TouchTriggerMode_BellowThreshold = 0,
    TouchTriggerMode_AboveThreshold = 1,
} TouchTriggerMode;

typedef enum __nfpack WakeUpSource
{
    WakeUpSource_BothSet1AndSet2 = 0,
    WakeUpSource_OnlySet1 = 1,
} WakeUpSource;

typedef enum __nfpack ValueTypes
{
    ValueTypes_Index = 0,
    ValueTypes_DeviceIndex = 256,
    ValueTypes_DeviceType = 65536,
} ValueTypes;

typedef enum __nfpack GpioPulsePolarity
{
    GpioPulsePolarity_Both = 0,
    GpioPulsePolarity_Falling = 1,
    GpioPulsePolarity_Rising = 2,
} GpioPulsePolarity;

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
    NANOCLR_NATIVE_DECLARE(NativeEnableWakeupByTouchPad___STATIC__nanoFrameworkHardwareEsp32EspNativeError__I4__I4__U1);
    NANOCLR_NATIVE_DECLARE(
        NativeEnableWakeupByUart___STATIC__nanoFrameworkHardwareEsp32EspNativeError__nanoFrameworkHardwareEsp32SleepWakeUpPort__I4);
    NANOCLR_NATIVE_DECLARE(NativeStartLightSleep___STATIC__nanoFrameworkHardwareEsp32EspNativeError);
    NANOCLR_NATIVE_DECLARE(NativeStartDeepSleep___STATIC__nanoFrameworkHardwareEsp32EspNativeError);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupCause___STATIC__nanoFrameworkHardwareEsp32SleepWakeupCause);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupGpioPin___STATIC__nanoFrameworkHardwareEsp32SleepWakeupGpioPin);
    NANOCLR_NATIVE_DECLARE(NativeGetWakeupTouchpad___STATIC__I4);

    //--//
};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_DenoiseSetting
{
    static const int FIELD___denoiseCapacitance = 1;
    static const int FIELD___denoiseRange = 2;

    //--//
};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_Esp32FilterSetting
{
    static const int FIELD___period = 1;

    //--//
};
/*
struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_MeasurementTime
{
    static const int FIELD__<SleepCycles>k__BackingField = 1;
    static const int FIELD__<MeasurementCycles>k__BackingField = 2;

    //--//
};
*/
struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_S2S3FilterSetting
{
    static const int FIELD___periodeSettingMode = 1;
    static const int FIELD___filterSettingDebounce = 2;
    static const int FIELD___filterSettingNoiseThreshold = 3;
    static const int FIELD___jitterSize = 4;
    static const int FIELD___filterSettingSmoothMode = 5;

    //--//
};
/*
struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchChargeSpeed
{
    static const int FIELD__<Speed>k__BackingField = 1;
    static const int FIELD__<Charge>k__BackingField = 2;

    //--//
};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPadEventArgs
{
    static const int FIELD__<PadNumber>k__BackingField = 3;
    static const int FIELD__<Touched>k__BackingField = 4;

    //--//
};
*/
struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad
{
    static const int FIELD_STATIC___touchHighVoltage = 1;
    static const int FIELD_STATIC___touchLowVoltage = 2;
    static const int FIELD_STATIC___touchHighVoltageAttenuation = 3;
    static const int FIELD_STATIC___isFilterOn = 4;
    static const int FIELD_STATIC___denoiseEnabled = 5;
    static const int FIELD_STATIC___touchPadEventHandler = 6;

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
    NANOCLR_NATIVE_DECLARE(
        NativeSetVoltage___STATIC__VOID__nanoFrameworkHardwareEsp32TouchTouchHighVoltage__nanoFrameworkHardwareEsp32TouchTouchLowVoltage__nanoFrameworkHardwareEsp32TouchTouchHighVoltageAttenuation);
    NANOCLR_NATIVE_DECLARE(
        NativeGetVoltage___STATIC__VOID__BYREF_nanoFrameworkHardwareEsp32TouchTouchHighVoltage__BYREF_nanoFrameworkHardwareEsp32TouchTouchLowVoltage__BYREF_nanoFrameworkHardwareEsp32TouchTouchHighVoltageAttenuation);
    NANOCLR_NATIVE_DECLARE(NativeStartFilter___STATIC__VOID__nanoFrameworkHardwareEsp32TouchIFilterSetting);
    NANOCLR_NATIVE_DECLARE(NativeStopFilter___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(NativeSetMeasurementTime___STATIC__VOID__U2__U2);
    NANOCLR_NATIVE_DECLARE(NativeGetMeasurementTime___STATIC__VOID__BYREF_U2__BYREF_U2);
    NANOCLR_NATIVE_DECLARE(NativeGetDenoise___STATIC__VOID__nanoFrameworkHardwareEsp32TouchDenoiseSetting);
    NANOCLR_NATIVE_DECLARE(NativeSetDenoise___STATIC__VOID__nanoFrameworkHardwareEsp32TouchDenoiseSetting);
    NANOCLR_NATIVE_DECLARE(NativeDenoiseEnabled___STATIC__VOID__BOOLEAN);

    //--//
};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPadEvent
{
    static const int FIELD__TouchPadNumber = 3;
    static const int FIELD__Touched = 4;

    //--//
};

struct Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPadEventHandler
{
    static const int FIELD___pinMap = 1;

    //--//
};

struct Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCount
{
    static const int FIELD__Count = 1;
    static const int FIELD__RelativeTime = 2;

    //--//
};

struct Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCounter
{
    static const int FIELD___pinNumberA = 1;
    static const int FIELD___pinNumberB = 2;
    static const int FIELD___polarity = 3;
    static const int FIELD___countActive = 4;
    static const int FIELD___filter = 5;
    static const int FIELD___syncLock = 6;
    static const int FIELD___disposedValue = 7;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeRead___SystemDeviceGpioGpioPulseCount__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeStart___VOID);
    NANOCLR_NATIVE_DECLARE(NativeStop___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32;

#endif // NANOFRAMEWORK_HARDWARE_ESP32_NATIVE_H
