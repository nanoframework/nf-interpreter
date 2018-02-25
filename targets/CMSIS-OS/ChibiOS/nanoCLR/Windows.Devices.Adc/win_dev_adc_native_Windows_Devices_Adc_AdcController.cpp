//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native.h"


///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Adc.AdcChannelMode (in managed code) !!!    //
///////////////////////////////////////////////////////////////////////////////////////
enum AdcChannelMode
{
        SingleEnded = 0,
        Differential
};


HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeOpenChannel___VOID__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // Get passed ADC device Id and channel
        int deviceId = stack.Arg1().NumericByRef().s4;
        int channel = stack.Arg2().NumericByRef().s4;

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

     }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetChannelCount___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int channelCount;

        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Return number of single ended channels
        int deviceId = pThis[ FIELD___deviceId ].NumericByRefConst().s4;
        switch(deviceId)
        {
            case 1: channelCount = 8; break;
            default: NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
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
        // Currently fixed 12 bit so return 4095
        stack.SetResult_I4(4095);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetMinValue___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    // Return 0 for now, is this signed ?
    stack.SetResult_I4(0);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeIsChannelModeSupported___BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int mode = stack.Arg1().NumericByRef().s4;
        
        // Only support Single ended mode for now
        stack.SetResult_Boolean( (mode == (int)AdcChannelMode::SingleEnded) ) ;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetResolutionInBits___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // Fixed at 12 bit for now
        stack.SetResult_I4(12);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
   {
      // declare the device selector string
      // ADC1 and ADC2
       char deviceSelectorString[] = 
            "ADC1,ADC2";

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
    }
    NANOCLR_NOCLEANUP();
}
