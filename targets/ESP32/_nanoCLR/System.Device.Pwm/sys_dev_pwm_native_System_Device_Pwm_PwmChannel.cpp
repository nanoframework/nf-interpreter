//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_pwm_native.h>
#include <targetPAL.h>
#include <Esp32_DeviceMapping.h>

// Used to map a PWM channel number to a pin number for High and low speed channels
static char HighSpeedPinMap[8] = {255, 255, 255, 255, 255, 255, 255, 255};
static char LowSpeedPinMap[8] = {255, 255, 255, 255, 255, 255, 255, 255};
// Pin functions from PWM1 to PWM16
static int PwmMapping[16] = {
    262400,
    262656,
    262912,
    263168,
    263424,
    263680,
    263936,
    264192,
    264448,
    264704,
    264960,
    265216,
    265472,
    265728,
    265984,
    266240};

#if SOC_LEDC_SUPPORT_HS_MODE
#define GetSpeedMode(timer) (ledc_mode_t)((timer > 3) ? LEDC_LOW_SPEED_MODE : LEDC_HIGH_SPEED_MODE)
#else
#define GetSpeedMode(timer) LEDC_LOW_SPEED_MODE
#endif

#define IDF_ERROR(result)                                                                                              \
    if (result != ESP_OK)                                                                                              \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);                                                                \
    }

namespace sys_dev_pwm_native_System_Device_Pwm_PwmChannelHelpers
{
uint32_t PwmController_Timer_resolution[8];

int GetChannel(int pin, int timerId, bool create);
uint32_t CalculateDuty(int timerId, uint32_t dutyCycle, PwmPulsePolarity polarity);
HRESULT ConfigureAndStart(CLR_RT_HeapBlock *pThis, bool create, bool noStart);
} // namespace sys_dev_pwm_native_System_Device_Pwm_PwmChannelHelpers

using namespace sys_dev_pwm_native_System_Device_Pwm_PwmChannelHelpers;

//
//  Look up Pin number to find channel, if create true and not present then add pin
//  return channel number or -1 if error
//
int sys_dev_pwm_native_System_Device_Pwm_PwmChannelHelpers::GetChannel(int pin, int timerId, bool create)
{
    int channel = -1; // Return if not found

    // Select map depending if high or low speed timers
    char *pMap = (timerId > 3) ? LowSpeedPinMap : HighSpeedPinMap;
    char *pMap2 = pMap;

    // look for pin in map
    for (int index = 0; index < 8; index++, pMap++)
    {
        if (*pMap == pin)
        {
            channel = index;
            break;
        }
    }

    if (create && channel == -1)
    {
        // if pin/channel not found then allocate one
        for (int index = 0; index < 8; index++, pMap2++)
        {
            if (*pMap2 == 255)
            {
                channel = index;
                *pMap2 = pin;
                break;
            }
        }
    }

    return channel;
}

//
// Work out the duty Cycle for the current duty resolution and polarity
//
uint32_t sys_dev_pwm_native_System_Device_Pwm_PwmChannelHelpers::CalculateDuty(
    int timerId,
    uint32_t dutyCycle,
    PwmPulsePolarity polarity)
{
    // if polarity Active low then reverse duty cycle
    if (polarity == PwmPulsePolarity::PwmPulsePolarity_ActiveLow)
    {
        dutyCycle = 10000 - dutyCycle;
    }

    // Return a duty cycle in the range of the current timer duty resolution
    uint32_t calculatedDuty = PwmController_Timer_resolution[timerId] * dutyCycle / 10000;

    return calculatedDuty;
}

