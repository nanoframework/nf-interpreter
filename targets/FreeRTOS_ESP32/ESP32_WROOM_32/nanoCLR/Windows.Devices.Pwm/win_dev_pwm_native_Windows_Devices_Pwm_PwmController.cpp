//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <targetPAL.h>
#include "win_dev_pwm_native.h"

uint32_t PwmController_Timer_resolution[8];

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_MaxFrequency___R8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // 40mhz with 1 bit duty cycle (50% fixed duty)
        double maxFrequency = 40000000.0;

        stack.SetResult_R8(maxFrequency);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
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
        // Returns number of pins for Pwm ( number of channels for Led Pwm)
        int pinCount = 16;

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

        // Retrieves the needed parameters from private class properties ( 0 - 7 )
        int timerId = (int)(pThis[ FIELD___controllerId ].NumericByRef().u4);
        
        unsigned int desiredFrequency = (unsigned int)stack.Arg1().NumericByRef().u4;

        ledc_timer_t timer = (ledc_timer_t)(timerId & 0x03);
        ledc_mode_t  mode = (timerId <= 4) ? LEDC_HIGH_SPEED_MODE : LEDC_LOW_SPEED_MODE;
        
        // Work out the optimumal duty resolution based on current frequency, default to 1 if not found
        // Working from 15 bit duty resolution down until we have a valid divisor
        int optimumDutyResolution = 1;
        for( int dutyResolution=15; dutyResolution>0; dutyResolution--)
        {
            uint32_t precision = ( 0x1 << dutyResolution );  // 2**depth
            
            uint64_t divParam = ( (uint64_t) LEDC_APB_CLK_HZ << 8 ) / desiredFrequency / precision;
            if ( divParam > 256) 
            {
                optimumDutyResolution = dutyResolution;
                break;
            }
        }

        ledc_timer_bit_t duty_res = (ledc_timer_bit_t)optimumDutyResolution;

        // Save resolution for working out values for percent duty cycle
        PwmController_Timer_resolution[timerId] = ( 0x1 << optimumDutyResolution );

        ledc_timer_config_t timer_conf {
            mode,
            duty_res,
            timer,
            desiredFrequency
        };

        esp_err_t result = ledc_timer_config( &timer_conf);
        if ( result != ESP_OK )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        stack.SetResult_R8((double)desiredFrequency);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
      // declare the device selector string
      // TIM0 to TIM3 are hish speed timers
      // TIM4 to TIM7 are low speed timers
       char deviceSelectorString[] = 
            "TIM0,TIM1,TIME2,TIM3,TIM4,TIM5,TIM6,TIM7";

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
