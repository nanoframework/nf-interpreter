//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_adc_native_target.h"

#define ADC_FREQ 16000000

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeOpenChannel___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channel;
    NF_PAL_ADC_PORT_PIN_CHANNEL adcDefinition;
    ADC_Init_TypeDef adcInit;
    CMU_Clock_TypeDef adcClock;
    ADC_TypeDef *adcDriver = NULL;

    adcInit = ADC_INIT_DEFAULT;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get channel from argument
    channel = stack.Arg1().NumericByRef().s4;

    adcDefinition = AdcPortPinConfig[channel];

    // we should remove from the build the ADC options that aren't implemented
    // plus we have to use the default to catch invalid ADC Ids
    switch (adcDefinition.adcIndex)
    {

#if GECKO_USE_ADC0
        case 0:
            adcDriver = ADC0;
            adcClock = cmuClock_ADC0;
            break;
#endif

#if GECKO_USE_ADC1
        case 1:
            adcDriver = ADC1;
            adcClock = cmuClock_ADC1;
            break;
#endif

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Enable ADC clock
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(adcClock, true);

    // Init to max ADC clock for Series 1
    adcInit.prescale = ADC_PrescaleCalc(ADC_FREQ, 0);
    adcInit.timebase = ADC_TimebaseCalc(0);

    // start ADC
    ADC_Init(adcDriver, &adcInit);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetChannelCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // Return value to the managed application
    stack.SetResult_I4(AdcChannelCount);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMaxValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // Currently fixed at 12 bit so return 4095 = ((2^12) - 1)
    stack.SetResult_I4(4095);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMinValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_I4(0);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeIsChannelModeSupported___BOOLEAN__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int mode = stack.Arg1().NumericByRef().s4;

    // Only support Single ended mode for now
    stack.SetResult_Boolean((mode == (int)AdcChannelMode_SingleEnded));

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetResolutionInBits___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // Fixed at 12 bit
    stack.SetResult_I4(12);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    // all required initialization for ADC is already handled in OpenChannel call

    NANOCLR_NOCLEANUP_NOLABEL();
}
