//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native_target.h"

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

extern const ADC_Config ADC_config[];

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeReadValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Get channel from _channelNumber field
        int channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;

        uint16_t adcValue0;
        uint32_t adcValue0MicroVolt;

        ADC_Params params;
        ADC_Params_init(&params);
        ADC_Handle adcHandler = ADC_open(channelNumber, &params);
        FAULT_ON_NULL(adcHandler);

        // sanity check
        if (adcHandler == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        int_fast16_t res = ADC_convert(adcHandler, &adcValue0);

        // close ADC
        ADC_close(adcHandler);

        if (res == ADC_STATUS_SUCCESS)
        {
            // get the raw value converted to micro volts
            adcValue0MicroVolt = ADC_convertToMicroVolts(adcHandler, adcValue0);

            // after the conversion need to get the value has if it was the raw one
            // the multiplier is 4096 (corresponding to 12bits)
            double rawValue =
                (adcValue0MicroVolt / (double)((ADCCC26XX_HWAttrs *)ADC_config[channelNumber].hwAttrs)->refVoltage) *
                4096;

            // load back to integer to return a proper value
            adcValue0 = rawValue;

            // set result with reading value
            stack.SetResult_I4(adcValue0);
        }
        else
        {
            // read operation failed
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeDisposeChannel___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    // left empty on purpose, nothing to do here

    NANOCLR_NOCLEANUP_NOLABEL();
}
