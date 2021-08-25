//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_pwm_native_target.h"

// PWMConfig _pwmConfig;

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_MaxFrequency___R8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    // FIXME : how can this value be determined ?
    stack.SetResult_R8(1.0);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_MinFrequency___R8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    // FIXME : how can this value be determined ?
    stack.SetResult_R8(1.0);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_PinCount___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    // TODO 
    // this is now hardcoded to TWO because this is the available PWM outputs in CC3220SF launchpad
    // ideally it should come from the PWM config file at target level
    stack.SetResult_I4(2);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::NativeSetDesiredFrequency___U4__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // the SimpleLink PWM driver doesn't require any general setting
        // so there is nothing to do here, just keeping the ceremony for the managed API
        unsigned int desiredFrequency = (unsigned int)stack.Arg1().NumericByRef().u4;

        stack.SetResult_R8(desiredFrequency);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // this is now hardcoded to ONE because there is only one PWM controller in CC3220SF launchpad
        // ideally it should come from the PWM config file at target level
        char deviceSelectorString[] = "TIM0";

        // because the caller is expecting a result to be returned
        // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
        stack.SetResult_String(deviceSelectorString);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
