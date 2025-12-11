//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_stm32_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_BackupMemory::ReadBytes___STATIC__VOID__U4__SZARRAY_U1,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_BackupMemory::WriteBytes___STATIC__VOID__U4__SZARRAY_U1,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_BackupMemory::GetSize___STATIC__I4,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Configuration::ConfigurePin___STATIC__VOID__I4__nanoFrameworkHardwareStm32GpioConfiguration,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Configuration::AddAdcChannel___STATIC__U4__U4__U4__U4,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Configuration::RemoveAdcChannel___STATIC__VOID__U4,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::get_WakeupReason___STATIC__nanoFrameworkHardwareStm32PowerWakeupReasonType,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::DisableWakeupPin___STATIC__VOID__nanoFrameworkHardwareStm32PowerWakeupPin,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::EnableWakeupPin___STATIC__VOID__nanoFrameworkHardwareStm32PowerWakeupPin,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Power::NativeEnterStandbyMode___STATIC__VOID,
    nullptr,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_RTC::GetAlarm___STATIC__SystemDateTime,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_RTC::Native_RTC_SetAlarm___STATIC__VOID__U1__U1__U1__U1,
    nullptr,
    nullptr,
    nullptr,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Utilities::NativeGetDeviceUniqueId___STATIC__VOID__SZARRAY_U1,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Utilities::NativeGetDeviceId___STATIC__U4,
    Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Utilities::NativeGetDeviceRevisionId___STATIC__U4,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Stm32 =
{
    "nanoFramework.Hardware.Stm32",
    0x8D28F472,
    method_lookup,
    { 100, 2, 0, 0 }
};

// clang-format on
