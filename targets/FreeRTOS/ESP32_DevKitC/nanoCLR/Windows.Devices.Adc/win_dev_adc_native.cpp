//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
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
    Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeReadValue___I4__I4__I4,
    Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeDisposeChannel___VOID__I4,
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
    Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeOpenChannel___VOID__I4__I4,
    Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetChannelCount___I4,
    Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetMaxValue___I4,
    Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetMinValue___I4,
    Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeIsChannelModeSupported___BOOLEAN__I4,
    Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetResolutionInBits___I4,
    NULL,
    NULL,
    Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::GetDeviceSelector___STATIC__STRING,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Adc =
{
    "Windows.Devices.Adc", 
    0xA56A9A60,
    method_lookup
};

