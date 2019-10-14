//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <targetPAL.h>
#include "win_dev_gpio_native_target.h"

// this timer is used to handle the debounce time
static virtual_timer_t debounceTimer;

volatile uint16_t lastPadValue;

// this is an utility function to get a ChibiOS PAL IoLine from our "encoded" pin number
ioline_t GetIoLine(int16_t pinNumber)
{
    stm32_gpio_t* port  = GPIO_PORT(pinNumber);
    int16_t pad = pinNumber % 16;

    return PAL_LINE(port, pad);

}

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
    
    // get pin number
    int16_t pinNumber = pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___pinNumber ].NumericByRefConst().s4;

    // read line
    uint16_t currentValue = palReadLine(GetIoLine(pinNumber));

    // read pad
    uint16_t lastPadValue = pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___lastInputValue ].NumericByRef().s4;

    if(lastPadValue == currentValue)
    {
        // value hasn't change for debounce interval so this is a valid change

        // flag to determine if there are any callbacks registered in managed code
        bool callbacksRegistered = (pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___callbacks ].Dereference() != NULL);

        // post a managed event with the current pin value, only if there is anyone listening otherwise don't bother
        if(callbacksRegistered)
        {
            PostManagedEvent( EVENT_GPIO, 0, pinNumber, currentValue );
        }
    }
}

