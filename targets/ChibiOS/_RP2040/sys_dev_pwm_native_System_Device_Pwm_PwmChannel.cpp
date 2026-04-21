//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>
#include <targetPAL.h>
#include <sys_dev_pwm_native.h>

// RP2040 PWM mapping:
// 8 PWM slices (0-7), each with channels A (0) and B (1)
// GPIO to slice: slice = GPIO / 2
// GPIO to channel: channel = GPIO % 2

typedef Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel PwmChannel;

// Get PWM driver for a given slice (timer) ID
static PWMDriver *GetDriverPwm(int timerId)
{
    switch (timerId)
    {
#if RP_PWM_USE_PWM0
        case 0:
            return &PWMD0;
#endif
#if RP_PWM_USE_PWM1
        case 1:
            return &PWMD1;
#endif
#if RP_PWM_USE_PWM2
        case 2:
            return &PWMD2;
#endif
#if RP_PWM_USE_PWM3
        case 3:
            return &PWMD3;
#endif
#if RP_PWM_USE_PWM4
        case 4:
            return &PWMD4;
#endif
#if RP_PWM_USE_PWM5
        case 5:
            return &PWMD5;
#endif
#if RP_PWM_USE_PWM6
        case 6:
            return &PWMD6;
#endif
#if RP_PWM_USE_PWM7
        case 7:
            return &PWMD7;
#endif
        default:
            return NULL;
    }
}

// RP2040: derive channel from pin number
// pin / 2 = slice, pin % 2 = channel
static int GetChannelPwm(int pin, int timerId)
{
    // validate the pin maps to the requested slice
    if ((pin / 2) != timerId)
    {
        return -1;
    }

    return pin % 2;
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

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    timerId = pThis[PwmChannel::FIELD___pwmTimer].NumericByRef().s4;
    desiredFrequency = stack.Arg1().NumericByRef().s4;

    if (desiredFrequency < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    pwmDriver = GetDriverPwm(timerId);

    // Set period for precision
    period = 1000;
    if (desiredFrequency >= 1000)
    {
        period = 100;
    }
    else if (desiredFrequency >= 1000000)
    {
        period = 10;
    }

    // Build RP2040 PWM config (2 channels per slice)
    pwmConfig = {
        (desiredFrequency * period), // clock frequency
        (pwmcnt_t)period,            // period
        NULL,                        // callback
        {
            {PWM_OUTPUT_ACTIVE_HIGH, NULL},
            {PWM_OUTPUT_ACTIVE_HIGH, NULL},
        },
    };

    pwmStop(pwmDriver);
    osDelay(5);
    pwmStart(pwmDriver, &pwmConfig);

    pThis[PwmChannel::FIELD___frequency].NumericByRef().s4 = desiredFrequency;

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

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    timerId = pThis[PwmChannel::FIELD___pwmTimer].NumericByRef().s4;
    channelId = pThis[PwmChannel::FIELD___channelNumber].NumericByRef().s4;

    if (stack.Arg1().NumericByRef().r8 < 0 || stack.Arg1().NumericByRef().r8 > 1.0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    dutyCycle = (uint32_t)(stack.Arg1().NumericByRef().r8 * PwmChannel::CONST_DutyCycleFactor);

    pwmDriver = GetDriverPwm(timerId);

    pwmEnableChannel(pwmDriver, channelId, PWM_PERCENTAGE_TO_WIDTH(pwmDriver, dutyCycle));

    pThis[PwmChannel::FIELD___dutyCycle].NumericByRef().u4 = dutyCycle;

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

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    timerId = pThis[PwmChannel::FIELD___pwmTimer].NumericByRef().s4;
    pinNumber = pThis[PwmChannel::FIELD___pinNumber].NumericByRef().s4;
    dutyCycle = pThis[PwmChannel::FIELD___dutyCycle].NumericByRef().u4;
    channelId = pThis[PwmChannel::FIELD___channelNumber].NumericByRef().s4;

    pwmDriver = GetDriverPwm(timerId);

    // RP2040: set pin to PWM alternate function (FUNCSEL=4)
    palSetPadMode(IOPORT1, pinNumber, PAL_MODE_ALTERNATE_PWM);

    pwmEnableChannel(pwmDriver, channelId, PWM_PERCENTAGE_TO_WIDTH(pwmDriver, dutyCycle));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStop___VOID(CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t channelId;

    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    timerId = pThis[PwmChannel::FIELD___pwmTimer].NumericByRef().s4;
    channelId = pThis[PwmChannel::FIELD___channelNumber].NumericByRef().s4;

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

    int pin = stack.Arg0().NumericByRef().s4;
    int timerId = stack.Arg1().NumericByRef().s4;

    stack.SetResult_I4(GetChannelPwm(pin, timerId));

    NANOCLR_NOCLEANUP_NOLABEL();
}
