//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_HARDWARE_STM32_NATIVE_H_
#define _NF_HARDWARE_STM32_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <hal.h>

struct Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power
{
    NANOCLR_NATIVE_DECLARE(get_WakeupReason___STATIC__nanoFrameworkHardwareStm32PowerWakeupReasonType);
    NANOCLR_NATIVE_DECLARE(DisableWakeupPin___STATIC__VOID__nanoFrameworkHardwareStm32PowerWakeupPin);
    NANOCLR_NATIVE_DECLARE(EnableWakeupPin___STATIC__VOID__nanoFrameworkHardwareStm32PowerWakeupPin);
    NANOCLR_NATIVE_DECLARE(NativeEnterStandbyMode___STATIC__VOID);

    //--//

};

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

struct Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_RTC
{
    NANOCLR_NATIVE_DECLARE(GetAlarm___STATIC__mscorlibSystemDateTime);
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

#endif  //_NF_HARDWARE_STM32_NATIVE_H_
