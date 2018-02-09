//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <targetPAL.h>
#include "win_dev_gpio_native.h"
#include "nf_rt_events_native.h"


///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Gpio.GpioPinDriveMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

enum GpioPinDriveMode
{
    GpioPinDriveMode_Input = 0,
    GpioPinDriveMode_InputPullDown,
    GpioPinDriveMode_InputPullUp,
    GpioPinDriveMode_Output,
    GpioPinDriveMode_OutputOpenDrain,
    GpioPinDriveMode_OutputOpenDrainPullUp,
    GpioPinDriveMode_OutputOpenSource,
    GpioPinDriveMode_OutputOpenSourcePullDown
};

///////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Gpio.GpioPinValue (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////

enum GpioPinValue
{
    GpioPinValue_Low = 0,
    GpioPinValue_High,
};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

// this structure is part of the target because it can have different sizes according to the device
//extern EXTConfig extInterruptsConfiguration;

// this array keeps track of the Gpio pins that are assigned to each channel
CLR_RT_HeapBlock* channelPinMapping[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

// this timer is used to handle the debounce time
static virtual_timer_t debounceTimer;

volatile uint16_t lastPadValue;

static void debounceTimer_Callback( void* arg )
{
    CLR_RT_HeapBlock*  pThis = (CLR_RT_HeapBlock*)arg;

    if(pThis == NULL)
    {
        // no Gpio pin here, leave now
        chSysUnlockFromISR();
        return;
    }

    // check if object has been disposed
    if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
    {
        // object has been disposed, leave now
        chSysUnlockFromISR();
        return;
    }
    
    // get pin number and take the port and pad references from that one
    int16_t pinNumber = pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___pinNumber ].NumericByRefConst().s4;
    stm32_gpio_t* port  = GPIO_PORT(pinNumber);
    int16_t pad = pinNumber % 16;

    // read pad
    uint16_t currentValue = palReadPad(port, pad);

    if(lastPadValue == currentValue)
    {
        // value hasn't change for debounce interval so this is a valid change
        // post a managed event with the current pin value
        PostManagedEvent( EVENT_GPIO, 0, pinNumber, currentValue );
    }
}

