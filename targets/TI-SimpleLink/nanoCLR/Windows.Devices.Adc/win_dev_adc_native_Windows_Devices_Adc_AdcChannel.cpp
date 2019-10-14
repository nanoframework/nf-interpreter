//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native_target.h"

// // this has to be an array because ChibiOS ADC API expects that
// static adcsample_t sampleBuffer[1 * 1];

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

// // not used, just left here if needed for debugging purposes
// static void adcerrorcallback(ADCDriver *adcp, adcerror_t err)
// {
//   (void)adcp;
//   (void)err;
// }

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeReadValue___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Get channel from _channelNumber field
        int channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;

        uint16_t adcValue0;
        uint32_t adcValue0MicroVolt;

        ADC_Params params;
        ADC_Params_init(&params);
        ADC_Handle adcHandler = ADC_open(channelNumber, &params); FAULT_ON_NULL(adcHandler);

        int_fast16_t res = ADC_convert(adcHandler, &adcValue0);

        if (res == ADC_STATUS_SUCCESS) 
        {
            adcValue0MicroVolt = ADC_convertRawToMicroVolts(adcHandler, adcValue0);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        

        ADC_close(adcHandler);

        stack.SetResult_I4(adcValue0MicroVolt);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeDisposeChannel___VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    // left empty on purpose, nothing to do here

    NANOCLR_NOCLEANUP_NOLABEL();
}
