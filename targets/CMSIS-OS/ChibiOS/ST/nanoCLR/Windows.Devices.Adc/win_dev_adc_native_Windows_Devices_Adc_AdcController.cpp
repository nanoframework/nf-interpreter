//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native_target.h"

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeOpenChannel___VOID__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Get channel from argument
        int channel = stack.Arg1().NumericByRef().s4;

        // get controller ID
        int controllerId = pThis[FIELD___controllerId].NumericByRef().s4;

        // we are filling this below with the appropriate ADC port pin config and ADC driver
        NF_PAL_ADC_PORT_PIN_CHANNEL adcDefinition;
        ADCDriver* adcDriver = NULL;

        // only one ADC controller for now, but check it anyways
        if(controllerId == 1)
        {
            adcDefinition = AdcPortPinConfig[channel];

            // we should remove form the build the ADC options that aren't implemented
            // plus we have to use the default to catch invalid ADC Ids
            switch(adcDefinition.adcIndex)
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

        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        if(adcDefinition.portId != NULL)
        {
            palSetGroupMode(adcDefinition.portId, PAL_PORT_BIT(adcDefinition.pin), 0, PAL_MODE_INPUT_ANALOG);
        }

        // start ADC
        adcStart(adcDriver, NULL);

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetChannelCount___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int channelCount;

        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        int controllerId = pThis[ FIELD___controllerId ].NumericByRefConst().s4;

        switch(controllerId)
        {
            case 1: 
                channelCount = AdcChannelCount;
                break;

            default: 
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        
        // Return value to the managed application
        stack.SetResult_I4(channelCount);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetMaxValue___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // Currently fixed at 12 bit so return 4095 = ((2^12) - 1)
        stack.SetResult_I4(4095);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetMinValue___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    stack.SetResult_I4(0);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeIsChannelModeSupported___BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int mode = stack.Arg1().NumericByRef().s4;
        
        // Only support Single ended mode for now
        stack.SetResult_Boolean( (mode == (int)AdcChannelMode::SingleEnded) ) ;
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetResolutionInBits___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // Fixed at 12 bit
        stack.SetResult_I4(12);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    int controllerId;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get pointer to _controllerId field in AdcController
    controllerId = pThis[FIELD___controllerId].NumericByRef().s4;

    // all required initialization for ADC is already handled in ChibiOS driver

    switch(controllerId)
    {
        case 1: 
            break;

        default: 
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String("ADC1");
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
