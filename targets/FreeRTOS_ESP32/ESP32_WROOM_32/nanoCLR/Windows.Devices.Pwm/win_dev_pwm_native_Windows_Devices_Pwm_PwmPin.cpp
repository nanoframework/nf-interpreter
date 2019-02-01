//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <targetPAL.h>
#include "win_dev_pwm_native.h"




// Used to map a PWM channel number to a pin number for High and low speed channels
static char HighSpeedPinMap[8] = { 255,255,255,255,255,255,255,255 };
static char LowSpeedPinMap[8] =  { 255,255,255,255,255,255,255,255 };

#define GetSpeedMode(timer) (ledc_mode_t)((timer>3) ? LEDC_LOW_SPEED_MODE : LEDC_HIGH_SPEED_MODE )
#define IDF_ERROR(result) if ( result != ESP_OK ) { NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER); }

extern uint32_t PwmController_Timer_resolution[8];



HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::ConfigureAndStart(CLR_RT_HeapBlock* pThis, bool create, bool noStart)
{
    NANOCLR_HEADER();
    {
       // Retrieves the needed parameters from private class properties or method parameters
        int timerId   = (int)(pThis[ FIELD___pwmTimer ].NumericByRef().u4);
        int pinNumber = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);
        uint32_t dutyCycle = (uint32_t)(pThis[ FIELD___dutyCycle ].NumericByRef().u4);
        PwmPulsePolarity polarity = (PwmPulsePolarity)(pThis[ FIELD___polarity ].NumericByRef().u4);

        // Configure channel
        ledc_mode_t  mode = GetSpeedMode(timerId);
        ledc_channel_t channel = (ledc_channel_t)GetChannel(pinNumber, timerId, create);
        if ( channel == -1 )
        {
            // Unable to create a new channel, all channels used up ?
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
        }

        ledc_timer_t timer_sel = (ledc_timer_t)(timerId & 0x03);

        // Work out the duty Cycle for the current duty resolution
        dutyCycle = CalculateDuty( timerId, dutyCycle, polarity );

        ledc_channel_config_t ledc_conf {
            pinNumber,
            mode,
            channel,
            LEDC_INTR_DISABLE,
            timer_sel,
            dutyCycle,
            0 
        };

        // Configure Channel which will also start it 
        IDF_ERROR( ledc_channel_config(&ledc_conf) );

        // Because it is started from the configure we optionally stop it and set idle level based on polarity
        if ( noStart) ledc_stop(mode, channel, polarity);

    }
    NANOCLR_NOCLEANUP();
}

//
//  Look up Pin number to find channel, if create true and not present then add pin
//  return channel number or -1 if error
//
int  Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::GetChannel (int pin, int timerId, bool create)
{
    int channel = -1;  // Return if not found
    
    // Selct map depending if high or low speed timers
    char * pMap =  (timerId > 3)? LowSpeedPinMap: HighSpeedPinMap;
    char * pMap2 = pMap;

    //look for pin in map
    for( int index=0; index<8; index++, pMap++ )
    {
        if (*pMap == pin ) { channel = index; break; }
    }

    if ( create && channel == -1) 
    {
        // if pin/channel not found then allocate one
        for( int index=0; index<8; index++, pMap2++ )
        {
            if (*pMap2 == 255 ) { channel = index; *pMap2 = pin; break; }
        }
    }
    
    return channel;
}

//
// Work out the duty Cycle for the current duty resolution and polarity
// 
uint32_t  Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::CalculateDuty(int timerId, uint32_t dutyCycle, PwmPulsePolarity polarity)
{
    // if polarity Active low then reverse duty cycle
    if (polarity == ActiveLow ) dutyCycle = 10000 - dutyCycle;

    // Return a duy cycle in the range of the current timer duty resolution
    return PwmController_Timer_resolution[timerId] * dutyCycle / 10000;
}

//
//  Pwmpin created
//  Validate pin and create an entry in Channel/Pin map
//
HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
       // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Check pin number is a valid for output
        int pinNumber = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);
        if ( !GPIO_IS_VALID_OUTPUT_GPIO(pinNumber) )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Create a new entry in channel table and configure channel which will also start channel
        NANOCLR_CHECK_HRESULT( ConfigureAndStart(pThis, true, true) );
    }
    NANOCLR_NOCLEANUP();
}


HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeSetActiveDutyCyclePercentage___VOID__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties or method parameters
        int timerId   = (int)(pThis[ FIELD___pwmTimer ].NumericByRef().u4);
        int pinNumber = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);
        PwmPulsePolarity polarity  = (PwmPulsePolarity)(pThis[ FIELD___polarity ].NumericByRef().u4);

        // retrieve Percents as 0 to 10000 ( 0% to 100%) 
        uint32_t dutyCycle = (uint32_t)stack.Arg1().NumericByRef().u4;

        // Get channel number usd for this pinNumber
        // FIXME check result
        ledc_channel_t channel = (ledc_channel_t)GetChannel (pinNumber, timerId, false);

        // Get speed mode based on Timer used
        ledc_mode_t speed_mode = GetSpeedMode(timerId);

        // Work out the duty Cycle for the current duty resolution
        dutyCycle = CalculateDuty( timerId, dutyCycle, polarity );

        // Update duty on channel
        IDF_ERROR( ledc_set_duty(speed_mode, channel, dutyCycle) );

        // Activate duty on channel
        IDF_ERROR( ledc_update_duty( speed_mode, channel) );
 
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeSetPolarity___VOID__U1( CLR_RT_StackFrame& stack )
{
    (void)stack;
    
    NANOCLR_HEADER();
    {
        // Nothing to do here, we handle it other places
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}


HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeStart___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Call configure to start PWM channel
        NANOCLR_CHECK_HRESULT( ConfigureAndStart(pThis, false, false) );
    
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeStop___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties or method parameters
        int timerId = (int)(pThis[ FIELD___pwmTimer ].NumericByRef().u4);
        int pinNumber = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);
        int polarity = (int)(pThis[ FIELD___polarity ].NumericByRef().u4);
      
        ledc_mode_t  speed_mode = GetSpeedMode(timerId);
        
        // FIX ME check result
        ledc_channel_t channel = (ledc_channel_t)GetChannel(pinNumber, timerId, false);
 
        ledc_stop(speed_mode, channel, (uint32_t)polarity);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Retrieves the needed parameters from private class properties or method parameters
        int timerId = (int)(pThis[ FIELD___pwmTimer ].NumericByRef().u4);
        int pinNumber = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);

        // Remove pin from pin/channel Map
        char * pMap =  (timerId > 3)? LowSpeedPinMap: HighSpeedPinMap;
        
        for( int index=0; index<8; index++, pMap++ )
        {
            if (*pMap == pinNumber ) { *pMap = 255; break; }
        }
    }
    NANOCLR_NOCLEANUP();
}
