//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_pwm_native_target.h>

#define CAP_COMPARE_CHANNELS_COUNT 4

// flag to prevent duplicate initialization
static bool DcRegulatorInit = false;

NF_PAL_PWM_INSTANCE PwmInstances[PWM_TIMERS_COUNT] = {
    {false, false, {255, 255, 255}},
    {false, false, {255, 255, 255}},
    {false, false, {255, 255, 255}}};

uint32_t ComputeDutyCycle(uint32_t currentCount, uint32_t dutyCycle)
{
    // Return a duty cycle in the range of the current timer duty resolution
    uint32_t dutyCycleValue =
        currentCount * dutyCycle / Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::CONST_DutyCycleFactor;

    return dutyCycleValue;
}

int32_t GetPwmConfigFromPin(int pin)
{
    if (pin >= -1)
    {
        for (int i = 0; i < PwmConfigCount; i++)
        {
            if (GET_ENCODED_PWM_PORT_PIN(PwmPortPinConfig[i]) == pin)
            {
                return i;
            }
        }
    }

    return -1;
}

int32_t GetPwmConfigFromTimeAndChannel(int timerId, int channelId)
{
    for (int i = 0; i < PwmConfigCount; i++)
    {
        if (PwmPortPinConfig[i].wtimerIndex == timerId && PwmPortPinConfig[i].capCompIndex == channelId)
        {
            return i;
        }
    }

    return -1;
}

int32_t GetPwmConfig(int pinNumber, int timerId, int channelId)
{
    int32_t configIndex = -1;

    if (pinNumber != -1)
    {
        configIndex = GetPwmConfigFromPin(pinNumber);
    }

    if (configIndex == -1)
    {
        configIndex = GetPwmConfigFromTimeAndChannel(timerId, channelId);
    }

    return configIndex;
}

TIMER_TypeDef *GetPwmTimer(int timerId)
{
    switch (timerId)
    {

        case 0:
            return WTIMER0;

        case 1:
            return WTIMER1;

        case 2:
            return WTIMER2;

        default:
            return nullptr;
    }
}

CMU_Clock_TypeDef_enum GetPwmClock(int timerId)
{
    switch (timerId)
    {

        case 0:
            return cmuClock_WTIMER0;

        case 1:
            return cmuClock_WTIMER1;

        case 2:
            return cmuClock_WTIMER1;

        default:
            return (CMU_Clock_TypeDef_enum)-1;
    }
}

void StopChannel(int32_t configIndex)
{
    bool okToDisable;
    const NF_PAL_PWM_PORT_PIN_CONFIG *pwmConfig;

    // grab PWM config
    pwmConfig = &PwmPortPinConfig[configIndex];

    // Stops PWM output on the channel associated with the selected pin
    // disable CC route pin
    GetPwmTimer(pwmConfig->wtimerIndex)->ROUTEPEN &= ~(TIMER_ROUTEPEN_CC0PEN << pwmConfig->capCompIndex);

    // reset GPIO
    GPIO_PinModeSet(pwmConfig->gpioPort, pwmConfig->portPin, gpioModeDisabled, 0);

    // free channel
    PwmInstances[pwmConfig->wtimerIndex].ChannelArray[pwmConfig->capCompIndex] = 255;

    // check if we can disable the timer
    okToDisable = true;
    for (int i = 0; i < PWM_CHANNEL_COUNT; i++)
    {
        if (PwmInstances[pwmConfig->wtimerIndex].ChannelArray[i] != 255)
        {
            // we can't disable the timer yet
            okToDisable = false;
            break;
        }
    }

    if (okToDisable)
    {
        // disable timer
        TIMER_Reset(GetPwmTimer(pwmConfig->wtimerIndex));

        // disable clock for WTIMER module
        CMU_ClockEnable(GetPwmClock(pwmConfig->wtimerIndex), false);

        // lower flag about time being configured
        PwmInstances[pwmConfig->wtimerIndex].IsConfigured = false;
    }
}

