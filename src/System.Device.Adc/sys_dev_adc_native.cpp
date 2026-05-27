#include "sys_dev_adc_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeReadValue___I4,
    Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeDisposeChannel___VOID,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeOpenChannel___VOID__I4,
    Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetChannelCount___I4,
    Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMaxValue___I4,
    Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMinValue___I4,
    Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeIsChannelModeSupported___BOOLEAN__I4,
    Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetResolutionInBits___I4,
    Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeInit___VOID,
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
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Adc =
{
    "System.Device.Adc",
    0xA2D36208,
    method_lookup,
    { 100, 2, 0, 0 }
};

// clang-format on
