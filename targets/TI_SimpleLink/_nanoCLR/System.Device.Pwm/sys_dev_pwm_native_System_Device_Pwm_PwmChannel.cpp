//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ti/drivers/PWM.h>
#include "sys_dev_pwm_native.h"
#include <target_windows_devices_pwm_config.h>

PWM_Handle PwmPins[PWM_PIN_COUNT];

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetDesiredFrequency___U4__U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // the SimpleLink PWM driver doesn't require any general setting
        // so there is nothing to do here, just keeping the ceremony for the managed API
        unsigned int desiredFrequency = (unsigned int)stack.Arg1().NumericByRef().u4;

        stack.SetResult_R8(desiredFrequency);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetActiveDutyCyclePercentage___VOID__U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties
        int pinNumber =
            (int)(pThis[Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::FIELD___pinNumber].NumericByRef().u4);

        // Retrieves the needed parameters from method parameters
        unsigned int dutyCycle = (unsigned int)stack.Arg1().NumericByRef().u4;

        PWM_setDuty(PwmPins[pinNumber], dutyCycle);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStart___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties
        int pinNumber =
            (int)(pThis[Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::FIELD___pinNumber].NumericByRef().u4);

        // get the controller
        CLR_RT_HeapBlock *controller = pThis[FIELD___pwmController].Dereference();

        // get the frequency from the controller
        double frequency =
            controller[Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::FIELD___frequency].NumericByRef().r8;

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

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStop___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties
        int pinNumber =
            (int)(pThis[Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::FIELD___pinNumber].NumericByRef().u4);

        // Stops PWM output on the channel associated with this handler
        PWM_stop(PwmPins[pinNumber]);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties
        int pinNumber =
            (int)(pThis[Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::FIELD___pinNumber].NumericByRef().u4);

        PWM_close(PwmPins[pinNumber]);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
