//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>
#include <targetPAL.h>
#include <swo.h>
#include "win_dev_pwm_native_target.h"

PWMConfig _pwmConfig;

extern PWMDriver* GetDriver(int timerId);

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_MaxFrequency___R8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties
        double maxFrequency = 0.0;

#if defined(STM32F4XX) || defined(STM32F7XX)
        int timerId = (int)(pThis[ FIELD___controllerId ].NumericByRef().u4);

        if (timerId == 1 || timerId >= 8) maxFrequency = (double)STM32_PCLK2_MAX;   // TIM1, TIM8 and TIM9 on APB2
        else maxFrequency = (double)STM32_PCLK1_MAX;        // other timers on APB1

#elif defined(STM32F0XX)

        maxFrequency = (double)STM32_PCLK_MAX;         // Only APB1 on this MCU

#endif
        stack.SetResult_R8(maxFrequency);

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_MinFrequency___R8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // FIXME : how can this value be determined ?
        stack.SetResult_R8(1.0);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_PinCount___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int pinCount = 0;
#if defined(STM32F4XX)
        pinCount = 42;
#elif defined(STM32F7XX)
        //FIXME: arbitrary value, here. Where do I find the information ?
        pinCount = 42;
#elif defined(STM32F0XX)
        pinCount = 24;
#endif
        stack.SetResult_I4(pinCount);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::NativeSetDesiredFrequency___U4__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties
        int timerId = (int)(pThis[ FIELD___controllerId ].NumericByRef().u4);
        unsigned int desiredFrequency = (unsigned int)stack.Arg1().NumericByRef().u4;

        // Gets the PWM driver associated with the requested timer
        PWMDriver * _drv;
        _drv = GetDriver(timerId);

        // Sets the period to something able to be precise enough with low and high frequencies
        // and that allows the clock frequency parameter to fit in an unsigned int
        int period = 1000;
        if (desiredFrequency >= 1000) period = 100;
        else if (desiredFrequency >= 1000000) period = 10;

        // Build the PWM config structure
        PWMConfig pwmConfig = 
        {
            (desiredFrequency * period),        // PWM clock frequency
            (pwmcnt_t)period,                   // PWM period
            NULL,                               // No callback
            // Enable all channels
            {
                {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                {PWM_OUTPUT_ACTIVE_HIGH, NULL}
            },
            0,
          #if STM32_PWM_USE_ADVANCED
            0,
          #endif            
            0,
        };

        // Starts the pwm driver
        pwmStop(_drv);
        osDelay(5);
        pwmStart(_drv, &pwmConfig);

        stack.SetResult_R8((double)desiredFrequency);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
    // declare the device selector string whose max size is "TIM1,TIM3,TIM4,TIM5,TIM8,TIM9," + terminator and init with the terminator
       char deviceSelectorString[ 30 + 1] = { 0 };

   #if STM32_PWM_USE_TIM1
       strcat(deviceSelectorString, "TIM1,");
   #endif
   #if STM32_PWM_USE_TIM2
       strcat(deviceSelectorString, "TIM2,");
   #endif
   #if STM32_PWM_USE_TIM3
       strcat(deviceSelectorString, "TIM3,");
   #endif
#ifndef STM32F0XX
   #if STM32_PWM_USE_TIM4
       strcat(deviceSelectorString, "TIM4,");
   #endif
   #if STM32_PWM_USE_TIM5
       strcat(deviceSelectorString, "TIM5,");
   #endif
   #if STM32_PWM_USE_TIM8
       strcat(deviceSelectorString, "TIM8,");
   #endif
   #if STM32_PWM_USE_TIM9
       strcat(deviceSelectorString, "TIM9,");
   #endif
#endif
       // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
