//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_HARDWARE_STM32_NATIVE_H
#define NF_HARDWARE_STM32_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <hal.h>

typedef enum __nfpack GpioConfiguration_IOMode
{
    GpioConfiguration_IOMode_Input = 0,
    GpioConfiguration_IOMode_Output = 1,
    GpioConfiguration_IOMode_AlternateFunction = 2,
    GpioConfiguration_IOMode_Analog = 3,
    GpioConfiguration_IOMode_OutputPushPull = 1,
    GpioConfiguration_IOMode_OutputOpenDrain = 17,
} GpioConfiguration_IOMode;

typedef enum __nfpack GpioConfiguration_PullUpDownActivation
{
    GpioConfiguration_PullUpDownActivation_Floating = 0,
    GpioConfiguration_PullUpDownActivation_PullUp = 1,
    GpioConfiguration_PullUpDownActivation_PullDown = 2,
} GpioConfiguration_PullUpDownActivation;

typedef enum __nfpack GpioConfiguration_Speed
{
    GpioConfiguration_Speed_Low = 0,
    GpioConfiguration_Speed_Medium = 1,
    GpioConfiguration_Speed_High = 2,
    GpioConfiguration_Speed_VeryHigh = 3,
} GpioConfiguration_Speed;

typedef enum __nfpack Power_WakeupPin
{
    Power_WakeupPin_Pin1 = 1,
    Power_WakeupPin_Pin2 = 2,
    Power_WakeupPin_Pin3 = 3,
} Power_WakeupPin;

typedef enum __nfpack Power_WakeupReasonType
{
    Power_WakeupReasonType_Undetermined = 0,
    Power_WakeupReasonType_FromStandby = 1,
    Power_WakeupReasonType_FromPin = 2,
} Power_WakeupReasonType;

struct Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_BackupMemory
{
    static const int FIELD_STATIC___size = 0;

    NANOCLR_NATIVE_DECLARE(ReadBytes___STATIC__VOID__U4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(WriteBytes___STATIC__VOID__U4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(GetSize___STATIC__I4);

    //--//

    // need to address the fact that the majority of the series have 32 bits wide registers and F1 has 16bits wide.
    // the backup size is calculated with the number of available backup registers and their size (in bytes)
    static const int BACKUP_SIZE = RTC_BKP_NUMBER * sizeof(RTC_BKP0R_Msk);
};

struct Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Configuration
{
    NANOCLR_NATIVE_DECLARE(ConfigurePin___STATIC__VOID__I4__nanoFrameworkHardwareStm32GpioConfiguration);
    NANOCLR_NATIVE_DECLARE(AddAdcChannel___STATIC__U4__U4__U4__U4);
    NANOCLR_NATIVE_DECLARE(RemoveAdcChannel___STATIC__VOID__U4);

    //--//
};

struct Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_GpioConfiguration
{
    static const int FIELD___mode = 1;
    static const int FIELD___pullUpDown = 2;
    static const int FIELD___speed = 3;
    static const int FIELD___alternateFunction = 4;

    //--//
};

struct Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power
{
    NANOCLR_NATIVE_DECLARE(get_WakeupReason___STATIC__nanoFrameworkHardwareStm32PowerWakeupReasonType);
    NANOCLR_NATIVE_DECLARE(DisableWakeupPin___STATIC__VOID__nanoFrameworkHardwareStm32PowerWakeupPin);
    NANOCLR_NATIVE_DECLARE(EnableWakeupPin___STATIC__VOID__nanoFrameworkHardwareStm32PowerWakeupPin);
    NANOCLR_NATIVE_DECLARE(NativeEnterStandbyMode___STATIC__VOID);

    //--//
};

struct Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_RTC
{
    NANOCLR_NATIVE_DECLARE(GetAlarm___STATIC__SystemDateTime);
    NANOCLR_NATIVE_DECLARE(Native_RTC_SetAlarm___STATIC__VOID__U1__U1__U1__U1);

    //--//
};

struct Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Utilities
{
    static const int FIELD_STATIC___deviceUniqueId = 1;
    static const int FIELD_STATIC___deviceId = 2;
    static const int FIELD_STATIC___deviceRevisionId = 3;

    NANOCLR_NATIVE_DECLARE(NativeGetDeviceUniqueId___STATIC__VOID__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeGetDeviceId___STATIC__U4);
    NANOCLR_NATIVE_DECLARE(NativeGetDeviceRevisionId___STATIC__U4);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Stm32;

#endif // NF_HARDWARE_STM32_NATIVE_H
