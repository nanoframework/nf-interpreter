//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native_target.h"

extern const uint_least8_t ADC_count;

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeOpenChannel___VOID__I4( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    // left empty on purpose, nothing to do here

    NANOCLR_NOCLEANUP_NOLABEL();
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
                channelCount = ADC_count;
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

    switch(controllerId)
    {
        case 1: 
            ADC_init();
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