static void GpioEventCallback(void *arg)
{
    CLR_RT_HeapBlock*  pThis = (CLR_RT_HeapBlock*)arg;

    NATIVE_INTERRUPT_START

    chSysLockFromISR();

    //CLR_RT_HeapBlock*  pThis = channelPinMapping[channel];

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

        NATIVE_INTERRUPT_END

        return;
    }
    
    // get pin number and take the port and pad references from that one
    int16_t pinNumber = pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___pinNumber ].NumericByRefConst().s4;

    // get IoLine from pin number
    ioline_t ioLine = GetIoLine(pinNumber);

    // check if there is a debounce time set
    int64_t debounceTimeoutMilsec = (CLR_INT64_TEMP_CAST) pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___debounceTimeout ].NumericByRefConst().s8 / TIME_CONVERSION__TO_MILLISECONDS;

    if(debounceTimeoutMilsec > 0)
    {
        // debounce set, need to handle it

        if(chVTIsArmed(&debounceTimer))
        {
            // there is a debounce timer already running so this change in pin value should be discarded 
            chSysUnlockFromISR();

            NATIVE_INTERRUPT_END
            
            return;
        }

        // read pad
        pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___lastInputValue ].NumericByRef().s4 = palReadLine(ioLine);

        // setup timer
        chVTSetI(&debounceTimer, TIME_MS2I(debounceTimeoutMilsec), debounceTimer_Callback, pThis);
    }
    else
    {
        // read pad
        pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___lastInputValue ].NumericByRef().s4 = palReadLine(ioLine);

        // flag to determine if there are any callbacks registered in managed code
        bool callbacksRegistered = (pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___callbacks ].Dereference() != NULL);

        // post a managed event with the current pin reading, only if there is anyone listening otherwise don't bother
        if(callbacksRegistered)
        {
            PostManagedEvent( EVENT_GPIO, 0, pinNumber, palReadLine(ioLine) );
        }
    }

    chSysUnlockFromISR();

    NATIVE_INTERRUPT_END
    
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

        stack.SetResult_I4( palReadLine(GetIoLine(pThis[ FIELD___pinNumber ].NumericByRefConst().s4)) );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::Toggle___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // get IoLine from pin number
        ioline_t ioLine = GetIoLine(pThis[ FIELD___pinNumber ].NumericByRefConst().s4);

        GpioPinDriveMode driveMode = (GpioPinDriveMode)pThis[ FIELD___driveMode ].NumericByRefConst().s4;

        // sanity check for drive mode set to output so we don't mess up writing to an input pin
        if ((driveMode == GpioPinDriveMode_Output) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrain) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrainPullUp) ||
            (driveMode == GpioPinDriveMode_OutputOpenSource) ||
            (driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
        {
            palToggleLine(ioLine);

            // store new state
            pThis[ FIELD___lastOutputValue ].NumericByRef().s4 = (GpioPinValue)(GpioPinValue_High ^ (GpioPinValue)pThis[ FIELD___lastOutputValue ].NumericByRef().s4);
        }

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // get IoLine from pin number
        ioline_t ioLine = GetIoLine(pThis[ FIELD___pinNumber ].NumericByRefConst().s4);

        // disable the EXT interrupt channel
        // it's OK to do always this, no matter if it's enabled or not
        palDisableLineEvent(ioLine);

        // set pin to input to save power
        palSetLineMode(ioLine, PAL_MODE_INPUT);
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
    NANOCLR_NOCLEANUP_NOLABEL();
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

        // get IoLine from pin number
        ioline_t ioLine = GetIoLine(pThis[ FIELD___pinNumber ].NumericByRefConst().s4);
        
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

        switch (driveMode)
        {
            case GpioPinDriveMode_Input:
                palSetLineMode(ioLine, PAL_MODE_INPUT);
                setupInterrupt = true;
                break;

            case GpioPinDriveMode_InputPullDown:
                palSetLineMode(ioLine, PAL_MODE_INPUT_PULLDOWN);
                setupInterrupt = true;
                break;

            case GpioPinDriveMode_InputPullUp:
                palSetLineMode(ioLine, PAL_MODE_INPUT_PULLUP);
                setupInterrupt = true;
                break;

            case GpioPinDriveMode_Output:
                palSetLineMode(ioLine, PAL_MODE_OUTPUT_PUSHPULL);
                break;

            case GpioPinDriveMode_OutputOpenDrain:
                palSetLineMode(ioLine, PAL_MODE_OUTPUT_OPENDRAIN);
                break;

            default:
                // all other modes are NOT supported
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // if drive mode is output, read the pad to update the managed field _lastOutputValue
        if(!driveModeIsInput)
        {
            pThis[ FIELD___lastOutputValue ].NumericByRef().s4 = palReadLine(ioLine);
        }

        if(callbacksRegistered && setupInterrupt)
        {
            // there are callbacks registered and...
            // the drive mode is input so need to setup the interrupt

            // save pin
            //channelPinMapping[pad] = pThis;

            palEnableLineEvent(ioLine, PAL_EVENT_MODE_BOTH_EDGES);
            palSetLineCallback(ioLine, GpioEventCallback, pThis);

            // protect this from GC so that the callback is where it's supposed to
            CLR_RT_ProtectFromGC         gc( *pThis );
            
            // read pad and store current value to check on debounce callback
            lastPadValue = palReadLine(ioLine);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeInit___BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        //int16_t pinNumber = stack.Arg1().NumericByRef().s4;

        // TODO is probably a good idea keep track of the used pins, so we can check that here
        // TODO is probably a good idea to check if this pin exists

        // Return value to the managed application
        stack.SetResult_Boolean(true );
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDebounceTimeout___VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    // nothing to do here as the debounce timeout is grabbed from the managed object when required

    NANOCLR_NOCLEANUP_NOLABEL();
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

        // get IoLine from pin number
        ioline_t ioLine = GetIoLine(pThis[ FIELD___pinNumber ].NumericByRefConst().s4);

        GpioPinDriveMode driveMode = (GpioPinDriveMode)pThis[ FIELD___driveMode ].NumericByRefConst().s4;

        GpioPinValue state = (GpioPinValue)stack.Arg1().NumericByRef().s4;

        // sanity check for drive mode set to output so we don't mess up writing to an input pin
        if ((driveMode == GpioPinDriveMode_Output) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrain) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrainPullUp) ||
            (driveMode == GpioPinDriveMode_OutputOpenSource) ||
            (driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
        {
            palWriteLine(ioLine, state);

            // update the managed field _lastOutputValue
            pThis[ FIELD___lastOutputValue ].NumericByRef().s4 = state;
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

        // get IoLine from pin number
        ioline_t ioLine = GetIoLine(pThis[ FIELD___pinNumber ].NumericByRefConst().s4);

        // get alternate function argument
		int32_t alternateFunction = stack.Arg1().NumericByRef().s4;

        palSetLineMode(ioLine, PAL_MODE_ALTERNATE(alternateFunction));

    }
    NANOCLR_NOCLEANUP();
}
