//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native.h"

// this has to be an array because ChibiOS ADC API expects that
static adcsample_t sampleBuffer[1 * 1];

// not used, just left here if needed for debug
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err)
{

  (void)adcp;
  (void)err;
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeReadValue___I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int value = 0;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Get channel from argument
        int channelNumber = stack.Arg1().NumericByRef().s4;

        // need to get the deviceId for the ADC controller of this channel
        // get pointer to AdcController field
        CLR_RT_HeapBlock* adcController = pThis[FIELD___adcController].Dereference();

        // get pointer to _deviceId field
        int deviceId = adcController[Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::FIELD___deviceId].NumericByRef().s4;

        // we are filling this bellow with the appropriate ADC port pin config
        NF_PAL_ADC_PORT_PIN_CHANNEL channelDef;
        ADCDriver* adc;

        // we should remove form the build the ADC options that aren't implemented
        // plus we have to use the default to catch invalid ADC Ids
        switch(deviceId)
        {

   #if STM32_ADC_USE_ADC1
            case 1: 
                channelDef = Adc1PortPinConfig[channelNumber];
                adc = &ADCD1;
                break;
   #endif

   #if STM32_ADC_USE_ADC2
            case 2: 
                channelDef = Adc2PortPinConfig[channelNumber];
                adc = &ADCD2;
                break;
   #endif

   #if STM32_ADC_USE_ADC3
            case 3: 
                channelDef = Adc3PortPinConfig[channelNumber];
                adc = &ADCD3;
                break;
   #endif

            default: 
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        bool enableVref = channelDef.adcChannel == ADC_CHANNEL_SENSOR  \
                        | channelDef.adcChannel == ADC_CHANNEL_VREFINT \
                        | channelDef.adcChannel == ADC_CHANNEL_VBAT;
        // need to enable VREF?
        if(enableVref)
        {
            adcSTM32EnableTSVREFE();
        }

        // need to setup the conversion group parameters
        ADCConversionGroup adcgrpcfg1 = {
            FALSE,
            1,
            NULL,
            NULL,  // replace with adcerrorcallback if required for debug
            0,                                      /* CR1 */
            ADC_CR2_SWSTART,                        /* CR2 */
            ADC_SMPR1_SMP_AN11(ADC_SAMPLE_3),       /* SMPR1 */
            0,                                      /* SMPR2 */
            0,                                      /* SQR1 */
            0,                                      /* SQR2 */
            ADC_SQR3_SQ1_N(channelDef.adcChannel)
        };

        // perform the conversion
        adcConvert(adc, &adcgrpcfg1, sampleBuffer, 1);
       
        // need to turn off VREF?
        if(enableVref)
        {
            adcSTM32DisableTSVREFE();
        }

        // set the return result with the conversion value from the array
        stack.SetResult_I4(sampleBuffer[0]);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeDisposeChannel___VOID__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP();
}
