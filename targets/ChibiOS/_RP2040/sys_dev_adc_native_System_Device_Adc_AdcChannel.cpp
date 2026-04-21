//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_adc_native_target.h>

// Single-element sample buffer for synchronous conversion
static adcsample_t sampleBuffer[1];

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeReadValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channelNumber;
    NF_PAL_ADC_PORT_PIN_CHANNEL adcDefinition;
    ADCConversionGroup adcgrpcfg;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;

    if (channelNumber < 0 || channelNumber >= AdcChannelCount)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    adcDefinition = AdcPortPinConfig[channelNumber];

    // Setup RP2040 ADC conversion group for single channel read
    adcgrpcfg = {
        FALSE,                       // circular
        1,                           // num_channels
        NULL,                        // end_cb
        NULL,                        // error_cb
        adcDefinition.adcChannel,    // channel
        0,                           // rrobin (0 = single channel)
        0,                           // div (0 = free-running, fastest)
        (adcDefinition.adcChannel == ADC_CHANNEL_TEMPSENSOR), // ts_enabled
    };

    // Enable temp sensor if needed
    if (adcDefinition.adcChannel == ADC_CHANNEL_TEMPSENSOR)
    {
        adcRPEnableTS(&ADCD1);
        osDelay(1);
    }

    // Perform synchronous conversion
    adcConvert(&ADCD1, &adcgrpcfg, sampleBuffer, 1);

    // Disable temp sensor if it was enabled
    if (adcDefinition.adcChannel == ADC_CHANNEL_TEMPSENSOR)
    {
        adcRPDisableTS(&ADCD1);
    }

    stack.SetResult_I4(sampleBuffer[0]);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeDisposeChannel___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP_NOLABEL();
}
