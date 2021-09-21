//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetPAL.h>
#include <sys_dev_pwm_native.h>

int GetChannelPwm(int pin, int timerId)
{
    int channel = -1;
#if defined(STM32F427xx) || defined(STM32F429xx) || defined(STM32F439xx) || defined(STM32F469xx) || defined(STM32F479xx)
    switch (timerId)
    {
        case 1:
            switch (pin)
            {
                case 8:          // PA8
                case 4 * 16 + 9: // PE9
                    channel = 0;
                    break;
                case 9:           // PA9
                case 4 * 16 + 11: // PE11
                    channel = 1;
                    break;
                case 10:          // PA10
                case 4 * 16 + 13: // PE13
                    channel = 2;
                    break;
                case 11:          // PA11
                case 4 * 16 + 14: // PE14
                    channel = 3;
                    break;
            }
            break;

        case 2:
            switch (pin)
            {
                case 0: // PA0, PA5, PA15
                case 5:
                case 15:
                    channel = 0;
                    break;
                case 1: // PA1, PB3
                case 1 * 16 + 3:
                    channel = 1;
                    break;
                case 2: // PA2, PB10
                case 1 * 16 + 10:
                    channel = 2;
                    break;
                case 3: // PA3, PB11
                case 1 * 16 + 11:
                    channel = 3;
                    break;
            }
            break;

        case 3:
            switch (pin)
            {
                case 0:
                case 5:
                case 15:
                    channel = 0;
                    break;
                case 1:
                case 1 * 16 + 3:
                    channel = 1;
                    break;
                case 2:
                case 1 * 16 + 10:
                    channel = 2;
                    break;
                case 3:
                case 1 * 16 + 11:
                    channel = 3;
                    break;
            }
            break;
        case 4:
            switch (pin)
            {
                case 1 * 16 + 6: // PB6, PD12
                case 3 * 16 + 12:
                    channel = 0;
                    break;
                case 1 * 16 + 7: // PB7, PD13
                case 3 * 16 + 13:
                    channel = 1;
                    break;
                case 1 * 16 + 8: // PB8, PD14
                case 3 * 16 + 14:
                    channel = 2;
                    break;
                case 1 * 16 + 9: // PB9, PD15
                case 3 * 16 + 15:
                    channel = 3;
                    break;
            }
            break;

        case 5:
            switch (pin)
            {
                case 0: // PA0, PH10
                case 7 * 16 + 10:
                    channel = 0;
                    break;
                case 1: // PA1, PH11
                case 7 * 16 + 11:
                    channel = 1;
                    break;
                case 2: // PA2, PH12
                case 7 * 16 + 12:
                    channel = 2;
                    break;
                case 3: // PA3, PI0
                case 8 * 16 + 0:
                    channel = 3;
                    break;
            }
            break;

        case 8:
            switch (pin)
            {
                case 2 * 16 + 6: // PC6, PI5
                case 7 * 16 + 5:
                    channel = 0;
                    break;
                case 2 * 16 + 7: // PC7, PI6
                case 7 * 16 + 6:
                    channel = 1;
                    break;
                case 2 * 16 + 8: // PC8, PI7
                case 7 * 16 + 7:
                    channel = 2;
                    break;
                case 2 * 16 + 9: // PC9
                    channel = 3;
                    break;
            }
            break;

        case 9:
            switch (pin)
            {
                case 2 * 16 + 6: // PC6, PI5
                case 7 * 16 + 5:
                    channel = 0;
                    break;
                case 2 * 16 + 7: // PC7, PI6
                case 7 * 16 + 6:
                    channel = 1;
                    break;
                case 2 * 16 + 8: // PC8, PI7
                case 7 * 16 + 7:
                    channel = 2;
                    break;
                case 2 * 16 + 9: // PC9
                    channel = 3;
                    break;
            }
            break;

        default:
            channel = -1;
            break;
    }
#endif
#if defined(STM32F411xx) || defined(STM32F412xx) || defined(STM32F401xx)
    switch (timerId)
    {
        case 1:
            switch (pin)
            {
                case 8: // PA8
                    channel = 0;
                    break;
                case 9: // PA9
                    channel = 1;
                    break;
                case 10: // PA10
                    channel = 2;
                    break;
                case 11: // PA11
                    channel = 3;
                    break;
            }
            break;

        case 2:
            switch (pin)
            {
                case 0: // PA0, PA5, PA15
                case 5:
                case 15:
                    channel = 0;
                    break;
                case 1: // PA1, PB3
                case 1 * 16 + 3:
                    channel = 1;
                    break;
                case 2: // PA2, PB10
                case 1 * 16 + 10:
                    channel = 2;
                    break;
                case 3: // PA3
                    channel = 3;
                    break;
            }
            break;
        case 3:
            switch (pin)
            {
                case 6: // PA6, PB4
                case 1 * 16 + 4:
                    channel = 0;
                    break;
                case 7: // PA7, PB5
                case 1 * 16 + 5:
                    channel = 1;
                    break;
                case 1 * 16 + 0: // PB0
                    channel = 2;
                    break;
                case 1 * 16 + 1: // PB1
                    channel = 3;
                    break;
            }
            break;
        case 4:
            switch (pin)
            {
                case 1 * 16 + 6: // PB6
                    channel = 0;
                    break;
                case 1 * 16 + 7: // PB7
                    channel = 1;
                    break;
                case 1 * 16 + 8: // PB8
                    channel = 2;
                    break;
                case 1 * 16 + 9: // PB9
                    channel = 3;
                    break;
            }
            break;

        case 5:
            switch (pin)
            {
                case 0: // PA0
                    channel = 0;
                    break;
                case 1: // PA1
                    channel = 1;
                    break;
                case 2: // PA2
                    channel = 2;
                    break;
                case 3: // PA3
                    channel = 3;
                    break;
            }
            break;

        case 9:
            switch (pin)
            {
                case 2: // PA2
                    channel = 0;
                    break;
                case 3: // PA3
                    channel = 1;
                    break;
            }
            break;

        default:
            channel = -1;
            break;
    }
#endif
#if defined(STM32F746xx)
    switch (timerId)
    {
        case 1:
            switch (pin)
            {
                case 8: // PA8, PE9
                case 4 * 16 + 9:
                    channel = 0;
                    break;
                case 9: // PA9, PE11
                case 4 * 16 + 11:
                    channel = 1;
                    break;
                case 10: // PA10, PE13
                case 4 * 16 + 13:
                    channel = 2;
                    break;
                case 11: // PA11, PE14
                case 4 * 16 + 14:
                    channel = 3;
                    break;
            }
            break;

        case 2:
            switch (pin)
            {
                case 0: // PA0, PA5, PA15
                case 5:
                case 15:
                    channel = 0;
                    break;
                case 1: // PA1, PB3
                case 1 * 16 + 3:
                    channel = 1;
                    break;
                case 2: // PA2, PB10
                case 1 * 16 + 10:
                    channel = 2;
                    break;
                case 3: // PA3, PB11
                case 1 * 16 + 11:
                    channel = 3;
                    break;
            }
            break;
        case 3:
            switch (pin)
            {
                case 6: // PA6, PB4, PC6
                case 1 * 16 + 4:
                case 2 * 16 + 6:
                    channel = 0;
                    break;
                case 7: // PA7, PB5, PC7
                case 1 * 16 + 5:
                case 2 * 16 + 7:
                    channel = 1;
                    break;
                case 1 * 16 + 0: // PB0, PC8
                case 2 * 16 + 8:
                    channel = 2;
                    break;
                case 1 * 16 + 1: // PB1, PC9
                case 2 * 16 + 9:
                    channel = 3;
                    break;
            }
            break;
        case 4:
            switch (pin)
            {
                case 1 * 16 + 6: // PB6, PD12
                case 3 * 16 + 12:
                    channel = 0;
                    break;
                case 1 * 16 + 7: // PB7, PD13
                case 3 * 16 + 13:
                    channel = 1;
                    break;
                case 1 * 16 + 8: // PB8, PD14
                case 3 * 16 + 14:
                    channel = 2;
                    break;
                case 1 * 16 + 9: // PB9, PD15
                case 3 * 16 + 15:
                    channel = 3;
                    break;
            }
            break;

        case 5:
            switch (pin)
            {
                case 0: // PA0
                    channel = 0;
                    break;
                case 1: // PA1
                    channel = 1;
                    break;
                case 2: // PA2
                    channel = 2;
                    break;
                case 3: // PA3
                    channel = 3;
                    break;
            }
            break;

        case 8:
            switch (pin)
            {
                case 2 * 16 + 6: // PC6
                    channel = 0;
                    break;
                case 2 * 16 + 7: // PC7
                    channel = 1;
                    break;
                case 2 * 16 + 8: // PC8
                    channel = 2;
                    break;
                case 2 * 16 + 9: // PC9
                    channel = 3;
                    break;
            }
            break;

        case 9:
            switch (pin)
            {
                case 2: // PA2, PE5
                case 4 * 16 + 5:
                    channel = 0;
                    break;
                case 3: // PA3, PE6
                case 4 * 16 + 6:
                    channel = 1;
                    break;
            }
            break;

        default:
            channel = -1;
            break;
    }
#endif
#if defined(STM32F769xx) || defined(STM32F765xx)
    switch (timerId)
    {
        case 1:
            switch (pin)
            {
                case 8: // PA8, PE9
                case 4 * 16 + 9:
                    channel = 0;
                    break;
                case 9: // PA9, PE11
                case 4 * 16 + 11:
                    channel = 1;
                    break;
                case 10: // PA10, PE13
                case 4 * 16 + 13:
                    channel = 2;
                    break;
                case 11: // PA11, PE14
                case 4 * 16 + 14:
                    channel = 3;
                    break;
            }
            break;

        case 2:
            switch (pin)
            {
                case 0: // PA0, PA5, PA15
                case 5:
                case 15:
                    channel = 0;
                    break;
                case 1: // PA1, PB3
                case 1 * 16 + 3:
                    channel = 1;
                    break;
                case 2: // PA2, PB10
                case 1 * 16 + 10:
                    channel = 2;
                    break;
                case 3: // PA3, PB11
                case 1 * 16 + 11:
                    channel = 3;
                    break;
            }
            break;
        case 3:
            switch (pin)
            {
                case 6: // PA6, PB4, PC6
                case 1 * 16 + 4:
                case 2 * 16 + 6:
                    channel = 0;
                    break;
                case 7: // PA7, PB5, PC7
                case 1 * 16 + 5:
                case 2 * 16 + 7:
                    channel = 1;
                    break;
                case 1 * 16 + 0: // PB0, PC8
                case 2 * 16 + 8:
                    channel = 2;
                    break;
                case 1 * 16 + 1: // PB1, PC9
                case 2 * 16 + 9:
                    channel = 3;
                    break;
            }
            break;
        case 4:
            switch (pin)
            {
                case 1 * 16 + 6: // PB6, PD12
                case 3 * 16 + 12:
                    channel = 0;
                    break;
                case 1 * 16 + 7: // PB7, PD13
                case 3 * 16 + 13:
                    channel = 1;
                    break;
                case 1 * 16 + 8: // PB8, PD14
                case 3 * 16 + 14:
                    channel = 2;
                    break;
                case 1 * 16 + 9: // PB9, PD15
                case 3 * 16 + 15:
                    channel = 3;
                    break;
            }
            break;

        case 5:
            switch (pin)
            {
                case 0: // PA0, PH10
                case 7 * 16 + 10:
                    channel = 0;
                    break;
                case 1: // PA1, PH11
                case 7 * 16 + 11:
                    channel = 1;
                    break;
                case 2: // PA2, PH12
                case 7 * 16 + 12:
                    channel = 2;
                    break;
                case 3: // PA3, PI0
                case 8 * 16 + 0:
                    channel = 3;
                    break;
            }
            break;

        case 8:
            switch (pin)
            {
                case 2 * 16 + 6: // PC6, PI5
                case 8 * 16 + 5:
                    channel = 0;
                    break;
                case 2 * 16 + 7: // PC7, PI6
                case 8 * 16 + 6:
                    channel = 1;
                    break;
                case 2 * 16 + 8: // PC8, PI7
                case 8 * 16 + 7:
                    channel = 2;
                    break;
                case 2 * 16 + 9: // PC9, PI2
                case 8 * 16 + 2:
                    channel = 3;
                    break;
            }
            break;

        case 9:
            switch (pin)
            {
                case 2: // PA2, PE5
                case 4 * 16 + 5:
                    channel = 0;
                    break;
                case 3: // PA3, PE6
                case 4 * 16 + 6:
                    channel = 1;
                    break;
            }
            break;

        default:
            channel = -1;
            break;
    }
#endif
#if defined(STM32F091xC)
    switch (timerId)
    {
        case 1:
            switch (pin)
            {
                case 8: // PA8
                    channel = 0;
                    break;
                case 9: // PA9
                    channel = 1;
                    break;
                case 10: // PA10
                    channel = 2;
                    break;
                case 11: // PA11
                    channel = 3;
                    break;
            }
            break;

        case 2:
            switch (pin)
            {
                case 0: // PA0, PA5, PA15
                case 5:
                case 15:
                    channel = 0;
                    break;
                case 1: // PA1, PB3
                case 1 * 16 + 3:
                    channel = 1;
                    break;
                case 2: // PA2, PB10
                case 1 * 16 + 10:
                    channel = 2;
                    break;
                case 3: // PA3, PB11
                case 1 * 16 + 11:
                    channel = 3;
                    break;
            }
            break;
        case 3:
            switch (pin)
            {
                case 6: // PA6, PB4, PC6
                case 1 * 16 + 4:
                case 2 * 16 + 6:
                    channel = 0;
                    break;
                case 7: // PA7, PB5, PC7
                case 1 * 16 + 5:
                case 2 * 16 + 7:
                    channel = 1;
                    break;
                case 1 * 16 + 0: // PB0, PC8
                case 2 * 16 + 8:
                    channel = 2;
                    break;
                case 1 * 16 + 1: // PB1, PC9
                case 2 * 16 + 9:
                    channel = 3;
                    break;
            }
            break;
        default:
            channel = -1;
            break;
    }
#endif
#if defined(STM32F407xx) || defined(STM32F405xx)
    switch (timerId)
    {
        case 1:
            switch (pin)
            {
                case 8:          // PA8
                case 4 * 16 + 9: // PE9
                    channel = 0;
                    break;
                case 9:           // PA9
                case 4 * 16 + 11: // PE11
                    channel = 1;
                    break;
                case 10:          // PA10
                case 4 * 16 + 13: // PE13
                    channel = 2;
                    break;
                case 11:          // PA11
                case 4 * 16 + 14: // PE14
                    channel = 3;
                    break;
            }
            break;

        case 2:
            switch (pin)
            {
                case 0: // PA0, PA5, PA15
                case 5:
                case 15:
                    channel = 0;
                    break;
                case 1: // PA1, PB3
                case 1 * 16 + 3:
                    channel = 1;
                    break;
                case 2: // PA2, PB10
                case 1 * 16 + 10:
                    channel = 2;
                    break;
                case 3: // PA3, PB11
                case 1 * 16 + 11:
                    channel = 3;
                    break;
            }
            break;

        case 3:
            switch (pin)
            {
                case 6:
                case 1 * 16 + 4:
                case 2 * 16 + 6:
                    channel = 0;
                    break;
                case 7:
                case 1 * 16 + 5:
                case 2 * 16 + 7:
                    channel = 1;
                    break;
                case 1 * 16 + 0:
                case 2 * 16 + 8:
                    channel = 2;
                    break;
                case 1 * 16 + 1:
                case 2 * 16 + 9:
                    channel = 3;
                    break;
            }
            break;
        case 4:
            switch (pin)
            {
                case 1 * 16 + 6: // PB6, PD12
                case 3 * 16 + 12:
                    channel = 0;
                    break;
                case 1 * 16 + 7: // PB7, PD13
                case 3 * 16 + 13:
                    channel = 1;
                    break;
                case 1 * 16 + 8: // PB8, PD14
                case 3 * 16 + 14:
                    channel = 2;
                    break;
                case 1 * 16 + 9: // PB9, PD15
                case 3 * 16 + 15:
                    channel = 3;
                    break;
            }
            break;

        case 5:
            switch (pin)
            {
                case 0: // PA0
                    channel = 0;
                    break;
                case 1: // PA1
                    channel = 1;
                    break;
                case 2: // PA2
                    channel = 2;
                    break;
                case 3: // PA3
                    channel = 3;
                    break;
            }
            break;

        case 8:
            switch (pin)
            {
                case 2 * 16 + 6: // PC6
                    channel = 0;
                    break;
                case 2 * 16 + 7: // PC7
                    channel = 1;
                    break;
                case 2 * 16 + 8: // PC8
                    channel = 2;
                    break;
                case 2 * 16 + 9: // PC9
                    channel = 3;
                    break;
            }
            break;

        case 9:
            switch (pin)
            {
                case 2: // PA2, PE5
                case 4 * 16 + 5:
                    channel = 0;
                    break;
                case 3: // PA3, PE6
                case 4 * 16 + 6:
                    channel = 1;
                    break;
            }
            break;

        default:
            channel = -1;
            break;
    }
#endif
    return channel;
}

