//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native.h"


HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeReadValue___I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int value = 0;

        int deviceId = stack.Arg1().NumericByRef().s4;
        int channelNumber = stack.Arg2().NumericByRef().s4;

        // FIXME - add some implementaion
       
        stack.SetResult_I4(value);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeDisposeChannel___VOID__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP();
}
