//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nano_gg_adc_native.h"
#include "nano_gg_adc_native_target.h"

#define ADC_IMPOSSIBLE_CLOCK 90000000UL

#if GECKO_USE_ADC0
ADC_Init_TypeDef adc0Init;
bool adc0Initialized = false;
#endif
#if GECKO_USE_ADC1
ADC_Init_TypeDef adc1Init;
bool adc1Initialized = false;
#endif

// add typedef for AdcConfiguration to ease access and improve readability
typedef Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcConfiguration AdcConfiguration;

HRESULT Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::NativeInit___VOID(
    CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    // all required initialization for ADC is handled in OpenChannel call

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::NativeOpenChannel___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channel;
    NF_PAL_ADC_PORT_PIN_CHANNEL adcChannelDefinition;
    CMU_Clock_TypeDef adcClock;
    ADC_TypeDef *adcDriver = NULL;
    ADC_Init_TypeDef *adcInit = NULL;
    bool *adcInitialized = NULL;

    CLR_RT_HeapBlock *adcConfiguration;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get channel from argument
    channel = stack.Arg1().NumericByRef().s4;

    // sanity check for existing configuration
    if (channel >= AdcChannelCount)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // grab ADC channel definition
    adcChannelDefinition = AdcPortPinConfig[channel];

    // we should remove from the build the ADC options that aren't implemented
    // plus we have to use the default to catch invalid ADC Ids
    switch (adcChannelDefinition.adcIndex)
    {

#if GECKO_USE_ADC0
        case 0:
            adcDriver = ADC0;
            adcClock = cmuClock_ADC0;
            adcInit = &adc0Init;
            adcInitialized = &adc0Initialized;
            break;
#endif

#if GECKO_USE_ADC1
        case 1:
            adcDriver = ADC1;
            adcClock = cmuClock_ADC1;
            adcInit = &adc1Init;
            adcInitialized = &adc1Initialized;
            break;
#endif

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // is this ADC already initialized?
    if (
#if GECKO_USE_ADC0
        (adcDriver == ADC0 && !adcInitialized)
#endif
#if GECKO_USE_ADC0 && GECKO_USE_ADC1
        ||
#endif
#if GECKO_USE_ADC1
        (adcDriver == ADC1 && !adcInitialized)
#endif
    )
    {
        // get a pointer to the managed AdcConfiguration object instance
        adcConfiguration = pThis[FIELD___acdConfiguration].Dereference();

        // init with default values
        *adcInit = ADC_INIT_DEFAULT;

        // now map the AdcConfiguration object to the native structure
        adcInit->ovsRateSel =
            (ADC_OvsRateSel_TypeDef)adcConfiguration[AdcConfiguration::FIELD___oversampleRate].NumericByRef().s4;

#if defined(_ADC_CTRL_LPFMODE_MASK)
        adcInit->lpfMode =
            (ADC_LPFilter_TypeDef)adcConfiguration[AdcConfiguration::FIELD___lowpassFilterMode].NumericByRef().s4;
#endif

        adcInit->warmUpMode =
            (ADC_Warmup_TypeDef)adcConfiguration[AdcConfiguration::FIELD___warmUpMode].NumericByRef().s4;
        adcInit->tailgate = (bool)adcConfiguration[AdcConfiguration::FIELD___tailgating].NumericByRef().u1;

        // Enable ADC clock
        CMU_ClockEnable(cmuClock_HFPER, true);
        CMU_ClockEnable(adcClock, true);

        // init to max possible ADC clock
        adcInit->prescale = ADC_PrescaleCalc(ADC_IMPOSSIBLE_CLOCK, 0);
        // let the drivers compute the appropriate value for the reference clock
        adcInit->timebase = ADC_TimebaseCalc(0);

#if defined(_ADC_CTRL_ADCCLKMODE_MASK)
        adcInit->em2ClockConfig = adcEm2ClockOnDemand;
#endif

        // start ADC
        ADC_Init(adcDriver, adcInit);

        // set ADC initialized flag
        *adcInitialized = true;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::NativeGetChannelCount___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::
    NativeIsChannelModeSupported___BOOLEAN__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::
    NativeGetSupportedResolutionsInBits___SZARRAY_nanoFrameworkGiantGeckoAdcSampleResolution(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::
    NativeStartContinuousConversion___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::
    NativeStoptContinuousConversion___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::
    NativeGetLastContinuousSamples___SZARRAY_I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_gg_adc_native_nanoFramework_GiantGecko_Adc_AdcController::
    NativeGetLastScanSampleForChannel___I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}