int GetAlternateFunctionPwm(int timerId)
{
    if (timerId >= 8)
    {
        return 3;
    }

    if (timerId >= 3)
    {
        return 2;
    }

    return 1;
}

PWMDriver *GetDriverPwm(int timerId)
{
    PWMDriver *pwmDriver = NULL;

    switch (timerId)
    {

#if STM32_PWM_USE_TIM1
        case 1:
            pwmDriver = &PWMD1;
            break;
#endif

#if STM32_PWM_USE_TIM2
        case 2:
            pwmDriver = &PWMD2;
            break;
#endif

#if STM32_PWM_USE_TIM3
        case 3:
            pwmDriver = &PWMD3;
            break;
#endif
#if STM32_PWM_USE_TIM4
        case 4:
            pwmDriver = &PWMD4;
            break;
#endif

#if STM32_PWM_USE_TIM5
        case 5:
            pwmDriver = &PWMD5;
            break;
#endif

#if STM32_PWM_USE_TIM8
        case 8:
            pwmDriver = &PWMD8;
            break;
#endif

#if STM32_PWM_USE_TIM9
        case 9:
            pwmDriver = &PWMD9;
            break;
#endif
    }

    return pwmDriver;
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetDesiredFrequency___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    uint32_t timerId;
    int32_t desiredFrequency;
    uint32_t period;

    PWMConfig pwmConfig;
    PWMDriver *pwmDriver;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    desiredFrequency = stack.Arg1().NumericByRef().s4;

    // parameter check
    if (desiredFrequency < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Gets the PWM driver associated with the requested timer
    pwmDriver = GetDriverPwm(timerId);

    // Sets the period to something able to be precise enough with low and high frequencies
    // and that allows the clock frequency parameter to fit in an unsigned int
    period = 1000;
    if (desiredFrequency >= 1000)
    {
        period = 100;
    }
    else if (desiredFrequency >= 1000000)
    {
        period = 10;
    }

    // Build the PWM config structure
    pwmConfig = {
        // PWM clock frequency
        (desiredFrequency * period),
        // PWM period
        (pwmcnt_t)period,
        // No callback
        NULL,
        // Enable all channels
        {{PWM_OUTPUT_ACTIVE_HIGH, NULL},
         {PWM_OUTPUT_ACTIVE_HIGH, NULL},
         {PWM_OUTPUT_ACTIVE_HIGH, NULL},
         {PWM_OUTPUT_ACTIVE_HIGH, NULL}},
        0,
#if STM32_PWM_USE_ADVANCED
        0,
#endif
        0,
    };

    // Starts the PWM driver
    pwmStop(pwmDriver);
    osDelay(5);
    pwmStart(pwmDriver, &pwmConfig);

    // store the frequency
    pThis[FIELD___frequency].NumericByRef().s4 = desiredFrequency;

    stack.SetResult_R8((double)desiredFrequency);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetActiveDutyCyclePercentage___VOID__R8(
    CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t channelId;
    uint32_t dutyCycle;

    PWMDriver *pwmDriver;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties or method parameters
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    channelId = pThis[FIELD___channelNumber].NumericByRef().s4;

    // parameter check
    if (stack.Arg1().NumericByRef().r8 < 0 || stack.Arg1().NumericByRef().r8 > 1.0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    dutyCycle = (uint32_t)(stack.Arg1().NumericByRef().r8 * CONST_DutyCycleFactor);

    // Gets the PWM driver associated with the requested timer
    pwmDriver = GetDriverPwm(timerId);

    // Enables the channel associated with the selected pin on that timer
    pwmEnableChannel(pwmDriver, channelId, PWM_PERCENTAGE_TO_WIDTH(pwmDriver, dutyCycle));

    // store the new duty cycle
    pThis[FIELD___dutyCycle].NumericByRef().u4 = dutyCycle;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStart___VOID(CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t pinNumber;
    uint32_t dutyCycle;
    int32_t channelId;

    PWMDriver *pwmDriver;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties or method parameters
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;
    dutyCycle = pThis[FIELD___dutyCycle].NumericByRef().u4;
    channelId = pThis[FIELD___channelNumber].NumericByRef().s4;

    // Gets the PWM driver associated with the requested timer
    pwmDriver = GetDriverPwm(timerId);

    // Sets the pin to the correct PWM alternate function and...
    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_ALTERNATE(GetAlternateFunctionPwm(timerId)));

    // ...enables the associated channel
    pwmEnableChannel(pwmDriver, channelId, PWM_PERCENTAGE_TO_WIDTH(pwmDriver, dutyCycle));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStop___VOID(CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t channelId;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties or method parameters
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    channelId = pThis[FIELD___channelNumber].NumericByRef().s4;

    // Stops PWM output on the channel associated with the selected pin
    pwmDisableChannel(GetDriverPwm(timerId), channelId);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::GetChannel___STATIC__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // Get pin and potential TIM
    int pin = stack.Arg0().NumericByRef().s4;
    int timerId = stack.Arg1().NumericByRef().s4;

    // Check if the combination is ok and set the result
    stack.SetResult_I4(GetChannelPwm(pin, timerId));

    NANOCLR_NOCLEANUP_NOLABEL();
}
