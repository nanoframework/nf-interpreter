//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_adc_native_target.h"

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeReadValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channelNumber;
    NF_PAL_ADC_PORT_PIN_CHANNEL adcDefinition;
    ADC_InitSingle_TypeDef adcInitSingle;

    // ADC related variables
    adcInitSingle = ADC_INITSINGLE_DEFAULT;
    uint32_t sample;
    ADC_TypeDef *adcDriver = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get channel from _channelNumber field
    channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;

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

    if (adcDefinition.posSel == adcPosSelAVDD)
    {
        // Ref is 5V to measure VDD
        adcInitSingle.reference = adcRef2V5;
    }
    else
    {
        // Ref is internal 2.5V for all the others, including the internal signals
        adcInitSingle.reference = adcRef2V5;
    }

    // single ended
    adcInitSingle.diff = false;
    // set acquisition time to meet minimum requirement
    adcInitSingle.acqTime = adcAcqTime4;
    // set pos
    adcInitSingle.posSel = adcDefinition.posSel;

    ADC_InitSingle(adcDriver, &adcInitSingle);

    // Start ADC conversion
    ADC_Start(adcDriver, adcStartSingle);

    // Wait for conversion to be complete
    while (!(ADC0->STATUS & _ADC_STATUS_SINGLEDV_MASK))
        ;

    // perform the conversion
    sample = ADC_DataSingleGet(adcDriver);

    // set the return result with the conversion value from the array
    stack.SetResult_I4(sample);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeDisposeChannel___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    // left empty on purpose, nothing to do here

    NANOCLR_NOCLEANUP_NOLABEL();
}
