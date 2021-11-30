//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_adc_native_target.h>

// this has to be an array because ChibiOS ADC API expects that
static adcsample_t sampleBuffer[1 * 1];

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

// not used, just left here if needed for debugging purposes
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err)
{
    (void)adcp;
    (void)err;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeReadValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channelNumber;
    NF_PAL_ADC_PORT_PIN_CHANNEL adcDefinition;
    ADCConversionGroup adcgrpcfg1;
    bool enableVref;

    ADCDriver *adcDriver = NULL;

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
#if STM32_ADC_USE_ADC1
        case 1:
            adcDriver = &ADCD1;
            break;
#endif

#if STM32_ADC_USE_ADC2
        case 2:
            adcDriver = &ADCD2;
            break;
#endif

#if STM32_ADC_USE_ADC3
        case 3:
            adcDriver = &ADCD3;
            break;
#endif
        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    enableVref = (adcDefinition.adcChannel == ADC_CHANNEL_SENSOR) | (adcDefinition.adcChannel == ADC_CHANNEL_VREFINT) |
                 (adcDefinition.adcChannel == ADC_CHANNEL_VBAT);

    // need to enable VREF?
    if (enableVref)
    {
        adcSTM32EnableTSVREFE();

        osDelay(10);
    }

    // need to setup the conversion group parameters
    adcgrpcfg1 = {
        FALSE,
        1,
        NULL,
        NULL,                             // replace with adcerrorcallback if required for debug
        0,                                /* CR1 */
        ADC_CR2_SWSTART,                  /* CR2 */
        ADC_SMPR1_SMP_AN11(ADC_SAMPLE_3), /* SMPR1 */
        0,                                /* SMPR2 */
        0,                                /* HTR */
        0,                                /* LTR */
        0,                                /* SQR1 */
        0,                                /* SQR2 */
        ADC_SQR3_SQ1_N(adcDefinition.adcChannel)};

    // perform the conversion
    adcConvert(adcDriver, &adcgrpcfg1, sampleBuffer, 1);

    // need to turn off VREF?
    if (enableVref)
    {
        adcSTM32DisableTSVREFE();
    }

    // set the return result with the conversion value from the array
    stack.SetResult_I4(sampleBuffer[0]);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeDisposeChannel___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    // left empty on purpose, nothing to do here

    NANOCLR_NOCLEANUP_NOLABEL();
}
