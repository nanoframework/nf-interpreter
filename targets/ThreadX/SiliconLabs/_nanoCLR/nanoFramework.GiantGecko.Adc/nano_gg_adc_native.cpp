//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nano_gg_adc_native.h"

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
    Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcChannel::NativeReadValue___I4__I4,
    Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcChannel::NativeDisposeChannel___VOID,
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
    Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::NativeInit___VOID,
    Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::NativeOpenChannel___VOID__I4,
    Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::NativeGetChannelCount___I4,
    Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::NativeIsChannelModeSupported___BOOLEAN__I4,
    Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::NativeGetSupportedResolutionsInBits___SZARRAY_nanoFrameworkGiantGeckoAdcSampleResolution,
    Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::NativeStartContinuousConversion___VOID__SZARRAY_I4__I4,
    Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::NativeStopContinuousConversion___VOID,
    Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::NativeGetLastContinuousSamples___SZARRAY_I4,
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

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_GiantGecko_Adc =
{
    "nanoFramework.GiantGecko.Adc",
    0xD40FA747,
    method_lookup,
    { 100, 2, 0, 0 }
};

// clang-format on