static void ExtInterruptHandler(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;

    chSysLockFromISR();

    CLR_RT_HeapBlock*  pThis = channelPinMapping[channel];

    if(pThis == NULL)
    {
        // no Gpio pin here, leave now
        chSysUnlockFromISR();
        return;
    }

    // check if object has been disposed
    if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
    {
        // object has been disposed, leave now
        chSysUnlockFromISR();
        return;
    }
    
    // get pin number and take the port and pad references from that one
    int16_t pinNumber = pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___pinNumber ].NumericByRefConst().s4;
    stm32_gpio_t* port  = GPIO_PORT(pinNumber);
    int16_t pad = pinNumber % 16;

    // check if there is a debounce time set
    int64_t debounceTimeoutMilsec = (CLR_INT64_TEMP_CAST) pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___debounceTimeout ].NumericByRefConst().s8 / TIME_CONVERSION__TO_MILLISECONDS;
                
    if(debounceTimeoutMilsec > 0)
    {
        // debounce set, need to handle it

        if(chVTIsArmed(&debounceTimer))
        {
            // there is a debounce timer already running so this change in pin value should be discarded 
            chSysUnlockFromISR();
            return;
        }

        // read pad
        lastPadValue = palReadPad(port, pad);

        // setup timer
        chVTSetI(&debounceTimer, MS2ST(debounceTimeoutMilsec), debounceTimer_Callback, pThis);
    }
    else
    {
        // post a managed event with the current pin reading
        PostManagedEvent( EVENT_GPIO, 0, pinNumber, palReadPad(port, pad) );
    }

    chSysUnlockFromISR();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::Read___WindowsDevicesGpioGpioPinValue( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        int16_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

        stack.SetResult_I4( palReadPad(GPIO_PORT(pinNumber), pinNumber % 16) );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // get pin number and take the port and pad references from that one
        int16_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
        stm32_gpio_t* port  = GPIO_PORT(pinNumber);
        int16_t pad = pinNumber % 16;

        // disable the EXT interrupt channel
        // it's OK to do always this, no matter if it's enabled or not
        chSysLock();
        extChannelDisable(&EXTD1, pad);
        chSysUnlock();

        // don't need to do anything else about the callbacks because the pin is 
        // removed from the managed event listener in the managed Dispose() method 

        // clear this channel in channel pin mapping
        channelPinMapping[pad] = NULL;

        // set pin to input to save power
        palSetPadMode(port, pad, PAL_MODE_INPUT);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeIsDriveModeSupported___BOOLEAN__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        GpioPinDriveMode driveMode = (GpioPinDriveMode)stack.Arg1().NumericByRef().s4;

        bool driveModeSupported = false;

        // check if the requested drive mode is support by ChibiOS config
        if ((driveMode == GpioPinDriveMode_Input) ||
            (driveMode == GpioPinDriveMode_InputPullDown) ||
            (driveMode == GpioPinDriveMode_InputPullUp) ||
            (driveMode == GpioPinDriveMode_Output) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrain))
        {
            driveModeSupported = true;
        }

        // Return value to the managed application
        stack.SetResult_Boolean( driveModeSupported ) ;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDriveMode___VOID__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // get pin number and take the port and pad references from that one
        int16_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
        stm32_gpio_t* port  = GPIO_PORT(pinNumber);
        int16_t pad = pinNumber % 16;

        // it's better cast this this to the appropriate enum
        GpioPinDriveMode driveMode = (GpioPinDriveMode)stack.Arg1().NumericByRef().s4;

        // check if drive mode is input
        bool driveModeIsInput = false;

        if( driveMode == GpioPinDriveMode_Input ||
            driveMode == GpioPinDriveMode_InputPullDown ||
            driveMode == GpioPinDriveMode_InputPullUp)
            {
                driveModeIsInput = true;
            }

        // flag to signal that interrupts need to be setup
        bool setupInterrupt = false;

        // flag to determine if there are any callbacks registered in managed code
        bool callbacksRegistered = (pThis[ FIELD___callbacks ].Dereference() != NULL);

        // initial check on whether there are callbacks registered AND the drive mode is input
        // the EXT driver can only handle 16 interrupts and each channel can serve only one port
        // ex: channel 0 can be triggered by pad 0 on port A, B, C.... but EXCLUSIVELY by ONLY ONE of those
        // see MCU programming manual for details (External interrupt/event controller (EXTI))
        if(extInterruptsConfiguration.channels[pad].mode != 0 && callbacksRegistered && driveModeIsInput)
        {
            // channel for this pad is already taken
            // TODO: probably better to have a dedicated exception here, this one is too generic...
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // always disable the EXT interrupt channel before setting the function on a pad otherwise the interrupts won't fire
        chSysLock();
        extChannelDisable(&EXTD1, pad);
        chSysUnlock();

        // protect from GC 

        // clear this channel in channel pin mapping
        channelPinMapping[pad] = NULL;

        switch (driveMode)
        {
            case GpioPinDriveMode_Input:
                palSetPadMode(port, pad, PAL_MODE_INPUT);
                setupInterrupt = true;
                break;

            case GpioPinDriveMode_InputPullDown:
                palSetPadMode(port, pad, PAL_MODE_INPUT_PULLDOWN);
                setupInterrupt = true;
                break;

            case GpioPinDriveMode_InputPullUp:
                palSetPadMode(port, pad, PAL_MODE_INPUT_PULLUP);
                setupInterrupt = true;
                break;

            case GpioPinDriveMode_Output:
                palSetPadMode(port, pad, PAL_MODE_OUTPUT_PUSHPULL);
                break;

            case GpioPinDriveMode_OutputOpenDrain:
                palSetPadMode(port, pad, PAL_MODE_OUTPUT_OPENDRAIN);
                break;

            default:
                // all other modes are NOT supported
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // if drive mode is output, read the pad to update the managed field _lastOutputValue
        if(!driveModeIsInput)
        {
            pThis[ FIELD___lastOutputValue ].NumericByRef().s4 = palReadPad(port, pad);
        }

        if(callbacksRegistered && setupInterrupt)
        {
            // there are callbacks registered and...
            // the drive mode is input so need to setup the interrupt
            chSysLock();

            // save pin
            channelPinMapping[pad] = pThis;

            int16_t extChannelCode = ((pinNumber/16) << 8);
            const EXTChannelConfig configToSetupChannel = {EXT_CH_MODE_BOTH_EDGES | extChannelCode, ExtInterruptHandler};
            extSetChannelModeI(&EXTD1, pad, &configToSetupChannel);                

            chSysUnlock();

            // protect this from GC so that the callback is where it's supposed to
            CLR_RT_ProtectFromGC         gc( *pThis );
            
            // read pad and store current value to check on debounce callback
            lastPadValue = palReadPad(port, pad);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeInit___BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int16_t pinNumber = stack.Arg1().NumericByRef().s4;

        // TODO is probably a good idea keep track of the used pins, so we can check that here
        // TODO is probably a good idea to check if this pin exists

        // Return value to the managed application
        stack.SetResult_Boolean(true );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDebounceTimeout___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    // nothing to do here as the debounce timeout is grabbed from the managed object when required

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::WriteNative___VOID__WindowsDevicesGpioGpioPinValue( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        int16_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
        GpioPinDriveMode driveMode = (GpioPinDriveMode)pThis[ FIELD___driveMode ].NumericByRefConst().s4;

        GpioPinValue state = (GpioPinValue)stack.Arg1().NumericByRef().s4;

        // sanity check for drive mode set to output so we don't mess up writing to an input pin
        if ((driveMode == GpioPinDriveMode_Output) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrain) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrainPullUp) ||
            (driveMode == GpioPinDriveMode_OutputOpenSource) ||
            (driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
        {
            if (state == GpioPinValue_Low)
            {
                palClearPad(GPIO_PORT(pinNumber), pinNumber % 16);
            }
            else 
            {
                palSetPad(GPIO_PORT(pinNumber), pinNumber % 16);
            }
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetAlternateFunction___VOID__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // get pin number and take the port and pad references from that one
        int16_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
        stm32_gpio_t* port  = GPIO_PORT(pinNumber);
        int16_t pad = pinNumber % 16;

        // get alternate function argument
		int32_t alternateFunction = stack.Arg1().NumericByRef().s4;

        palSetPadMode(port, pad, PAL_MODE_ALTERNATE(alternateFunction));

    }
    NANOCLR_NOCLEANUP();
}
