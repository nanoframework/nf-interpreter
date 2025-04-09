//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANO_GG_ADC_NATIVE_H
#define NANO_GG_ADC_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack AdcChannelMode
{
    AdcChannelMode_SingleEnded = 0,
    AdcChannelMode_Differential = 1,
} AdcChannelMode;

typedef enum __nfpack AquisitionTime
{
    AquisitionTime__1Cyle = 1,
    AquisitionTime__2Cyles = 2,
    AquisitionTime__3Cyles = 3,
    AquisitionTime__4Cyles = 4,
    AquisitionTime__8Cyles = 5,
    AquisitionTime__16Cyles = 6,
    AquisitionTime__32Cyles = 7,
    AquisitionTime__64Cyles = 8,
    AquisitionTime__128Cyles = 9,
    AquisitionTime__256Cyles = 10,
} AquisitionTime;

typedef enum __nfpack LowpassFilterMode
{
    LowpassFilterMode_Bypass = 0,
    LowpassFilterMode_RcFilter = 1,
    LowpassFilterMode_DecouplingCapacitor = 2,
} LowpassFilterMode;

typedef enum __nfpack OversampleRate
{
    OversampleRate__2Samples = 0,
    OversampleRate__4Samples = 1,
    OversampleRate__8Samples = 2,
    OversampleRate__16Samples = 3,
    OversampleRate__32Samples = 4,
    OversampleRate__64Samples = 5,
    OversampleRate__128Samples = 6,
    OversampleRate__256Samples = 7,
    OversampleRate__512Samples = 8,
    OversampleRate__1024Samples = 9,
    OversampleRate__2048Samples = 10,
    OversampleRate__4096Samples = 11,
} OversampleRate;

typedef enum __nfpack PrsSampleTrigger
{
    PrsSampleTrigger_Disabled = -1,
    PrsSampleTrigger_PrsChannel0 = 0,
    PrsSampleTrigger_PrsChannel1 = 1,
    PrsSampleTrigger_PrsChannel2 = 2,
    PrsSampleTrigger_PrsChannel3 = 3,
    PrsSampleTrigger_PrsChannel4 = 4,
    PrsSampleTrigger_PrsChannel5 = 5,
    PrsSampleTrigger_PrsChannel6 = 6,
    PrsSampleTrigger_PrsChannel7 = 7,
    PrsSampleTrigger_PrsChannel8 = 8,
    PrsSampleTrigger_PrsChannel9 = 9,
    PrsSampleTrigger_PrsChannel10 = 10,
    PrsSampleTrigger_PrsChannel11 = 11,
} PrsSampleTrigger;

typedef enum __nfpack ReferenceVoltage
{
    ReferenceVoltage_Internal1_25V = 0,
    ReferenceVoltage_Internal2_5V = 1,
    ReferenceVoltage_BufferedVdd = 2,
    ReferenceVoltage_InternalDifferencial_5V = 3,
    ReferenceVoltage_SingleEndedExternalPin6 = 4,
    ReferenceVoltage_DiffExternalPin6And7 = 5,
    ReferenceVoltage_Unbuffered2Vdd = 6,
    ReferenceVoltage_InternalBandgap = 128,
    ReferenceVoltage_ScaledAvdd = 129,
    ReferenceVoltage_ScaledSingleEndedExternalPin6 = 130,
    ReferenceVoltage_RawSingleEndedExternalPin6 = 131,
    ReferenceVoltage_EntropyGeneration = 132,
    ReferenceVoltage_ScaledExternalPin6And7 = 133,
    ReferenceVoltage_RawExternalPin6And7 = 134,
} ReferenceVoltage;

typedef enum __nfpack SampleResolution
{
    SampleResolution__12bits = 0,
    SampleResolution__8bit = 1,
    SampleResolution__6bit = 2,
    SampleResolution_Oversampling = 3,
} SampleResolution;

typedef enum __nfpack SingleSampleInput
{
    SingleSampleInput_Channel0 = 0,
    SingleSampleInput_Channel1 = 1,
    SingleSampleInput_Channel2 = 2,
    SingleSampleInput_Channel3 = 3,
    SingleSampleInput_Channel4 = 4,
    SingleSampleInput_Channel5 = 5,
    SingleSampleInput_Channel6 = 6,
    SingleSampleInput_Channel7 = 7,
    SingleSampleInput_TemperatureReference = 8,
    SingleSampleInput_VddDiv3 = 9,
    SingleSampleInput_Vdd = 10,
    SingleSampleInput_Vss = 11,
    SingleSampleInput_VrefDiv2 = 12,
    SingleSampleInput_DacOutput0 = 13,
    SingleSampleInput_DacOutput1 = 14,
    SingleSampleInput_Atest = 15,
    SingleSampleInput_Positive0Negative1 = 16,
    SingleSampleInput_Positive2Negative3 = 17,
    SingleSampleInput_Positive4Negative5 = 18,
    SingleSampleInput_Positive6Negative7 = 19,
    SingleSampleInput_Differential0 = 20,
} SingleSampleInput;

typedef enum __nfpack WarmUpMode
{
    WarmUpMode_Normal = 0,
    WarmUpMode_WarmupFastBG = 1,
    WarmUpMode_KeepScanRefWarm = 2,
    WarmUpMode_KeepAdcWarm = 3,
} WarmUpMode;

struct Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcChannel
{
    static const int FIELD___syncLock = 2;
    static const int FIELD___disposed = 3;
    static const int FIELD___channelNumber = 4;
    static const int FIELD___adcChannelConfiguration = 5;

    NANOCLR_NATIVE_DECLARE(NativeReadValue___I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeDisposeChannel___VOID);

    //--//
};

struct Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcChannelConfiguration
{
    static const int FIELD___prsSampleTrigger = 1;
    static const int FIELD___aquisitionTime = 2;
    static const int FIELD___referenceVoltage = 3;
    static const int FIELD___sampleResolution = 4;
    static const int FIELD___channelMode = 5;

    //--//
};

struct Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcConfiguration
{
    static const int FIELD___oversampleRate = 1;
    static const int FIELD___lowpassFilterMode = 2;
    static const int FIELD___warmUpMode = 3;
    static const int FIELD___warnupTimeBase = 4;
    static const int FIELD___prescale = 5;
    static const int FIELD___tailgating = 6;

    //--//
};

struct Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___continuousSamplingStarted = 2;
    static const int FIELD___adcConfiguration = 3;
    static const int FIELD___adcChannelConfiguration = 4;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeOpenChannel___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetChannelCount___I4);
    NANOCLR_NATIVE_DECLARE(NativeIsChannelModeSupported___BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetSupportedResolutionsInBits___SZARRAY_nanoFrameworkGiantGeckoAdcSampleResolution);
    NANOCLR_NATIVE_DECLARE(NativeStartContinuousConversion___VOID__SZARRAY_I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeStopContinuousConversion___VOID);
    NANOCLR_NATIVE_DECLARE(NativeGetLastContinuousSamples___SZARRAY_I4);

    //--//
};

struct Library_nano_gg_adc_native_System_Device_Adc_AdcChannelBase
{
    static const int FIELD___adcController = 1;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_GiantGecko_Adc;

#endif // NANO_GG_ADC_NATIVE_H