HRESULT sys_dev_pwm_native_System_Device_Pwm_PwmChannelHelpers::ConfigureAndStart(
    CLR_RT_HeapBlock *pThis,
    bool create,
    bool noStart)
{
    int32_t timerId;
    int32_t pinNumber;
    uint32_t dutyCycle;

    PwmPulsePolarity polarity;

    ledc_mode_t mode;
    ledc_channel_t channel;
    ledc_timer_t timer_sel;
    ledc_channel_config_t ledc_conf;

    NANOCLR_HEADER();

    // Retrieves the needed parameters from private class properties or method parameters
    timerId = pThis[Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::FIELD___pwmTimer].NumericByRef().s4;
    pinNumber = pThis[Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::FIELD___pinNumber].NumericByRef().s4;
    dutyCycle = pThis[Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::FIELD___dutyCycle].NumericByRef().u4;
    polarity = (PwmPulsePolarity)(pThis[Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::FIELD___polarity]
                                      .NumericByRef()
                                      .u4);

    // Configure channel
    mode = GetSpeedMode(timerId);
    channel = (ledc_channel_t)GetChannel(pinNumber, timerId, create);

    if (channel == -1)
    {
        // Unable to create a new channel, all channels used up ?
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    timer_sel = (ledc_timer_t)(timerId & 0x03);

    // Work out the duty Cycle for the current duty resolution
    dutyCycle = CalculateDuty(timerId, dutyCycle, polarity);

    ledc_conf = {pinNumber, mode, channel, LEDC_INTR_DISABLE, timer_sel, dutyCycle, 0, 0};

    // Configure Channel which will also start it
    IDF_ERROR(ledc_channel_config(&ledc_conf));

    // Because it is started from the configure we optionally stop it and set idle level based on polarity
    if (noStart)
    {
        ledc_stop(mode, channel, polarity);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    int32_t pinNumber;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Check pin number is a valid for output
    pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;

    if (!GPIO_IS_VALID_OUTPUT_GPIO(pinNumber))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Create a new entry in channel table and configure channel which will also start channel
    NANOCLR_CHECK_HRESULT(ConfigureAndStart(pThis, true, true));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetDesiredFrequency___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t desiredFrequency;
    int32_t optimumDutyResolution;
    uint32_t precision;
    uint64_t divParam;
    esp_err_t result;

    ledc_timer_t timer;
    ledc_mode_t mode;
    ledc_timer_bit_t duty_res;
    ledc_timer_config_t timer_conf;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties ( 0 - 7 )
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    desiredFrequency = stack.Arg1().NumericByRef().s4;

    // parameter check
    if (desiredFrequency < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    timer = (ledc_timer_t)(timerId & 0x03);

#if SOC_LEDC_SUPPORT_HS_MODE
    mode = (timerId <= 4) ? LEDC_HIGH_SPEED_MODE : LEDC_LOW_SPEED_MODE;
#else
    mode = LEDC_LOW_SPEED_MODE;
#endif

    // Work out the optimal duty resolution based on current frequency, default to 1 if not found
    // Working from 15 bit duty resolution down until we have a valid divisor
    optimumDutyResolution = 1;

    for (int dutyResolution = 15; dutyResolution > 0; dutyResolution--)
    {
        precision = (0x1 << dutyResolution); // 2**depth

        divParam = ((uint64_t)LEDC_APB_CLK_HZ << 8) / desiredFrequency / precision;

        if (divParam > 256)
        {
            optimumDutyResolution = dutyResolution;
            break;
        }
    }

    duty_res = (ledc_timer_bit_t)optimumDutyResolution;

    // Save resolution for working out values for percent duty cycle
    PwmController_Timer_resolution[timerId] = (0x1 << optimumDutyResolution);

    timer_conf = {mode, duty_res, timer, (uint32_t)desiredFrequency, LEDC_AUTO_CLK};

    result = ledc_timer_config(&timer_conf);
    if (result != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // store the frequency
    pThis[FIELD___frequency].NumericByRef().s4 = desiredFrequency;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetActiveDutyCyclePercentage___VOID__R8(
    CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t pinNumber;
    uint32_t dutyCycle;

    ledc_channel_t channel;

    PwmPulsePolarity polarity;
    ledc_mode_t speed_mode;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties or method parameters
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;
    polarity = (PwmPulsePolarity)(pThis[FIELD___polarity].NumericByRef().u4);

    // parameter check
    if (stack.Arg1().NumericByRef().r8 < 0 || stack.Arg1().NumericByRef().r8 > 1.0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // retrieve percentage as 0 to 10000 (0% to 100%)
    dutyCycle = (uint32_t)(stack.Arg1().NumericByRef().r8 * CONST_DutyCycleFactor);

    // Get channel number used for this pinNumber
    // FIXME check result
    channel = (ledc_channel_t)GetChannel(pinNumber, timerId, false);

    // Get speed mode based on Timer used
    speed_mode = GetSpeedMode(timerId);

    // Work out the duty Cycle for the current duty resolution
    dutyCycle = CalculateDuty(timerId, dutyCycle, polarity);

    // Update duty on channel
    IDF_ERROR(ledc_set_duty(speed_mode, channel, dutyCycle));

    // Activate duty on channel
    IDF_ERROR(ledc_update_duty(speed_mode, channel));

    // store the new duty cycle
    pThis[FIELD___dutyCycle].NumericByRef().u4 = dutyCycle;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStart___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Call configure to start PWM channel
    NANOCLR_CHECK_HRESULT(ConfigureAndStart(pThis, false, false));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStop___VOID(CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t pinNumber;
    int32_t polarity;

    ledc_mode_t speed_mode;
    ledc_channel_t channel;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties or method parameters
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;
    polarity = pThis[FIELD___polarity].NumericByRef().s4;

    speed_mode = GetSpeedMode(timerId);

    // FIX ME check result
    channel = (ledc_channel_t)GetChannel(pinNumber, timerId, false);

    ledc_stop(speed_mode, channel, (uint32_t)polarity);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t pinNumber;
    char *pMap;

    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties or method parameters
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;

    // Remove pin from pin/channel Map
    pMap = (timerId > 3) ? LowSpeedPinMap : HighSpeedPinMap;

    for (int index = 0; index < 8; index++, pMap++)
    {
        if (*pMap == pinNumber)
        {
            *pMap = 255;
            break;
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::GetChannel___STATIC__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    int32_t pinSetup;

    NANOCLR_HEADER();

    int pin = stack.Arg0().NumericByRef().s4;
    int timerId = stack.Arg1().NumericByRef().s4;
    int pwm = 0;

    // Check if the combination is ok and set the result
    for (pwm = timerId * 2; pwm < timerId * 2 + 2; pwm++)
    {
        pinSetup = (int32_t)Esp32_GetMappedDevicePinsWithFunction(PwmMapping[pwm]);

        if (pinSetup == pin)
        {
            // The channel is actually the pin number
            stack.SetResult_I4(pin);
            break;
        }
    }

    if (pwm == timerId * 2 + 2)
    {
        stack.SetResult_I4(-1);
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}