void DeInitPwm()
{
    for (int i = 0; i < PWM_TIMERS_COUNT; i++)
    {
        for (int j = 0; j < PWM_CHANNEL_COUNT; j++)
        {
            if (PwmInstances[i].ChannelArray[j] != 255)
            {
                StopChannel(GetPwmConfigFromPin(PwmInstances[i].ChannelArray[j]));
            }
        }
    }
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t pinNumber;
    int32_t channelId;
    int32_t configIndex;
    int32_t desiredFrequency;
    PwmPulsePolarity polarity;
    // int32_t routeLocationBit;
    TIMER_InitCC_TypeDef timerCCInit;
    const NF_PAL_PWM_PORT_PIN_CONFIG *pwmConfig;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not nullptr
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // reset config for NOT FOUND
    configIndex = -1;

    if (!DcRegulatorInit)
    {
        // init DCDC regulator with kit specific parameters
        EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
        EMU_DCDCInit(&dcdcInit);
    }

    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;
    channelId = pThis[FIELD___channelNumber].NumericByRef().s4;
    desiredFrequency = pThis[FIELD___frequency].NumericByRef().s4;
    polarity = (PwmPulsePolarity)(pThis[Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::FIELD___polarity]
                                      .NumericByRef()
                                      .u4);

    // try to find config
    configIndex = GetPwmConfig(pinNumber, timerId, channelId);

    if (configIndex == -1)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // grab PWM config
    pwmConfig = &PwmPortPinConfig[configIndex];

    // check if this channel is already in use
    if (PwmInstances[pwmConfig->wtimerIndex].ChannelArray[pwmConfig->capCompIndex] != 255)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
    }

    // store encoded GPIO port and pin number
    pThis[FIELD___pinNumber].NumericByRef().s4 = GET_ENCODED_PWM_PORT_PIN(PwmPortPinConfig[configIndex]);
    PwmInstances[pwmConfig->wtimerIndex].ChannelArray[pwmConfig->capCompIndex] =
        pThis[FIELD___pinNumber].NumericByRef().s4;

    // check if timer needs to be initialized
    if (!PwmInstances[pwmConfig->wtimerIndex].IsConfigured)
    {
        // timer is not configured, configure it

        // Enable clock for WTIMER module
        CMU_ClockEnable(GetPwmClock(timerId), true);

        // set top value to max PWM_FREQ frequency
        TIMER_TopSet(GetPwmTimer(timerId), CMU_ClockFreqGet(GetPwmClock(timerId)) / desiredFrequency);

        // set duty cycle to 100%
        TIMER_CompareSet(WTIMER0, 0, TIMER_TopGet(WTIMER0) - 1);

        // Initialize the timer
        TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
        TIMER_Init(GetPwmTimer(pwmConfig->wtimerIndex), &timerInit);

        // set flag to indicate that timer is configured
        PwmInstances[pwmConfig->wtimerIndex].IsConfigured = true;
    }

    // config GPIO
    GPIO_PinModeSet(pwmConfig->gpioPort, pwmConfig->portPin, gpioModePushPull, polarity);

    // Route WTIMER CC to location
    GetPwmTimer(pwmConfig->wtimerIndex)->ROUTELOC0 |= pwmConfig->capCompLocation << pwmConfig->capCompIndex * 8;

    // Configure WTIMER Compare/Capture for output compare
    // Use PWM mode and config polarity too
    timerCCInit = TIMER_INITCC_DEFAULT;
    timerCCInit.mode = timerCCModePWM;
    timerCCInit.outInvert = (polarity == PwmPulsePolarity_ActiveLow);
    TIMER_InitCC(GetPwmTimer(pwmConfig->wtimerIndex), pwmConfig->capCompIndex, &timerCCInit);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetDesiredFrequency___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    uint32_t timerId;
    int32_t desiredFrequency;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not nullptr
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    desiredFrequency = stack.Arg1().NumericByRef().s4;

    // parameter check
    if (desiredFrequency < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Set top value to overflow at the desired PWM_FREQ frequency
    TIMER_TopSet(GetPwmTimer(timerId), (CMU_ClockFreqGet(GetPwmClock(timerId)) / desiredFrequency) - 1);

    // store the frequency
    pThis[FIELD___frequency].NumericByRef().s4 = desiredFrequency;

    stack.SetResult_R8((double)desiredFrequency);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetActiveDutyCyclePercentage___VOID__R8(
    CLR_RT_StackFrame &stack)
{
    uint32_t pinNumber;
    int32_t configIndex;
    uint32_t dutyCycle;
    const NF_PAL_PWM_PORT_PIN_CONFIG *pwmConfig;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not nullptr
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // parameter check
    if (stack.Arg1().NumericByRef().r8 < 0 || stack.Arg1().NumericByRef().r8 > 1.0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;

    // try to find config
    configIndex = GetPwmConfigFromPin(pinNumber);

    // grab PWM config
    pwmConfig = &PwmPortPinConfig[configIndex];

    dutyCycle = (uint32_t)(stack.Arg1().NumericByRef().r8 * CONST_DutyCycleFactor);

    // set compare value for initial duty cycle
    TIMER_CompareSet(
        GetPwmTimer(pwmConfig->wtimerIndex),
        pwmConfig->capCompIndex,
        ComputeDutyCycle(TIMER_TopGet(GetPwmTimer(pwmConfig->wtimerIndex)), dutyCycle));

    // store the new duty cycle
    pThis[FIELD___dutyCycle].NumericByRef().u4 = dutyCycle;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStart___VOID(CLR_RT_StackFrame &stack)
{
    uint32_t pinNumber;
    int32_t configIndex;
    const NF_PAL_PWM_PORT_PIN_CONFIG *pwmConfig;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not nullptr
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;

    // try to find config
    configIndex = GetPwmConfigFromPin(pinNumber);

    // grab PWM config
    pwmConfig = &PwmPortPinConfig[configIndex];

    // enable CC route pin so the GPIO get driven by the timer
    GetPwmTimer(pwmConfig->wtimerIndex)->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN << pwmConfig->capCompIndex;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStop___VOID(CLR_RT_StackFrame &stack)
{
    uint32_t pinNumber;
    int32_t configIndex;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not nullptr
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;

    // try to find config
    configIndex = GetPwmConfigFromPin(pinNumber);

    StopChannel(configIndex);

    // "dispose" pin number
    pThis[FIELD___pinNumber].NumericByRef().s4 = -1;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    uint32_t pinNumber;
    int32_t configIndex;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not nullptr
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;

    // try to find config
    configIndex = GetPwmConfigFromPin(pinNumber);

    if (configIndex > -1)
    {
        // hasn't been disposed yet, so stop the channel
        StopChannel(configIndex);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::GetChannel___STATIC__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    int32_t configIndex;
    const NF_PAL_PWM_PORT_PIN_CONFIG *pwmConfig;

    NANOCLR_HEADER();

    // Get pin and potential TIM
    int pinNumber = stack.Arg0().NumericByRef().s4;

    // try to find config
    configIndex = GetPwmConfigFromPin(pinNumber);

    if (configIndex > -1)
    {
        // grab PWM config
        pwmConfig = &PwmPortPinConfig[configIndex];

        // Check if the combination is ok and set the result
        stack.SetResult_I4(pwmConfig->capCompIndex);
    }
    else
    {
        stack.SetResult_I4(-1);
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}
