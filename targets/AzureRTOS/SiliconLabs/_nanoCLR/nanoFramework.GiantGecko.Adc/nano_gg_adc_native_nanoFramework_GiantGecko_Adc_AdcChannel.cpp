//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nano_gg_adc_native.h"
#include "nano_gg_adc_native_target.h"

// add typedef for AdcChannelConfiguration to ease access and improve readability
typedef Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcChannelConfiguration AdcChannelConfiguration;

void ParseAdcChannelConfig(CLR_RT_HeapBlock *channelConfiguration, void *adcInit)
{
    ADC_InitSingle_TypeDef channelInitSingle = ADC_INITSINGLE_DEFAULT;
    // ADC_InitScan_TypeDef channelInitScan = ADC_INITSCAN_DEFAULT;

    // now map the AdcChannelConfiguration object to the native structure

    // handle PRS differently because it's a C# enum and -1 is the disabled value
    // if PRS is disabled, use the default value
    int32_t prsSel = channelConfiguration[AdcChannelConfiguration::FIELD___prsSampleTrigger].NumericByRef().s4;
    channelInitSingle.prsSel = prsSel == -1 ? adcPRSSELCh0 : (ADC_PRSSEL_TypeDef)prsSel;

    channelInitSingle.acqTime =
        (ADC_AcqTime_TypeDef)channelConfiguration[AdcChannelConfiguration::FIELD___aquisitionTime].NumericByRef().s4;
    channelInitSingle.reference =
        (ADC_Ref_TypeDef)channelConfiguration[AdcChannelConfiguration::FIELD___referenceVoltage].NumericByRef().s4;
    channelInitSingle.resolution =
        (ADC_Res_TypeDef)channelConfiguration[AdcChannelConfiguration::FIELD___sampleResolution].NumericByRef().s4;
    channelInitSingle.diff =
        (AdcChannelMode)channelConfiguration[AdcChannelConfiguration::FIELD___channelMode].NumericByRef().s4 ==
                AdcChannelMode_Differential
            ? true
            : false;

    memcpy(adcInit, &channelInitSingle, sizeof(ADC_InitSingle_TypeDef));
}

HRESULT Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcChannel::NativeReadValue___I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t channelNumber;
    NF_PAL_ADC_PORT_PIN_CHANNEL adcDefinition;

    // ADC related variables
    ADC_TypeDef *adcDriver = NULL;
    ADC_InitSingle_TypeDef channelInitSingle;
    uint64_t samplesAccumulator = 0;
    int32_t averageCount;
    int32_t samplesCount;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get channel from _channelNumber field
    channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;

    // OK to skip validation here because the channelNumber has been validated in the constructor
    adcDefinition = AdcPortPinConfig[channelNumber];

    // we should remove form the build the ADC options that aren't implemented
    // plus we have to use the default to catch invalid ADC Ids
    switch (adcDefinition.adcIndex)
    {

#if GECKO_USE_ADC0
        case 0:
            adcDriver = ADC0;
            break;
#endif

#if GECKO_USE_ADC1
        case 1:
            adcDriver = ADC1;
            break;
#endif

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // get the average counter
    averageCount = stack.Arg1().NumericByRef().s4;

    // sanity check (need to take at least one sample)
    if (averageCount < 1)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    samplesCount = averageCount;

    // get a pointer to the channel AdcChannelConfiguration object instance
    ParseAdcChannelConfig(pThis[FIELD___adcChannelConfiguration].Dereference(), &channelInitSingle);

    // sanity check (only support single ended mode at this time)
    if (channelInitSingle.diff)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
    }

    // select positive input
    channelInitSingle.posSel = (ADC_PosSel_TypeDef)adcDefinition.posSel;

    ADC_InitSingle(adcDriver, &channelInitSingle);

    while (samplesCount--)
    {
        // start the conversion
        ADC_Start(adcDriver, adcStartSingle);

        // Wait for conversion to complete
        while (ADC0->STATUS & ADC_STATUS_SINGLEACT)
            ;

        // store sample
        samplesAccumulator += ADC_DataSingleGet(adcDriver);
    }

    // set the return result with the conversion value averaged from the accumulator
    stack.SetResult_I4(samplesAccumulator / averageCount);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcChannel::NativeDisposeChannel___VOID(
    CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    // left empty on purpose, nothing to do here

    NANOCLR_NOCLEANUP_NOLABEL();
}
