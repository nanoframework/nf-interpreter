//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_stm32_native.h"
#include <hal.h>

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::get_WakeupReason___STATIC__nanoFrameworkHardwareStm32PowerWakeupReasonType,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::DisableWakeupPin___STATIC__VOID__nanoFrameworkHardwareStm32PowerWakeupPin,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::EnableWakeupPin___STATIC__VOID__nanoFrameworkHardwareStm32PowerWakeupPin,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::NativeEnterStandbyMode___STATIC__VOID,
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
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_BackupMemory::ReadBytes___STATIC__VOID__U4__SZARRAY_U1,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_BackupMemory::WriteBytes___STATIC__VOID__U4__SZARRAY_U1,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_BackupMemory::GetSize___STATIC__I4,
    NULL,
    NULL,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_RTC::GetAlarm___STATIC__mscorlibSystemDateTime,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_RTC::Native_RTC_SetAlarm___STATIC__VOID__U1__U1__U1__U1,
    NULL,
    NULL,
    NULL,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Utilities::NativeGetDeviceUniqueId___STATIC__VOID__SZARRAY_U1,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Utilities::NativeGetDeviceId___STATIC__U4,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Utilities::NativeGetDeviceRevisionId___STATIC__U4,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Stm32 =
{
    "nanoFramework.Hardware.Stm32", 
    0x024670A2,
    method_lookup,
    { 1, 0, 4, 3 }
};
