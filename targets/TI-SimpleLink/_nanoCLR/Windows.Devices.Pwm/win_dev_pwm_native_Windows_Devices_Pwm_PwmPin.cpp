//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_pwm_native_target.h"

////////////////////////////////////////////////////////////////////


PWM_Handle PwmPins[PWM_PIN_COUNT];


int GetChannel (int pin, int timerId)
{
    // TODO 
    return -1;
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeSetActiveDutyCyclePercentage___VOID__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties
        int pinNumber = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);

        // Retrieves the needed parameters from method parameters
        unsigned int dutyCycle = (unsigned int)stack.Arg1().NumericByRef().u4;

        PWM_setDuty(PwmPins[pinNumber], dutyCycle);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeSetPolarity___VOID__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeStart___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties
        int pinNumber = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);

        // get the controller
        CLR_RT_HeapBlock* controller = pThis[ FIELD___pwmController ].Dereference();

        // get the frequency from the controller
        double frequency = controller[ Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::FIELD___actualFrequency ].NumericByRef().r8;

        PWM_Params params;
        PWM_Params_init(&params);
        params.dutyUnits = PWM_DUTY_US;
        params.dutyValue = 0;
        params.periodUnits = PWM_PERIOD_HZ;
        params.periodValue = frequency;
        PwmPins[pinNumber] = PWM_open(pinNumber, &params);
        if (PwmPins[pinNumber] == NULL)
        {
            // Board_PWM did not open
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        PWM_start(PwmPins[pinNumber]);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeStop___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties
        int pinNumber = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);

        // Stops PWM output on the channel associated with this handler
        PWM_stop(PwmPins[pinNumber]);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties
        int pinNumber = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);

        PWM_close(PwmPins[pinNumber]);
    }
    NANOCLR_NOCLEANUP();
}
